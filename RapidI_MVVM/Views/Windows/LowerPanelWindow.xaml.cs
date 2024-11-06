using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for LowerPanelWindow.xaml
	/// </summary>
    public partial class LowerPanelWindow : FadeableWin
	{
		
        public LowerPanelWindow() : base()
		{
			this.InitializeComponent();
			
			// Insert code required on object creation below this point.
        }

        //public void ShowWin()
        //{
            
        //    this.Opacity = 0;
        //    this.Show();

        //    var anim = new DoubleAnimation(1, (Duration)TimeSpan.FromSeconds(0.3));
        //    this.BeginAnimation(UIElement.OpacityProperty, anim);
        //}

        //public void HideWin()
        //{
        //    var anim = new DoubleAnimation(0, (Duration)TimeSpan.FromSeconds(0.5));
        //    anim.Completed += (s, _) => this.Hide();
        //    this.BeginAnimation(UIElement.OpacityProperty, anim);
        //}
    }
}