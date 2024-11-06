using System;
using System.Collections.Generic;
using System.Linq;
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
    /// Interaction logic for CamProfileMeasurementWin.xaml
    /// </summary>
    public partial class CamProfileMeasurementWin : Window
    {
        private static CamProfileMeasurementWin _MyInstance;
        public static CamProfileMeasurementWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new CamProfileMeasurementWin();
            }
            return _MyInstance;
        }

        public CamProfileMeasurementWin()
        {
            InitializeComponent();
        }

        private void OKBtn_Click(object Sender, EventArgs e)
        {
            try
            {
                double AngularSteps = Convert.ToDouble(AngularStepsTxtBx.Text);
                int PointsToBundle = Convert.ToInt32(PointstoBundleTxtBx.Text);
                RWrapper.RW_MainInterface.MYINSTANCE().GetCamProfileDistanceFromCenter(AngularSteps, PointsToBundle);
                this.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileMeasurement001", ex);
            }
        }
        private void CancelBtn_Click(object Sender, EventArgs e)
        {
            try
            {
                this.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileMeasurement002", ex);
            }
        }
        void btnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                _MyInstance = null;
            }
            catch (Exception ex)
            {
                _MyInstance = null;
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileMeasurement003", ex);
            }

        }
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);
            if (e.ButtonState == MouseButtonState.Pressed)
                DragMove();
        }

    }
}
