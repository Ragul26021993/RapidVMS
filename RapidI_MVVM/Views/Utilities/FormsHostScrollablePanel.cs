using System;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Forms.Integration;

namespace Rapid.Utilities
{
    /// <summary>
    /// This Windows serves as window for the open gl drawing surfaces.
    /// This has not to be shown in taskbar,Windowstyle will be none,cannot resize.
    /// </summary>
    public class FormsHostScrollablePanel : WindowsFormsHost
    {
        private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.Panel ContainerPanel;
        private System.Windows.Forms.PictureBox pictureBox;

        public FormsHostScrollablePanel()
            : base()
        {
            InitializeMyComponent();
            this.GotFocus += new RoutedEventHandler(FormsHostScrollablePanel_GotFocus);
        }
        private void InitializeMyComponent()
        {
            this.ContainerPanel = new Panel();
            this.pictureBox = new System.Windows.Forms.PictureBox();
            // 
            // ContainerPanel
            // 
            this.ContainerPanel.Location = new System.Drawing.Point(0, 0);
            this.ContainerPanel.Margin = new System.Windows.Forms.Padding(0);
            this.ContainerPanel.AutoScroll = true;
            this.ContainerPanel.Name = "ContainerPanel";
            this.ContainerPanel.Size = new System.Drawing.Size(800, 600);
            this.ContainerPanel.TabIndex = 0;
            this.ContainerPanel.TabStop = false;
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(800, 600);
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            // 
            // Form1
            // 
            this.ContainerPanel.Controls.Add(pictureBox);
            this.Child = ContainerPanel;

            this.FocusVisualStyle = null;
        }
        
        public System.Windows.Forms.PictureBox GetOglHost()
        {
            return pictureBox;
        }
        public void SetWinSize(int WinWidth, int WinHeight)
        {
            this.Width = WinWidth;
            this.Height = WinHeight;
            SetImageViewerSize(WinWidth, WinHeight);
        }
        public void SetImageViewerSize(int ImWidth, int ImHeight)
        {
            pictureBox.Width = ImWidth;
            pictureBox.Height = ImHeight;
        }
        private void FormsHostScrollablePanel_GotFocus(object sender, RoutedEventArgs e)
        {
            pictureBox.Focus();
        }

        /// <summary>
        /// This provides the handle as IntPtr of the pic box.
        /// </summary>
        public IntPtr GetHandle
        {
            get
            {
                return pictureBox.Handle;
                //System.Windows.Interop.WindowInteropHelper h = new System.Windows.Interop.WindowInteropHelper(this); 
                //return h.EnsureHandle();
            }
        }
        public void SetCursor(System.Windows.Forms.Cursor Csr)
        {
            this.pictureBox.Cursor = Csr;
        }

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