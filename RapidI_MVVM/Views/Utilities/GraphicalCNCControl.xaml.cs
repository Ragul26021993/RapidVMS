using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for GraphicalCNCControl.xaml
    /// </summary>
    public partial class GraphicalCNCControl : UserControl
	{
        RadialGradientBrush fillBrush;
        bool IsLoadingCompleted = false;

        public double AngleFromCenterClockWise { get; set; }
        public double RadialDistanceFromCenter { get; set; }
		public GraphicalCNCControl()
		{
			this.InitializeComponent();
            fillBrush = new RadialGradientBrush();
            fillBrush.MappingMode = BrushMappingMode.Absolute;
            GradientStopCollection gr = new GradientStopCollection(3);
            gr.Add(new GradientStop(Color.FromArgb(255,20,255,0),0));
            gr.Add(new GradientStop(Color.FromArgb(255, 247, 255, 107), 0.65));
            gr.Add(new GradientStop(Color.FromArgb(255, 255, 86, 48), 1));
            fillBrush.GradientStops = gr;
            this.Loaded += new RoutedEventHandler(GraphicalCNCControl_Loaded);
		}

        void GraphicalCNCControl_Loaded(object sender, RoutedEventArgs e)
        {
            if (!IsLoadingCompleted)
            {
                this.CurrentSelectedArea.Fill = fillBrush;
                this.btnBase.Click += new RoutedEventHandler(btnBase_Click);
                this.btnBase.PreviewMouseLeftButtonUp += new MouseButtonEventHandler(btnBase_PreviewMouseLeftButtonUp);
                IsLoadingCompleted = true;
            }
        }

        void btnBase_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            this.CurrentSelectedArea.Visibility = System.Windows.Visibility.Hidden;
        }

        void btnBase_Click(object sender, RoutedEventArgs e)
        {
            //get the current mouse position
            CalculateXYPositions(Mouse.GetPosition(btnBase));
            this.CurrentSelectedArea.Width = RadialDistanceFromCenter;
            this.CurrentSelectedArea.Height = RadialDistanceFromCenter;
            this.CurrentSelectedArea.Visibility = System.Windows.Visibility.Visible;
        }

        protected override Size ArrangeOverride(Size arrangeBounds)
        {
            fillBrush.Center = new Point(arrangeBounds.Width / 2, arrangeBounds.Height / 2);
            fillBrush.RadiusX = fillBrush.Center.X;
            fillBrush.RadiusY = fillBrush.Center.Y;
            return base.ArrangeOverride(arrangeBounds);
        }

        void CalculateXYPositions(Point CurrentPos)
        {
            Point CenterCoOrdinates = new Point(this.DesiredSize.Width / 2, this.DesiredSize.Height / 2);
            //Modify the current position to relate from centre co-ordinates
            CurrentPos.X = CurrentPos.X - CenterCoOrdinates.X;
            CurrentPos.Y = CenterCoOrdinates.Y - CurrentPos.Y;
            //Now find the slope of the line joining from centre to the current point
            double slope = (CurrentPos.Y) / (CurrentPos.X);
            AngleFromCenterClockWise = Math.Round(Math.Abs((Math.Atan(slope))) * (180 / Math.PI), 2);
            //find the radius of the inscribed circle 
            RadialDistanceFromCenter = Math.Sqrt(Math.Pow(CurrentPos.X, 2) + Math.Pow(CurrentPos.Y, 2));
            if (CurrentPos.X < 0 && CurrentPos.Y > 0)
                AngleFromCenterClockWise = 180 - AngleFromCenterClockWise;
            else if (CurrentPos.X < 0 && CurrentPos.Y < 0)
                AngleFromCenterClockWise = 180 + AngleFromCenterClockWise;
            else if (CurrentPos.X > 0 && CurrentPos.Y < 0)
                AngleFromCenterClockWise = 360 - AngleFromCenterClockWise;
        }
	}
}