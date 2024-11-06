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
    public partial class ProbeStylusTypeSelectionWin : Window
    {
        public event RoutedEventHandler WinClosing;
        public bool WindowOpen = false;

        public ProbeStylusTypeSelectionWin()
        {
            this.InitializeComponent();
            this.KeyDown += new KeyEventHandler(HandleKeyPress);
            // Insert code required on object creation below this point.
        }
      public void ArrangeForCurrentProbeType()
        {
            switch (GlobalSettings.CurrentProbeType)
            {
                case GlobalSettings.ProbeType.Normal_Probe:
                    break;
                case GlobalSettings.ProbeType.TProbe_AlongX:
                    LeftStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    RightStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    StraightStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    FrontStylusBtn.Visibility = System.Windows.Visibility.Collapsed;
                    BackStylusBtn.Visibility = System.Windows.Visibility.Collapsed;
                    break;
                case GlobalSettings.ProbeType.TProbe_AlongY:
                    LeftStylusBtn.Visibility = System.Windows.Visibility.Collapsed;
                    RightStylusBtn.Visibility = System.Windows.Visibility.Collapsed;
                    StraightStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    FrontStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    BackStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    break;
                case GlobalSettings.ProbeType.Star_Probe:
                    LeftStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    RightStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    FrontStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    BackStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    StraightStylusBtn.Visibility = System.Windows.Visibility.Visible;
                    break;
            }
        }

        private void RadiioBtn_CheckChanged(object sender, RoutedEventArgs e)
        {
            //Get the radio button that was clicked
            RadioButton rb = (RadioButton)sender;
            //We are clicking unnecesarily. just ignore the click!
            //if ((bool)rb.IsChecked) return;

            //Make an array of radiobuttons and uncheck all of them
            RadioButton[] ProbeSides = { LeftStylusBtn, RightStylusBtn, BackStylusBtn, FrontStylusBtn, StraightStylusBtn };
            
            //Get probe point....
            if (!WindowOpen && (bool)rb.IsChecked)
            {
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(Array.IndexOf(ProbeSides, rb));
                WindowOpen = true;
            }
        }

        private void LeftBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!WindowOpen)
            {
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(0);
                WindowOpen = true;
                this.Close();
                this.Show();
            }
        }
        private void RightBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!WindowOpen)
            {
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(1);
                WindowOpen = true;
                this.Close();
                this.Show();
            }
        }
        private void BackBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!WindowOpen)
            {
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(2);
                WindowOpen = true;
                this.Close();
                this.Show();
            }
        }
        private void FrontBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!WindowOpen)
            {
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(3);
                WindowOpen = true;
                this.Close();
                this.Show();
            }
        }
        private void StraightBtn_Checked(object sender, RoutedEventArgs e)
        {
            if (!WindowOpen)
            {
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(4);
                WindowOpen = true;
                this.Close();
                this.Show();
            }
        }
        private void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            //this.Close();
            WindowOpen = false;
            this.Hide();
            LeftStylusBtn.IsChecked = false;
            RightStylusBtn.IsChecked = false;
            FrontStylusBtn.IsChecked = false;
            BackStylusBtn.IsChecked = false;
            StraightStylusBtn.IsChecked = false;
        }
        public void SendStylusData()
        {
            if ((bool)LeftStylusBtn.IsChecked)
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(0);
            else if ((bool)RightStylusBtn.IsChecked)
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(1);
            else if ((bool)BackStylusBtn.IsChecked)
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(2);
            else if ((bool)FrontStylusBtn.IsChecked)
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(3);
            else if ((bool)StraightStylusBtn.IsChecked)
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch(4);
        }
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            //RaiseEvent();
            this.Hide();
            e.Cancel = true;
        }

        private void HandleKeyPress(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.F12)
            {
                RWrapper.RW_PartProgram.MYINSTANCE().Handle_Program_btnClicks("Probe Position");
            }
        }
    }
}