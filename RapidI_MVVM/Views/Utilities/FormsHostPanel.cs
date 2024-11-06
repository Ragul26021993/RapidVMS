using System;
using System.Windows;
using System.Windows.Forms.Integration;

namespace Rapid.Utilities
{
    /// <summary>
    /// This Windows serves as window for the open gl drawing surfaces.
    /// This has not to be shown in taskbar,Windowstyle will be none,cannot resize.
    /// </summary>
    public class FormsHostPanel : WindowsFormsHost
    {
        //private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.PictureBox pictureBox;

        public FormsHostPanel()
            : base()
        {
            InitializeMyComponent();
            //this.SizeChanged += new SizeChangedEventHandler(FormsHostPanel_SizeChanged);
            this.GotFocus += new RoutedEventHandler(FormsHostPanel_GotFocus);
        }
        private void InitializeMyComponent()
        {
            this.pictureBox = new System.Windows.Forms.PictureBox();
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(0, 0);
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            // 
            // Form1
            // 
            this.Child = pictureBox;           

            this.FocusVisualStyle = null;
        }

        public System.Windows.Forms.PictureBox GetOglHost()
        {
            return pictureBox;
        }
        //private void FormsHostPanel_SizeChanged(object sender, RoutedEventArgs e)
        //{
        //    this.pictureBox.Size = new System.Drawing.Size((int)this.ActualWidth, (int)this.ActualHeight);
        //}
        public void FormsHostPanel_GotFocus(object sender, RoutedEventArgs e)
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
            if (Csr != null)
            {
                this.pictureBox.Cursor = Csr;
            }
        }
        //protected override void Dispose(bool disposing)
        //{
        //    if (disposing && (components != null))
        //    {
        //        components.Dispose();
        //    }
        //    base.Dispose(disposing);
        //}
        public void SetSize(int w, int h)
        {
            this.Width = w; this.Height = h;
            pictureBox.Width = w; pictureBox.Height = h;
        }
    }
}