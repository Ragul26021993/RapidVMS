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
    /// Interaction logic for CamProfileShapeWin.xaml
    /// </summary>
    public partial class CamProfileShapeWin : Window
    {
        private static CamProfileShapeWin _MyInstance;
        public static CamProfileShapeWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new CamProfileShapeWin();
            }
            return _MyInstance;
        }

        public CamProfileShapeWin()
        {
            InitializeComponent();
        }
        private void OKBtn_Click(object Sender, EventArgs e)
        {
            try
            {
                double CircleRadius = Convert.ToDouble(InnerCircleRadiusTxtBx.Text);
                double AngularSteps = Convert.ToDouble(AngularStepsTxtBx.Text);
                double UpperTolerance = Convert.ToDouble(UpperToleranceTxtBx.Text);
                double LowerTolerance = Convert.ToDouble(LowerToleranceTxtBx.Text);

                //RWrapper.RW_MainInterface.MYINSTANCE().CreateCamProfileShape(CircleRadius, 0, AngularSteps, LowerTolerance, UpperTolerance,true, NoofPts_Bundle_4_IDTool_Msrmnt);
                this.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileShape001", ex);
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
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileShape002", ex);
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
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileShape003", ex);
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
