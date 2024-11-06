using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Animation;
using System.Windows.Media;
using System.Windows.Controls;

namespace Rapid.Windows
{
    /// <summary>
    /// This Windows serves as window for the open gl drawing surfaces.
    /// This has not to be shown in taskbar,Windowstyle will be none,cannot resize.
    /// </summary>
   public class OpenGlWinDouble : Window
    {

       private Window OPenGLLayerWin;
       private bool UpdateGraphicsForScroll = false;

       public OpenGlWinDouble()
            : base()
        {
            try
            {
                this.WindowStyle = WindowStyle.None;
                this.ShowInTaskbar = false;
                this.Effect = new System.Windows.Media.Effects.DropShadowEffect();
                this.ResizeMode = ResizeMode.NoResize;
                this.Background = System.Windows.Media.Brushes.LightBlue;
                OPenGLLayerWin = new Window();
                OPenGLLayerWin.WindowStyle = System.Windows.WindowStyle.None;
                OPenGLLayerWin.ShowInTaskbar = false;
                OPenGLLayerWin.ResizeMode = System.Windows.ResizeMode.NoResize;

                this.MouseEnter += new MouseEventHandler(FormsWin_MouseEnter);
                this.MouseLeave += new MouseEventHandler(FormsWin_MouseLeave);
                this.MouseDown += new MouseButtonEventHandler(FormsWin_MouseDown);
                this.MouseUp += new MouseButtonEventHandler(FormsWin_MouseUp);
                this.MouseMove += new MouseEventHandler(FormsWin_MouseMove);
                this.MouseWheel += new MouseWheelEventHandler(FormsWin_MouseWheel);
                this.KeyDown += new KeyEventHandler(FormsWin_KeyDown);
                this.KeyUp += new KeyEventHandler(FormsWin_KeyUp);

                OPenGLLayerWin.MouseEnter += new MouseEventHandler(FormsWin_MouseEnter);
                OPenGLLayerWin.MouseLeave += new MouseEventHandler(FormsWin_MouseLeave);
                OPenGLLayerWin.MouseDown += new MouseButtonEventHandler(FormsWin_MouseDown);
                OPenGLLayerWin.MouseUp += new MouseButtonEventHandler(FormsWin_MouseUp);
                OPenGLLayerWin.MouseMove += new MouseEventHandler(FormsWin_MouseMove);
                OPenGLLayerWin.MouseWheel += new MouseWheelEventHandler(FormsWin_MouseWheel);
                OPenGLLayerWin.KeyDown += new KeyEventHandler(FormsWin_KeyDown);
                OPenGLLayerWin.KeyUp += new KeyEventHandler(FormsWin_KeyUp);
            }
           catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MV03a", ex);
            }
        }
        /// <summary>
        /// This property allows  to set the StartPosition of this window.
        /// </summary>
        public Point StartPosition
        {
            get {return  new Point(this.Left, this.Top); }
            set
            {
                this.Top = value.Y;
                this.Left = value.X;
                OPenGLLayerWin.Left = value.X;
                OPenGLLayerWin.Top = value.Y;
            }
        }
        /// <summary>
        /// This provides the handle as IntPtr of this window.
        /// </summary>
        public IntPtr GetBaseHandle
        {
            get
            {
                System.Windows.Interop.WindowInteropHelper h = new System.Windows.Interop.WindowInteropHelper(this);
                return h.EnsureHandle();
            }
        }

        public IntPtr GetLayerHandle
        {
            get
            {
                System.Windows.Interop.WindowInteropHelper h = new System.Windows.Interop.WindowInteropHelper(OPenGLLayerWin);
                return h.EnsureHandle();
            }
        }

        public void SetWinSize(int width, int height)
        {
            this.Width = width; this.Height = height;
            OPenGLLayerWin.Width = width; OPenGLLayerWin.Height = height;
        }

        public void SetWinPosition(int X, int Y)
        {
            this.Left = X; this.Top = Y;
            OPenGLLayerWin.Left = X; OPenGLLayerWin.Top = Y;
        }

        public void ShowWin()
        {
            this.Opacity = 0;
            this.Show();
            OPenGLLayerWin.Show();
            OPenGLLayerWin.Owner = this;
            var anim = new DoubleAnimation(1, (Duration)TimeSpan.FromSeconds(0.1));
            anim.Completed += (s, _) => UpdateGraphics();
            this.BeginAnimation(UIElement.OpacityProperty, anim);
            OPenGLLayerWin.BeginAnimation(UIElement.OpacityProperty, anim);
        }

        public void HideWin()
        {
            var anim = new DoubleAnimation(0, (Duration)TimeSpan.FromSeconds(0.1));
            anim.Completed += (s, _) => this.Hide();
            this.BeginAnimation(UIElement.OpacityProperty, anim);
            OPenGLLayerWin.BeginAnimation(UIElement.OpacityProperty, anim);
        }
        
        void UpdateGraphics()
        {
            RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
            RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
            
        }

        #region event handlers

