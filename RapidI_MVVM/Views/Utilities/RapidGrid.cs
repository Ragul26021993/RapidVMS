using System;
using System.Windows.Controls;
using System.Windows;

namespace Rapid.Utilities
{
    public class RapidGrid : Grid
    {        
        public event RoutedEventHandler ValueChange;
        public static readonly DependencyProperty TotalColumnsProperty = DependencyProperty.Register("TotalColumns", typeof(int), typeof(RapidGrid),
                                            new FrameworkPropertyMetadata(20, new PropertyChangedCallback(HandleValueChange)));
        public static readonly DependencyProperty TotalRowsProperty = DependencyProperty.Register("TotalRows", typeof(int), typeof(RapidGrid),
                                            new FrameworkPropertyMetadata(15, new PropertyChangedCallback(HandleValueChange)));
        public int TotalColumns
        {
            get { return (int)GetValue(TotalColumnsProperty); }
            set { SetValue(TotalColumnsProperty, value); }
        }
        public int TotalRows
        {
            get { return (int)GetValue(TotalRowsProperty); }
            set { SetValue(TotalRowsProperty, value); LoadingCompleted = false; FocusBoxGrid_Loaded(null, null); }
        }
        public int GapBetweenCells
        {
            get
            {
                return _GapBetweenCells;
            }
            set
            {
                _GapBetweenCells = value;
            }
        }

        private static void HandleValueChange(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            RapidGrid U = (RapidGrid)d;
            U.LoadingCompleted = false;
            U.FocusBoxGrid_Loaded(null, null);
        }

        int _GapBetweenCells = 2;
        public bool MouseDragAllowed = false;
        public int NoOfSelectedRows { get; private set; }
        public int NoOfSelectedColumns { get; private set; }
        private bool DisableMouseMove { get; set; }
        bool LoadingCompleted;
        double UnitWidth, UnitHeight;

        public RapidGrid()
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

                    double scaleY = (double)(this.RenderSize.Width - GapBetweenCells * TotalColumns) / TotalColumns;
                    double scaleX = (double)(this.RenderSize.Height - GapBetweenCells * TotalRows) / TotalRows;
                    double scale = scaleY < scaleX ? scaleY : scaleX;

                    int HorizCount = 0;
                    UnitWidth = scale;
                    UnitHeight = UnitWidth;

                    double gridwidth = (TotalColumns * scale) + GapBetweenCells * TotalColumns;
                    double gridheight = (TotalRows * scale) + GapBetweenCells * TotalRows;

                    double NoOfPxPerBoxHorizontally = (int)gridwidth / TotalColumns;
                    double NOofPxPerBoxVertically = (int)gridheight / TotalRows;

                    while (HorizCount < TotalRows)
                    {
                        int VeriCount = 0;
                        while (VeriCount < TotalColumns)
                        {
                            System.Windows.Shapes.Rectangle rect = new System.Windows.Shapes.Rectangle();
                            rect.Stroke = System.Windows.Media.Brushes.Black;
                            rect.StrokeThickness = 2;
                            rect.Fill = System.Windows.Media.Brushes.White;
                            rect.Width = UnitWidth;
                            rect.Height = UnitHeight;
                            rect.Margin = new Thickness(VeriCount * NoOfPxPerBoxHorizontally, HorizCount * NOofPxPerBoxVertically, 0, 0);
                            rect.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                            rect.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                            rect.Tag = new RectTagproperty(HorizCount, VeriCount);
                            rect.MouseEnter += new System.Windows.Input.MouseEventHandler(rect_MouseEnter);
                            rect.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(rect_MouseLeftButtonDown);
                            VeriCount += 1;
                            this.Children.Add(rect);
                        }
                        HorizCount += 1;
                    }
                    SelectAll();
                    rect_MouseEnter(this.Children[0], null);
                }
                LoadingCompleted = true;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:RG87", ex); }
        }

        void rect_MouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            try
            {
                DisableMouseMove = DisableMouseMove ? false : true;
                NoOfSelectedRows = 0;
                NoOfSelectedColumns = 0;
                int NoOfPxPerBoxHorizontally = (int)this.RenderSize.Width / TotalColumns;
                int NOofPxPerBoxVertically = (int)this.RenderSize.Height / TotalRows;
                System.Windows.Shapes.Rectangle rect = (System.Windows.Shapes.Rectangle)sender;
                RectTagproperty CRect = (RectTagproperty)rect.Tag;
                Point p = CRect.position;
                NoOfSelectedRows = (int)p.Y + 1;
                NoOfSelectedColumns = (int)p.X + 1;
                if (CRect.isSelected)
                {
                    CRect.isSelected = false;
                    rect.Fill = System.Windows.Media.Brushes.White;
                }
                else
                {
                    CRect.isSelected = true;
                    rect.Fill = System.Windows.Media.Brushes.Plum;
                }
                //rect_MouseEnter(sender, null);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:RGrid07", ex); }
        }

        void rect_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                if (!DisableMouseMove && MouseDragAllowed)
                {
                    NoOfSelectedRows = 0;
                    NoOfSelectedColumns = 0;
                    int NoOfPxPerBoxHorizontally = (int)this.RenderSize.Width / TotalColumns;
                    int NOofPxPerBoxVertically = (int)this.RenderSize.Height / TotalRows;
                    System.Windows.Shapes.Rectangle rect = (System.Windows.Shapes.Rectangle)sender;
                    RectTagproperty CRect = (RectTagproperty)rect.Tag;
                    Point p = CRect.position;
                    NoOfSelectedRows = (int)p.Y + 1;
                    NoOfSelectedColumns = (int)p.X + 1;
                    for (int i = 0; i < this.Children.Count; i++)
                    {
                        System.Windows.Shapes.Rectangle R = (System.Windows.Shapes.Rectangle)this.Children[i];
                        RectTagproperty CuttentRectangle = (RectTagproperty)R.Tag;
                        Point pp = CuttentRectangle.position;
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
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:RGrid08", ex); }
        }

        public void ResetConditions()
        {
            try
            {
                if (this.Children.Count >= 1)
                    rect_MouseEnter(this.Children[0], null);
                DisableMouseMove = false;
                ClearSelection();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:RGrid09", ex); }
        }
        protected void ClearSelection()
        {
            for (int i = 0; i < this.Children.Count; i++)
            {
                System.Windows.Shapes.Rectangle R = (System.Windows.Shapes.Rectangle)this.Children[i];
                RectTagproperty CuttentRectangle = (RectTagproperty)R.Tag;
                Point pp = CuttentRectangle.position;
                R.Fill = System.Windows.Media.Brushes.White;
                CuttentRectangle.isSelected = false;
            }
        }

        public void SelectAll()
        {
            try
            {
                for (int i = 0; i < this.Children.Count; i++)
                {
                    System.Windows.Shapes.Rectangle R = (System.Windows.Shapes.Rectangle)this.Children[i];
                    RectTagproperty CuttentRectangle = (RectTagproperty)R.Tag;
                    Point pp = CuttentRectangle.position;
                    R.Fill = System.Windows.Media.Brushes.Plum;
                    CuttentRectangle.isSelected = true;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:RGrid10", ex);// throw;
            }
        }
    }
    public class RectTagproperty
    {
        public bool isSelected = false;
        public Point position = new Point();
        public RectTagproperty(double XPosition = 0, double YPosition = 0)
        {
            position.X = XPosition;
            position.Y = YPosition;
        }
    }
}
