using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Animation;
using System.Windows.Media;

namespace Rapid.Windows
{
    /// <summary>
    /// This Windows serves as window for the open gl drawing surfaces.
    /// This has not to be shown in taskbar,Windowstyle will be none,cannot resize.
    /// </summary>
   public class OpenGlWin : Window
    {
        public OpenGlWin()
            : base()
        {
            this.WindowStyle = WindowStyle.None;
            this.ShowInTaskbar = false;
			this.Effect = new System.Windows.Media.Effects.DropShadowEffect();
            this.ResizeMode = ResizeMode.NoResize;
            this.Background = System.Windows.Media.Brushes.LightBlue;
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
            }
        }
        /// <summary>
        /// This provides the handle as IntPtr of this window.
        /// </summary>
        public IntPtr Handle
        {
            get
            {
                System.Windows.Interop.WindowInteropHelper h = new System.Windows.Interop.WindowInteropHelper(this);
                return h.EnsureHandle();
            }
        }

        public void ShowWin()
        {
            this.Opacity = 0;
            this.Show();

            var anim = new DoubleAnimation(1, (Duration)TimeSpan.FromSeconds(0.1));
            anim.Completed += (s, _) => UpdateGraphics();
            this.BeginAnimation(UIElement.OpacityProperty, anim);
        }

        public void HideWin()
        {
            var anim = new DoubleAnimation(0, (Duration)TimeSpan.FromSeconds(0.1));
            anim.Completed += (s, _) => this.Hide();
            this.BeginAnimation(UIElement.OpacityProperty, anim);
        }
        
        void UpdateGraphics()
        {
            RWrapper.RW_MainInterface.MYINSTANCE().UpdateRCadGraphics();
            RWrapper.RW_MainInterface.MYINSTANCE().UpdateDxfGraphics();
            
        }
    }
}