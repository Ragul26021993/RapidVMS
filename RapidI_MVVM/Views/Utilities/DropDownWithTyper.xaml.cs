using System.Windows;
using System.Windows.Controls;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for DropDownWithTyper.xaml
    /// </summary>
    public partial class DropDownWithTyper : Grid
    {
        public DropDownWithTyper()
        {
            this.InitializeComponent();
        }

        private void SetMag_Click(object sender, RoutedEventArgs e)
        {
            if (GlobalSettings.MultilevelZoomMahine)
                RWrapper.RW_MainInterface.MYINSTANCE().SetMagnification(txtManualMagValue.Text + "X");
            btnTg.Content = txtManualMagValue.Text + "X";
        }

        public void MakeMultiLevelSelectorVisible()
        {
            gLBHolder.RowDefinitions[1].Height = new GridLength(40);
            gridMagTyper.Visibility = System.Windows.Visibility.Visible;
        }
    }
}
