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

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ImageSuperImpose.xaml
    /// </summary>
    public partial class ImageSuperImpose : Window
    {
        bool TwoPointTaken = false;
        bool FirstPointTaken = false;  
        static ImageSuperImpose ImageOverlapWindw = null;
        private RoutedEventHandler WinClosing;
        System.String folderpath = "D:\\SuperImposeImage";
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            this.Hide();
            e.Cancel = true;
        }
        public static ImageSuperImpose MyInstance()
        {
            if (ImageOverlapWindw == null)
            {
                ImageOverlapWindw = new ImageSuperImpose();
            }
            return ImageOverlapWindw;
        }
        public ImageSuperImpose()
        {
            InitializeComponent();
            this.Loaded +=new RoutedEventHandler(ImageSuperImpose_Loaded);           
        }

        void ImageSuperImpose_Loaded(object sender, RoutedEventArgs e)
        {
            //initializing OpenGlWindow
            RWrapper.RW_MainInterface.MYINSTANCE().IntializeOpengl(RapidCadHost.GetHandle, RapidCadHost.GetOglHost().Width, RapidCadHost.GetOglHost().Height, 3, 800, 600);
            RapidCadHost.GetOglHost().MouseEnter += new EventHandler(RapidCadHost_MouseEnter);
            RapidCadHost.GetOglHost().MouseLeave += new EventHandler(RapidCadHost_MouseLeave);
            RapidCadHost.GetOglHost().MouseDown += new System.Windows.Forms.MouseEventHandler(RapidCadHost_MouseDown);
            RapidCadHost.GetOglHost().MouseUp += new System.Windows.Forms.MouseEventHandler(RapidCadHost_MouseUp);
            RapidCadHost.GetOglHost().MouseMove += new System.Windows.Forms.MouseEventHandler(RapidCadHost_MouseMove);
            RapidCadHost.GetOglHost().MouseWheel += new System.Windows.Forms.MouseEventHandler(RapidCadHost_MouseWheel);
            ContainerPanel.SizeChanged += new SizeChangedEventHandler(ContainerPanel_SizeChanged);
            RWrapper.RW_OverlapImgWin.MyInstance().UpdateGraphics();
            RWrapper.RW_SuperImposeImage.MYINSTANCE().SetFolderPath(folderpath);
        }

        void RapidCadHost_MouseEnter(object sender, EventArgs e)
        {
            try
            {
                if ((System.Windows.Forms.Control.MouseButtons & System.Windows.Forms.MouseButtons.Middle) == System.Windows.Forms.MouseButtons.Middle)
                {
                    Point _Pos = this.PointFromScreen(new Point(System.Windows.Forms.Control.MousePosition.X, System.Windows.Forms.Control.MousePosition.Y));
                    // RWrapper.RW_MainInterface.MYINSTANCE().OnRCadMiddleMouseDown(_Pos.X, _Pos.Y);
                }
                RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseEnter();
                this.RapidCadHost.Focus();
                RapidCadHost.FormsHostPanel_GotFocus(sender, null);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25", ex);
            }
        }
        void RapidCadHost_MouseLeave(object sender, EventArgs e)
        {
            try
            {
                RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseLeave();
                this.Focus();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25", ex);
            }
        }
        void RapidCadHost_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            try
            {
                if (e.Button == System.Windows.Forms.MouseButtons.Left)
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinLeftMouseDown(e.X, e.Y);
                else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMiddleMouseDown(e.X, e.Y);
                else if (e.Button == System.Windows.Forms.MouseButtons.Right)
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinRightMouseDown(e.X, e.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV27", ex);
            }
        }
        void RapidCadHost_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            try
            {
                if (e.Button == System.Windows.Forms.MouseButtons.Left)
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinLeftMouseUp(e.X, e.Y);
                else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMiddleMouseUp(e.X, e.Y);
                else if (e.Button == System.Windows.Forms.MouseButtons.Right)
                    RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinRightMouseUp(e.X, e.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV28", ex);
            }
        }
        void RapidCadHost_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            try
            {
                RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseMove(e.X, e.Y);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV29", ex);
            }
        }
        void RapidCadHost_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            try
            {
                RWrapper.RW_OverlapImgWin.MyInstance().OnOverlapImgWinMouseWheel(e.Delta);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV26", ex);
            }
        }
        void ContainerPanel_SizeChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                RapidCadHost.Width = ContainerPanel.ActualWidth;
                RapidCadHost.Height = ContainerPanel.ActualHeight;
                //RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(1, RapidCadHost.GetOglHost().Width, RapidCadHost.GetOglHost().Height);
                //RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV26", ex);
            }
        }
        protected void BtnTakeLeftTop_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (FirstPointTaken)
                {
                    TwoPointTaken = true;
                }
                else
                {
                    FirstPointTaken = true;
                }
                RWrapper.RW_SuperImposeImage.MYINSTANCE().SetTopLeft(0, 0);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ImS02", ex);
            }
        }
        protected void BtnTakeBottomRight_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (FirstPointTaken)
                {
                    TwoPointTaken = true;
                }
                else
                {
                    FirstPointTaken = true;
                }
                RWrapper.RW_SuperImposeImage.MYINSTANCE().SetBottomRight(0, 0);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ImS03", ex);
            }
        }
        protected void BtnTakeImage_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (TwoPointTaken)
                {
                    RWrapper.RW_SuperImposeImage.MYINSTANCE().SetFolderPath(folderpath);
                    RWrapper.RW_SuperImposeImage.MYINSTANCE().InitialiseGrid();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:ImS04", ex);
            }
        }
        bool CreateFolderIFNotAlreadyExist(string path)
        {
            try
            {
                if (System.IO.Directory.Exists(path))
                {
                    return true;
                }
                else
                {
                    System.IO.Directory.CreateDirectory(path);
                }
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return false;
            }
        }
    }
}
