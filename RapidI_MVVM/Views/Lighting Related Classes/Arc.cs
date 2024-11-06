using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Shapes;

namespace Rapid.LightingControl
{
    /// <summary>
    /// This serves as a base class which have methods to draw Arc.
    /// It includes Radius,StartAngle,EndAngle of the Arc.
    /// Basically it deals with drawing and it doesn't bother about any interactional behaviours.
    /// This Class can be inherited to implement additional logics related to interaction with the shape.
    /// </summary>
    internal  class Arc : Shape
    {
        #region Implementataion of absract Shape class method
        //Implementataion of the DefiningGeometry  property.
        protected override Geometry DefiningGeometry
        {
            get { return GetArcGeometry(); }
        }
        #endregion

        #region Dependencies Properties and other properties declarations
        //Dependency Properties.
        public static readonly DependencyProperty StartAngleProperty = DependencyProperty.Register("StartAngle", typeof(double), typeof(Arc), new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.AffectsRender));
        public static readonly DependencyProperty EndAngleProperty = DependencyProperty.Register("EndAngle", typeof(double), typeof(Arc), new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.AffectsRender));
        public static readonly DependencyProperty RadiusProperty = DependencyProperty.RegisterAttached("Radius", typeof(int), typeof(Arc), new FrameworkPropertyMetadata(0, FrameworkPropertyMetadataOptions.AffectsRender, new PropertyChangedCallback(ReCalculate)));
       
       //The callback function to handle the change in the Radius property.
        private static void ReCalculate(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
                Arc Ob = (Arc)d;
                Ob.Width = (int)(e.NewValue) * 2;
                Ob.Height = (int)(e.NewValue) * 2;
        }
        //Properties declarations.
        //The Radius of the Arc 
        public int Radius
        {
            get { return (int)(GetValue(RadiusProperty)); }
            set { SetValue(RadiusProperty, value); }
        }
        //The start angle of the arc.
        public double StartAngle
        {
            get { return (Double)(GetValue(StartAngleProperty)); }
            set { SetValue(StartAngleProperty, value); }
        }
        //The end angle of the arc.
        public double EndAngle
        {
            get { return (Double)(GetValue(EndAngleProperty)); }
            set { SetValue(EndAngleProperty, value); }
        }
        
        #endregion

        #region Calculating Geometry
        //The methods to calculate the geometry
        private Geometry GetArcGeometry()
        {
            Point startPoint = PointAtAngle(Math.Min(StartAngle, EndAngle));
            Point endPoint = PointAtAngle(Math.Max(StartAngle, EndAngle));

            Size arcSize = new Size(Math.Max(0, (RenderSize.Width - StrokeThickness) / 2), Math.Max(0, (RenderSize.Height - StrokeThickness))/2);
            bool isLargeArc = Math.Abs(EndAngle - StartAngle) > 180;

            StreamGeometry geom = new StreamGeometry();
            using (StreamGeometryContext context = geom.Open())
            {
                context.BeginFigure(startPoint, false, false);
                context.ArcTo(endPoint, arcSize, 0, isLargeArc, SweepDirection.Counterclockwise, true, false);
            }
            geom.Transform = new TranslateTransform(StrokeThickness / 2, StrokeThickness / 2);
            geom.Freeze();



            double TotalSweep = EndAngle - StartAngle;
            double MidAngle = (StartAngle + EndAngle) / 2;
            Point MidStartPoint, MidEndPoint;
            if ((MidAngle < -45 && MidAngle > -135) || (MidAngle > 225 && MidAngle < 315))
            {
                MidStartPoint = MidPointsAtAngle(StartAngle + (TotalSweep / 3), true);
                MidEndPoint = MidPointsAtAngle(StartAngle + 2 * (TotalSweep / 3), true);
            }
            else
            {
                MidStartPoint = MidPointsAtAngle(StartAngle + (TotalSweep / 3), false);
                MidEndPoint = MidPointsAtAngle(StartAngle + 2 * (TotalSweep / 3), false);
            }

            //PathGeometry pg = new PathGeometry();
            //PathFigure p = new PathFigure();
            //p.StartPoint = MidStartPoint;
            //p.Segments.Add(new ArcSegment(MidEndPoint, arcSize, 0, isLargeArc, SweepDirection.Counterclockwise, true));
            //pg.Figures.Add(p);
            //MidPathFigure = pg.Figures[0];


            //MidStartPoint.X += StrokeThickness / 2;
            //MidStartPoint.Y += StrokeThickness / 3;

            //MidEndPoint.X += StrokeThickness / 2;
            //MidEndPoint.Y += StrokeThickness / 3;
            // arcSize = new Size(Math.Max(0, (RenderSize.Width - StrokeThickness) / 2), Math.Max(0, (RenderSize.Height - StrokeThickness)) / 2);
            StreamGeometry midGeom = new StreamGeometry();
            using (StreamGeometryContext context = midGeom.Open())
            {
                if (StartAngle < EndAngle)  //lighting arc is in anti-clockwise direction
                {
                    
                    if ((MidAngle < -45 && MidAngle > -135) || (MidAngle > 225 && MidAngle < 315))
                    {
                        context.BeginFigure(MidStartPoint, false, false);
                        context.ArcTo(MidEndPoint, arcSize, 0, false, SweepDirection.Counterclockwise, true, false);
                    }
                    else
                    {
                        context.BeginFigure(MidEndPoint, false, false);
                        context.ArcTo(MidStartPoint, arcSize, 0, false, SweepDirection.Clockwise, true, false);
                    }
                }
                else  //lighting arc is in clockwise direction
                {
                    if ((MidAngle < -45 && MidAngle > -135) || (MidAngle > 225 && MidAngle < 315))
                    {
                        context.BeginFigure(MidEndPoint, false, false);
                        context.ArcTo(MidStartPoint, arcSize, 0, false, SweepDirection.Counterclockwise, true, false);
                    }
                    else
                    {
                        context.BeginFigure(MidStartPoint, false, false);
                        context.ArcTo(MidEndPoint, arcSize, 0, false, SweepDirection.Clockwise, true, false);
                    }
                }
            }
            TransformGroup tg = new TransformGroup();
            tg.Children.Add(new TranslateTransform(StrokeThickness / 2, StrokeThickness / 2));


            midGeom.Transform = tg;


            MidPathFigure = ((PathGeometry)midGeom.GetWidenedPathGeometry(new Pen(this.Stroke, -1))).Figures[0];
            return geom;
        }
        //This function will return the points for a given angle for the arc shape.
        protected internal Point PointAtAngle(double angle)
        {
            double radAngle = angle * (Math.PI / 180);
            double xRadius = (RenderSize.Width - StrokeThickness) / 2;
            double yRadius = (RenderSize.Height - StrokeThickness)/2;

            double x = xRadius + xRadius * Math.Cos(radAngle);
            double y = yRadius - yRadius * Math.Sin(radAngle);
            return new Point(x, y);
        }
        protected internal Point MidPointsAtAngle(double angle, bool UpsideDownText)
        {
            double radAngle = angle * (Math.PI / 180);
            double xRadius = (RenderSize.Width - StrokeThickness) / 2;
            double yRadius = (RenderSize.Height - StrokeThickness) / 2;
            //double xRadius = (RenderSize.Width) / 2;
            //double yRadius = (RenderSize.Height) / 2;

            double x, y;
            //double x = xRadius + xRadius * Math.Cos(radAngle);
            //double y = yRadius - yRadius * Math.Sin(radAngle);
            if (UpsideDownText)
            {
                x = xRadius + (xRadius + StrokeThickness / 6) * Math.Cos(radAngle);
                y = yRadius - (yRadius + StrokeThickness / 6) * Math.Sin(radAngle);
            }
            else
            {
                x = xRadius + (xRadius - StrokeThickness / 6) * Math.Cos(radAngle);
                y = yRadius - (yRadius - StrokeThickness / 6) * Math.Sin(radAngle);
            }
            
            return new Point(x, y);
        }
        #endregion

        #region Overriding Measure and Arrange methods
        //Overriding the Measure and Arrange functions for Arc class.
        protected override Size MeasureOverride(Size availableSize)
        {
            Size desiredSize = base.MeasureOverride(availableSize);
            return desiredSize;
        }
        protected override Size ArrangeOverride(Size finalSize)
        {
            base.ArrangeOverride(finalSize);
            return finalSize;
        }


        //Properties related to the Writing on Shape ( Text on Lighting)
        private static readonly DependencyPropertyKey MidPathFigureKey =
             DependencyProperty.RegisterReadOnly("MidPathFigure",
                                                 typeof(PathFigure),
                                                 typeof(Arc),
                                                 new FrameworkPropertyMetadata(null));

        public static readonly DependencyProperty MidPathFigureProperty =
            MidPathFigureKey.DependencyProperty;

        public PathFigure MidPathFigure
        {
            protected set { SetValue(MidPathFigureKey, value); }
            get { return (PathFigure)GetValue(MidPathFigureProperty); }
        }

        public static readonly DependencyProperty TextProperty = DependencyProperty.RegisterAttached("Text", typeof(string), typeof(Arc), new FrameworkPropertyMetadata(string.Empty, FrameworkPropertyMetadataOptions.AffectsRender));
        public static readonly DependencyProperty ForegroundProperty = DependencyProperty.RegisterAttached("Foreground", typeof(Brush), typeof(Arc), new FrameworkPropertyMetadata(Brushes.Black, FrameworkPropertyMetadataOptions.AffectsRender));
        public static readonly DependencyProperty FontSizeProperty = DependencyProperty.Register("FontSize", typeof(double), typeof(Arc), new FrameworkPropertyMetadata((double)10.0, FrameworkPropertyMetadataOptions.AffectsRender));
        public string Text
        {
            get { return (string)(GetValue(TextProperty)); }
            set { SetValue(TextProperty, value); }
        }
        public Brush Foreground
        {
            get { return (Brush)(GetValue(ForegroundProperty)); }
            set { SetValue(ForegroundProperty, value); }
        }
        public double FontSize
        {
            get { return (double)(GetValue(FontSizeProperty)); }
            set { SetValue(FontSizeProperty, value); }
        }
        public double PseudoEndAngle { private get; set; }

        #endregion
    }
}