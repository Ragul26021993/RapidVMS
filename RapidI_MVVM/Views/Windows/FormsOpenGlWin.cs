using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media.Animation;
using System.Windows.Media;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using Rapid.Utilities;

namespace Rapid.Windows
{
    /// <summary>
    /// This Windows serves as window for the open gl drawing surfaces.
    /// This has not to be shown in taskbar,Windowstyle will be none,cannot resize.
    /// </summary>
    public class FormsOpenGlWin : Form
    {
        private System.ComponentModel.IContainer components = null;
        public System.Windows.Forms.PictureBox pictureBox1;

        private bool UpdateGraphicsForScroll = false;

        public FormsOpenGlWin()
            : base()
        {
            InitializeComponent();
            this.ShowInTaskbar = false;

            pictureBox1.MouseEnter += new EventHandler(FormsWin_MouseEnter);
            pictureBox1.MouseLeave += new EventHandler(FormsWin_MouseLeave);
            pictureBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(FormsWin_MouseDown);
            pictureBox1.MouseUp += new System.Windows.Forms.MouseEventHandler(FormsWin_MouseUp);
            pictureBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(FormsWin_MouseMove);
            this.MouseWheel += new System.Windows.Forms.MouseEventHandler(FormsWin_MouseWheel);

            this.KeyDown += new System.Windows.Forms.KeyEventHandler(FormsWin_KeyDown);
            this.KeyUp += new System.Windows.Forms.KeyEventHandler(FormsWin_KeyUp);

            this.Scroll += new ScrollEventHandler(FormsWin_Scroll);
            pictureBox1.Paint += new PaintEventHandler(FormsWin_Paint);
        }
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormsOpenGlWin));
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBox1
            // 
            //this.pictureBox1.Image = System.Drawing.Image.FromFile("C:\\Users\\Public\\Pictures\\Sample Pictures\\Desert.jpg");
            this.pictureBox1.Location = new System.Drawing.Point(0, 0);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(0);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(800, 600);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = false;
            this.ClientSize = new System.Drawing.Size(800, 600);
            this.Controls.Add(this.pictureBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);

        }

        public void SetWinPosition(int LeftPos, int TopPos)
        {
            this.Left = LeftPos;
            this.Top = TopPos;
        }
        public void SetWinSize(int WinWidth, int WinHeight)
        {
            this.Width = WinWidth;
            this.Height = WinHeight;
            SetImageViewerSize(WinWidth, WinHeight);
        }
        public void SetImageViewerSize(int ImWidth, int ImHeight)
        {
            pictureBox1.Width = ImWidth;
            pictureBox1.Height = ImHeight;
            if (pictureBox1.Width > this.Width || pictureBox1.Height > this.Height)
                this.AutoScroll = true;
        }

        /// <summary>
        /// This provides the handle as IntPtr of the pic box.
        /// </summary>
        public IntPtr GetHandle
        {
            get
            {
                return pictureBox1.Handle;
                //System.Windows.Interop.WindowInteropHelper h = new System.Windows.Interop.WindowInteropHelper(this); 
                //return h.EnsureHandle();
            }
        }

        [DllImport("user32.dll")]
        private static extern int SetWindowLong(HandleRef hWnd, int nIndex, int dwNewLong);

        /// <summary>
        /// sets the owner of a System.Windows.Forms.Form to a System.Windows.Window
        /// </summary>
        /// <param name="form"></param>
        /// <param name="owner"></param>
        public void SetOwner(System.Windows.Window Owner)
        {
            WindowInteropHelper helper = new WindowInteropHelper(Owner);
            SetWindowLong(new HandleRef(this, this.Handle), -8, helper.Handle.ToInt32());
        }
        public void SetOwned(System.Windows.Window Owned)
        {
            WindowInteropHelper helper = new WindowInteropHelper(Owned);
            helper.Owner = this.Handle;
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
        void FormsWin_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseDown(e.X, e.Y);
            else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseDown(e.X, e.Y);
            else if (e.Button == System.Windows.Forms.MouseButtons.Right)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseDown(e.X, e.Y);
        }
        void FormsWin_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoLeftMouseUp(e.X, e.Y);
            else if (e.Button == System.Windows.Forms.MouseButtons.Middle)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMiddleMouseUp(e.X, e.Y);
            else if (e.Button == System.Windows.Forms.MouseButtons.Right)
                RWrapper.RW_MainInterface.MYINSTANCE().OnVideoRightMouseUp(e.X, e.Y);
        }
        void FormsWin_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseMove(e.X, e.Y);
        }
        void FormsWin_MouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().OnVideoMouseWheel(e.Delta);
        }

        void FormsWin_KeyDown(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            try
            {
                Key NewKey = KeyInterop.KeyFromVirtualKey((int)e.KeyCode);
                System.Windows.Input.KeyEventArgs eNew = new System.Windows.Input.KeyEventArgs(Keyboard.PrimaryDevice, HwndSource.FromVisual(GlobalSettings.MainWin) as HwndSource, (Int32)DateTime.Now.Ticks, NewKey);
                
                //Insert  the DRO values in CNC goto textboxes
                if (e.KeyCode == Keys.Insert)
                {

                    if ((e.Modifiers & Keys.Control) == Keys.Control)
                    {
                        GlobalSettings.CNCModule.txtgotodroX.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[0]);
                        GlobalSettings.CNCModule.txtgotodroY.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[1]);
                        GlobalSettings.CNCModule.txtgotodroZ.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[2]);
                        GlobalSettings.CNCModule.txtgotodroR.Text = String.Format("{0:0.0000}", RWrapper.RW_DRO.CurrentUCSDRO[3] * (180 / Math.PI));
                    }
                }
                //Acivate/Inactivate CNC
                else if (e.KeyCode == Keys.F6)
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
                else if (e.KeyCode == Keys.F2)
                    GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Build").First(), null);
                //Edit program
                else if (e.KeyCode == Keys.F3)
                    GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Edit").First(), null);
                //Run program
                else if (e.KeyCode == Keys.F5)
                {
                    if (!GlobalSettings.MainWin.OtherWinToolbarRotated)
                        GlobalSettings.MainWin.ProgramRBtn.IsChecked = true;
                    GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Run").First(), null);
                }
                else if (e.KeyCode == Keys.F7 && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    //ExtendedDD.btnSnapMode.IsChecked = (bool)ExtendedDD.btnSnapMode.IsChecked ? false : true;
                    GlobalSettings.MainWin.SnapStateButton_Click(null, null);
                }
                else if (e.KeyCode == Keys.F8 && GlobalSettings.RapidMachineType != GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    //if ((bool)ExtendedDD.btnSnapMode.IsChecked)
                    //{
                    //ExtendedDD.btnVSnapMode.IsChecked = (bool)ExtendedDD.btnVSnapMode.IsChecked ? false : true;
                    GlobalSettings.MainWin.SnapStateButton_Click(null, null);
                    //}
                }
                else if (e.KeyCode == Keys.U && (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.ProbeOnly || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.HeightGauge))
                {
                    if ((e.Modifiers & Keys.Control) == Keys.Control)
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
                if (GlobalSettings.MainWin.SettingsLoginWindow.IsVisible == true && e.KeyCode == Keys.Enter)
                    e.Handled = true;
                if (e.KeyCode == Keys.P)
                {
                    if ((e.Modifiers & Keys.Control) == Keys.Control)
                    {
                        this.SaveScreen();
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyDown(eNew);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FOGL02", ex); }
        }
        void FormsWin_KeyUp(object sender, System.Windows.Forms.KeyEventArgs e)
        {
            Key NewKey = KeyInterop.KeyFromVirtualKey((int)e.KeyCode);
            System.Windows.Input.KeyEventArgs eNew = new System.Windows.Input.KeyEventArgs(Keyboard.PrimaryDevice, HwndSource.FromVisual(GlobalSettings.MainWin) as HwndSource, (Int32)DateTime.Now.Ticks, NewKey);
            RWrapper.RW_MainInterface.MYINSTANCE().Form_KeyUp(eNew);
        }

        void FormsWin_Scroll(object sender, ScrollEventArgs e)
        {
            UpdateGraphicsForScroll = true;
        }
        void FormsWin_Paint(object sender, PaintEventArgs e)
        {
            if (UpdateGraphicsForScroll)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().UpdateVideoGraphics();
                UpdateGraphicsForScroll = false;
            }
        }

        #endregion

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}