        void FormsWin_MouseEnter(object sender, EventArgs e)
        {
            //this.Focus();
            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseEnter();
        }
        void FormsWin_MouseLeave(object sender, EventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseLeave();
        }
        void FormsWin_MouseDown(object sender, MouseEventArgs e)
        {
            Point mousePoint = e.GetPosition((IInputElement)sender);
            if (e.LeftButton == MouseButtonState.Pressed)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseDown(mousePoint.X, mousePoint.Y);
            else if (e.MiddleButton == MouseButtonState.Pressed)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseDown(mousePoint.X, mousePoint.Y);
            else if (e.RightButton == MouseButtonState.Pressed)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseDown(mousePoint.X, mousePoint.Y);
        }
        void FormsWin_MouseUp(object sender, MouseEventArgs e)
        {
            Point mousePoint = e.GetPosition((IInputElement)sender);
            if (e.LeftButton == MouseButtonState.Pressed)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseUp(mousePoint.X, mousePoint.Y);
            else if (e.MiddleButton == MouseButtonState.Pressed)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseUp(mousePoint.X, mousePoint.Y);
            else if (e.RightButton == MouseButtonState.Pressed)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseUp(mousePoint.X, mousePoint.Y);
        }
        void FormsWin_MouseMove(object sender, MouseEventArgs e)
        {
            Point mousePoint = e.GetPosition((IInputElement)sender);
            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseMove(mousePoint.X, mousePoint.Y);
        }
        void FormsWin_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseWheel(e.Delta);
        }

        void FormsWin_KeyDown(object sender, KeyEventArgs e)
        {
            try
            {
                //Key NewKey = KeyInterop.KeyFromVirtualKey((int)e.KeyCode);
                //System.Windows.Input.KeyEventArgs eNew = new System.Windows.Input.KeyEventArgs(Keyboard.PrimaryDevice, HwndSource.FromVisual(GlobalSettings.MainWin) as HwndSource, (Int32)DateTime.Now.Ticks, NewKey);

                //Insert  the DRO values in CNC goto textboxes
                if (e.Key == Key.Insert)
                {

                    if (e.KeyboardDevice.Modifiers == ModifierKeys.Control)
                    {
                        GlobalSettings.CNCModule.txtgotodroX.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[0]);
                        GlobalSettings.CNCModule.txtgotodroY.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[1]);
                        GlobalSettings.CNCModule.txtgotodroZ.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[2]);
                        GlobalSettings.CNCModule.txtgotodroR.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[3] * (180 / Math.PI));
                    }
                }
                //Acivate/Inactivate CNC
                else if (e.Key == Key.F6)
                {
                    if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                    {
                        if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
                        {
                            System.Windows.MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                            return;
                        }
                    }
                    if (!RWrapper.RW_CNC.MYINSTANCE().CNCmode) //GlobalSettings.MainWin.CNCRBtn.Content.ToString() == "Manual")
                    {
                        //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) GlobalSettings.MainWin.CNCRBtn.Content = "CNC";
                        RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
                    }
                    else
                    {
                        //if (RWrapper.RW_MainInterface.MYINSTANCE().MachineCardFlag < 2) GlobalSettings.MainWin.CNCRBtn.Content = "Manual";
                        RWrapper.RW_CNC.MYINSTANCE().Activate_ManualModeDRO();
                    }
                }
                //Build program
                else if (e.Key == Key.F2)
                    GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Build").First(), null);
                //Edit program
                else if (e.Key == Key.F3)
                    GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Edit").First(), null);
                //Run program
                else if (e.Key == Key.F5)
                {
                    if (!GlobalSettings.MainWin.OtherWinToolbarRotated)
                        GlobalSettings.MainWin.ProgramRBtn.IsChecked = true;
                    GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Run").First(), null);
                }
                else if (e.Key == Key.F7 && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    //ExtendedDD.btnSnapMode.IsChecked = (bool)ExtendedDD.btnSnapMode.IsChecked ? false : true;
                    GlobalSettings.MainWin.SnapStateButton_Click(null, null);
                }
                else if (e.Key == Key.F8 && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    //if ((bool)ExtendedDD.btnSnapMode.IsChecked)
                    //{
                    //ExtendedDD.btnVSnapMode.IsChecked = (bool)ExtendedDD.btnVSnapMode.IsChecked ? false : true;
                    GlobalSettings.MainWin.SnapStateButton_Click(null, null);
                    //}
                }
                else if (e.Key == Key.U && (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge))
                {
                    if (e.KeyboardDevice.Modifiers == ModifierKeys.Control)
                    {
                        int AuxVal = 0;
                        if (GlobalSettings.MainWin.AuxillaryLightIsON)
                        {
                            GlobalSettings.MainWin.AuxillaryLightIsON = false;
                            AuxVal = 0;
                        }
                        else
                        {
                            GlobalSettings.MainWin.AuxillaryLightIsON = true;
                            AuxVal = 64;
                        }
                        int[] tmpVals = { 0, 0, 0, 0, 0, AuxVal };
                        RWrapper.RW_CNC.MYINSTANCE().UpdateLight(tmpVals);
                    }
                }
                if (GlobalSettings.MainWin.SettingsLoginWindow.IsVisible == true && e.Key == Key.Enter)
                    e.Handled = true;
                if (e.Key == Key.P)
                {
                    if (e.KeyboardDevice.Modifiers == ModifierKeys.Control)
                    {
                        //this.SaveScreen();
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyDown(e);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FOGL02", ex); }
        }
        void FormsWin_KeyUp(object sender, KeyEventArgs e)
        {

            RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyUp(e);
        }

        void FormsWin_Scroll(object sender, MouseWheelEventArgs e)
        {
            UpdateGraphicsForScroll = true;
        }
        //void FormsWin_Paint(object sender,  e)
        //{
        //    if (UpdateGraphicsForScroll)
        //    {
        //        RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
        //        UpdateGraphicsForScroll = false;
        //    }
        //}

        #endregion


        public void SetOwner(System.Windows.Window Owner)
        {
            this.Owner = Owner;
        }
        public void SetOwned(System.Windows.Window Owned)
        {
            Owned.Owner = this;
        }

       
    }
}