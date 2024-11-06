using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Forms.Integration;
using System.Windows.Input;
using System.Windows.Media.Animation;
using System.Windows.Media;
using System.Runtime.InteropServices;
using System.Windows.Interop;

namespace Rapid.Windows
{
    /// <summary>
    /// This Windows serves as window for the open gl drawing surfaces.
    /// This has not to be shown in taskbar,Windowstyle will be none,cannot resize.
    /// </summary>
    public class FormsHostWin : WindowsFormsHost
    {
        //private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.PictureBox pictureBox;

        public FormsHostWin()
            : base()
        {
            InitializeMyComponent();
            this.SizeChanged += new SizeChangedEventHandler(FormsHostPanel_SizeChanged);
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
        }

        public System.Windows.Forms.PictureBox GetOglHost()
        {
            return pictureBox;
        }
        private void FormsHostPanel_SizeChanged(object sender, RoutedEventArgs e)
        {
            this.pictureBox.Size = new System.Drawing.Size((int)this.Width, (int)this.Height);
        }
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
    }
}