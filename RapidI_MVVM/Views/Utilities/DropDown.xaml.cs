using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using Rapid.Windows;

namespace Rapid.Utilities
{
    public partial class DropDown : FadeableWin
    {
       
        private bool IsHiding { get; set; }
        public DropDown() : base()
        {
            InitializeComponent();
            bdrTooltip.Visibility = System.Windows.Visibility.Hidden;
        }
        //Overriding the base function to have the custom functionality for hiding
        protected override void FadeOut_Completed(object sender, EventArgs e)
        {
            IsHiding = false;
            base.FadeOut_Completed(sender,e);
        }
        //This property allows  to set the Header text of this window.
        public string HeaderText
        {
            get { return UpText.Text; }
            set { UpText.Text = value; }
        }
        public Point StartPosition
        {
            get { return new Point(this.Left, this.Top); }
            set
            {
                this.Top = value.Y;
                this.Left = value.X;
            }
        }
        protected override void OnMouseLeave(MouseEventArgs e)
        {
            if (IsAllowFading && !IsHiding)
            {
                if (this.Opacity == 1) { IsHiding = true; FadeOut.Begin(); }
                else { this.Opacity = 0; }
            }
            if (this.Owner != null)
                this.Owner.Focus();
        }
        protected override void OnMouseEnter(MouseEventArgs e)
        {
            IsHiding = false;
            base.OnMouseEnter(e);
        }
        //This property allows  to set the content of this window without any memory leak.
        public void SetContent( UIElement cont)
        {
            //Making the earlier content to be eligible to be collected by GC.
            //This prevents memory leak.
            if (this.LayoutRoot.Children.Count > 2)
            {
                this.LayoutRoot.Children.RemoveAt(2);
            }
                System.Windows.Controls.Grid.SetRow(cont, 2);

                this.Width = ((FrameworkElement)cont).Width + 5;
                this.LayoutRoot.Children.Add((UIElement)cont);
        }
        //When ever Esc key is pressed just hide the window.
        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            if (e.Key == Key.Escape)
            {
                base.FadeOut_Completed(this, null);
            }
            base.OnKeyDown(e);
        }
        //function to show the tooltip of the selected tool.
        internal void ShowTooltip(ref ScrollablePanel panel, ref ContentControl ContentCon, ref string TooltipText)
        {
            Vector v = VisualTreeHelper.GetOffset(ContentCon);

            v.X -= panel._offset.X;

            //TransformGroup t = (TransformGroup)VisualTreeHelper.GetTransform(pane);

            //TranslateTransform tt = (TranslateTransform)t.Children[0];

            ////tt.X
            txtTooltip.Text = TooltipText;
            Point MousePoint = Mouse.GetPosition(this);
            bdrTooltip.Visibility = System.Windows.Visibility.Visible;
            if (v.X + (TooltipText.Length * 8) >= this.Width)
                bdrTooltip.Margin = new Thickness(this.Width - (TooltipText.Length * 8), 2, 0, 0);
            else
                bdrTooltip.Margin = new Thickness(v.X + 30, 2, 0, 0);
        }
        //function to hide the tooltip shown.
        public void HideTooltip()
        {
            bdrTooltip.Visibility = System.Windows.Visibility.Hidden;
        }
     }
}