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
using System.Windows.Shapes;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for ShapeInfoWin.xaml
	/// </summary>
	public partial class FilterLineTypeSelectionWin : Window
	{
        public event RoutedEventHandler WinClosing;
        public FilterLineTypeSelectionWin()
		{
			this.InitializeComponent();
			
			// Insert code required on object creation below this point.
		}
        
        private void LeftBtn_Click(object sender, RoutedEventArgs e)
        {
            //RWrapper.RW_MainInterface.MYINSTANCE().SetOuterMostLineSide(1);
            this.Close();
        }
        private void RightBtn_Click(object sender, RoutedEventArgs e)
        {
            //RWrapper.RW_MainInterface.MYINSTANCE().SetOuterMostLineSide(0);
            this.Close();
        }
        private void UpBtn_Click(object sender, RoutedEventArgs e)
        {
            //RWrapper.RW_MainInterface.MYINSTANCE().SetOuterMostLineSide(2);
            this.Close();
        }
        private void DownBtn_Click(object sender, RoutedEventArgs e)
        {
            //RWrapper.RW_MainInterface.MYINSTANCE().SetOuterMostLineSide(3);
            this.Close();
        }
        private void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            this.Hide();
            e.Cancel = true;
        }
    }
}