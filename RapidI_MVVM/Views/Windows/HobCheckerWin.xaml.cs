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
using System.ComponentModel;
using System.Threading;
using System.IO;
using System.Data;
using System.Data.OleDb;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for HobCheckerWin.xaml
    /// </summary>
    public partial class HobCheckerWin : Window
    {
        public static bool SendMeasurementsOnceOnly = false;        //previously measurements exports 2 times, now we will hack and send only once when this varr is true... vinod..
        private static HobCheckerWin _MyInstance;
        public static HobCheckerWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new HobCheckerWin();
            }
            return _MyInstance;
        }
        bool GettingCentreAxis = false;
        bool CycleStarted = false;
        DateTime CycleStartTime;
        TimeSpan ElapsedCycleTime;
        double dia1, MasterLength;
        int BC_StepNumber;
        int MasterCylinderStepNumber = 0, RunoutStepNumber = 0, FirstToothStepNumber = 0;
        bool CurrentMeasurementComplete;
        double LeadAngle, PressureAngle, Module, GashLead, RakeAngle;
        string ReportName;
        Grid[] MeasurementGrids;
        int Measurement_No, Measurements_To_Do;

       //System.Windows.Threading.DispatcherTimer clockWatch;
       System.Threading.Timer clockWatch;
        //Delegate void ClockTickHandler(object sender, EventArgs e);
       delegate void UpdateLabelDelegate(TimeSpan cycTime);

        /// <summary>
        /// Initialise the window
        /// </summary>
        public HobCheckerWin()
        {
            InitializeComponent();
            RWrapper.RW_DelphiCamModule.MYINSTANCE().DelphiCamEvent += new RWrapper.RW_DelphiCamModule.CompletedthisTask(FinishedTaskEventHandler);
            MeasurementGrids = new Grid[] { gridFormPos, gridSpacing, gridGashLead };
            DatePickerBx.SelectedDate = DateTime.Now;
            //clockWatch = new System.Windows.Threading.DispatcherTimer();
            //clockWatch.Tick += new EventHandler(ClockTicker);
            //clockWatch.Interval = new TimeSpan(0, 0, 1);
            //clockWatch.Start();


            SerialNo_TxtBx.SelectionChanged += SerialNumberChangedHandler;

            //Rapid.Utilities.StateObjClass stateObj = new Rapid.Utilities.StateObjClass();
            //stateObj.TimerCanceled = false;
            //stateObj.SomeValue = 1;
            //System.Threading.TimerCallback timerDelegate = new System.Threading.TimerCallback(TimerTask);
            clockWatch = new System.Threading.Timer(new TimerCallback(ClockTicker), null, 0, 1000);
            clockWatch.Change(0, 1000);
            //TimerCallback tcb = new TimerCallback(ClockTicker);
            //clockWatch = new Timer(ClockTicker, null, 0, 1000);
            LoadHobDataFromDB();
            //DatePickerBx.Text = DatePickerBx.SelectedDate.ToString();
            if (GlobalSettings.CompanyName == "Customised Technologies (P) Ltd")
            {
                Btn_SetCalib.Visibility = System.Windows.Visibility.Visible;
                Btn_StartRunOut_Copy.Visibility = System.Windows.Visibility.Visible;
                tempCalculator.Visibility = System.Windows.Visibility.Visible;
                chkY_Runout.Visibility = System.Windows.Visibility.Visible;
                BtnRestoreValues.Visibility = System.Windows.Visibility.Visible;
               // BtnSimulateExcel.Visibility = System.Windows.Visibility.Visible;
            }
        }

        private void ClockTicker(object obj) //(object sender, EventArgs e)
        {

            TimeSpan temp = ElapsedCycleTime;
            if (CycleStarted)
            {
                temp = DateTime.Now.Subtract(CycleStartTime);
                temp = temp.Add(ElapsedCycleTime);
                //if (temp.TotalSeconds < 1) return;
            }
            //else
            //    temp = new TimeSpan(0);

            this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                              new UpdateLabelDelegate(UpdateClockDisplay), temp);           //else
           //    temp = 0;
               // txtCycleTime.Text = "00:00";
              //prevTick = DateTime.Now;
            
      }

        private void UpdateClockDisplay(TimeSpan TimerValue)
        {
            //if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            //{
                txtCurrentTime.Text = String.Format("{0: HH:mm:ss}", DateTime.Now);
                txtCycleTime.Text = TimerValue.ToString("hh\\:mm\\:ss"); // String.Format("{0: mm\\:ss}",  temp);
            //}
            //else
                
        }

        //private void TimerTask(object stateObj)
        //{
        //    //Rapid.Utilities.StateObjClass st = (Rapid.Utilities.StateObjClass)stateObj;
        //    //System.Threading.Interlocked.Increment(ref st.SomeValue);
        //    //this.Dispatcher.BeginInvoke(
        //    if (st.TimerCanceled)
        //    {
        //        st.TimerReference.Dispose();
        //    }
        //}


        private void StartBtn_Click(object Sender, EventArgs e)
        {
            try
            {
                if (Start1Btn.Content.ToString() == "Start")
                {
                    if (chkFirstTooth.IsChecked == true)
                        Measurement_No = 5;
                    else if (chkFormPos.IsChecked == true)
                        Measurement_No = 1;
                    else if (chkGashSpacing.IsChecked == true && chkFormPos.IsChecked == false) // && chkLeadofGash.IsChecked == false)
                        Measurement_No = 2;
                    else if (chkLeadofGash.IsChecked == true && chkFormPos.IsChecked == false && chkGashSpacing.IsChecked == false)
                        Measurement_No = 3;
                    else
                    {
                        MessageBox.Show("Please choose the type of measurement you want to do before clicking on Start", "Rapid-I");
                        return;
                    }
                    Start1Btn.Content = "Pause";
                    CycleStarted = true; CycleStartTime = DateTime.Now;
                    ElapsedCycleTime = new TimeSpan(0);
                    StartMeasurement(true);
                }
                else if (Start1Btn.Content.ToString() == "Pause")
                {
                    Start1Btn.Content = "Resume";
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(true);
                    CycleStarted = false;
                    ElapsedCycleTime += DateTime.Now.Subtract(CycleStartTime);
                }
                else if (Start1Btn.Content.ToString() == "Resume")
                {
                    Start1Btn.Content = "Pause";
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(false);
                    CycleStartTime = DateTime.Now; CycleStarted = true;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:HobCheckerWin001", ex);
            }
        }
       
        private void StopBtn_Click(object Sender, EventArgs e)
        {
            try
            {
                Stop_Click(Start1Btn);
                if (Measurement_No == 1 && !CurrentMeasurementComplete)
                    gridFormPos.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
                if (Measurement_No == 2 && !CurrentMeasurementComplete)
                    gridSpacing.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
                if (Measurement_No == 3 && !CurrentMeasurementComplete)
                    gridGashLead.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:HobCheckerWin002", ex);
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
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:HobCheckerWin003", ex);
            }

        }

        /// <summary>
        /// Start Doing the Between Centres axis point picking and calculations
        /// The user has to touch the master cylinder for CNC mode operations to begin.
        /// </summary>
        /// <param name="Sender"></param>
        /// <param name="e"></param>
        private void GetCentreBtn_Click(object Sender, EventArgs e)
        {
            //Button Btn = (Button)Sender;
            //bool issChecked = Btn.IsChecked;
            if (GetCentreBtn.Content.ToString() == "Pause") // GettingCentreAxis)
            {
                RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(true);
                // If Checked, that is, calibration is running, stop it. 
                //RWrapper.RW_DelphiCamModule.MYINSTANCE().StopProcess();
               // if (this.GetCentreProgress.Value != this.GetCentreProgress.Maximum && GetCentreThreader != null)
                 //   GetCentreThreader.CancelAsync();
                GetCentreBtn.Content = "Resume";
                CycleStarted = false;
                ElapsedCycleTime += DateTime.Now.Subtract(CycleStartTime);
                //PauseGetCentre.IsEnabled = false;
            }
            else if (GetCentreBtn.Content.ToString() == "Start")
            {   //Start calibrating the cylinder
                //GetBCAxis Gbca = Call_BC_Axis;
                dia1 = Convert.ToDouble(Diameter_txtBox.Text);
                MasterLength = 1; // Convert.ToDouble(MasterLength_txtBox.Text);
                //Levels = 1; // Convert.ToInt32(Master_ZLevel_txtBox.Text);
                BC_StepNumber = 1;
                gb_CAxis.Background = new SolidColorBrush(Colors.White);
                MasterCylinderStepNumber = 0;
                Thread tt1 = new Thread(new ThreadStart(Call_BC_Axis));
                tt1.Start();
                CycleStarted = true; CycleStartTime = DateTime.Now;
                ElapsedCycleTime = new TimeSpan(0);
                //PauseGetCentre.IsEnabled = true;
                //this.Dispatcher.BeginInvoke(Call_BC_Axis, System.Windows.Threading.DispatcherPriority.Background); //RWrapper.RW_DelphiCamModule.MYINSTANCE().GetBC_Axis, Convert.ToDouble(Diameter_txtBox.Text);


                //if (!GetCentreThreader.IsBusy)
                //{
                //Measurement_No = 10;
                //if (GetCentreThreader == null)   GetCentreThreader = new BackgroundWorker();
                //GetCentreThreader.WorkerSupportsCancellation = true;
                //GetCentreThreader.DoWork += new DoWorkEventHandler(GetCentreThreader_DoWork);
                //GetCentreThreader.RunWorkerCompleted += new RunWorkerCompletedEventHandler(GetCentreThreader_RunWorkerCompleted);
                //GetCentreThreader.ProgressChanged += new ProgressChangedEventHandler(GetCentreThreader_ProgressChanged);
                GetCentreBtn.Content = "Pause";
                Measurement_No = 0;
                //GetCentreThreader.RunWorkerAsync();
                //}
            }
            else
            {
                RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(false);
                GetCentreBtn.Content = "Pause";
                CycleStartTime = DateTime.Now; CycleStarted = true;
            }
            GettingCentreAxis = !GettingCentreAxis;
        }

        /// <summary>
        /// This button decides wether the value given is DP (inches) or Module (mm)
        /// </summary>
        /// <param name="Sender"></param>
        /// <param name="e"></param>
        private void ModuleType_Click(object Sender, EventArgs e)
        {
            try
            {
                System.Windows.Controls.Primitives.ToggleButton Btn = Btn_ModuleType;
                if (Btn == null) return;
                //    (System.Windows.Controls.Primitives.ToggleButton)Sender;
                if (Btn.IsChecked == true & Btn.Content != "") //It is DP. Convert value to Module from inches to mm
                {
                    Btn.Content = "DP ";
                    Module = 25.4 / Convert.ToDouble(Module_TxtBx.Text);
                }
                else
                {
                    Btn.Content = "Module ";
                    Module = Convert.ToDouble(Module_TxtBx.Text);
                }
            }
            catch (Exception ex)
            { 

            }
            return;
        }

        /// <summary>
        /// The Y-axis is moved to the Between Centres axis coordinate. It doesnt move in any other axis. 
        /// Be careful when clicking on the button - if component is present, it will go and hit it!
        /// </summary>
        /// <param name="Sender"></param>
        /// <param name="e"></param>
        private void Btn_GotoCentre_Click(object Sender, EventArgs e)
        {
            if ((bool)RightMoveRBtn.IsChecked)
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[4] = 1;
            else
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[4] = -1;
            Button btn = (Button)Sender;
            //bool GotoHome = false;
            //if (btn.Content.ToString().Contains("Home"))
            //    GotoHome = true;
            RWrapper.RW_CNC.MYINSTANCE().MoveToC_Axis_Y_Position(); //GotoHome);
        }

        /// <summary>
        /// The event handler when the CamModule raises the event at the end of each step. 
        /// </summary>
        /// <param name="FractionCompleted">The fraction of work completed</param>
        private void FinishedTaskEventHandler(double FractionCompleted)
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                GetCentreProgress.Value = Convert.ToInt32(FractionCompleted * 100);
                // GetCentreThreader.ReportProgress(Convert.ToInt32(TaskType * 100));
                switch (Measurement_No)
                {
                    case 0: //Master Cylinder Measurement
                        if (FractionCompleted == 1)
                        {
                            if (MasterCylinderStepNumber == 0)
                            {
                                MasterCylinderStepNumber++;
                                double AngToMove = 0;
                                if (BC_StepNumber != 1)
                                    AngToMove = 2 * Math.PI / BC_StepNumber;
                                RWrapper.RW_DelphiCamModule.MYINSTANCE().CalculatePtsFor_BC_Axis(AngToMove);
                            }
                            else if (MasterCylinderStepNumber < BC_StepNumber)
                            {
                               MasterCylinderStepNumber++;
                               //MessageBox.Show("StepNumber = " + Convert.ToString(MasterCylinderStepNumber));
                               RWrapper.RW_DelphiCamModule.MYINSTANCE().RunMasterCylinder_Rotation(MasterCylinderStepNumber);
                            }

                            else if (MasterCylinderStepNumber == BC_StepNumber)
                            {
                                MasterCylinderStepNumber = 1000;
                                RWrapper.RW_DelphiCamModule.MYINSTANCE().RunMasterCylinder_Rotation(MasterCylinderStepNumber);
                                 if (GettingCentreAxis) Stop_Click(GetCentreBtn);
                                 GettingCentreAxis = false;
                                CentreX.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[0]);
                                CentreY.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[1]);
                                gb_CAxis.Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                                ////GetCentreBtn_Click(null, null);
                               // Btn_Reset_Calib_Click(null, null);
                               //  MasterCylinderStepNumber = 1;
                               //RWrapper.RW_DelphiCamModule.MYINSTANCE().CalculatePtsFor_BC_Axis(0);
                            }
                        }
                        if (FractionCompleted == -1000)
                        {
                            //if (GetCentreBtn.Content.ToString() == "Pause")
                                //GetCentreBtn_Click(null, null);
                            //PauseGetCentre.Content = "Resume";
                            
                        }
                        break;
                    case 1:
                        if (FractionCompleted == 1.0)
                        {
                            //MeasurementGrids[Measurement_No - 1].Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                            CompleteMeasurementReports();
                            if ((bool)chkAutoMeasure.IsChecked)
                            {
                                if ((bool)chkGashSpacing.IsChecked)
                                {
                                    Measurement_No = 2; StartMeasurement(false);
                                }
                                else if ((bool)chkLeadofGash.IsChecked)
                                {
                                    Measurement_No = 3; StartMeasurement(false);
                                }
                            }                             
                        }
                        break;
                   
                    case 2:
                        if (FractionCompleted == 1.0)
                        {
                            CompleteMeasurementReports();
                            if ((bool)chkAutoMeasure.IsChecked && (bool)chkLeadofGash.IsChecked)
                            {
                                //MeasurementGrids[Measurement_No - 1].Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                                Measurement_No = 3;
                                StartMeasurement(false);
                            }
                        }
                        break;
                    
                    case 3:
                        if (FractionCompleted == 1.0)
                        {
                            MeasurementGrids[Measurement_No - 1].Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                            CompleteMeasurementReports();
                            //if ((bool)chkAutoMeasure.IsChecked)
                            //    ShowExcelReport();                                
                        }
                        break;
                    case 4: // Runout correction is On. Code will come here after each stage is complete. 
                        if (FractionCompleted == 1.0)
                        {
                            if (RunoutStepNumber == 0)
                            {
                                RunoutStepNumber++;
                                RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_RunoutCorrection(RunoutStepNumber, 5, (bool)chkY_Runout.IsChecked); // Convert.ToInt32(txt_PtsCt.Text));
                            }
                            else if (RunoutStepNumber == -1)
                                break;
                            else if (RunoutStepNumber < 5)
                            {   //First stage complete. Go to the next stage
                                if (FractionCompleted == 1)
                                {
                                    RunoutStepNumber++;
                                    //MessageBox.Show(Convert.ToString(RunoutStepNumber), "Rapid-I");
                                    //if (RunoutStepNumber == 4) RunoutStepNumber++;
                                    if (!(bool)chkDoRotaryRunout.IsChecked) // && RunoutStepNumber != 3)
                                        RunoutStepNumber++;
                                    RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_RunoutCorrection(RunoutStepNumber, 5, (bool)chkY_Runout.IsChecked); //Convert.ToInt32(txt_PtsCt.Text));
                                }
                            }
                            else if (RunoutStepNumber == 5)
                            {
                                //2nd step is complete. Call calculation and implementation of Runout Correction
                                if (FractionCompleted == 1)
                                {
                                    RunoutStepNumber = 1000;
                                    bool finishedRunout = RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_RunoutCorrection(RunoutStepNumber, 5, (bool)chkY_Runout.IsChecked);
                                    //MessageBox.Show(Convert.ToString(finishedRunout));
                                    try
                                    {
                                        if (finishedRunout)
                                        {
                                            GettingCentreAxis = false;
                                            CentreX.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[0]);
                                            CentreY.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[1]);
                                            if ((bool)chkDoRotaryRunout.IsChecked) // && RunoutStepNumber != 3)
                                            {
                                                tb_Runout_Top.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().Hob_RunoutParams[4]);
                                                tb_Runout_Bottom.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().Hob_RunoutParams[5]);
                                            }
                                            gb_Runout.Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                                            Stop_Click(Btn_StartRunOut);
                                        }
                                        else
                                        {
                                            StopRunoutClick(null, null);
                                        }
                                    }
                                    catch (Exception ex)
                                    {
                                        MessageBox.Show("Could not complete the final steps of Runout correction");
                                    }
                                }
                            }
                        }
                        break;
                    case 5:
                        if (FractionCompleted == 1)
                        {
                            int tempC = FirstToothStepNumber;
                            if (FirstToothStepNumber > 100 && FirstToothStepNumber < 200)
                                tempC -= 100;
                            else if (FirstToothStepNumber < 300)
                            {
                                tempC -= 200;
                                if (tempC == 2) tempC++;
                            }
                            //MessageBox.Show("Finished this step");
                            //if ((bool)chkMasterHob.IsChecked)
                            //    FirstToothStepNumber++;//tempC = 300;
                            if (tempC < 3)
                            {
                                FirstToothStepNumber++;
                                RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_StartToothParameters(FirstToothStepNumber);
                            }
                            else
                            {
                                FirstToothStepNumber = 1000;
                                if ((bool)chkMasterHob.IsChecked) FirstToothStepNumber = 2000;
                                RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_StartToothParameters(FirstToothStepNumber);
                                GridFirstTooth.Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                                CalculatedOD.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[15]);
                                Stop_Click(Btn_FirstToothStart);
                                ToothDepth_Spacing_TxtBx.Background = Brushes.LightGray; // new SolidColorBrush(Color.FromRgb(102, 102, 102));
                                ToothDepth_Spacing_TxtBx.Text = string.Format("{0:F4}", 1.125 * Module);
                                ToothDepth_Lead.Background = Brushes.LightGray; //new SolidColorBrush(Color.FromRgb(102, 102, 102));
                                ToothDepth_Lead.Text = string.Format("{0:F4}", 1.125 * Module);
                                ToothDepth_TxtBx.Background = Brushes.LightGray; //new SolidColorBrush(Color.FromRgb(102, 102, 102));
                                ToothDepth_TxtBx.Text = string.Format("{0:F4}", 2.25 * Module);
                                C_Axis0.Text = ConvertRadianstoDegrees(RWrapper.RW_DelphiCamModule.MYINSTANCE().ToothMidPt[3]);
                                if ((bool)chkAutoMeasure.IsChecked)
                                {
                                    if ((bool)chkFormPos.IsChecked)
                                    {
                                        Measurement_No = 1; StartMeasurement(true);
                                    }
                                    if ((bool)chkGashSpacing.IsChecked)
                                    {
                                        Measurement_No = 2; StartMeasurement(false);
                                    }
                                    else if ((bool)chkLeadofGash.IsChecked)
                                    {
                                        Measurement_No = 3; StartMeasurement(false);
                                    }
                                }
                            }
                        }
                        break;
                }
            }
            else
            {
                this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_DelphiCamModule.CompletedthisTask(FinishedTaskEventHandler), FractionCompleted);
            }   
        }

        /// <summary>
        /// Removes the current values for BC-centre, if we want to do again. But internally, the value is not reset until we
        /// analyse axis again. 
        /// </summary>
        /// <param name="Sender"></param>
        /// <param name="e"></param>
        private void Btn_Reset_Calib_Click(object Sender, EventArgs e)
        {   
            //Color cl = new Color();
            //cl.A = 255;cl.R = 192; cl.G = 192; cl.B = 255;
            //gb_CAxis.Background = new SolidColorBrush(cl);
            gb_CAxis.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            CentreX.Text = "-.-"; CentreY.Text = "-.-";
            GetCentreProgress.Value = 0;
            MasterCylinderStepNumber = 0; CycleStarted = false;
            RWrapper.RW_DelphiCamModule.MYINSTANCE().Reset_BC_Axis();
            if (!CycleStarted) ElapsedCycleTime = new TimeSpan(0);
        }

        /// <summary>
        /// This method is called in a separate thread for calculating Between centres axis centre
        /// </summary>
        private void Call_BC_Axis()
        {
            RWrapper.RW_DelphiCamModule.MYINSTANCE().GetBC_Axis(dia1,(int) MasterLength, BC_StepNumber);
            //RWrapper.RW_DelphiCamModule.MYINSTANCE().RunMasterCylinder_Rotation(MasterCylinderStepNumber);
        }

        private void Call_BC_Axis1(double ddia)
        {
            RWrapper.RW_DelphiCamModule.MYINSTANCE().GetBC_Axis(ddia, (int) MasterLength,1);// Convert.ToInt32(Master_ZLevel_txtBox.Text));
        }

        private void StartFirstToothThread()
        {
            RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_StartToothParameters(FirstToothStepNumber);
        }
        /// <summary>
        /// This is the method that will be called in a separate thread to let the front end work by itself.
        /// </summary>
        private void Call_HobMeasure()
        {
            RWrapper.RW_DelphiCamModule.MYINSTANCE().Start(Measurement_No);
        }

        private void Window_DragEnter(object sender, DragEventArgs e)
        {

        }

        //private void PauseCentre_Click(object sender, EventArgs e)
        //{
        //    if (GettingCentreIsPaused)
        //    {
        //        GettingCentreIsPaused = false;
        //        PauseGetCentre.Content = "Pause";
        //    }
        //    else
        //    {
        //        GettingCentreIsPaused = true;
        //        PauseGetCentre.Content = "Resume";
        //    }
        //    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(GettingCentreIsPaused);
        //}


        private void StopGetCentreClick(object sender, EventArgs e)
        {
            Stop_Click(GetCentreBtn);
            if (!RWrapper.RW_DelphiCamModule.MYINSTANCE().GotAxis)
                gb_CAxis.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));

        }

        private void Btn_SetCalib_Click(object sender, EventArgs e)
        {
            RWrapper.RW_DelphiCamModule.MYINSTANCE().Set_C_Axis();
            Measurement_No = 0;
            //FinishedTaskEventHandler(1);
            MasterCylinderStepNumber = 1000;
           // RWrapper.RW_DelphiCamModule.MYINSTANCE().RunMasterCylinder_Rotation(MasterCylinderStepNumber);
            if (GettingCentreAxis) Stop_Click(GetCentreBtn);
            CentreX.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[0]);
            CentreY.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[1]);
            gb_CAxis.Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));

        }

        private void Set_Runout_Click(object sender, EventArgs e)
        {
            RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated = Set_HobParameters(1);
            if (!RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated) return;

            RWrapper.RW_DelphiCamModule.MYINSTANCE().Set_Runout();
            Measurement_No = 4; RunoutStepNumber = 1000;
            CentreX.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[0]);
            CentreY.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().BC_Values[1]);
            //tb_Runout_Top.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().Hob_RunoutParams[4]);
            //tb_Runout_Bottom.Text = string.Format("{0:F4}", RWrapper.RW_DelphiCamModule.MYINSTANCE().Hob_RunoutParams[5]);
            gb_Runout.Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
        }

        private void BtnStartRunout_Click(object sender, EventArgs e)
        {
            if (Btn_StartRunOut.Content.ToString() == "Start")
            {
                if (CycleStarted && Measurement_No != 4) return;

                Measurement_No = 4;
                ResetAll_Background(3);
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated = Set_HobParameters(1);
                if (!RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated) return;
                //We got all required parameters; let us start the cycle
                RWrapper.RW_DelphiCamModule.MYINSTANCE().Reset_BC_Axis();
                double hubdia = Convert.ToDouble(Diameter_txtBox.Text);
                StreamWriter sw = new StreamWriter(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\AxisLog\\SpacingPts.csv", true);
                sw.WriteLine(SerialNo_TxtBx.Text + ", Runout, " + DateTime.Now.Date.ToShortDateString() + ", " + DateTime.Now.ToShortTimeString());
                sw.Close();

                if (hubdia > 0)
                {
                    dia1 = Convert.ToDouble(Diameter_txtBox.Text);
                    RunoutStepNumber = 0;
                    Thread tt1 = new Thread(new ThreadStart(Call_BC_Axis));
                    tt1.Start();
                    //RunoutCorrectionDone = false; 
                    //RWrapper.RW_DelphiCamModule.MYINSTANCE().Run_RunoutCorrection(RunoutStepNumber, 5, (bool)chkY_Runout.IsChecked); // Convert.ToInt32(txt_PtsCt.Text));
                }
                Btn_StartRunOut.Content = "Pause";
                CycleStarted = true; CycleStartTime = DateTime.Now;
                gb_Runout.Background = new SolidColorBrush(Color.FromRgb(255, 255, 255));
            }
            else if (Btn_StartRunOut.Content.ToString() == "Pause")
            {
                if (RunoutStepNumber != 1000)
                {
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(true);
                    Btn_StartRunOut.Content = "Resume";
                    CycleStarted = false;
                    ElapsedCycleTime += DateTime.Now.Subtract(CycleStartTime);
                }
            }
            else
            {
                RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(false);
                Btn_StartRunOut.Content = "Pause";
                CycleStartTime = DateTime.Now; CycleStarted = true;
            }
        }

        private void StopRunoutClick(object sender, EventArgs e)
        {
            Stop_Click(Btn_StartRunOut);
            if (RunoutStepNumber < 1000)
                gb_Runout.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
        }

        void Stop_Click(Button bb)
        {
            try
            {
                bb.Content = "Start";
                ElapsedCycleTime += DateTime.Now.Subtract(CycleStartTime);
                CycleStarted = false;
                GettingCentreAxis = false;
                RWrapper.RW_DelphiCamModule.MYINSTANCE().StopProcess();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:HobCheckerWin002", ex);
            }

        }

        private bool Set_HobParameters(int Level)
        {
            try
            {
                //Set the measurements to be done.... 
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[19] = Measurements_To_Do;
                //HobODiameter//
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[3] = Convert.ToDouble(OuterDiameter_TxtBx.Text);
                //Active Hob Length
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[9] = Convert.ToDouble(ActiveLength_txtBox.Text);
                //Total Hob Length
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[17] = Convert.ToDouble(TotalLength_TxtBox.Text);
                double HubWidth = (RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[17] - RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[9]) / 2;
                if (HubWidth < 0)
                {
                    MessageBox.Show("The total length of the hob is entered to be smaller than the active length!", "Rapid-I");
                    return false;
                }
                else if (HubWidth > 30)
                {
                    MessageBox.Show("Please Check the Total Length you have entered. Something is not OK!", "Rapid-I");
                    return false;
                }
                //Hub Diameter
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[14] = Convert.ToDouble(Diameter_txtBox.Text);
                if (Level < 2) //We have to do Runout checking. 
                {
                    //if (txt_PtsCt.Text == "") return false;
                    return true;
                }

                //Rake Face direction Front = 1;  Back = -1;
                if ((bool)RightMoveRBtn.IsChecked)
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[4] = 1; //Front
                else
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[4] = -1; //Back

                //Helics_Direction RH = -1; LH = 1;
                if ((bool)UpRBtn.IsChecked)
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[5] = 1; //LH
                else
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[5] = -1; //RH
                //Module = 
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[12] = Module; // Convert.ToDouble(Module_TxtBx.Text);

                if ((bool)chkCutterMode.IsChecked)
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[20] = 1;
                else
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[20] = 0;

                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[13] = GashLead;
                double pA = 0;
                pA = Convert.ToDouble(PressureAngle) * Math.PI / 180;
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[16] = pA;
                //First Tooth done at this tooth from Top;
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[18] = Convert.ToDouble(Txt_FirstTooth.Text);
                //Noofgashes = 
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[6] = Convert.ToDouble(NoofGashes_TxtBx.Text);

                //startDepthofTooth
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[1] = Convert.ToDouble(StartDepth_TxtBx.Text);
               
                if (Level < 5)
                    return true;

                //Rake Angle -- formerly used for Lead Angle. Not needed anymore 22 Apr 
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[10] = Convert.ToDouble(RakeAngle);
                               

                //Teeth per gash
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[8] = Convert.ToDouble(NumberofTeeth_TxtBx.Text);
                //NoOfStarts = 
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[11] = Convert.ToInt32(NoofStarts_TxtBx.Text);
                //Gash_Pitch = 
                //if (Btn_GashLead.Content.ToString().Contains("Angle"))
                //{
                //    GashLead = Math.PI * RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[3] * Math.Tan(LeadAngle * Math.PI / 180);
                //}
                //else
                    //GashLead = Convert.ToDouble(leadofGash_TxtBx.Text);

                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[0] = Measurement_No;

                switch (Measurement_No)
                {
                    case 1:
                        //TotalToothDepth = 
                        RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[2] = Convert.ToDouble(ToothDepth_TxtBx.Text);
                        //NoofPointOnTooth = 
                        RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[7] = Convert.ToInt32(PointsPerTooth_Face_TxtBx.Text);
                        break;
                    case 2:
                        //TotalToothDepth = 
                        RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[2] = Convert.ToDouble(ToothDepth_Spacing_TxtBx.Text);
                        break;
                    case 3:
                        //TotalToothDepth = 
                        RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[2] = Convert.ToDouble(ToothDepth_Lead.Text);
                        break;
                }
                return true;
            }
            catch (Exception ex)
            {
                if (Level < 2)
                    MessageBox.Show("Please ensure you have entered the Hob Outer Dia", "Rapid-I");
                else if (Level < 5)
                    MessageBox.Show("Please ensure you have entered Lead and Gash properties", "Rapid-I");
                else
                    MessageBox.Show("Please enter all parameters before clicking on Start", "Rapid-I");
                return false;
            }
        }

        private void Btn_BeginFirstToothMeasure_Click(object sender, EventArgs e)
        {
            try
            {
                Button btn = (Button)sender;
                if (btn.Content.ToString() == "Start")
                {
                    if (CycleStarted && Measurement_No != 5) return;

                    //if (RunoutStepNumber != 1000)
                    //{
                    //    MessageBox.Show("Please complete Run-Out correction first!", "Rapid-I", MessageBoxButton.OK);
                    //    return;
                    //}
                    //if (FirstToothStepNumber 
                    Measurement_No = 5;
                    ResetAll_Background(2);
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated = Set_HobParameters(4);
                    if (!RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated) return;

                    if ((bool)rb_4Pt.IsChecked)
                        FirstToothStepNumber = 1;
                    else
                        FirstToothStepNumber = 101;

                    if ((bool)chkMasterHob.IsChecked)
                        FirstToothStepNumber = 201;

                    StreamWriter sw = new StreamWriter(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\AxisLog\\SpacingPts.csv", true);
                    sw.WriteLine(SerialNo_TxtBx.Text + ", FirstTooth, " + DateTime.Now.Date.ToShortDateString() + ", " + DateTime.Now.ToShortTimeString());
                    sw.Close();
                    Thread tt1 = new Thread(new ThreadStart(StartFirstToothThread));
                    tt1.Start();

                    GridFirstTooth.Background = new SolidColorBrush(Color.FromRgb(255, 255, 255));
                    CycleStarted = true; CycleStartTime = DateTime.Now;
                    btn.Content = "Pause";
                    WriteFrontEndValuesToFile();
                }
                else if (btn.Content.ToString() == "Pause")
                {
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(true);
                    btn.Content = "Resume";
                    CycleStarted = false;
                    ElapsedCycleTime += DateTime.Now.Subtract(CycleStartTime);
                }
                else
                {
                    RWrapper.RW_DelphiCamModule.MYINSTANCE().countinue_pause_delphicammodule(false);
                    btn.Content = "Pause";
                    CycleStartTime = DateTime.Now; CycleStarted = true;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:HobCheckerWin006", ex);
            }
        }

        private void Btn_StopFirstTooth_Click(object sender, EventArgs e)
        {
            Stop_Click(Btn_FirstToothStart);
            if (FirstToothStepNumber < 1000)
                GridFirstTooth.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
        }

        private void Btn_AutoMeasure_Click(object sender, EventArgs e)
        {
            CheckBox btn = (CheckBox)sender;
            RWrapper.RW_DelphiCamModule.MYINSTANCE().AutoMeasureMode = (bool)btn.IsChecked;
        }

        private void CompleteMeasurementReports()
        {
            bool finishedReportGen = false;
            try
            {
                CurrentMeasurementComplete = true;
                MeasurementGrids[Measurement_No - 1].Background = new SolidColorBrush(Color.FromRgb(187, 255, 168));
                GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNTHobChecker(Measurement_No); // Radius_mn2, TrueAngle_mn2, (int)RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[7]);
                    //vinod....this is to set setting for hobcheckertemplate. 
                if (GlobalSettings.MainExcelInstance.IsHobCheckerMeasurementNo123(Measurement_No, true))
                {
                    if (GlobalSettings.MainExcelInstance.Reset_All(true, false))
                    {
                        GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
                        GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                        if (GlobalSettings.MainExcelInstance.GenerateExcel(true, true))
                        {
                            finishedReportGen = true;
                            //MessageBox.Show("Completed Measurements successfully");
                        }

                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Could not write Form Data to Report" + "                " + ex.Message, "Rapid-I");
            }

            RWrapper.RW_DelphiCamModule.MYINSTANCE().DoingHobMeasurement = false;

            //RWrapper.RW_DelphiCamModule.MYINSTANCE().StopProcess();

            if (Measurement_No == 1)
                if ((bool)chkAutoMeasure.IsChecked && (((bool)chkGashSpacing.IsChecked || (bool)chkLeadofGash.IsChecked)))
                    return;
            if (Measurement_No == 2)
                if ((bool)chkAutoMeasure.IsChecked && (bool)chkLeadofGash.IsChecked)
                    return;

            if (finishedReportGen) ShowExcelReport();

            //if ((Measurement_No == 1 && Measurements_To_Do < 4) ||
            //    (Measurement_No == 2 && Measurements_To_Do < 8) ||
            //    (Measurement_No == 3)
            //    )
            StopBtn_Click(null, null);
            Thread.Sleep(1000);
            StartBtn_Click(null, null);
        }

        private void Angle_ChangedHandler(object sender, EventArgs e)
        {
            try
            {
                TextBox tb = (TextBox)sender;
                if (tb.Text == "") return;
                
                double angg = GetAngleValue(tb.Text);
                if (double.IsNaN(angg)) throw new Exception();

                if (tb.Name.Contains("Lead"))
                    LeadAngle = angg;
                else if (tb.Name.Contains("Pressure"))
                    PressureAngle = angg;
                else if (tb.Name.Contains("Rake"))
                    RakeAngle = angg;

                if (tb.Name.Contains("Gash"))
                {
                    if (!Btn_GashLead.Content.ToString().Contains("Angle"))
                    {
                        GashLead = Convert.ToDouble(leadofGash_TxtBx.Text);
                        //return;
                    }
                    else
                    {
                        GashLeadString = leadofGash_TxtBx.Text;
                        angg = 90 - angg;
                        GashLead = Math.PI * Convert.ToDouble(OuterDiameter_TxtBx.Text) * Math.Tan(angg * Math.PI / 180);
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Please check the value you have entered. Use only numbers and ';'", "Rapid-I");
            }
        }

        private void Reset_FirstTooth(object sender, EventArgs e)
        {
            GridFirstTooth.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            RWrapper.RW_DelphiCamModule.MYINSTANCE().FirstToothDone = false;
            CycleStarted = false;
            CalculatedOD.Text = "-.-";  C_Axis0.Text = "-.-";
            ResetAll_Background(2);
            if (!CycleStarted) ElapsedCycleTime = new TimeSpan(0);
        }

        private void Reset_Runout(object sender, EventArgs e)
        {
            gb_Runout.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            gridFormPos.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            gridGashLead.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            gridSpacing.Background = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            RunoutStepNumber = 0; CycleStarted = false;
            RWrapper.RW_DelphiCamModule.MYINSTANCE().Reset_RunoutValues();
            GetCentreProgress.Value = 0;
            MasterCylinderStepNumber = 0; CycleStarted = false;
            RWrapper.RW_DelphiCamModule.MYINSTANCE().Reset_BC_Axis();
            Reset_FirstTooth(null, null);
            ClearTextboxes(wpRunoutVal);
            ResetAll_Background(3);
            tb_Runout_Bottom.Text = "-.-";
            tb_Runout_Top.Text = "-.-";
            if (!CycleStarted) ElapsedCycleTime = new TimeSpan(0);
        }

        private void StartMeasurement(bool FirstTimeRunningforThisHob)
        {
            try
            {
                CurrentMeasurementComplete = false;
                RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated = Set_HobParameters(8);
                if (!RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParametersUpdated) return;
                //Write Header in pointsfile
                string Meastype = "Generic";
                switch (Measurement_No)
                {
                    case 0:
                        Meastype = "Master_Calibration";
                        break;
                    case 1:
                        Meastype = "Form_Position";
                        break;
                    case 2:
                        Meastype = "Indexing_Gash";
                        break;
                    case 3:
                        Meastype = "Gash_Lead";
                        break;
                }
                MeasurementGrids[Measurement_No - 1].Background = new SolidColorBrush(Color.FromRgb(255, 255, 255));
                StreamWriter sw = new StreamWriter(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Log\\AxisLog\\SpacingPts.csv", true);
                sw.WriteLine(SerialNo_TxtBx.Text + ", " + Meastype + ", " + DateTime.Now.Date.ToShortDateString() + ", " + DateTime.Now.ToShortTimeString());
                sw.Close();
                Thread tt1 = new Thread(new ThreadStart(Call_HobMeasure));
                tt1.Start();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:HobCheckerWin005-a", ex);
            }
            //After the measurement starts, lets make the report file and start entering data...
            //First Check if we have already made the report excel file for current hob - 
            if (FirstTimeRunningforThisHob)
            {
                ResetAll_Background(1);
                System.Collections.ArrayList HobPersonalData = new System.Collections.ArrayList();
                try
                {
                    string CustomerName, _Date, Operator; //, AccuracyClass;
                    CustomerName = CustomerName_TxtBx.Text;
                    _Date = DatePickerBx.Text;
                    Operator = Operator_TxtBx.Text;
                    string NowDate = DateTime.Now.Date.ToShortDateString();
                    ReportName = GlobalSettings.ReportPath + "\\" + SerialNo_TxtBx.Text + "-" + NowDate + ".xls";
                    int thisReportNameIndex = 0;
                    while (System.IO.File.Exists(ReportName))
                    {
                        thisReportNameIndex++;
                        ReportName = GlobalSettings.ReportPath + "\\" + SerialNo_TxtBx.Text + "-" + NowDate + "-" + thisReportNameIndex + ".xls";
                    }

                    //System.Collections.Generic.List<System.Collections.ArrayList> HobPersonalData = new System.Collections.Generic.List<System.Collections.ArrayList>();
                    HobPersonalData.Add(Module);
                    HobPersonalData.Add(NoofGashes_TxtBx.Text);
                    HobPersonalData.Add(GashLead);
                    if (RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[5] == 1)
                        HobPersonalData.Add("LH");
                    else
                        HobPersonalData.Add("RH");

                    HobPersonalData.Add(NoofStarts_TxtBx.Text);
                    HobPersonalData.Add(LeadAngle);
                    HobPersonalData.Add(RWrapper.RW_DelphiCamModule.MYINSTANCE().HobParameters[15]);
                    HobPersonalData.Add(ActiveLength_txtBox.Text);
                    //Fill up for Tooth Depth
                    if (FirstToothStepNumber == 1000)
                        HobPersonalData.Add(2.25 * Module);
                    else
                        HobPersonalData.Add(ToothDepth_TxtBx.Text);

                    TextBlock AccB = (TextBlock)AccuracyClass_TxtBox.SelectedItem;
                    HobPersonalData.Add(AccB.Text);
                    HobPersonalData.Add(PressureAngle_TxtBx.Text);
                    HobPersonalData.Add(MachineNo_TxtBx.Text);
                    HobPersonalData.Add(NowDate);
                    HobPersonalData.Add(CustomerName);
                    HobPersonalData.Add(OrderNo_TxtBx.Text);
                    HobPersonalData.Add(SerialNo_TxtBx.Text);
                    HobPersonalData.Add(Operator);

                    //Writing parameters and current run to hob details dB
                    HobDetails.Tables[3].Clear();
                    oDA = new OleDbDataAdapter("SELECT * FROM HobParams", basicConnectString);
                    OleDbCommandBuilder cbd3 = new OleDbCommandBuilder(oDA);
                    oDA.Fill(HobDetails.Tables[3]);
                    bool WritetoDB = true;
                    for (int i = 0; i < HobDetails.Tables[3].Rows.Count; i++)
                    {
                        if (HobDetails.Tables[3].Rows[i]["SerialNumber"].ToString() == SerialNo_TxtBx.Text)
                        {
                            WritetoDB = false;
                            break;
                        }
                    }
                    if (WritetoDB)
                    {
                        int custID = CheckForExistingValue(CustomerName_TxtBx.Text, HobDetails.Tables[0], "CustomerName");
                        if (custID < 0)
                        {
                            HobDetails.Tables[0].Rows.Clear();
                            oDA = new OleDbDataAdapter("SELECT * FROM CustomerDetails", basicConnectString);
                            OleDbCommandBuilder cbd = new OleDbCommandBuilder(oDA);
                            oDA.Fill(HobDetails.Tables[0]);

                            DataRow ddr = HobDetails.Tables[0].NewRow();
                            ddr["CustomerName"] = CustomerName_TxtBx.Text;
                            HobDetails.Tables[0].Rows.Add(ddr);
                            oDA.Update(HobDetails.Tables[0]);
                            HobDetails.Tables[0].Rows.Clear();
                            oDA.Fill(HobDetails.Tables[0]);
                            //UpdateTable("CustomerDetails"); //, HobDetails.Tables[0]);
                            custID = (int)HobDetails.Tables[0].Rows[HobDetails.Tables[0].Rows.Count - 1]["Customer_ID"];
                        }

                        int operatorID = CheckForExistingValue(Operator_TxtBx.Text, HobDetails.Tables[1], "OperatorName");
                        if (operatorID < 0)
                        {
                            HobDetails.Tables[1].Clear();
                            oDA = new OleDbDataAdapter("SELECT * FROM CustomerDetails", basicConnectString);
                            OleDbCommandBuilder cbd = new OleDbCommandBuilder(oDA);
                            oDA.Fill(HobDetails.Tables[1]);

                            DataRow ddr = HobDetails.Tables[1].NewRow();
                            ddr["OperatorName"] = Operator_TxtBx.Text;
                            HobDetails.Tables[1].Rows.Add(ddr);
                            oDA.Update(HobDetails.Tables[1]);
                            HobDetails.Tables[1].Clear();
                            oDA.Update(HobDetails.Tables[1]);
                            operatorID = (int)HobDetails.Tables[0].Rows[HobDetails.Tables[0].Rows.Count - 1]["Operator_ID"];
                        }
                        //else
                        //    dr["Operator_ID"] = operatorID;

                        int machineID = CheckForExistingValue(MachineNo_TxtBx.Text, HobDetails.Tables[2], "MachineNumber");
                        if (machineID < 0)
                        {
                            HobDetails.Tables[2].Clear();
                            oDA = new OleDbDataAdapter("SELECT * FROM MachineDetails", basicConnectString);
                            OleDbCommandBuilder cbd = new OleDbCommandBuilder(oDA);
                            oDA.Fill(HobDetails.Tables[2]);

                            DataRow ddr = HobDetails.Tables[2].NewRow();
                            ddr["MachineNumber"] = Operator_TxtBx.Text;
                            HobDetails.Tables[1].Rows.Add(ddr);
                            oDA.Update(HobDetails.Tables[2]);
                            HobDetails.Tables[2].Clear();
                            oDA.Update(HobDetails.Tables[2]);
                            machineID = (int)HobDetails.Tables[0].Rows[HobDetails.Tables[0].Rows.Count - 1]["Machine_ID"];
                        }
                        //else
                        //    dr["Machine_ID"] = machineID;

                        for (int ii = 0; ii < HobDetails.Tables[3].Rows.Count; ii++)
                        {
                            if ((HobDetails.Tables[3].Rows[ii]["SerialNumber"]).ToString() == SerialNo_TxtBx.Text)
                            {
                                WritetoDB = true;
                                break;
                            }
                        }
                        if (WritetoDB)
                        {
                            DataRow dr = HobDetails.Tables[3].NewRow();
                            dr["Customer_ID"] = custID;
                            dr["Operator_ID"] = operatorID;
                            dr["Machine_ID"] = machineID;
                            dr["SerialNumber"] = SerialNo_TxtBx.Text;

                            dr["HubDia"] = Diameter_txtBox.Text;
                            dr["OuterDia"] = OuterDiameter_TxtBx.Text;
                            dr["ActiveLength"] = ActiveLength_txtBox.Text;
                            dr["TotalLength"] = TotalLength_TxtBox.Text;
                            if ((bool)UpRBtn.IsChecked)
                                dr["LeadDirection"] = 1;
                            else
                                dr["LeadDirection"] = -1;
                            dr["HobModule"] = Module;
                            dr["NumberOfStarts"] = NoofStarts_TxtBx.Text;
                            dr["AccuracyClass"] = AccuracyClass_TxtBox.SelectedIndex;
                            dr["GashLead"] = leadofGash_TxtBx.Text;
                            dr["GashLead"] = GashLead;
                            dr["NumberofGashes"] = NoofGashes_TxtBx.Text;
                            dr["NumberofTeeth"] = NumberofTeeth_TxtBx.Text;
                            dr["NumberofPointsForRake"] = PointsPerTooth_Face_TxtBx.Text;
                            dr["RakeAngle"] = RakeAngle;
                            if ((bool)chkCutterMode.IsChecked)
                                dr["CutterType"] = 1;
                            else
                                dr["CutterType"] = 0;
                            HobDetails.Tables[3].Rows.Add(dr);
                            oDA.Update(HobDetails.Tables[3]);
                        }
                    }
                    HobDetails.Tables[4].Clear();
                    oDA = new OleDbDataAdapter("SELECT * FROM HobRunDetails", basicConnectString);
                    OleDbCommandBuilder cbd4 = new OleDbCommandBuilder(oDA);
                    oDA.Fill(HobDetails.Tables[4]);
                    DataRow dr4 = HobDetails.Tables[4].NewRow();
                    dr4["SerialNumber"] = SerialNo_TxtBx.Text;
                    dr4["RunDateTime"] = DateTime.Now;
                    dr4["PCNumber"] = OrderNo_TxtBx.Text;
                    dr4["RakePointsNumber"] = PointsPerTooth_Face_TxtBx.Text;
                    dr4["GashLeadTeethNumber"] = NumberofTeeth_TxtBx.Text;
                    HobDetails.Tables[4].Rows.Add(dr4);
                    oDA.Update(HobDetails.Tables[4]);
    
                    int cust, machineNo, opno;
                    cust = CustomerName_TxtBx.SelectedIndex;
                    machineNo = MachineNo_TxtBx.SelectedIndex;
                    opno = Operator_TxtBx.SelectedIndex;
                    LoadHobDataFromDB();
                    CustomerName_TxtBx.SelectedIndex = cust;
                    MachineNo_TxtBx.SelectedIndex = machineNo;
                    Operator_TxtBx.SelectedIndex = opno;
                }
                catch (Exception ex1)
                {
                    MessageBox.Show("Could not get all required data for running this task. Please enter all data before clicking Start!", "Rapid-I", MessageBoxButton.OK, MessageBoxImage.Error);
                    StopBtn_Click(null, null);
                    return;
                }

                try
                {
                    GlobalSettings.MainExcelInstance.HobPersonalData = HobPersonalData;
                    GlobalSettings.MainExcelInstance.ReportName = ReportName;
                    GlobalSettings.MainExcelInstance.IsHobCheckerMeasurementNo123(0, true);        //vinod....this is to set setting for hobcheckertemplate. 
                    GlobalSettings.MainExcelInstance.Reset_All();
                    GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
                    GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                    GlobalSettings.MainExcelInstance.GenerateExcel(false, true);
                }
                catch (Exception ex2)
                {
                    MessageBox.Show("Could not initialise Excel Report for this run. Please check the settings and ensure that the report/template is already not open!", "Rapid-I", MessageBoxButton.OK, MessageBoxImage.Error);
                    return;
                }
            }
        }

        private void ResetAll(object sender, EventArgs e)
        {
            System.Windows.MessageBoxResult proceedresult;
            proceedresult = MessageBox.Show("Are you sure you want to clear all entries now? You will lose runout and first tooth data as well.", "Rapid-I", MessageBoxButton.YesNo);
            if (proceedresult == MessageBoxResult.No)
                return;
            //Clear all data - 
            //1. Clear Runout setting
            Reset_Runout(null, null);
            Reset_FirstTooth(null, null);
            ClearTextboxes(wpHobParams);
            ClearTextboxes(wpFormParams);
            ClearTextboxes(wpPersonalData);
            //ClearTextboxes(wpSpacingParams);
            //ClearTextboxes(wpLeadParams);
            FillDefaultValues();
            StopBtn_Click(null, null);
            ResetAll_Background(3);
        }

        private void ClearTextboxes(StackPanel WP)
        {
            foreach (object obj in WP.Children)
            {
                if (obj.GetType() == typeof(TextBox))
                {
                    TextBox tb = (TextBox)obj;
                    tb.Text = "";
                }
            }
        }

        private void FillDefaultValues()
        {
            StartDepth_TxtBx.Text = "0.3";
            leadofGash_TxtBx.Text = "0";
            //PointsPerTooth_Lead_TxtBx.Text = "1";
        }

        private void ShowExcelReport()
        {
            if (File.Exists(ReportName))
                System.Diagnostics.Process.Start(ReportName);
            else
                MessageBox.Show("Could not find the report file - " + ReportName, "Rapid-I 5.0");
        }

        private double GetAngleValue(string AngleString)
        {
            double angle = 0;
            string[] ss; char[] cc = { Convert.ToChar(";") };
            ss = AngleString.Split(cc);
            if (ss[0] == "") return double.NaN;

            if (ss.Length == 1)
                angle = Convert.ToDouble(ss[0]);
            else
            {
                for (int i = 0; i < ss.Length; i++)
                {
                    //bool aa = double.TryParse(ss[i], angle);
                    if (ss[i] == "") ss[i] = "0";
                    try
                    {
                        double xx = double.Parse(ss[i]);
                    }
                    catch (Exception ex)
                    {
                        return double.NaN;
                    }
                    angle += Convert.ToDouble(ss[i]) / Math.Pow(60, i);
                }
            }

            return angle;
        }

        private void ResetThisRun(object sender, EventArgs e)
        {
            StopBtn_Click(null, null);
            ResetAll_Background(1);
        }

        private void RunMeasurementTest(object sender, EventArgs e)
        {
            StartMeasurement(true);
        }

        string GashLeadString;
        private void GashType_Click(object Sender, EventArgs e)
        {
            try
            {
                System.Windows.Controls.Primitives.ToggleButton Btn = Btn_GashLead;
                if (Btn == null) return;
                    //(System.Windows.Controls.Primitives.ToggleButton)Sender;
                if (Btn.IsChecked == true & Btn.Content != "") // Take Gash lead in Helix Angle format
                {
                    Btn.Content = "Helix Angle ";
                    if (GashLeadString == null)
                    {
                        double gangle = 180 * Math.Atan(GashLead / (Math.PI * Convert.ToDouble(OuterDiameter_TxtBx.Text))) / Math.PI;
                        gangle = 90 - gangle;
                        double minutes = gangle - Math.Floor(gangle);
                        GashLeadString = Math.Floor(gangle).ToString() + ";" + Math.Floor(minutes * 60).ToString();
                    }
                    leadofGash_TxtBx.Text = GashLeadString;
                    //double gashangle = GetAngleValue(leadofGash_TxtBx.Text);
                    //GashLead = Math.PI * Convert.ToDouble(OuterDiameter_TxtBx.Text) * Math.Tan(gashangle * Math.PI / 180);
                }
                else
                {
                    Btn.Content = "Gash Lead ";
                    GashLeadString = leadofGash_TxtBx.Text;
                    //GashLead = Convert.ToDouble(leadofGash_TxtBx.Text);
                    leadofGash_TxtBx.Text = String.Format("{0:0.0000}", GashLead);
                }
            }
            catch (Exception ex)
            {

            }
        }

        private void TestExcelReport(object Sender, EventArgs e)
        {
            System.Collections.Generic.List<System.Collections.ArrayList> msl = new System.Collections.Generic.List<System.Collections.ArrayList>();
            for (int i = 0; i < 10; i++)
            {
                System.Collections.ArrayList MValueList = new System.Collections.ArrayList();
                MValueList.Add("Err" + i);
		        MValueList.Add("Dist");
		        MValueList.Add(0.010);
			    MValueList.Add(0.0);
			    MValueList.Add(0.02);
			    MValueList.Add(-0.02);
		        MValueList.Add(i);
                msl.Add(MValueList);
            }
            GlobalSettings.MainExcelInstance.ReportName = GlobalSettings.ReportPath + "\\" + "Test" + "-" + DateTime.Now.ToShortDateString() + ".xls"; ;
            GlobalSettings.MainExcelInstance.IsHobCheckerMeasurementNo123(0, true);        //vinod....this is to set setting for hobcheckertemplate. 
            GlobalSettings.MainExcelInstance.Reset_All();
            GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
            GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
            GlobalSettings.MainExcelInstance.GenerateExcel(false, true);

            GlobalSettings.MainExcelInstance.MeasurementStringList = msl;
            if (GlobalSettings.MainExcelInstance.IsHobCheckerMeasurementNo123(1, true))
            {
                if (GlobalSettings.MainExcelInstance.Reset_All(true, false))
                {
                    GlobalSettings.MainExcelInstance.ShowAvging = GlobalSettings.ShowAveragingInExcel;
                    GlobalSettings.MainExcelInstance.InsertImageForDataTransfer = GlobalSettings.InsertImageForDataTransfer;
                    if (GlobalSettings.MainExcelInstance.GenerateExcel(true, true))
                    {
                        MessageBox.Show("Completed Measurements successfully");
                    }

                }
            }
        }

        private void Set_CAxis_Zero(object Sender, EventArgs e)
        {
            string rval = ConvertRadianstoDegrees(RWrapper.RW_DRO.DROValue[3]);
            C_Axis0.Text = rval; 

        }

        private string ConvertRadianstoDegrees(double angle)
        {
            double Val = angle * 180 / Math.PI;
            string sign = Val < 0 ? "-" : "";
            Val = Math.Abs(Val);
            string RVal = sign + Math.Floor(Val).ToString() + '\u00B0';
            Val = (Val - Math.Floor(Val)) * 60;
            RVal += Math.Floor(Val).ToString("00") + '\'';
            Val = (Val - Math.Floor(Val)) * 60;
            RVal += Math.Floor(Val).ToString("00") + '"';
            return RVal;
        }
                
        private void WriteFrontEndValuesToFile()
        {
            string FileName = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\HobFEParams.txt";
            System.IO.StreamWriter fs;
            try
            {
                fs  = new System.IO.StreamWriter(FileName, false);
                fs.WriteLine(OrderNo_TxtBx.Text);
                fs.WriteLine(SerialNo_TxtBx.Text);
                fs.WriteLine(MachineNo_TxtBx.Text);
                fs.WriteLine(CustomerName_TxtBx.Text);
                fs.WriteLine(Operator_TxtBx.Text);
                fs.WriteLine(Diameter_txtBox.Text);
                fs.WriteLine(OuterDiameter_TxtBx.Text);
                //fs.WriteLine(Txt_HubDia.Text);
                fs.WriteLine(ActiveLength_txtBox.Text);
                fs.WriteLine(TotalLength_TxtBox.Text);
                fs.WriteLine(Module);
                fs.WriteLine(NoofStarts_TxtBx.Text);
                fs.WriteLine(AccuracyClass_TxtBox.SelectedIndex);
                fs.WriteLine(GashLead);
                fs.WriteLine(NoofGashes_TxtBx.Text);
                fs.WriteLine(NumberofTeeth_TxtBx.Text);
                fs.WriteLine(PointsPerTooth_Face_TxtBx.Text);
                fs.WriteLine(Txt_FirstTooth.Text);
                fs.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "            " + ex.StackTrace);
            }
            finally
            {
            }
        }
        private void RestoreFrontEndValues()
        {
            string FileName = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\HobFEParams.txt";
            System.IO.StreamReader fs = new System.IO.StreamReader(FileName);
            string ss = "";
            try
            {
                ss = fs.ReadLine(); OrderNo_TxtBx.Text = ss;
                ss = fs.ReadLine(); SerialNo_TxtBx.Text = ss;
                ss = fs.ReadLine(); MachineNo_TxtBx.Text = ss;
                ss = fs.ReadLine(); CustomerName_TxtBx.Text = ss;
                ss = fs.ReadLine(); Operator_TxtBx.Text = ss;
                ss = fs.ReadLine(); Diameter_txtBox.Text = ss;
                ss = fs.ReadLine(); OuterDiameter_TxtBx.Text = ss;
                //ss = fs.ReadLine(); Txt_HubDia.Text = ss;
                ss = fs.ReadLine(); ActiveLength_txtBox.Text = ss;
                ss = fs.ReadLine(); TotalLength_TxtBox.Text = ss;
                ss = fs.ReadLine(); Module_TxtBx.Text = ss; Module = Convert.ToDouble(ss);
                ss = fs.ReadLine(); NoofStarts_TxtBx.Text = ss;
                ss = fs.ReadLine(); AccuracyClass_TxtBox.SelectedIndex = Convert.ToInt32(ss);
                ss = fs.ReadLine(); leadofGash_TxtBx.Text = ss; GashLead = Convert.ToDouble(ss); Angle_ChangedHandler(leadofGash_TxtBx, null);
                ss = fs.ReadLine(); NoofGashes_TxtBx.Text = ss;
                ss = fs.ReadLine(); NumberofTeeth_TxtBx.Text = ss;
                ss = fs.ReadLine(); PointsPerTooth_Face_TxtBx.Text = ss;
                ss = fs.ReadLine(); Txt_FirstTooth.Text = ss;
                fs.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + "            " + ex.StackTrace);
            }
            fs.Close();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            RestoreFrontEndValues();
        }

        string connector = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\HobParameters.mdb";
        //DB HobDB;
        DataSet HobDetails; // hobDetailsTable, CustomerTable, OperatorTable;
        OleDbDataAdapter oDA;
        string[] TableNames = new string[] { "CustomerDetails", "OperatorDetails", "MachineDetails", "HobParams" };
        string[] paramNames = new string[] { "CustomerName", "OperatorName", "MachineNumber" };
        string basicConnectString = "Provider=Microsoft.Ace.OLEDB.12.0; Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData)
                    + "\\Rapid-I 5.0\\Database\\HobParameters.mdb";

        private void LoadHobDataFromDB()
        {
            try
            {
                ComboBox[] choices = { CustomerName_TxtBx, Operator_TxtBx, MachineNo_TxtBx };
                HobDetails = new DataSet();
                for (int j = 0; j < 3; j++)
                {
                    choices[j].Items.Clear();
                    DataTable ddt = GetTablefromDB(TableNames[j]);
                    string[] columnNames;
                    columnNames = ddt.Columns.Cast<DataColumn>().Select(x => x.ColumnName).ToArray();
                    HobDetails.Tables.Add(ddt.DefaultView.ToTable(true, columnNames));
                    for (int i = 0; i < HobDetails.Tables[j].Rows.Count; i++)
                    {
                        choices[j].Items.Add(HobDetails.Tables[j].Rows[i][paramNames[j]]);
                    }
                }

                HobDetails.Tables.Add(GetTablefromDB("HobParams"));
                for (int i = 0; i < HobDetails.Tables[3].Rows.Count; i++)
                {
                    if (SerialNo_TxtBx.Items.Contains(HobDetails.Tables[3].Rows[i]["SerialNumber"])) continue;
                    SerialNo_TxtBx.Items.Add(HobDetails.Tables[3].Rows[i]["SerialNumber"]);
                }
                HobDetails.Tables.Add(GetTablefromDB("HobRunDetails"));
            }
            catch (Exception ex)
            {
                MessageBox.Show("Could not load Details from the database" + "                " + ex.Message, "Rapid-I");
            }
        }

        private DataTable GetTablefromDB(string TableName)
        {
            string querystring = "SELECT * from " + TableName;
            System.Data.DataTable DT = new DataTable();
            using (OleDbConnection connection = new OleDbConnection(basicConnectString))
            {
                OleDbCommand comm = new OleDbCommand(querystring, connection);
                connection.Open();
                oDA = new OleDbDataAdapter(comm);
                oDA.Fill(DT);
                connection.Close();
            }
            //Get the 
            return DT;
        }

        //private void UpdateTable(string TableName)
        //{
        //    string querystring = "SELECT * from " + TableName;
        //    //using (OleDbConnection connection = new OleDbConnection(basicConnectString))
        //    //{
        //        //OleDbCommand comm = new OleDbCommand(querystring, connection);
        //        //connection.Open();
        //        //OleDbDataAdapter oDA = new OleDbDataAdapter(comm);
        //    oDA = new OleDbDataAdapter(querystring, basicConnectString);
        //    OleDbCommandBuilder cbd = new OleDbCommandBuilder(oDA);
        //        //cbd.GetInsertCommand();
        //        //oDA.UpdateCommand = comm;
        //        //comm.ExecuteNonQuery();
        //        oDA.Update(HobDetails.Tables[Array.IndexOf(TableNames, TableName)]); //HobDetails, TableName);
        //        //connection.Close();
        //    //}
        //}

        private void SerialNumberChangedHandler(object sender, EventArgs e)
        {
            try
            {
                if (SerialNo_TxtBx.SelectedItem == null) return;
                int finalIndex = -1;

                DataTable dt = GetTablefromDB("HobParams");
                for (int i = 0; i < HobDetails.Tables[3].Rows.Count; i++)
                {
                    if (SerialNo_TxtBx.SelectedItem.ToString() == HobDetails.Tables[3].Rows[i]["SerialNumber"].ToString())
                        finalIndex = i;
                }
                //If we do not have an entry, do not update anything on the screen
                if (finalIndex < 0) return;

                DataRow dr = HobDetails.Tables[3].Rows[finalIndex];
                int currentCustomerID = (int)  dr["Customer_ID"];
                for (int ii = 0; ii < HobDetails.Tables[0].Rows.Count; ii++)
                {
                    int cid = (int)HobDetails.Tables[0].Rows[ii]["Customer_ID"];
                    if (cid == currentCustomerID)
                    {
                        CustomerName_TxtBx.Text = Convert.ToString(HobDetails.Tables[0].Rows[ii]["CustomerName"]);
                        break;
                    }
                }

                Diameter_txtBox.Text = Convert.ToString(dr["HubDia"]);
                OuterDiameter_TxtBx.Text = Convert.ToString(dr["OuterDia"]);
                ActiveLength_txtBox.Text = Convert.ToString(dr["ActiveLength"]);
                TotalLength_TxtBox.Text = Convert.ToString(dr["TotalLength"]);
                Module_TxtBx.Text = Convert.ToString(dr["HobModule"]);
                Module = Convert.ToDouble(Module_TxtBx.Text);
                NoofStarts_TxtBx.Text = Convert.ToString(dr["NumberOfStarts"]);
                AccuracyClass_TxtBox.SelectedIndex = Convert.ToInt32(dr["AccuracyClass"]);
                leadofGash_TxtBx.Text = Convert.ToString(dr["GashLead"]);
                GashLead = Convert.ToDouble(dr["GashLead"]); Angle_ChangedHandler(leadofGash_TxtBx, null);
                NoofGashes_TxtBx.Text = Convert.ToString(dr["NumberofGashes"]);
                NumberofTeeth_TxtBx.Text = Convert.ToString(dr["NumberofTeeth"]);
                PointsPerTooth_Face_TxtBx.Text = Convert.ToString(dr["NumberofPointsForRake"]);
                RakeAngle = Convert.ToDouble(dr["RakeAngle"]); Angle_ChangedHandler(RakeAngle_TxtBx, null);
                if (dr["CutterType"] == DBNull.Value || Convert.ToInt32(dr["CutterType"]) == 0)
                    chkCutterMode.IsChecked = false;
                else
                    chkCutterMode.IsChecked = true;

                if (Convert.ToInt32(dr["LeadDirection"]) > 0)
                    UpRBtn.IsChecked = true;
                else
                    DownRBtn.IsChecked = true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Could not get all the parameter data" + "            " + ex.Message);
            }
        }

        private int CheckForExistingValue(string name, DataTable dt, string parameterName)
        {
            //bool gotName = false;
            for (int ii = 0; ii < dt.Rows.Count; ii++)
            {
                if (Convert.ToString(dt.Rows[ii][parameterName]) == name) return (int)dt.Rows[ii][0];
            }
            return -1;
        }
        private void ToothDepthRake_TextChanged(object sender, EventArgs e)
        {
            if (ToothDepth_TxtBx.Text == "") return;
            if (Math.Round(double.Parse(ToothDepth_TxtBx.Text), 3) != Math.Round(2.25 * Module, 3))
                ToothDepth_TxtBx.Background = Brushes.White;
            else
                ToothDepth_TxtBx.Background = Brushes.LightGray;
        }

        private void Default_ToothDepth_Click(object sender, EventArgs e)
        {
            if (Module != 0)
                ToothDepth_TxtBx.Text = string.Format("{0:F4}", 2.25 * Module);
        }

        private void ResetAll_Background(int UptoLevel)
        {
            SolidColorBrush bluebrush = new SolidColorBrush(Color.FromRgb(155, 209, 255));
            gridFormPos.Background = bluebrush;
            gridSpacing.Background = bluebrush;
            gridGashLead.Background = bluebrush;
            if (UptoLevel == 1) return;
            GridFirstTooth.Background = bluebrush;
            ToothDepth_Spacing_TxtBx.Background = new SolidColorBrush(Color.FromRgb(255, 255, 255));
            ToothDepth_Lead.Background = new SolidColorBrush(Color.FromRgb(255, 255, 255));
            ToothDepth_TxtBx.Background = new SolidColorBrush(Color.FromRgb(255, 255, 255));
            if (UptoLevel == 2) return;
            gb_Runout.Background = bluebrush;
        }

        private void Meas_Type_CheckChanged(object sender, RoutedEventArgs e)
        {
            CheckBox[] MeasCKs = { chkFormPos, chkGashSpacing, chkLeadofGash };
            CheckBox ck = (CheckBox)sender;
            int index = Array.IndexOf(MeasCKs, ck) + 1;
            int multiplier = -1;
            if ((bool)ck.IsChecked)
                multiplier = 1;
            Measurements_To_Do += multiplier * (int)Math.Pow(2.0, (double)index);
        }

        private void gI_Click(object sender, RoutedEventArgs e)
        {
            Set_HobParameters(8);
            Measurement_No = 2;
            StartMeasurement(true);
            CompleteMeasurementReports();
        }
    }
}
