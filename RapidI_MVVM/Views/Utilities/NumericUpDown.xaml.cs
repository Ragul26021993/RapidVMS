using System;
using System.Windows;
using System.Windows.Controls;
using Rapid.AttachedProperties;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for NumericUpDown.xaml
    /// </summary>
    public partial class NumericUpDown : UserControl
    {
        public NumericUpDown()
        {
            InitializeComponent();
            this.Value = this.Minimum;
            this.StepValue = 1;
        }
        //The property to raise the current value.
        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(double), 
            typeof(NumericUpDown), new PropertyMetadata(0D, new PropertyChangedCallback(HandleValueChange)));

        public static readonly DependencyProperty MaskProperty = DependencyProperty.Register("Mask", typeof(MaskType), 
            typeof(NumericUpDown), new PropertyMetadata(MaskType.Decimal));

        public static readonly DependencyProperty MaximumProperty = DependencyProperty.Register("Maximum", 
            typeof(double), typeof(NumericUpDown), new PropertyMetadata(10D));

        public static readonly DependencyProperty MinimumProperty = DependencyProperty.Register("Minimum", 
            typeof(double), typeof(NumericUpDown), new PropertyMetadata(0D));

        public event RoutedEventHandler ValueChanged;
            private static void HandleValueChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
            {
                NumericUpDown U = (NumericUpDown)d;
               if( U.Mask == MaskType.Integer)
                   U._valueBox.Text = ((double)e.NewValue).ToString("F0", System.Globalization.CultureInfo.InvariantCulture).Split('.')[0];
               else
                   U._valueBox.Text = ((double)e.NewValue).ToString("F2", System.Globalization.CultureInfo.InvariantCulture);
               NumericUpDown instance = (NumericUpDown)d;
               if (instance.ValueChanged != null)
                   instance.ValueChanged(instance, null);
            }
            public double StepValue { get; set; }
            public double Value
            {
                get { return (double)GetValue(ValueProperty); }
                set { SetValue(ValueProperty, value); }
            }
            public double Maximum
            {
                get { return (double)GetValue(MaximumProperty); }
                set { SetValue(MaximumProperty, value); }
            }
            public double Minimum
            {
                get { return (double)GetValue(MinimumProperty); }
                set { SetValue(MinimumProperty, value); }
            }
            public MaskType Mask
            {
                get { return (MaskType)GetValue(MaskProperty); }
                set { SetValue(MaskProperty, value); }
            }
            private void HandleClick(object sender, RoutedEventArgs e)
            {
                if (sender.Equals(_btnUp)) 
                {
                    if (!(Math.Round(this.Value,1) >= this.Maximum))
                        this.Value += StepValue; 
                }
                else if (sender.Equals(_btnDown))
                {
                    if (!(Math.Round(this.Value, 1) <= this.Minimum))
                        this.Value -= StepValue;
                }
            }
    }
}
