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
	public partial class ShapeInfoWin : Window
	{
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
		public ShapeInfoWin()
		{
			this.InitializeComponent();
			
			// Insert code required on object creation below this point.
		}
        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        { this.Result = true; this.Close(); }
        //On cancel click  just return the dialog result as false.
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        { this.Result = false; this.Close(); }
        //if user presses enter key in textbox just make the dialog result to be true.
        private void txtNewName_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) { this.Result = true; e.Handled = true; this.Close(); }
            else if (e.Key == Key.Escape) { this.Result = false; e.Handled = true; this.Close(); }
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