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

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for MessageWin.xaml
	/// </summary>
	public partial class MessageWin : OpenGlWin
	{
		public MessageWin()
		{
			this.InitializeComponent();
		}

        private void Btn_Cancel_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().Abort_HomePosition_Command();
        }

        public void ShowCancelButton()
        {
            this.LayoutRoot.ColumnDefinitions[1].Width = new GridLength(0.2, GridUnitType.Star);
        }

        public void HideCancelButton()
        {
            this.LayoutRoot.ColumnDefinitions[1].Width = new GridLength(0, GridUnitType.Star);
        }
    }
}