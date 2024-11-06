using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Rapid.Windows
{
    /// <summary>
    /// Base class which provide ready to use Fade In/Out behaviours.
    /// </summary>
    public abstract class FadeableWin : Window
    {
        //This event will be raised when ever window becomes invisible in UI (ie., Opacity=0)
        public event System.Windows.RoutedEventHandler WinClosing;
        //The storyboards that contains the timelines of the Fading in & Fading out behaviours
        protected internal System.Windows.Media.Animation.Storyboard FadeIn, FadeOut;
        //This property determines whether the window is allowed to do fadeinOut or not.
        public bool IsAllowFading { get; set; }
        public FadeableWin()
        {
            this.Name = "FadeableWin";
            FadeIn = new Storyboard();
            FadeOut = new Storyboard();

            DoubleAnimation first = new DoubleAnimation(0.0D, 1.0D, new Duration(new TimeSpan(0, 0, 0, 0, 500)));
            first.BeginTime = new TimeSpan(0, 0, 0, 0, 100);
            first.AutoReverse = false;
            Storyboard.SetTarget(first, this);
            Storyboard.SetTargetProperty(first, new PropertyPath(Window.OpacityProperty));

            DoubleAnimation second = new DoubleAnimation(1.0D, 0.0D, new Duration(new TimeSpan(0, 0, 0, 0, 500)));
            second.BeginTime = new TimeSpan(0, 0, 0, 0, 100);
            second.AutoReverse = false;
            Storyboard.SetTarget(second, this);
            Storyboard.SetTargetProperty(second, new PropertyPath(Window.OpacityProperty));

            FadeIn.Children.Add(first);
            FadeOut.Children.Add(second);

            FadeIn.Completed += new EventHandler(FadeIn_Completed);
            FadeOut.Completed += new EventHandler(FadeOut_Completed);
        }
        //The overridable function for the completion of FadeIn timeline
        protected virtual void FadeIn_Completed(object sender, EventArgs e)
        {
            this.Opacity = 1;
        }
        //The overridable function for the completion of FadeOut timeline
        protected virtual void FadeOut_Completed(object sender, EventArgs e)
        {
            this.Opacity = 0;
            if (this.WinClosing != null)
                this.WinClosing(this, null);
        }
        //Overriding the MouseEnter to call the Fade-In timeline
        protected override void OnMouseEnter(MouseEventArgs e)
        {
            if (IsAllowFading)
            {
                FadeOut.Stop();
                if (this.Opacity == 0) { FadeIn.Begin(); }
                else if (this.Opacity > 0) { this.Opacity = 1; }
            }
            //this.Focus();
            base.OnMouseEnter(e);
        }
        //Overriding the MouseLeave to call the Fade-Out timeline
        protected override void OnMouseLeave(MouseEventArgs e)
        {
            if (IsAllowFading)
            {
                if (this.Opacity == 1) { FadeOut.Begin(); }
                else { this.Opacity = 0; }
            }
            if (this.Owner != null)
                this.Owner.Focus();
            base.OnMouseLeave(e);
        }
        //Function to show the window.
        public void ShowWindow()
        {
            if (IsAllowFading)
            { OnMouseEnter(null); }
            else
                FadeOut.Stop();
            this.Opacity = 1;
            this.Show();
        }
        //Function to hide the window.
        public void HideWindow()
        {
            OnMouseLeave(null);
        }
    }
}