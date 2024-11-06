using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for TolerenceControl.xaml
    /// </summary>
    public partial class ToleranceControl : Grid
    {
        private double IndicatorPosRange, ValueRange, IndicatorWidth;
        public ToleranceControl()
        {
            this.InitializeComponent();
        }

        //The property to raise the current value.
        private static readonly DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(double),
            typeof(ToleranceControl), new PropertyMetadata(-1D, new PropertyChangedCallback(HandleValueChange)));

        //private static readonly DependencyProperty MaximumProperty = DependencyProperty.Register("Maximum",
        //    typeof(double), typeof(ToleranceControl), new PropertyMetadata(10D, new PropertyChangedCallback(HandleMaxChange)));

        //private static readonly DependencyProperty MinimumProperty = DependencyProperty.Register("Minimum",
        //    typeof(double), typeof(ToleranceControl), new PropertyMetadata(0D, new PropertyChangedCallback(HandleMinChange)));

        private static readonly DependencyProperty ToleranceModeOnProperty = DependencyProperty.Register("ToleranceModeOn",
           typeof(bool), typeof(ToleranceControl), new PropertyMetadata(false, new PropertyChangedCallback(HandleNomPtChange)));

        private static readonly DependencyProperty NominalProperty = DependencyProperty.Register("Nominal",
            typeof(double), typeof(ToleranceControl), new PropertyMetadata(5D, new PropertyChangedCallback(HandleNomChange)));

        private static readonly DependencyProperty UpperLimitProperty = DependencyProperty.Register("UpperLimit",
            typeof(double), typeof(ToleranceControl), new PropertyMetadata(5D, new PropertyChangedCallback(HandleUpChange)));

        private static readonly DependencyProperty LowerLimitProperty = DependencyProperty.Register("LowerLimit",
            typeof(double), typeof(ToleranceControl), new PropertyMetadata(5D, new PropertyChangedCallback(HandleLoChange)));

        private static void HandleValueChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ToleranceControl Me = (ToleranceControl)d;
            //Me.ValueTxtBlk.Text = ((double)e.NewValue).ToString();
            Me.ResetVisuals();
        }

        private static void HandleNomChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ToleranceControl Me = (ToleranceControl)d;
            //Me.MaxTxtBlk.Text = ((double)e.NewValue).ToString();
            Me.ResetVisuals();
        }

        private static void HandleNomPtChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ToleranceControl Me = (ToleranceControl)d;
            Me.ResetVisuals();
        }

        private static void HandleUpChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ToleranceControl Me = (ToleranceControl)d;
            //Me.MaxTxtBlk.Text = ((double)e.NewValue).ToString();
            Me.ResetVisuals();
        }

        private static void HandleLoChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ToleranceControl Me = (ToleranceControl)d;
            //Me.MaxTxtBlk.Text = ((double)e.NewValue).ToString();
            Me.ResetVisuals();
        }

        private void ResetVisuals()
        {
            IndicatorPosRange = this.ColumnDefinitions[1].ActualWidth - ValueIndicator.ActualWidth;

            double Maximum = Nominal + UpperLimit, Minimum = Nominal + LowerLimit;
            if (!ToleranceModeOn)
            {
                Maximum = UpperLimit;
                Minimum = LowerLimit;
            }
            ValueRange = Maximum - Minimum;
            IndicatorWidth = ValueIndicator.ActualWidth;
            ValueIndicator.Width = IndicatorWidth;
            double GreenLevel;           
            byte RedVal = 255, Greenval = 0;
            if (Nominal < Minimum || Nominal > Maximum || Maximum == Minimum)
            {
                GreenLevel = (Minimum + Maximum) / 2;
                NominalPt.Offset = 0.5;
            }
            else
            {
                GreenLevel = Nominal;
                NominalPt.Offset = 0.15 + (Nominal - Minimum) / (Maximum - Minimum) * 0.7;
            }
            
            if (Value >= GreenLevel && Value <= Maximum && GreenLevel != Maximum)
            {
                double fraction = (Value - GreenLevel) / (Maximum - GreenLevel);
                RedVal = (byte)(fraction * 128);
                Greenval = (byte)(255 - RedVal);               
            }
            else if (Value <= GreenLevel && Value >= Minimum && GreenLevel != Minimum)
            {
                double fraction = (GreenLevel - Value) / (GreenLevel - Minimum);
                RedVal = (byte)(fraction * 128);
                Greenval = (byte)(255 - RedVal);                
            }
            else if (GreenLevel == Maximum && GreenLevel == Minimum && Value == GreenLevel)
            {
                Greenval = 255;
                RedVal = 0;
            }
            else
            {
                Greenval = 0;
                RedVal = 255;
            }
            
            var Colr = new Color();
            Colr.R = RedVal;
            Colr.G = Greenval;
            Colr.B = 0;
            Colr.A = 255;
            //BackRect.Fill = new SolidColorBrush(Colr);
            if (Value < Minimum)
            {
                //BackRect.Fill = ValueOutOfRange;
                ValueIndicator.HorizontalAlignment = System.Windows.HorizontalAlignment.Center;
                Grid.SetColumn(ValueIndicator, 0);
                ValueIndicator.Margin = new Thickness(1, 1, 1, 1);
            }
            else if (Value > Maximum)
            {
                // BackRect.Fill = ValueOutOfRange;
                ValueIndicator.HorizontalAlignment = System.Windows.HorizontalAlignment.Center;
                Grid.SetColumn(ValueIndicator, 2);
                ValueIndicator.Margin = new Thickness(1, 1, 1, 1 );
            }
            else if (Maximum != Minimum)
            {
                //BackRect.Fill = ValueOK;
                ValueIndicator.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                Grid.SetColumn(ValueIndicator, 1);
                ValueIndicator.Margin = new Thickness((Value - Minimum) / ValueRange * IndicatorPosRange, ValueIndicator.Margin.Top, 0, ValueIndicator.Margin.Bottom);
            }
        }

        public double Value
        {
            get { return (double)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }
        //public double Maximum
        //{
        //    get { return (double)GetValue(MaximumProperty); }
        //    set { SetValue(MaximumProperty, value); }
        //}
        //public double Minimum
        //{
        //    get { return (double)GetValue(MinimumProperty); }
        //    set { SetValue(MinimumProperty, value); }
        //}
        public double Nominal
        {
            get { return (double)GetValue(NominalProperty); }
            set { SetValue(NominalProperty, value); }
        }
        public double UpperLimit
        {
            get { return (double)GetValue(UpperLimitProperty); }
            set { SetValue(UpperLimitProperty, value); }
        }
        public double LowerLimit
        {
            get { return (double)GetValue(LowerLimitProperty); }
            set { SetValue(LowerLimitProperty, value); }
        }
        public bool ToleranceModeOn
        {
            get { return (bool)GetValue(ToleranceModeOnProperty); }
            set { SetValue(ToleranceModeOnProperty, value); }
        }
        //public double NominalPoint
        //{
        //    //get { return (double)GetValue(NominalPointProperty); }
        //    //set { SetValue(NominalPointProperty, value); }
        //    get;
        //    set;
        //}
    }
}