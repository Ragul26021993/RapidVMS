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
    //created by vinod 12/02/2014 ...... draw cam profile shape for a ID Tool or OD Tool.
    /// <summary>
    /// Interaction logic for CamProfileShapeandMeasurementWin.xaml
    /// </summary>
    public partial class CamProfileShapeandMeasurementWin : Window
    {
        private static CamProfileShapeandMeasurementWin _MyInstance;

        public static double CircleMaxRadius, CircleMinRadius, AngularSteps;       //cam profile parameteres.... vinod
        public static bool ToolType_Inner;
       
        public static CamProfileShapeandMeasurementWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new CamProfileShapeandMeasurementWin();
            }
            return _MyInstance;
        }

        public CamProfileShapeandMeasurementWin()
        {
                    
            InitializeComponent();
        }

        private void OKBtn_Click(object Sender, EventArgs e)     //vinod ... now parameteres are set.. so for measurement in same window.. we disablle these parameters and active
        {                                                        //new parameteres according to touch or vision method
            try
            {
                //InstructionTxtBl.Visibility = System.Windows.Visibility.Hidden;
                InnerRBtn.IsEnabled = false;
                OuterRBtn.IsEnabled = false;
                //OtrMnTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //OtrMxTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //InrMnTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //InrMxTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //LwrTlrncTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //UprTlrncTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //MsrmntTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //MsrmntAnglTxtBl.Visibility = System.Windows.Visibility.Hidden;
                //MsrmntPtsTxtBl.Visibility = System.Windows.Visibility.Hidden;
                TouchRBtn.IsEnabled = false;
                VisionRBtn.IsEnabled = false;
                AngularStepsTxtBx.IsEnabled = false;
                OuterCircleMinRadiusTxtBx.IsEnabled = false;
                OuterCircleMaxRadiusTxtBx.IsEnabled = false;
                InnerCircleMinRadiusTxtBx.IsEnabled = false;
                InnerCircleMaxRadiusTxtBx.IsEnabled = false;
                LowerToleranceTxtBx.IsEnabled = false;
                UpperToleranceTxtBx.IsEnabled = false;
                Angular_step_4_IDTool_MsrmntTxtBx.IsEnabled = false;
                NoofPts_Bundle_4_IDTool_MsrmntTxtBx.IsEnabled = false;
                OKBtn.Visibility = System.Windows.Visibility.Hidden;
                CancelBtn.Visibility = System.Windows.Visibility.Hidden;
                        
                ToolType_Inner = ((bool)InnerRBtn.IsChecked);
                int NoofPts_Bundle_4_IDTool_Msrmnt = Convert.ToInt32(NoofPts_Bundle_4_IDTool_MsrmntTxtBx.Text);
            
                if (ToolType_Inner)                                                                     //IDTool
                {
                    double LowerTolerance = Convert.ToDouble(LowerToleranceTxtBx.Text);         
                    double UpperTolerance = Convert.ToDouble(UpperToleranceTxtBx.Text);
                   
                    CircleMinRadius = Convert.ToDouble(InnerCircleMinRadiusTxtBx.Text);       
                    CircleMaxRadius = Convert.ToDouble(InnerCircleMaxRadiusTxtBx.Text);
                    AngularSteps = Convert.ToDouble(AngularStepsTxtBx.Text);
                    //calling fn to set parameteres for a cam profile
                    RWrapper.RW_MainInterface.MYINSTANCE().CreateCamProfileShape(CircleMinRadius, CircleMaxRadius, AngularSteps, LowerTolerance, UpperTolerance, ToolType_Inner, NoofPts_Bundle_4_IDTool_Msrmnt);

                    if (((bool)TouchRBtn.IsChecked))
                    {
                        //gridHolder.RowDefinitions[12].Height = new GridLength()
                        CompleteBtn.Visibility = System.Windows.Visibility.Hidden;          //vision mode

                        StartBtn.Visibility = System.Windows.Visibility.Visible;                //new parameteres according to touch or vision method
                        StopBtn.Visibility = System.Windows.Visibility.Visible;            
                    }
                    else
                    {
                        CompleteBtn.Visibility = System.Windows.Visibility.Visible;          //vision mode
                        StartBtn.Visibility = System.Windows.Visibility.Hidden;                //new parameteres according to touch or vision method
                        StopBtn.Visibility = System.Windows.Visibility.Hidden;
                    }
                }
                else                                   //ODTool
                {
                    CircleMinRadius = Convert.ToDouble(OuterCircleMinRadiusTxtBx.Text);       
                    CircleMaxRadius = Convert.ToDouble(OuterCircleMaxRadiusTxtBx.Text);
                    AngularSteps = Convert.ToDouble(AngularStepsTxtBx.Text);

                    //calling fn to set parameteres for a cam profile
                    RWrapper.RW_MainInterface.MYINSTANCE().CreateCamProfileShape(0.0, CircleMaxRadius, AngularSteps, 0.0, 0.0, ToolType_Inner, NoofPts_Bundle_4_IDTool_Msrmnt);

                    if (((bool)TouchRBtn.IsChecked))            //touch mode
                    {
                        StartBtn.Visibility = System.Windows.Visibility.Visible;
                        StopBtn.Visibility = System.Windows.Visibility.Visible; 
                    }
                    else                                        //vision mode
                    {
                        CompleteBtn.Visibility = System.Windows.Visibility.Visible;         //new parameteres according to touch or vision method
                    }
                }
                
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileShapeandMeasurement001", ex);
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
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileShapeandMeasurement002", ex);
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
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CamProfileShapeandMeasurement003", ex);
            }

        }
        private void StartBtn_Click(object Sender, EventArgs e)         //for touch mode
        {
            try
            {
                if (StartBtn.Content.ToString() == "Start".ToString())
                {
                    StartBtn.Content = "Pause";
                    if (CamProfileShapeandMeasurementWin.ToolType_Inner)
                        RWrapper.RW_DelphiCamModule.MYINSTANCE().Start(CircleMaxRadius, CircleMinRadius, true, 0);         //ID tool    vinod
                    else
                        RWrapper.RW_DelphiCamModule.MYINSTANCE().Start(CircleMaxRadius, CircleMinRadius, false, 0);        //OD tool
                }
                else if (StartBtn.Content.ToString() == "Pause".ToString())
                {
                    StartBtn.Content = "Continue";
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(true);
                }
                else if (StartBtn.Content.ToString() == "Continue".ToString())
                {
                    StartBtn.Content = "Pause";
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(false);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelphiCamComp001", ex);
            }
        }
        private void StopBtn_Click(object Sender, EventArgs e)          //touch mode
        {
            try
            {
                Cam_MeasurementBtn.Visibility = System.Windows.Visibility.Visible;              //vinod   show the button if process is cmplt
                StartBtn.Visibility = System.Windows.Visibility.Hidden;
                StopBtn.Visibility = System.Windows.Visibility.Hidden;
                RWrapper.RW_DelphiCamModule.MYINSTANCE().StopProcess();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelphiCamComp002", ex);
            }
        }
        private void Cam_MeasurementBtn_Click(object Sender, EventArgs e)                       //export cam profile data to excel
        {
            double Angular_step_4_IDTool_Msrmnt = Convert.ToDouble(Angular_step_4_IDTool_MsrmntTxtBx.Text);
            int NoofPts_Bundle_4_IDTool_Msrmnt = Convert.ToInt32(NoofPts_Bundle_4_IDTool_MsrmntTxtBx.Text);
            try
            {
                if (CamProfileShapeandMeasurementWin.ToolType_Inner)
                    RWrapper.RW_MainInterface.MYINSTANCE().GetCamProfileDistanceFromCenter(Angular_step_4_IDTool_Msrmnt, NoofPts_Bundle_4_IDTool_Msrmnt);
                else
                    RWrapper.RW_MainInterface.MYINSTANCE().GetCamProfileDistanceFromCenter(CamProfileShapeandMeasurementWin.AngularSteps, 1);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelphiCamComp004", ex);
            }
        }
        private void CompleteBtn_Click(object Sender, EventArgs e)              //for vision mode
        {
            try
            {
                Cam_MeasurementBtn.Visibility = System.Windows.Visibility.Visible;                  //vinod   show the button if process is cmplt
                CompleteBtn.Visibility = System.Windows.Visibility.Hidden;
                RWrapper.RW_DelphiCamModule.MYINSTANCE().StopProcess();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelphiCamComp002", ex);
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
