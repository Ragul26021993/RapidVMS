using System;
using System.Windows.Controls;

namespace Rapid.Views.Windows
{
    /// <summary>
    /// Interaction logic for DrillChecker.xaml
    /// </summary>
    public partial class DrillChecker : UserControl
    {
        public DrillChecker()
        {
            InitializeComponent();
        }

        public delegate void DrillCheckerButtonClickHandler(object sender, DrillCheckerButtonClickEventArgs e);

        public event DrillCheckerButtonClickHandler DrillCheckerButtonStateChanged;

        private void ButtonState_Changed(object sender, ButtonStateChangedEventArgs e)
        {
            switch (e.NewValue)
            {
                case ButtonState.Up:
                    DrillCheckerButtonStateChanged?.Invoke(this, new DrillCheckerButtonClickEventArgs(Convert.ToInt16(((CustomButton)sender).Tag), 1));
                    break;
                case ButtonState.Down:
                    DrillCheckerButtonStateChanged?.Invoke(this, new DrillCheckerButtonClickEventArgs(Convert.ToInt16(((CustomButton)sender).Tag), 0));
                    break;
                default:
                    break;
            }
        }
    }
}
