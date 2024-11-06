using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;

namespace Rapid.Views.Windows
{
    /// <summary>
    /// Interaction logic for OnlineSystemController.xaml
    /// </summary>
    public partial class OnlineSystemController : Window, INotifyPropertyChanged
    {
        public OnlineSystemController()
        {
            InitializeComponent();
            this.Speed = 2;
        }
        public event PropertyChangedEventHandler PropertyChanged;

        private int totanumber, passed, failed;
        public int TotalNumberDone
        {
            get
            {
                return totanumber;
            }
            set
            {
                totanumber = value;
                txtTotal.Text = totanumber.ToString();
                //OnPropertyChanged("TotalNumberDone");
            }
        }

        public int PassNumber
        {
            get
            {
                return passed;
            }
            set
            {
                passed = value;
                //OnPropertyChanged("PassNumber");
                txtPass.Text = passed.ToString();
            }
        }

        public int FailNumber
        {
            get
            {
                return failed;
            }
            set
            {
                failed = value;
                //OnPropertyChanged("FailNumber");
                txtFail.Text = failed.ToString();
            }
        }

        public double Speed { get; set; }
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            //Let us load up the combo box with all available part programs....
            this.Height = 600;
            txtSpeed.Text = this.Speed.ToString();
        }

        private void OnPropertyChanged(string propName)
        {
            var handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(propName));
            }
        }
        private void btnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private void BtnStartOperations_Click(object sender, RoutedEventArgs e)
        {
            //Whenever we start, we assume that ALL slots in the rotating table are EMPTY. 
            if (cbComponentsList.SelectedIndex < 0) return;
            RWrapper.RW_CNC.MYINSTANCE().CurrentOnlineSystemState = "00000000";
            RWrapper.RW_CNC.MYINSTANCE().MoveTable(false, false, this.Speed);
        }

        private void BtnStopOperations_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_CNC.MYINSTANCE().StopOnlineSystem();
            Button BtnPPStop = (Button)GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Stop").First();
            GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(BtnPPStop, null);
        }

        private void BtnAdminControls_Click(object sender, RoutedEventArgs e)
        {
            if (this.Height == 600)
                this.Height += 460;
            else
                this.Height = 600;
        }

        private void DrillChecker_DrillCheckerButtonStateChanged(object sender, DrillCheckerButtonClickEventArgs e)
        {
            RWrapper.RW_CNC.MYINSTANCE().OperateStep(e.ButtonIndex, e.State);
        }

        private void txtSpeed_TextChanged(object sender, TextChangedEventArgs e)
        {
            double tempSpeed = 2;
            double.TryParse(txtSpeed.Text, out tempSpeed);
            this.Speed = tempSpeed;
        }
    }
}
