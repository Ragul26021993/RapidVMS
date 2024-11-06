using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Rapid.LightingControl
{
    //This class contains all logic to Draw a fully functional single zone lighting.
    //That contains Base Lighting drawing, Drawing on MouseScroll,And text drawing.
    class SingleLightingZone : System.Windows.Controls.Grid
    {
        //Instances of Two arcs.First is the base arc and second is drawn as the mouse scroll happens.
        protected internal Arc First, Second;
        //Text that has to be drawn on the path.
        protected internal TextOnPathVisuals DrawnArcText;
        //Initializing the Arcs & assigining properties on them.
        public SingleLightingZone()
        {
            First = new Arc();
            Second = new Arc();
            DrawnArcText = new TextOnPathVisuals();
            DrawnArcText.FontWeight = FontWeights.Bold;
            First.Stroke = this.BaseColor;
            Second.Stroke = this.FillColor;
            GetStepValue = (MaximumValue - MinimumValue) / MaximumValue;
            this.Children.Add(First);
            this.Children.Add(Second);
            this.Children.Add(DrawnArcText);
            DrawnArcText.HorizontalAlignment = System.Windows.HorizontalAlignment.Center;
            DrawnArcText.VerticalAlignment = System.Windows.VerticalAlignment.Stretch;
        }
        //Drawing the text over the shape.
        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);
            DrawnArcText.Width = 2 * First.Radius;
            DrawnArcText.Height = 2 * First.Radius;
            DrawnArcText.PathFigure = First.MidPathFigure;
            First.HorizontalAlignment = System.Windows.HorizontalAlignment.Center;
        }

        #region Properties
        //The Value of the scrolling for the zone.
        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register("Value", typeof(int),
            typeof(SingleLightingZone), new FrameworkPropertyMetadata((int)0, new PropertyChangedCallback(UpdateColorsOnValueChage)));
        //the Minimum limit for the zone
        public static readonly DependencyProperty MinimumValueProperty = DependencyProperty.Register("MinimumValue",
            typeof(int), typeof(SingleLightingZone), new FrameworkPropertyMetadata((int)0, new PropertyChangedCallback(RangeChanged)));
        //The Maximum value for the zone.
        public static readonly DependencyProperty MaximumValueProperty = DependencyProperty.Register("MaximumValue",
            typeof(int), typeof(SingleLightingZone), new FrameworkPropertyMetadata((int)64.0, new PropertyChangedCallback(RangeChanged)));
        //The color of the Base Arc.
        public static readonly DependencyProperty BaseColorProperty = DependencyProperty.Register("BaseColor", typeof(Brush),
            typeof(SingleLightingZone), new FrameworkPropertyMetadata(Brushes.Green, FrameworkPropertyMetadataOptions.AffectsRender));
        //the Color of the Fill Arc.(Arc that draws when scroll happens).
        public static readonly DependencyProperty FillColorProperty = DependencyProperty.Register("FillColor", typeof(Brush),
            typeof(SingleLightingZone), new FrameworkPropertyMetadata(Brushes.Yellow, FrameworkPropertyMetadataOptions.AffectsRender));

        public int Value
        {
            get { return (int)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }
        public int MinimumValue
        {
            get { return (int)GetValue(MinimumValueProperty); }
            set { SetValue(MinimumValueProperty, value); }
        }
        public int MaximumValue
        {
            get { return (int)GetValue(MaximumValueProperty); }
            set { SetValue(MaximumValueProperty, value); }
        }
        public Brush BaseColor
        {
            get { return (Brush)GetValue(BaseColorProperty); }
            set
            {
                SetValue(BaseColorProperty, value);
                First.Stroke = this.BaseColor;
            }
        }
        public Brush FillColor
        {
            get { return (Brush)GetValue(FillColorProperty); }
            set
            {
                SetValue(FillColorProperty, value);
                Second.Stroke = this.FillColor;
            }
        }
        public void DrawTicks(double FullRadius, int NoOfDivisions, int LongerTickInterval)
        {
            System.Windows.Shapes.Path Tick;
            LineGeometry TickLine;
            int i = 1;
            double unitAng = (EndAngle - StartAngle) / NoOfDivisions;
            double InnerDist = Radius - Thickness * 0.75, OuterDist = Radius - Thickness * 0.4, LongerOuterDist = Radius - Thickness * 0.25;
            if (EndAngle > StartAngle)
            {
                for (double itr = StartAngle + unitAng; itr < EndAngle; itr += unitAng, i++)
                {
                    Tick = new System.Windows.Shapes.Path();
                    Tick.StrokeThickness = 1;
                    Tick.Stroke = System.Windows.Media.Brushes.White;
                    Tick.Opacity = 0.5;
                    TickLine = new LineGeometry();
                    if (i % LongerTickInterval == 0)
                    {
                        TickLine.StartPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * InnerDist, FullRadius - Math.Sin(itr * Math.PI / 180) * InnerDist);
                        TickLine.EndPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * LongerOuterDist, FullRadius - Math.Sin(itr * Math.PI / 180) * LongerOuterDist);
                    }
                    else
                    {
                        TickLine.StartPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * InnerDist, FullRadius - Math.Sin(itr * Math.PI / 180) * InnerDist);
                        TickLine.EndPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * OuterDist, FullRadius - Math.Sin(itr * Math.PI / 180) * OuterDist);
                    }
                    Tick.Data = TickLine;
                    this.Children.Add(Tick);
                    Grid.SetZIndex(Tick, 99);
                }
            }
            else
            {
                for (double itr = StartAngle + unitAng; itr > EndAngle; itr += unitAng, i++)
                {
                    Tick = new System.Windows.Shapes.Path();
                    Tick.StrokeThickness = 1;
                    Tick.Stroke = System.Windows.Media.Brushes.White;
                    Tick.Opacity = 0.5;
                    TickLine = new LineGeometry();
                    if (i % LongerTickInterval == 0)
                    {
                        TickLine.StartPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * InnerDist, FullRadius - Math.Sin(itr * Math.PI / 180) * InnerDist);
                        TickLine.EndPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * LongerOuterDist, FullRadius - Math.Sin(itr * Math.PI / 180) * LongerOuterDist);
                    }
                    else
                    {
                        TickLine.StartPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * InnerDist, FullRadius - Math.Sin(itr * Math.PI / 180) * InnerDist);
                        TickLine.EndPoint = new Point(FullRadius + Math.Cos(itr * Math.PI / 180) * OuterDist, FullRadius - Math.Sin(itr * Math.PI / 180) * OuterDist);
                    }
                    Tick.Data = TickLine;
                    this.Children.Add(Tick);
                    Grid.SetZIndex(Tick, 99);
                }
            }
            Tick = new System.Windows.Shapes.Path();
            Tick.StrokeThickness = 1;
            Tick.Stroke = System.Windows.Media.Brushes.White;
            Tick.Opacity = 1;
            TickLine = new LineGeometry();
            TickLine.StartPoint = new Point(FullRadius + Math.Cos(StartAngle * Math.PI / 180) * (Radius - Thickness), FullRadius - Math.Sin(StartAngle * Math.PI / 180) * (Radius - Thickness));
            TickLine.EndPoint = new Point(FullRadius + Math.Cos(StartAngle * Math.PI / 180) * Radius, FullRadius - Math.Sin(StartAngle * Math.PI / 180) * Radius);
            Tick.Data = TickLine;
            this.Children.Add(Tick);
            Tick = new System.Windows.Shapes.Path();
            Tick.StrokeThickness = 1;
            Tick.Stroke = System.Windows.Media.Brushes.White;
            Tick.Opacity = 1;
            TickLine = new LineGeometry();
            TickLine.StartPoint = new Point(FullRadius + Math.Cos(EndAngle * Math.PI / 180) * (Radius - Thickness), FullRadius - Math.Sin(EndAngle * Math.PI / 180) * (Radius - Thickness));
            TickLine.EndPoint = new Point(FullRadius + Math.Cos(EndAngle * Math.PI / 180) * Radius, FullRadius - Math.Sin(EndAngle * Math.PI / 180) * Radius);
            Tick.Data = TickLine;
            this.Children.Add(Tick);
        }

        //Callback function for recalculating the values of if Minimum or Maximum is changed.
        static void RangeChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            SingleLightingZone ar = (SingleLightingZone)d;
            if (e.Property == SingleLightingZone.MaximumValueProperty)
            {
                ar.GetStepValue = (int)((int)e.NewValue - ar.MinimumValue) / (int)e.NewValue;
            }
            else
            {
                ar.GetStepValue = (int)((int)ar.MaximumValue - (int)e.NewValue) / ar.MaximumValue;
            }
        }
        //callback functions to update colors when the value of the Zone changes.
        static void UpdateColorsOnValueChage(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            SingleLightingZone ar = (SingleLightingZone)d;
            //Validate the value.
            if ((int)e.NewValue > ar.MaximumValue) { ar.Value = ar.MaximumValue; }
            if ((int)e.NewValue < ar.MinimumValue) { ar.Value = ar.MinimumValue; }
            double StepValue = (ar.First.EndAngle - ar.First.StartAngle) / (ar.MaximumValue - ar.MinimumValue);
            //Now update the color status
            ar.Second.EndAngle = ar.First.StartAngle + StepValue * (int)e.NewValue;
        }

        #region Other Common CLR properties.
        //The Foreground(Text Color)
        public Brush Foreground { set { Second.Foreground = value; } }
        //The Text value.
        public string Text { set { Second.Text = value; } }
        //The Start Angle
        public double StartAngle
        {
            get { return First.StartAngle; }
            set
            {
                First.StartAngle = value; Second.StartAngle = value; Second.EndAngle = value;

            }
        }
        //The End Angle
        public double EndAngle
        {
            get { return First.EndAngle; }
            set
            {
                First.EndAngle = value; Second.PseudoEndAngle = value;

            }
        }
        //The Thickness of the Arc
        public double Thickness { get { return First.StrokeThickness; } set { First.StrokeThickness = value; Second.StrokeThickness = 0.5 * value; } }
        //The radius of the Arc
        public int Radius { get { return First.Radius; } set { First.Radius = value; Second.Radius = value - (int)(0.5 * Second.StrokeThickness); } }
        public PenLineCap LineCap
        {
            set
            {
                First.StrokeStartLineCap = value;
                First.StrokeEndLineCap = value;
                Second.StrokeStartLineCap = value;
                Second.StrokeEndLineCap = value;
            }
        }
        //The step by which increment /decrement should happen in Value.
        public int GetStepValue { get; set; }
        #endregion

        #endregion
    }
}
