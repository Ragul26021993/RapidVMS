using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using RapidI.Help;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for PictureViewer.xaml
    /// </summary>
    public partial class PictureViewer : Window
    {
        //For singleton class application
        double ViewPortWidth, ViewPortHeight; //, RenderHeight, RenderWidth;
        Point FirstMouseDownPosition;
        private static PictureViewer _instance = null;
        public static PictureViewer GetInstance
        {
            get
            {
                if (_instance == null)
                    _instance = new PictureViewer();
                return _instance;
            }
        }
        //private Constructor for picture viewer.
        private PictureViewer()
        {
            InitializeComponent();
            OpenButton.Tag=new string[]{"Picture Viewer" ,"Open"};
            btnReset.Tag = new string[] { "Picture Viewer", "Reset" };
            btnReset.PreviewMouseRightButtonDown += new MouseButtonEventHandler(OpenButton_PreviewMouseRightButtonDown);
            OpenButton.PreviewMouseRightButtonDown +=new MouseButtonEventHandler(OpenButton_PreviewMouseRightButtonDown);
            //handling the Mouse related events for Image to Pan & Zoom.
            ImgObject.PreviewMouseDown += new MouseButtonEventHandler(ImgObject_PreviewMouseLeftButtonDown);
            ImgObject.PreviewMouseMove += new MouseEventHandler(ImgObject_PreviewMouseMove);
            ImgObject.PreviewMouseUp += new MouseButtonEventHandler(ImgObject_PreviewMouseLeftButtonUp);
            ImgObject.PreviewMouseWheel += new MouseWheelEventHandler(ImgObject_PreviewMouseWheel);
        }
        //Display Help On Rigth Click 
        public void OpenButton_PreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                string BtnName = tagStr[1];
                string BtnLocationName = tagStr[0];
                if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
                    HelpWindow.GetObj().Show();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07a", ex); }
        }
        //handling the Zoom of image.
        void ImgObject_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
        {
            UIElement element = (UIElement)sender;
            MatrixTransform xform = element.RenderTransform as MatrixTransform;
            Matrix matrix = xform.Matrix;
            Point center = e.GetPosition(ImgObject);
            matrix.Translate(-center.X, -center.Y);
            if (e.Delta >= 0)
            {
                matrix.Scale(1.2, 1.2);
            }
            else
            {
                matrix.Scale(0.8, 0.8);
            }
            matrix.Translate(center.X, center.Y);
            xform.Matrix = matrix;
            e.Handled = true;
        }
        //handling the panning
        void ImgObject_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            FirstMouseDownPosition = e.GetPosition(ImgObject);
            ImgObject.CaptureMouse();
        }
        void ImgObject_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (!ImgObject.IsMouseCaptured) return;
            //Check panning conditions.Pan only when it is zoomed beyond the visible limits of windows.
            //if (RenderWidth > ViewPortWidth || RenderHeight > ViewPortHeight)
            //{
            //    double Xdiff = (RenderWidth - ViewPortWidth)/2;
            //    double Ydiff = (RenderHeight - ViewPortHeight)/2;
            //    //get the instance of the Translate transform
            //    TranslateTransform MyT = (TranslateTransform)((TransformGroup)ImgObject.RenderTransform).Children.First(T => T is TranslateTransform);
            //    Point CurrentPoint = e.GetPosition(ImgObject);
            //    bool DoTranslate = false;
            //    if (MyT.X <= Xdiff && Math.Abs((CurrentPoint.X - FirstMouseDownPosition.X)) > 1.0)
            //        DoTranslate=true;
            //    else if (MyT.Y <= Ydiff && Math.Abs((CurrentPoint.Y- FirstMouseDownPosition.Y)) > 1.0)
            //        DoTranslate=true;
            //    else if(MyT.X < 0)
            //    {
            //        if (MyT.X >= -1 * Xdiff && Math.Abs((CurrentPoint.X - FirstMouseDownPosition.X)) > 1.0)
            //            DoTranslate=true;
            //    }
            //    else if(MyT.Y < 0)
            //     {
            //         if (MyT.Y >= -1 * Ydiff && Math.Abs((CurrentPoint.X - FirstMouseDownPosition.X)) > 1.0)
            //             DoTranslate = true;
            //     }
            //        if(DoTranslate)
            //        {
            //            MyT.X += CurrentPoint.X - FirstMouseDownPosition.X;
            //            MyT.Y += CurrentPoint.Y - FirstMouseDownPosition.Y;
            //        }
            //}

            Point CurrentPoint = e.GetPosition(ImgObject);
            MatrixTransform xform = ImgObject.RenderTransform as MatrixTransform;
            Matrix matrix = xform.Matrix;
            matrix.Translate(CurrentPoint.X - FirstMouseDownPosition.X, CurrentPoint.Y - FirstMouseDownPosition.Y);
            FirstMouseDownPosition = CurrentPoint;
            xform.Matrix = matrix;
        }
        void ImgObject_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            ImgObject.ReleaseMouseCapture();
        }

        //handling the opening of picture & closing of this window & best fit/reset functions as well.
        private void OpenButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            //TODO: Add event handler implementation here.
            System.Windows.Forms.OpenFileDialog OpenDialog = new System.Windows.Forms.OpenFileDialog();
            OpenDialog.Filter = "Image Files (*.jpeg)|*.jpg|(*.TIFF)|*.tiff|(*.BMP)|*.bmp|(*.GIF)|*.gif|All Files (*.*)|*.*";
            OpenDialog.Title = "Open Image File";
            OpenDialog.ShowDialog();
            if (OpenDialog.FileName.Length != 0)
            {
                //BitmapImage newImage = new BitmapImage();
                //newImage.BeginInit();
                //newImage.UriSource = new Uri(OpenDialog.FileName, UriKind.RelativeOrAbsolute);
                //newImage.EndInit();
                //ImgObject.Source = newImage;
                ShowImageFile(OpenDialog.FileName);
            }
            BestFit();
        }
        public void ShowImageFile(string FileName)
        {
            BitmapImage newImage = new BitmapImage();
            newImage.BeginInit();
            newImage.UriSource = new Uri(FileName, UriKind.RelativeOrAbsolute);
            newImage.EndInit();
            ImgObject.Source = newImage;
        }
        private void BestFitButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            if (sender.Equals(btnActualSize))
            {
                BestFit();
            }
            else
            {
                UnloadImage();
            }
        }
        private void BestFit()
        {
            //get instances of the transforms & make them to their default value.
            //ScaleTransform myScale = (ScaleTransform)((TransformGroup)ImgObject.RenderTransform).Children.First(Tr => Tr is ScaleTransform);
            //TranslateTransform MyT = (TranslateTransform)((TransformGroup)ImgObject.RenderTransform).Children.First(T => T is TranslateTransform);
            //myScale.ScaleX = 1;
            //myScale.ScaleY = 1;
            //MyT.X = 0;
            //MyT.Y = 0;

            MatrixTransform xform = ImgObject.RenderTransform as MatrixTransform;
            Matrix matrix = xform.Matrix;
            matrix.SetIdentity();
            xform.Matrix = matrix;
        }
        private void UnloadImage()
        {
            ImgObject.Source = null;
            GC.Collect();
            System.Threading.Thread.Sleep(10);
            GC.WaitForPendingFinalizers();
            System.Threading.Thread.Sleep(10);
            System.Windows.Forms.Application.DoEvents();
            System.Threading.Thread.Sleep(50);
            GC.Collect();
            System.Threading.Thread.Sleep(10);
            GC.WaitForPendingFinalizers();
            System.Threading.Thread.Sleep(10);
            System.Windows.Forms.Application.DoEvents();
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            //_instance = null;
            this.Hide();
            e.Cancel = true;
        }

        private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            ViewPortWidth = e.NewSize.Width;
            ViewPortHeight = e.NewSize.Height - 50;
        }

        protected override void OnManipulationStarting(ManipulationStartingEventArgs args)
        {
            args.ManipulationContainer = this;
            args.Mode = ManipulationModes.All;

            //// Adjust Z-order
            //FrameworkElement element = args.Source as FrameworkElement;
            //Panel pnl = element.Parent as Panel;

            //for (int i = 0; i < pnl.Children.Count; i++)
            //    Panel.SetZIndex(pnl.Children[i],
            //        pnl.Children[i] == element ? pnl.Children.Count : i);

            args.Handled = true;
            base.OnManipulationStarting(args);
        }
        protected override void OnManipulationDelta(ManipulationDeltaEventArgs args)
        {
            UIElement element = args.Source as UIElement;
            MatrixTransform xform = element.RenderTransform as MatrixTransform;
            Matrix matrix = xform.Matrix;
            ManipulationDelta delta = args.DeltaManipulation;
            Point center = args.ManipulationOrigin;
            matrix.Translate(-center.X, -center.Y);
            matrix.Scale(delta.Scale.X, delta.Scale.Y);
            matrix.Rotate(delta.Rotation);
            matrix.Translate(center.X, center.Y);
            matrix.Translate(delta.Translation.X, delta.Translation.Y);
            xform.Matrix = matrix;

            args.Handled = true;
            base.OnManipulationDelta(args);
        }

        public void ClearAll()
        {
            UnloadImage();
        }
    }
}



