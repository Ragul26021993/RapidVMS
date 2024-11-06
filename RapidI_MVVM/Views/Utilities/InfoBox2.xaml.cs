using System.Windows;
using System.Windows.Controls;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for InfoBox2.xaml
    /// </summary>
    public partial class InfoBox2 : Grid
	{
        public int AxisStatus { get; set; }
		public InfoBox2()
		{
			this.InitializeComponent();
		}

        //handling the change in axis 
        private void rdbStatus_Checked(object sender, RoutedEventArgs e)
        {
            if (sender.Equals(rdbRadial))
                AxisStatus = 0;
            else if (sender.Equals(rdbDiametrical))
                AxisStatus = 1;
            else if (sender.Equals(rdbXaxis))
                AxisStatus = 2;
            else if (sender.Equals(rdbYaxis))
                AxisStatus = 3;
        }

        private void ChkBx_Checked(object sender, RoutedEventArgs e)
        {
            txtStatus.Text = "R";
            txtStatus2.Text = "Theta";
        }

        private void ChkBx_Unchecked(object sender, RoutedEventArgs e)
        {
            txtStatus.Text = "X";
            txtStatus2.Text = "Y";
        }
	}
}