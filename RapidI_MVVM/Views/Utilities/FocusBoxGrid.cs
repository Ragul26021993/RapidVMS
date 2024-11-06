using System;
using System.Windows.Controls;
using System.Windows;

namespace Rapid.Utilities
{
    class FocusBoxGrid : Grid
    {
        public event RoutedEventHandler ValueChange;
        public static readonly DependencyProperty TotalColumnsProperty = DependencyProperty.Register("TotalColumns", typeof(int), typeof(FocusBoxGrid),
                                            new FrameworkPropertyMetadata(20, FrameworkPropertyMetadataOptions.AffectsRender));
        public static readonly DependencyProperty TotalRowsProperty = DependencyProperty.Register("TotalRows", typeof(int), typeof(FocusBoxGrid),
                                            new FrameworkPropertyMetadata(15, FrameworkPropertyMetadataOptions.AffectsRender));
        public int TotalColumns
        {
            get { return (int)GetValue(TotalColumnsProperty); }
            set { SetValue(TotalColumnsProperty, value); }
        }
        public int TotalRows
        {
            get { return (int)GetValue(TotalRowsProperty); }
            set { SetValue(TotalRowsProperty, value); }
        }
        public int NoOfSelectedRows { get; private set; }
        public int NoOfSelectedColumns { get; private set; }
        private bool DisableMouseMove { get; set; }
        bool LoadingCompleted;
        double UnitWidth, UnitHeight;

        public FocusBoxGrid() 
        {
            this.Loaded += new RoutedEventHandler(FocusBoxGrid_Loaded);
            DisableMouseMove = true;
        }

        void FocusBoxGrid_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                if (!LoadingCompleted)
                {
                    this.Children.Clear();
                    if (this.RenderSize.Width == 0 && this.RenderSize.Height == 0) return;

                    double SquareLength = this.RenderSize.Width >= this.RenderSize.Height ? this.RenderSize.Height : this.RenderSize.Width;
                    //double SquareHeight = this.RenderSize.Height;// >= this.RenderSize.Height ? this.RenderSize.Height : this.RenderSize.Width;
                    double NoOfPxPerBoxHorizontally = (SquareLength + this.Margin.Left * 2) / TotalColumns;
                    double NOofPxPerBoxVertically = (SquareLength + this.Margin.Top * 2) / TotalRows;
                    //double NoOfPxPerBoxHorizontally = this.RenderSize.Width / TotalColumns;
                    //double NOofPxPerBoxVertically = this.RenderSize.Height / TotalRows;
                    int HorizCount = 0;
                    UnitWidth = NoOfPxPerBoxHorizontally; //- 2;
                    UnitHeight = UnitWidth; //NOofPxPerBoxVertically - Math.Abs(NoOfPxPerBoxHorizontally - NOofPxPerBoxVertically);
                    //double UnitHeight = NOofPxPerBoxVertically - 2;
                    NoOfPxPerBoxHorizontally = (int)this.RenderSize.Width / TotalColumns;
                    NOofPxPerBoxVertically = (int)this.RenderSize.Height / TotalRows;
                    while (HorizCount < TotalColumns)
                    {
                        int VeriCount = 0;
                        while (VeriCount < TotalRows)
                        {
                            System.Windows.Shapes.Rectangle rect = new System.Windows.Shapes.Rectangle();
                            //rect.StrokeThickness = 0.5;
                            //rect.Stroke = System.Windows.Media.Brushes.Black;
                            rect.Fill = System.Windows.Media.Brushes.White;
                            rect.Width = UnitWidth;
                            rect.Height = UnitHeight;
                            rect.Margin = new Thickness(HorizCount * NoOfPxPerBoxHorizontally + 1, VeriCount * NOofPxPerBoxVertically + 1, 0, 0);
                            rect.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                            rect.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                            rect.Tag = new Point(HorizCount, VeriCount);
                            rect.MouseEnter += new System.Windows.Input.MouseEventHandler(rect_MouseEnter);
                            rect.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(rect_MouseLeftButtonDown);
                            VeriCount += 1;
                            this.Children.Add(rect);
                        }
                        HorizCount += 1;
                    }
                    //Seelct the first box always(default one focus box will always be present.
                    rect_MouseEnter(this.Children[0], null);
                }
                LoadingCompleted = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FBG01", ex); }
        }

        void rect_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            try
            {
                DisableMouseMove = DisableMouseMove ? false : true;
                rect_MouseEnter(sender, null);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FBG02", ex); }
        }

        void rect_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                if (!DisableMouseMove)
                {
                    NoOfSelectedRows = 0;
                    NoOfSelectedColumns = 0;
                    int NoOfPxPerBoxHorizontally = (int)this.RenderSize.Width / TotalColumns;
                    int NOofPxPerBoxVertically = (int)this.RenderSize.Height / TotalRows;
                    System.Windows.Shapes.Rectangle rect = (System.Windows.Shapes.Rectangle)sender;
                    Point p = (Point)rect.Tag;
                    NoOfSelectedRows = (int)p.Y + 1;
                    NoOfSelectedColumns = (int)p.X + 1;
                    for (int i = 0; i < this.Children.Count; i++)
                    {
                        System.Windows.Shapes.Rectangle R = (System.Windows.Shapes.Rectangle)this.Children[i];
                        Point pp = (Point)R.Tag;
                        if (pp.X <= p.X && pp.Y <= p.Y)
                        {
                            R.Fill = System.Windows.Media.Brushes.Plum;
                            //R.StrokeThickness = 1;
                            R.Width = UnitWidth + 1;
                            R.Height = UnitHeight + 1;
                            R.Margin = new Thickness(pp.X * NoOfPxPerBoxHorizontally + 0.5, pp.Y * NOofPxPerBoxVertically + 0.5, 0, 0);
                            //R.Stroke = System.Windows.Media.Brushes.LightBlue;
                        }
                        else
                        {
                            //R.Stroke = System.Windows.Media.Brushes.Black;
                            //R.StrokeThickness = 0.5;
                            R.Width = UnitWidth;
                            R.Height = UnitHeight;
                            R.Margin = new Thickness(pp.X * NoOfPxPerBoxHorizontally + 1, pp.Y * NOofPxPerBoxVertically + 1, 0, 0);
                            R.Fill = System.Windows.Media.Brushes.White;
                            //R.Margin = new Thickness(pp.X * NoOfPxPerBoxHorizontally + 0.5, pp.Y * NOofPxPerBoxVertically + 0.5, 0, 0);
                        }
                        if (ValueChange != null)
                            ValueChange(this, null);
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FBG03", ex); }
        }

        public void ResetConditions()
        {
            try
            {
                DisableMouseMove = false;
                if (this.Children.Count >= 1)
                    rect_MouseEnter(this.Children[0], null);
                DisableMouseMove = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FBG04", ex); }
        }
    }
}
