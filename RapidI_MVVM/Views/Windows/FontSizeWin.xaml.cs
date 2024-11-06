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
	public partial class FontSizeWin : Window
	{
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
		public FontSizeWin(int LowerLimit, int UpperLimit)
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
            for (int i = LowerLimit; i <= UpperLimit; i++)
                FontSizeCBx.Items.Add(i);
            FontSizeCBx.SelectedIndex = 0;
            FontSizeCBx.KeyDown += new KeyEventHandler(FontSizeCBx_KeyDown);
            this.KeyDown += new KeyEventHandler(FontSizeCBx_KeyDown);
		}
        //Function to set the current value in the combobox
        public void SetCurrentFontSize(int FSize)
        {
            int ItemIndex = FontSizeCBx.Items.IndexOf(FSize);
            if(ItemIndex < 0)
                 FontSizeCBx.SelectedIndex = 0;
            else
                FontSizeCBx.SelectedIndex = ItemIndex;
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
            if (e.Key == Key.Enter) { this.Result = true; this.Close(); }
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
        private void FontSizeCBx_KeyDown(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            if (e.Key == Key.Enter) { this.Result = true; this.Close(); }
            else if (e.Key == Key.Escape) { this.Result = false; this.Close(); }
        }
	}
}