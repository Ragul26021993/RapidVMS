using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Globalization;
using System.Text.RegularExpressions;

namespace Rapid.AttachedProperties
{
    public enum MaskType
    {
        Any,
        Integer,
        PositiveInteger,
        Decimal,
        PositiveDecimal
    }

    public sealed class AttachedTextBoxBehaviour
    {
        private AttachedTextBoxBehaviour() { }

        #region MinimumValue Property

        public static double GetMinimumValue(DependencyObject obj)
        {
            return (double)obj.GetValue(MinimumValueProperty);
        }

        public static void SetMinimumValue(DependencyObject obj, double value)
        {
            obj.SetValue(MinimumValueProperty, value);
        }

        public static readonly DependencyProperty MinimumValueProperty =
            DependencyProperty.RegisterAttached(
                "MinimumValue",
                typeof(double),
                typeof(AttachedTextBoxBehaviour),
                new FrameworkPropertyMetadata(double.NaN)
                );

        #endregion

        #region MaximumValue Property

        public static double GetMaximumValue(DependencyObject obj)
        {
            return (double)obj.GetValue(MaximumValueProperty);
        }

        public static void SetMaximumValue(DependencyObject obj, double value)
        {
            obj.SetValue(MaximumValueProperty, value);
        }

        public static readonly DependencyProperty MaximumValueProperty =
            DependencyProperty.RegisterAttached(
                "MaximumValue",
                typeof(double),
                typeof(AttachedTextBoxBehaviour),
                new FrameworkPropertyMetadata(double.NaN)
                );

        #endregion

        #region Mask Property

        public static MaskType GetMask(DependencyObject obj)
        {
            return (MaskType)obj.GetValue(MaskProperty);
        }

        public static void SetMask(DependencyObject obj, MaskType value)
        {
            obj.SetValue(MaskProperty, value);
        }

        public static readonly DependencyProperty MaskProperty =
            DependencyProperty.RegisterAttached(
                "Mask",
                typeof(MaskType),
                typeof(AttachedTextBoxBehaviour),
                new FrameworkPropertyMetadata(MaskChangedCallback)
                );

        private static void MaskChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            TextBox _this = (d as TextBox);
            if (_this == null)
                return;

            _this.PreviewTextInput -= TextBox_PreviewTextInput;
            _this.PreviewKeyDown -= TextBox_NegateSpaceInput;
            DataObject.RemovePastingHandler(_this, (DataObjectPastingEventHandler)TextBoxPastingEventHandler);

            if ((MaskType)e.NewValue != MaskType.Any)
            {
                _this.Text = "0";
                _this.PreviewTextInput += TextBox_PreviewTextInput;
                _this.PreviewKeyDown += TextBox_NegateSpaceInput;
                DataObject.AddPastingHandler(_this, (DataObjectPastingEventHandler)TextBoxPastingEventHandler);
            }
        }

        #endregion

        #region MaxTextLength Property

        public static int GetMaxTextLength(DependencyObject obj)
        {
            return (int)obj.GetValue(MaxTextLengthProperty);
        }

        public static void SetMaxTextLength(DependencyObject obj, int value)
        {
            obj.SetValue(MaxTextLengthProperty, value);
        }

        public static readonly DependencyProperty MaxTextLengthProperty =
            DependencyProperty.RegisterAttached(
                "MaxTextLength",
                typeof(int),
                typeof(AttachedTextBoxBehaviour),
                new FrameworkPropertyMetadata(OnMaxTextLengthChanged)
                );

        private static void OnMaxTextLengthChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            TextBox _this = d as TextBox;
            int TxtLength = (int)e.NewValue;
            if (_this.Text.Length > TxtLength)
                _this.Text = _this.Text.Substring(0, TxtLength);

