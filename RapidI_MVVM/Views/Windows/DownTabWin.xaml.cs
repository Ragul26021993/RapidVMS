using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;

namespace Rapid.Windows
{
	public partial class DownTabWin : FadeableWin
	{
        static DependencyProperty CollapsedSizeProperty = DependencyProperty.Register("CollapsedSize", typeof(Size), typeof(DownTabWin), new PropertyMetadata(new Size(200, 30)));
        static DependencyProperty ExpandedSizeProperty = DependencyProperty.Register("ExpandedSize", typeof(Size), typeof(DownTabWin), new PropertyMetadata(new Size(450, 250)));
        public Size CollapsedSize
        {
            get
            { return (Size)GetValue(CollapsedSizeProperty); }
            set
            { SetValue(CollapsedSizeProperty, value); }
        }
        public Size ExpandedSize
        {
            get
            { return (Size)GetValue(ExpandedSizeProperty); }
            set
            { SetValue(ExpandedSizeProperty, value); }
        }
		public DownTabWin():base()
		{
			this.InitializeComponent();
		}
        public  void ChangeSize(Size newSize)
        {
            this.Height = newSize.Height;
            this.Width = newSize.Width;
        }
	}
}