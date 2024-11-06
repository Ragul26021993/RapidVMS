using System.Windows.Controls;
using System.Windows.Input;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for ScrollableShapeList.xaml
    /// </summary>
    public partial class ScrollableShapeList : Grid
	{
		public ScrollableShapeList()
		{
			this.InitializeComponent();
		}
        protected override void OnMouseEnter(MouseEventArgs e)
        {
            this.Focus();
            base.OnMouseEnter(e);
        }
        public new   UIElementCollection Children
        {
            get{return this.stShapes.Children;}
        }

        private void _btnUp_Click(object sender, System.Windows.RoutedEventArgs e)
        {
        	this.scrMain.LineUp();
        }

        private void _btnDown_Click(object sender, System.Windows.RoutedEventArgs e)
        {
        	this.scrMain.LineDown();
        }
	}

}