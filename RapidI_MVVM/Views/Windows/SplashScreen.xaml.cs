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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Rapid
{
	/// <summary>
	/// Interaction logic for SplashScreen.xaml
	/// </summary>
	public partial class SplashScreen : Window
	{
		public SplashScreen()
		{
			this.InitializeComponent();
            this.CacheMode = null;
		}

		private void btnClose_Click(object sender, System.Windows.RoutedEventArgs e)
		{
			this.Hide();
		}

        //protected override void OnMouseDown(MouseButtonEventArgs e)
        //{
        //    //if (!this.btnClose.IsMouseOver)
        //    //    System.Diagnostics.Process.Start("http:\\www.rapidi.in");
        //    base.OnMouseDown(e);
        //}

        private void Open_hyperlink1(object Sender, EventArgs e)
        { 
                System.Diagnostics.Process.Start("http:\\www.customisedtechnologies.com");
        }

        private void Open_hyperlink2(object Sender, EventArgs e)
        {  
                System.Diagnostics.Process.Start("http:\\www.rapidi.in");
        }

	}
}