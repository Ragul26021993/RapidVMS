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
using System.Collections;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class StarProbeAutoCalibWin : Window
    {
        public int ForCalibration = 0;
        Hashtable ProbHead = new Hashtable();

        public StarProbeAutoCalibWin()
        {
            this.InitializeComponent();// Insert code required on object creation below this point.

            ForCalibration = 2;
            //StartPauseBtn.Click += new RoutedEventHandler(StartPauseBtn_Click);
            StopBtn.Click += new RoutedEventHandler(StopBtn_Click);
            RWrapper.RW_MainInterface.MYINSTANCE().ClassControl(RWrapper.ClassNames.SphereCalibration, true);

            //RWrapper.RW_SphereCalibration.MYINSTANCE().ValueCaliberatedEvent -= new RWrapper.RW_SphereCalibration.CaliberatedValue(StarProbeAutoCalibWin_ValueCaliberatedEvent);
            //RWrapper.RW_SphereCalibration.MYINSTANCE().ValueCaliberatedEvent += new RWrapper.RW_SphereCalibration.CaliberatedValue(StarProbeAutoCalibWin_ValueCaliberatedEvent);
            ProbeTypeSelectionCBx.SelectionChanged += new SelectionChangedEventHandler(ProbeTypeSelectionCBx_SelectionChanged);
            this.Closing += new System.ComponentModel.CancelEventHandler(StarProbeAutoCalibWin_Closing);

            ProbHead["Left"] = 1;
            ProbHead["Right"] = 2;
            ProbHead["Front"] = 3;
            ProbHead["Back"] = 4;
            ProbHead["Straight"] = 5;

            //if (ForCalibration == 0)
            //{
            //    ProbeHeadSelectionCBx.Visibility = System.Windows.Visibility.Hidden;
            //    ProbeHeadSelectionLbl.Visibility = System.Windows.Visibility.Hidden;
                FillCombobox(ref ProbeTypeSelectionCBx, "Probe", "ProbeType", "Frequency");
                InspectionTypeCBx.Items.Add("Probe Calibration");
                InspectionTypeCBx.Items.Add("Probe Inspection");
                InspectionTypeCBx.Items.Add("Auto Sphere");
                InspectionTypeCBx.Items.Add("Touch-Vision Calibration");
                InspectionTypeCBx.Items.Add("Probe OffSet Inspection");
            //}
            //else
            //{
            //    FillCombobox(ref ProbeTypeSelectionCBx, "Probe", "ProbeType", "Frequency");
            //    this.Title = "Prob Inspection";
            //}
            ReadParameters();
        }

        void StartPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            if (((string)StartPauseBtn.Content) == "Start")
            {
                //Find out what type of inspection we want to do first!
                ForCalibration = InspectionTypeCBx.SelectedIndex;

                double[] RadiusVals = { double.Parse(SphereRadiusTxtBx.Text), double.Parse(ProbeRadiusTxtBx.Text) };
                StartPauseBtn.Content = "Pause";
                StartPauseBtn.ToolTip = "Pause";
                int currSideofTouch = 0;
                if (ProbeTypeSelectionCBx.Text == "NormalProbe")
                    currSideofTouch = 0;
                else
                  currSideofTouch =  Convert.ToInt32(ProbHead[ProbeHeadSelectionLBx.SelectedItem.ToString()]);

                if (ForCalibration == 0)
                    RWrapper.RW_SphereCalibration.MYINSTANCE().StartSphereCallibration(RadiusVals, ForCalibration, currSideofTouch, ProbeTypeSelectionCBx.SelectedIndex + 1, (bool)CircularInterpolChBk.IsChecked, (bool)IncludeStraightChBk.IsChecked);
                else
                {
                    if (ProbeTypeSelectionCBx.Text == "NormalProbe")
                    {
                        RWrapper.RW_SphereCalibration.MYINSTANCE().StartSphereCallibration(RadiusVals, ForCalibration, currSideofTouch, ProbeTypeSelectionCBx.SelectedIndex + 1, (bool)CircularInterpolChBk.IsChecked, false);
                    }
                    else if (ProbeHeadSelectionLBx.SelectedItem.ToString() != "")
                    {
                        RWrapper.RW_SphereCalibration.MYINSTANCE().StartSphereCallibration(RadiusVals, ForCalibration, Convert.ToInt32(ProbHead[ProbeHeadSelectionLBx.SelectedItem.ToString()]), ProbeTypeSelectionCBx.SelectedIndex + 1, (bool)CircularInterpolChBk.IsChecked, false);
                    }
                }
            }
            else if (((string)StartPauseBtn.Content) == "Pause")
            {
                StartPauseBtn.Content = "Continue";
                StartPauseBtn.ToolTip = "Continue";
                RWrapper.RW_SphereCalibration.MYINSTANCE().PauseCalibration();
            }
            else if (((string)StartPauseBtn.Content) == "Continue")
            {
                StartPauseBtn.Content = "Pause";
                StartPauseBtn.ToolTip = "Pause";
                RWrapper.RW_SphereCalibration.MYINSTANCE().ContinueCalibration();
            }
        }

        public void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            StartPauseBtn.Content = "Start";
            StartPauseBtn.ToolTip = "Start";
            RWrapper.RW_SphereCalibration.MYINSTANCE().StopCalibration();
        }

        void StarProbeAutoCalibWin_Closing(object sender, EventArgs e)
        {
            RWrapper.RW_SphereCalibration.MYINSTANCE().StopCalibration();
            SaveParameters();
        }

        void StarProbeAutoCalibWin_ValueCaliberatedEvent(int CurrentSide, double[] SphereRadius, double[] CaliberationOffset)
        {
            try
            {
                MessageBoxResult result = MessageBox.Show("Do you want the values to be automatically updated in the database", "Rapid I", MessageBoxButton.OKCancel);
                if (result == MessageBoxResult.OK)
                {
                    System.Data.DataRow[] row = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("ProbeSettings");
                    if (CurrentSide == 1)
                    {
                        row[0]["StarProbeLeftDia"] = 2 * SphereRadius[0];
                    }
                    else if (CurrentSide == 2)
                    {
                        row[0]["StarProbeRightDia"] = 2 * SphereRadius[1];
                        row[0]["StarProbeRightOffsetX"] = CaliberationOffset[0];
                        row[0]["StarProbeRightOffsetY"] = CaliberationOffset[1];
                        row[0]["StarProbeRightOffsetZ"] = CaliberationOffset[2];
                    }
                    else if (CurrentSide == 3)
                    {
                        row[0]["StarProbeFrontDia"] = 2 * SphereRadius[2];
                        row[0]["StarProbeFrontOffsetX"] = CaliberationOffset[3];
                        row[0]["StarProbeFrontOffsetY"] = CaliberationOffset[4];
                        row[0]["StarProbeFrontOffsetZ"] = CaliberationOffset[5];
                    }
                    else if (CurrentSide == 4)
                    {
                        row[0]["StarProbeBackDia"] = 2 * SphereRadius[3];
                        row[0]["StarProbeBackOffsetX"] = CaliberationOffset[6];
                        row[0]["StarProbeBackOffsetY"] = CaliberationOffset[7];
                        row[0]["StarProbeBackOffsetZ"] = CaliberationOffset[8];
                    }
                    bool updateresult = GlobalSettings.SettingsReader.UpdateTable("ProbeSettings");
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:StarProb001", ex);
            }
        }

        private void FillCombobox(ref ComboBox Combobx, string tablename, string column, string OrderByColumn = "", string filter = "")
        {
            try
            {
                Combobx.Items.Clear();
                System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection();
                con.ConnectionString = RWrapper.RW_MainInterface.DBConnectionString;
                string query = "";
                if (filter == "" || filter == null)
                {
                    query = "Select (" + column + ") from " + tablename;
                }
                else
                {
                    query = "Select (" + column + ") from " + tablename + " where " + filter;
                }

                if (OrderByColumn != "")
                {
                    //This is because in SQL Query Order by Clause should always be at the last .
                    query += " Order By " + OrderByColumn + " ASC";
                }

                System.Data.OleDb.OleDbDataAdapter adapter = new System.Data.OleDb.OleDbDataAdapter(query, con);
                System.Data.DataTable DT = new System.Data.DataTable(tablename);
                adapter.Fill(DT);
                foreach (System.Data.DataRow Drow in DT.Rows)
                {
                    Combobx.Items.Add(Drow[0]);
                }
                if (Combobx.Items.Count > 0)
                {
                    Combobx.SelectedIndex = 0;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("StarProbAutoCalib:EX03", ex);
            }
        }

        private void ProbeTypeSelectionCBx_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox Comb = (ComboBox)sender;
            if (ForCalibration != 0)
            {
                if (Comb.SelectedItem.ToString() == "NormalProbe")
                {
                    ProbeHeadSelectionLBx.Visibility = System.Windows.Visibility.Hidden;
                    ProbeHeadSelectionLbl.Visibility = System.Windows.Visibility.Hidden;
                }
                else if (Comb.SelectedItem.ToString() == "TProbeAlongX")
                {
                    ProbeHeadSelectionLBx.Visibility = System.Windows.Visibility.Visible;
                    ProbeHeadSelectionLbl.Visibility = System.Windows.Visibility.Visible;
                    ProbeHeadSelectionLBx.Items.Clear();
                    ProbeHeadSelectionLBx.Items.Add("Left");
                    ProbeHeadSelectionLBx.Items.Add("Right");
                    ProbeHeadSelectionLBx.Items.Add("Straight");
                    ProbeHeadSelectionLBx.SelectedIndex = 0;
                }
                else if (Comb.SelectedItem.ToString() == "TProbeAlongY")
                {
                    ProbeHeadSelectionLBx.Visibility = System.Windows.Visibility.Visible;
                    ProbeHeadSelectionLbl.Visibility = System.Windows.Visibility.Visible;
                    ProbeHeadSelectionLBx.Items.Clear();
                    ProbeHeadSelectionLBx.Items.Add("Front");
                    ProbeHeadSelectionLBx.Items.Add("Back");
                    ProbeHeadSelectionLBx.Items.Add("Straight");
                    ProbeHeadSelectionLBx.SelectedIndex = 0;
                }
                else if (Comb.SelectedItem.ToString() == "StarProbe")
                {
                    ProbeHeadSelectionLBx.Items.Clear();
                    ProbeHeadSelectionLBx.Visibility = System.Windows.Visibility.Visible;
                    ProbeHeadSelectionLbl.Visibility = System.Windows.Visibility.Visible;
                    ProbeHeadSelectionLBx.Items.Add("Left");
                    ProbeHeadSelectionLBx.Items.Add("Right");
                    ProbeHeadSelectionLBx.Items.Add("Front");
                    ProbeHeadSelectionLBx.Items.Add("Back");
                    ProbeHeadSelectionLBx.Items.Add("Straight");
                    ProbeHeadSelectionLBx.SelectedIndex = 0;
                }
            }
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\StarProbeAutoCalibWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("ProbeRadius", ProbeRadiusTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("SphereRadius", SphereRadiusTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ProbeTypeSelection", ProbeTypeSelectionCBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ProbeHeadSelection", ProbeHeadSelectionLBx.SelectedItem.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CircularInterpol", CircularInterpolChBk.IsChecked.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "ProbeRadius")
                                {
                                    ProbeRadiusTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "SphereRadius")
                                {
                                    SphereRadiusTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "ProbeTypeSelection")
                                {
                                    ProbeTypeSelectionCBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "ProbeHeadSelection")
                                {
                                    ProbeHeadSelectionLBx.SelectedIndex = i; // ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "CircularInterpol")
                                {
                                    CircularInterpolChBk.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception)
            {

            }
        }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\StarProbeAutoCalibWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("ProbeRadius", ProbeRadiusTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("SphereRadius", SphereRadiusTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ProbeTypeSelection", ProbeTypeSelectionCBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ProbeHeadSelection", ProbeHeadSelectionLBx.SelectedItem.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CircularInterpol", CircularInterpolChBk.IsChecked.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters = null;
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion
    }
}
