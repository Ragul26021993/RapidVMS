using System;
using System.Windows.Controls;
using System.Windows.Input;
//using RapidI.Utilities;
//using RapidI.DGItemEntities;
//using RapidI.CustomWindows;
//using RapidI.REnums;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for ShapeStatusWin.xaml
    /// </summary>
    public partial class VideoPanel : Grid
	{
		double Videowidth = 0, Videoheight = 0;// VideoLeft = 0, VideoTop = 0;

		public VideoPanel()
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
			//VideoHost.SetWinSize(800, 600);
			//VideoHost.SetImageViewerSize(800, 600);           
		}

		//[return: MarshalAs(UnmanagedType.Bool)]
		//[DllImport("user32.dll")]
		//internal static extern bool GetClientRect(IntPtr hwnd, ref RECT lpRect);

		//[StructLayout(LayoutKind.Sequential)]
		//internal struct RECT
		//{
		//    internal int left;
		//    internal int top;
		//    internal int right;
		//    internal int bottom;
		//}

		public void HookupEventsAndInitialise(int DisplayWidth, int DisplayHeight, int VideoWidth, int VideoHeight, bool CameraIsOn)
		{
			this.Width = DisplayWidth; this.Height = DisplayHeight;
			//VideoHost.Width = DisplayWidth; VideoHost.Height = DisplayHeight;
			VideoHost.SetSize(DisplayWidth, DisplayHeight);
            //double x = VideoHost.pictureBox.Width, y = VideoHost.pictureBox.Height;
            //RECT rect = new RECT();
            //GetClientRect(VideoHost.GetHandle, ref rect);
            if (!CameraIsOn) return;
			System.Threading.Thread.Sleep(1000); // 
			bool ConnectedVideo = RWrapper.RW_MainInterface.MYINSTANCE().IntializeOpengl(VideoHost.GetHandle, DisplayWidth, DisplayHeight, 0, VideoWidth, VideoHeight);
			//if (RapidCam.Initialise(VideoHost.Handle, 1, 102))
			//{
			//    if (RapidCam.SetVideoWindow(DisplayWidth, DisplayHeight, 20, true, 0, 0, 0))
			//        RapidCam.Preview();
			//}
			VideoHost.GetOglHost().MouseEnter += new EventHandler(VideoHost_MouseEnter);
			VideoHost.GetOglHost().MouseLeave += new EventHandler(VideoHost_MouseLeave);
			VideoHost.GetOglHost().MouseDown += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseDown);
			VideoHost.GetOglHost().MouseUp += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseUp);
			VideoHost.GetOglHost().MouseMove += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseMove);
			VideoHost.GetOglHost().MouseWheel += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseWheel);
            //VideoHost.KeyDown += new KeyEventHandler(VideoHost_KeyEventHandler);

            this.MouseEnter += VideoPanel_MouseEnter1;
            this.MouseLeave += VideoPanel_MouseLeave;
            //this.MouseDown += VideoPanel_MouseDown;
            this.MouseWheel += VideoPanel_MouseWheel;
            //this.MouseEnter += new EventHandler(VideoHost_MouseEnter);
            //this.MouseLeave += new EventHandler(VideoHost_MouseLeave);
            //this.MouseDown += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseDown);
            //this.MouseUp += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseUp);
            //this.MouseMove += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseMove);
            //this.MouseWheel += new System.Windows.Forms.MouseEventHandler(VideoHost_MouseWheel);
            if (System.Environment.Is64BitOperatingSystem)
			{
				//RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(3, GlobalSettings.Ch_12_Value);
				//RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(9, GlobalSettings.Ch_34_Value);
			}
			RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
		}

        private void VideoPanel_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseWheel(e.Delta);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25a", ex);
            }
            //throw new NotImplementedException();
        }

        //private void VideoPanel_MouseDown(object sender, MouseButtonEventArgs e)
        //{
        //    try
        //    {
        //        if (e.LeftButton == MouseButtonState.Pressed)
        //            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseDown(e.GetPosition(this).X, e.GetPosition(this).Y);
        //        else if (e.MiddleButton == MouseButtonState.Pressed)
        //            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseDown(e.GetPosition(this).X, e.GetPosition(this).Y);
        //        else if (e.RightButton == MouseButtonState.Pressed)
        //            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseDown(e.GetPosition(this).X, e.GetPosition(this).Y);
        //    }
        //    catch (Exception ex)
        //    {
        //        //throw new NotImplementedException();
        //        RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25a", ex);
        //    }
        //}

        private void VideoPanel_MouseLeave(object sender, MouseEventArgs e)
        {
            try
            {
                VideoHost_MouseLeave(sender, (EventArgs)e);
            }
            catch (Exception ex)
            {
                //throw new NotImplementedException();
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25a", ex);
            }
        }
        private void VideoPanel_MouseEnter1(object sender, MouseEventArgs e)
        {
            try
            {
                VideoHost_MouseEnter(sender, (EventArgs)e);
            }
            catch (Exception ex)
            {
                //throw new NotImplementedException();
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25a", ex);
            }
        }

		void VideoHost_MouseEnter(object sender, EventArgs e)
		{
			try
			{
				RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseEnter();
				VideoHost.Focus();
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25", ex);
			}
		}

		void VideoHost_MouseLeave(object sender, EventArgs e)
		{
			try
			{
				RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseLeave();
				this.Focus();
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV25", ex);
			}
		}

		void VideoHost_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			try
			{
				if (e.Button == System.Windows.Forms.MouseButtons.Left)
					RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseDown(e.X, e.Y);
				else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
					RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseDown(e.X, e.Y);
				else if (e.Button == System.Windows.Forms.MouseButtons.Right)
					RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseDown(e.X, e.Y);
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV27", ex);
			}
		}

		void VideoHost_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			try
			{
				if (e.Button == System.Windows.Forms.MouseButtons.Left)
					RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseUp(e.X, e.Y);
				else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
					RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseUp(e.X, e.Y);
				else if (e.Button == System.Windows.Forms.MouseButtons.Right)
					RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseUp(e.X, e.Y);
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV28", ex);
			}
		}

		void VideoHost_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			try
			{
				RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseMove(e.X, e.Y);
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV29", ex);
			}
		}

		void VideoHost_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			try
			{
				RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseWheel(e.Delta);
			}
			catch (Exception ex)
			{
				RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV26", ex);
			}
		}

		public void DigitalZoomOn()
		{
			if (!GlobalSettings.DoubleImageEnabled_During_DigiZoom) return;
			//VideoHost.SetImageViewerSize((int)(Videowidth * 2), (int)(Videoheight * 2));
			RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(0, (int)(Videowidth * 2), (int)(Videoheight * 2));
			RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
		}

		public void DigitalZoomOff()
		{
			if (!GlobalSettings.DoubleImageEnabled_During_DigiZoom) return;
			//VideoHost.SetImageViewerSize((int)(Videowidth), (int)(Videoheight));
			RWrapper.RW_MainInterface.MYINSTANCE().ResizetheWindow(0, (int)(Videowidth), (int)(Videoheight));
			RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
		}

        //void VideoHost_KeyEventHandler(object sender, System.Windows.Input.KeyEventArgs e)
        //{
        //    RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyDown(e);
        //}

    }
}