            _this.PreviewTextInput -= TextBox_PreviewTextLengthInput;
            _this.PreviewKeyDown -= TextBox_NegateSpaceTextLengthInput;
            DataObject.RemovePastingHandler(_this, (DataObjectPastingEventHandler)TextBoxPastingTextLengthEventHandler);
            _this.PreviewTextInput += TextBox_PreviewTextLengthInput;
            _this.PreviewKeyDown += TextBox_NegateSpaceTextLengthInput;
            DataObject.AddPastingHandler(_this, (DataObjectPastingEventHandler)TextBoxPastingTextLengthEventHandler);
        }
        #endregion

        #region Validation functions and handlers

        private static bool ValidText(string text, MaskType Masking)
        {
            string ExpStr;
            Regex regex;
            string LocalDecSeparator = NumberFormatInfo.CurrentInfo.NumberDecimalSeparator;
            if (LocalDecSeparator == ".")
                LocalDecSeparator = "\\.";

            switch (Masking)
            {
                case MaskType.Any:
                    return true;
                case MaskType.Decimal:
                    ExpStr = "^[\\+-]?\\d*" + LocalDecSeparator + "?\\d*([Ee]-?\\d*)?$";
                    regex = new Regex(ExpStr);
                    return regex.IsMatch(text);
                case MaskType.PositiveDecimal:
                    ExpStr = "^\\+?\\d*" + LocalDecSeparator + "?\\d*([Ee]-?\\d*)?$";
                    regex = new Regex(ExpStr);
                    return regex.IsMatch(text);
                case MaskType.Integer:
                    ExpStr = "^[\\+-]?\\d*$";
                    regex = new Regex(ExpStr);
                    return regex.IsMatch(text);
                case MaskType.PositiveInteger:
                    ExpStr = "^\\+?\\d*$";
                    regex = new Regex(ExpStr);
                    return regex.IsMatch(text);
            }
            return true;
        }
        private static bool WithinLimits(TextBox _this, string text)
        {
            try
            {
                double val;

                //Since text is already validated before being passed to this function,
                //the failing of "double.TryParse" in the following code only means user is in the process
                //of entering the values. That is, tryParse may fail if user has entered -ve sign or 
                //decimal point first and is in the process of entering further characters, hence return true.
                if (!double.TryParse(text, out val)) return true;

                //If min val and max val has not been attached, the execution will go to catch in the following line
                double MinVal = GetMinimumValue(_this), MaxVal = GetMaximumValue(_this);
                if (MinVal.Equals(double.NaN))
                    return true;
                if (MaxVal.Equals(double.NaN))
                    return true;
                if (val < MinVal)
                    return false;
                if (val > MaxVal)
                    return false;

                return true;  //Value within limits
            }
            catch
            {
                return true;
            }
        }
        private static bool WithinMaxLength(TextBox _this, string text)
        {
            try
            {
                int MaxTxtLength = GetMaxTextLength(_this);
                if (MaxTxtLength == 0) return true;
                return text.Length <= MaxTxtLength;
            }
            catch
            {
                return true;
            }
        }

        private static void TextBoxPastingEventHandler(object sender, DataObjectPastingEventArgs e)
        {
            TextBox _this = (sender as TextBox);
            if (e.DataObject.GetDataPresent(typeof(String)))
            {
                String text = (String)e.DataObject.GetData(typeof(String));
                if (!ValidText(text, GetMask(_this)))
                {
                    e.Handled = true;
                    e.CancelCommand();
                    return;
                }
                if (!WithinLimits(_this, text))
                {
                    e.Handled = true;
                    e.CancelCommand();
                    return;
                }
            }
            else
            {
                e.Handled = true;
                e.CancelCommand();
            }
        }
        private static void TextBox_PreviewTextInput(object sender, System.Windows.Input.TextCompositionEventArgs e)
        {
            TextBox _this = (sender as TextBox);
            String text;

            if (_this.CaretIndex < _this.Text.Length)
            {
                if (_this.SelectionLength > 0)
                    text = _this.Text.Substring(0, _this.SelectionStart) + e.Text + _this.Text.Substring(_this.SelectionStart + _this.SelectionLength);
                else
                    text = _this.Text.Substring(0, _this.CaretIndex) + e.Text + _this.Text.Substring(_this.CaretIndex);
            }
            else
            {
                text = _this.Text + e.Text;
            }

            if (!ValidText(text, GetMask(_this)))
            {
                e.Handled = true;
                return;
            }
            if (!WithinLimits(_this, text))
            {
                e.Handled = true;
                return;
            }
        }
        private static void TextBox_NegateSpaceInput(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Space)
            {
                e.Handled = true;
            }
        }

        private static void TextBoxPastingTextLengthEventHandler(object sender, DataObjectPastingEventArgs e)
        {
            TextBox _this = (sender as TextBox);
            if (e.DataObject.GetDataPresent(typeof(String)))
            {
                String text = (String)e.DataObject.GetData(typeof(String));
                MaskType _thisMType = GetMask(_this);
                if (!WithinMaxLength(_this, text) && _thisMType == MaskType.Any)
                {
                    _this.Text = text.Substring(0, GetMaxTextLength(_this));
                    //e.Handled = true;
                    e.CancelCommand();
                }
            }
            else
            {
                //e.Handled = true;
                e.CancelCommand();
            }
        }
        private static void TextBox_PreviewTextLengthInput(object sender, System.Windows.Input.TextCompositionEventArgs e)
        {
            TextBox _this = (sender as TextBox);
            String text;

            if (_this.CaretIndex < _this.Text.Length)
            {
                if (_this.SelectionLength > 0)
                    text = _this.Text.Substring(0, _this.SelectionStart) + e.Text + _this.Text.Substring(_this.SelectionStart + _this.SelectionLength);
                else
                    text = _this.Text.Substring(0, _this.CaretIndex) + e.Text + _this.Text.Substring(_this.CaretIndex);
            }
            else
            {
                text = _this.Text + e.Text;
            }

            if (!WithinMaxLength(_this, text))
                e.Handled = true;
        }
        private static void TextBox_NegateSpaceTextLengthInput(object sender, KeyEventArgs e)
        {
            TextBox _this = sender as TextBox;
            int TxtLength = GetMaxTextLength(_this);
            if (_this.Text.Length >= TxtLength && e.Key == Key.Space && _this.SelectionLength == 0)
            {
                e.Handled = true;
            }
        }

        #endregion
    }
}
