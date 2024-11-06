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
using System.Data;
using System.Data.OleDb;
using Rapid.Utilities;
using System.Collections;
using RWrapper;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for TIS_Control.xaml
    /// </summary>
    public partial class TIS_Control : Window
    {
        public TIS_Control()
        {
            InitializeComponent();
        }

        //public System.Collections.ObjectModel.ObservableCollection<ShankAlignStep> ShankAlignCol;
        DataTable TISToolsTable, TISParametersTable;
        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            List<string> CbList = new List<string>();
            TISTools.FillScrollBarButtons("TIS_Tools", "TIS_Tools", HandleTISToolsClick, true, CbList);
            //Instantiate the Shank Alignment level values and add a default height of 15mm...
            //ShankAlignCol = new System.Collections.ObjectModel.ObservableCollection<ShankAlignStep>();
            RWrapper.TIS_Control.MYINSTANCE().ShankAlignColl.Add(new ShankAlignStep(1, 0, true));
            RWrapper.TIS_Control.MYINSTANCE().ShankAlignColl.Add(new ShankAlignStep(2, 15, true));
            dgShankAlignparams.ItemsSource = RWrapper.TIS_Control.MYINSTANCE().ShankAlignColl;
            //new RWrapper.TIS_Control();
            RWrapper.TIS_Control.MYINSTANCE().RotationalScanEvent += new RWrapper.TIS_Control.RotaryScanningEventHandler(HandleRotaryScanUpdate);
            RWrapper.TIS_Control.MYINSTANCE().FinishedRotaryScanEvent += new RWrapper.TIS_Control.FinishedRotaryScanningEventHandler(HandleRotaryScanFinished);
            //Load the TIS Tool Types and their parameters required - 
            TISToolsTable = new DataTable();
            TISParametersTable = new DataTable();
            string ConnStr;
            if (System.Environment.Is64BitOperatingSystem)
                ConnStr = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) +
                                                    "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
            else
                ConnStr = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) +
                                                    "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";

            OleDbDataAdapter thisAdapter = new OleDbDataAdapter("SELECT * from TIS_Tools_Lookup", ConnStr);
            OleDbCommandBuilder ThisCommander = new OleDbCommandBuilder(thisAdapter);
            thisAdapter.Fill(TISToolsTable);
            thisAdapter = new OleDbDataAdapter("SELECT * from TIS_Tool_Parameters", ConnStr);
            ThisCommander = new OleDbCommandBuilder(thisAdapter);
            thisAdapter.Fill(TISParametersTable);
        }

        private void chkTaperAngleChecked(object sender, RoutedEventArgs e)
        {
            if ((bool)chkTaperedShank.IsChecked)
                spTaperAngle.Visibility = System.Windows.Visibility.Visible;
            else
                spTaperAngle.Visibility = System.Windows.Visibility.Hidden;
        }

        /// <summary>
        /// Called by X or Y axis centre position Go to button click
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void GotoAxisPosition(object sender, RoutedEventArgs e)
        {
            // Get a string array for sending Go to command
            string[] CncGoTotxts;
            //If CNC Mode is not ON, switch it on. 
            if (!RWrapper.RW_CNC.MYINSTANCE().getCNCMode())  RWrapper.RW_CNC.MYINSTANCE().Activate_CNCModeDRO();
            //Setup Target Position
            if (((Button)sender).Content.ToString().Contains("X"))
                CncGoTotxts = new string[] { txtXCentreVal.Text, RWrapper.RW_DRO.DROValue[1].ToString(), RWrapper.RW_DRO.DROValue[2].ToString(), RWrapper.RW_DRO.DROValue[3].ToString() };
            else
                CncGoTotxts = new string[] { RWrapper.RW_DRO.DROValue[0].ToString(), txtYCentreVal.Text, RWrapper.RW_DRO.DROValue[2].ToString(), RWrapper.RW_DRO.DROValue[3].ToString() };
            //Go...
            RWrapper.RW_CNC.MYINSTANCE().GotoPosition(CncGoTotxts);
        }

        private void btnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private void BtnGetAxis_Click(object sender, RoutedEventArgs e)
        {
            //RWrapper.RW_MainInterface.MYINSTANCE().CaptureCAxis();

        }

        public void HandleRotaryScanUpdate(int FCount)
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                txtCurrentScanStatus.Text = FCount.ToString();
            }
            else
                this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.TIS_Control.RotaryScanningEventHandler(HandleRotaryScanUpdate), FCount);
        }

        private void HandleRotaryScanFinished(double[] Results)
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                switch (RWrapper.TIS_Control.MYINSTANCE().CurrentTool)
                {
                    case RWrapper.TISMeasureTool.GetAxis:
                        txtXCentreVal.Text = String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", Results[0]);
                        txtYCentreVal.Text = String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", Results[1]);
                        break;
                }
            }
            else
                this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.TIS_Control.FinishedRotaryScanningEventHandler(HandleRotaryScanFinished));

        }

        private void HandleTISToolsClick(object sender, RoutedEventArgs e)
        {
            string selectedTool = ((ButtonProperties)((RadioButton)sender).Tag).ButtonName;
            DataRow[] ClickedTool = TISToolsTable.Select("ToolName = '" + selectedTool + "'");
            if (ClickedTool.Length <= 0) return;

            DataRow[] ParamData = TISParametersTable.Select("TISToolID = " + ClickedTool[0]["TISToolID"]);
            System.Collections.ObjectModel.ObservableCollection<TISToolParameters> ParamList = new System.Collections.ObjectModel.ObservableCollection<TISToolParameters>();
            foreach(DataRow param in ParamData)
            {
                TISToolParameters ttp = new TISToolParameters();
                ttp.ToolParamID = Convert.ToInt32( param["TISToolID"]);
                ttp.Parameter = param["Parameter"].ToString();
                ttp.ParameterValue = Convert.ToDouble(param["ParameterValue"]);
                ParamList.Add(ttp);
            }
            dgParameters.ItemsSource = ParamList;
            dgParameters.UpdateLayout();
            dgParameters.RowStyle = (Style)FindResource("transparentDatarow");
            dgParameters.Columns[0].Visibility = System.Windows.Visibility.Hidden;
            dgParameters.Columns[1].CellStyle = (Style)FindResource("EmptyRightAlignedDGCellStyle");
            dgParameters.Columns[1].Width = 160;
            dgParameters.Columns[2].Width = 70;
            dgParameters.Columns[2].CellStyle = (Style)FindResource("BorderedDGCellStyle");

            switch (selectedTool)
            {
                case "ShankDia":
                    break;

            }
        }

        private void StandardButtonClick(object sender, RoutedEventArgs e)
        {
            try
            {
                Button CallingButton = (Button)sender;
                string SelectedFunction = CallingButton.Content.ToString();
                System.Collections.Generic.List<double> paramVal;
                switch (SelectedFunction)
                {
                    case "Get Axis":
                        {
                            RWrapper.TIS_Control.MYINSTANCE().CurrentTool = RWrapper.TISMeasureTool.GetAxis;
                            RWrapper.RW_UCSParameter.MYINSTANCE().SelectUCS(0);
                            paramVal = new List<double>();
                            paramVal.Add(2.0);
                            RWrapper.TIS_Control.MYINSTANCE().StartMeasurement(paramVal);
                            break;
                        }
                    case "Reset":
                        {
                            RWrapper.TIS_Control.MYINSTANCE().ResetTISAxis();
                            txtXCentreVal.Text = "0.0000";
                            txtYCentreVal.Text = "0.0000";
                            break;
                        }
                    case "Add Level":
                        {
                            ShankAlignStep LastStep = (ShankAlignStep)RWrapper.TIS_Control.MYINSTANCE().ShankAlignColl[RWrapper.TIS_Control.MYINSTANCE().ShankAlignColl.Count - 1];
                            RWrapper.TIS_Control.MYINSTANCE().ShankAlignColl.Add(new ShankAlignStep(LastStep.Level + 1, LastStep.Height + 10, true));
                            break;
                        }
                    case "Datum":
                    case "Top Ref":
                        {
                            int funcIndex = 0;
                            if (SelectedFunction == "Top Ref") funcIndex = 1;

                            if (CallingButton.Style == (Style)FindResource("ScrollToolbar_NormalButton2"))
                            {
                                //Datum and Top Ref should always be done in MCS. Hence we will first switch to MCS and then take the value. 
                                RWrapper.RW_UCSParameter.MYINSTANCE().SelectUCS(0);
                                double[] DatumPt = { RWrapper.RW_DRO.CurrentUCSDRO[0], RWrapper.RW_DRO.CurrentUCSDRO[1], RWrapper.RW_DRO.CurrentUCSDRO[2], RWrapper.RW_DRO.CurrentUCSDRO[3] };
                                RWrapper.TIS_Control.MYINSTANCE().SetPoint(DatumPt, funcIndex, true);
                                CallingButton.Style = (Style)FindResource("ScrollToolBar_Normal_Green");
                            }
                            else
                            {
                                CallingButton.Style = (Style)FindResource("ScrollToolbar_NormalButton2");
                                double[] DatumPt = { 0, 0, 0, 0 };
                                RWrapper.TIS_Control.MYINSTANCE().SetPoint(DatumPt, funcIndex, false);
                            }
                            break;
                        }
                    case "Align":
                        RWrapper.TIS_Control.MYINSTANCE().CurrentTool = RWrapper.TISMeasureTool.ShankAlignment;
                        paramVal = new List<double>();
                        double temp;
                        if ((bool)chkTaperedShank.IsChecked)
                            temp = Convert.ToDouble(tbTaperAngle.Text);
                        else
                            temp = 0;
                        paramVal.Add(temp);
                        temp = 0.0;
                        if (tbShankDia.Text != "")
                            double.TryParse(tbShankDia.Text, out temp);
                        paramVal.Add(temp);
                        RWrapper.TIS_Control.MYINSTANCE().StartMeasurement(paramVal);
                        break;
                    case "v":
                        RWrapper.TIS_Control.MYINSTANCE().RestoreAxisofRotation();
                        break;
                }
            }
            catch(Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:TIS04", ex);
            }
        }
    }



    public class TISToolParameters
    {
        public int ToolParamID { get; set; }
        public string Parameter { get; set; }
        public double ParameterValue { get; set; }
    }

    public enum TISMeasureTool
    {
        GetAxis,
        ShankAlignment,
        ShankDia,
        CuttingDia,
        FaceRunout,
        PointAngle,
        StepLength,
        BallNose,
        MaxLine,
        MaxArc,
        Helix,
        CoreDia,
        CornerDia,
        BackTaper,
    };

}
