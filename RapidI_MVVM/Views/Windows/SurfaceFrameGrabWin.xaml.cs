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
using System.Windows.Controls.Primitives;
using System.Data;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for SurfaceFrameGrabWin.xaml
    /// </summary>
    public partial class SurfaceFrameGrabWin : Window, INotifyPropertyChanged
    {
        #region Variable & Event Declaration

        int _T6MAskFactor = 5, _T6BinaryFactorLower = 40, _T6BinaryFactorUpper = 180,
            _T6MaskingLevel = 1, _T6PixelThreshold = 30, _T6PixelScanRange = 20, _T6CutoffPixels = 15, _T6JumpThreshold = 20;
        private bool dragStarted = true;
        public event RoutedEventHandler WinClosing;
        public event PropertyChangedEventHandler PropertyChanged;
        private static SurfaceFrameGrabWin _MYInstance;
        bool IsUpdatingFG;

        int _ChannelNo = 1;
        DataRow[] Drow;
        DataRow[] PresetValues;

        #endregion

        private void Notify(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }

        public static SurfaceFrameGrabWin MyInstance()
        {
            _MYInstance = null;
            if (_MYInstance == null)
            {
                _MYInstance = new SurfaceFrameGrabWin();
            }
            return _MYInstance;
        }

        private SurfaceFrameGrabWin()
        {
            InitializeComponent();           
            init();
        }

        private void init()
        {
            try
            {
                Slider[] EdgeDetectionParam = { T6MAskFactorSlider, T6BinaryFactorLowerSlider, T6BinaryFactorUpperSlider, T6MaskingLevelSlider, T6PixelThresholdSlider, T6PixelScanRangeSlider, T6CutoffPixelsSlider, T6JumpThresholdSlider };
                for (int i = 0; i < EdgeDetectionParam.Length; i++)
                {
                    EdgeDetectionParam[i].ValueChanged += new RoutedPropertyChangedEventHandler<double>(Slider_ValueChanged);
                }
                T6ApplyDirectionalScan.Click += new RoutedEventHandler(T6ApplyDirectionalScan_Click);
                ReadFromDatabase();
            }
            catch(Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_001", ex);
            }
            bool updateDB = false;
            if(updateDB == false)
            {
                UpdateDatabase();
                string Entityname = "5";
                tbTrialsNumber.Text = RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED(Entityname).ToString();
                updateDB = true;
            }
            
        }

        void T6ApplyDirectionalScan_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //UpdateDatabase();
                if (!IsUpdatingFG)
                    NotifyFramework();

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_002", ex);
            }
        }

        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            UpdateDatabase();
            string Entityname = "5";
            tbTrialsNumber.Text = RWrapper.RW_MainInterface.MYINSTANCE().SetUserChosenSurfaceED(Entityname).ToString();
            //this.Close();
        }
        //On cancel click  just return the dialog result as false.
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().CancelPressed();
            this.Close();
        }

        void NotifyFramework()
        {
            try
            {
                IsUpdatingFG = true;
                                //RWrapper.RW_MainInterface.MYINSTANCE().SetFGandCloudPointSize((int)PtSizeValueSlider.Value);
                    RWrapper.RW_MainInterface.MYINSTANCE().SetAlgo6Param(T6MAskFactor, T6BinaryFactorLower, T6BinaryFactorUpper, T6PixelThreshold,
                        T6MaskingLevel, T6PixelScanRange, T6CutoffPixels, 3, T6JumpThreshold, (bool)rbAverage.IsChecked);
                IsUpdatingFG = false;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_003", ex);
            }
        }

        #region Slider Events
        void Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            try
            {
                if (!this.dragStarted)
                {
                    //UpdateDatabase();
                    NotifyFramework();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_004", ex);
            }
        }
        private void Slider_DragCompleted(object sender, DragCompletedEventArgs e)
        {
            this.dragStarted = false;
            Slider_ValueChanged(sender, null);
        }
        private void Slider_DragStarted(object sender, DragStartedEventArgs e)
        {
            this.dragStarted = true;
        }
        #endregion

        #region Public Properties
        public int T6MAskFactor
        {
            get
            {
                return _T6MAskFactor;
            }
            set
            {
                _T6MAskFactor = value;
                if (_T6MAskFactor > 17) _T6MAskFactor = 17;
                Notify("T6MAskFactor");
            }
        }

        public int T6BinaryFactorLower
        {
            get
            {
                return _T6BinaryFactorLower;
            }
            set
            {
                _T6BinaryFactorLower = value;
                Notify("T6BinaryFactorLower");
            }
        }

        public int T6BinaryFactorUpper
        {
            get
            {
                return _T6BinaryFactorUpper;
            }
            set
            {
                _T6BinaryFactorUpper = value;
            }
        }

        public int T6MaskingLevel
        {
            get
            {
                return _T6MaskingLevel;
            }
            set
            {
                _T6MaskingLevel = value;
                Notify("T6MaskingLevel");
            }
        }

        public int T6PixelThreshold
        {
            get
            {
                return _T6PixelThreshold;
            }
            set
            {
                _T6PixelThreshold = value;
                Notify("T6PixelThreshold");
            }
        }

        public int T6PixelScanRange
        {
            get
            {
                return _T6PixelScanRange;
            }
            set
            {
                _T6PixelScanRange = value;
                Notify("T6PixelScanRange");
            }
        }

        public int T6CutoffPixels
        {
            get
            {
                return _T6CutoffPixels;
            }
            set
            {
                _T6CutoffPixels = value;
                Notify("T6CutoffPixels");
            }
        }

        public int T6JumpThreshold
        {
            get
            {
                return _T6JumpThreshold;
            }
            set
            {
                _T6JumpThreshold = value;
                Notify("T6JumpThreshold");
            }
        }
        #endregion

        #region Read Write Database
        void UpdateDatabase()
        {
            try
            {
                Drow[0]["T6MaskFactor"] = T6MAskFactor;
                Drow[0]["T6LowerBinaryFactor"] = T6BinaryFactorLower;
                Drow[0]["T6UpperBinaryFactor"] = T6BinaryFactorUpper;
                Drow[0]["T6MaskingLevel"] = T6MaskingLevel;
                Drow[0]["T6PixelThreshold"] = T6PixelThreshold;
                Drow[0]["T6PixelScanRange"] = T6PixelScanRange;
                Drow[0]["T6CutoffPixels"] = T6CutoffPixels;
                Drow[0]["T6JumpThreshold"] = T6JumpThreshold;
                Drow[0]["T6ApplyDirectionalScan"] = T6ApplyDirectionalScan.IsChecked;
                GlobalSettings.SettingsReader.UpdateTable("SurfaceFGSettings");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_005", ex);
            }
        }
        void ReadFromDatabase()
        {
            try
            {
                Drow = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("SurfaceFGSettings");
                if (Drow.Length == 0)
                {
                    DataRow ddr = GlobalSettings.SettingsReader.GetTable("SurfaceFGSettings").NewRow();
                    Array.Resize(ref Drow, 1);
                    Drow[0] = ddr;
                }
                T6MAskFactor = Convert.ToInt32(Drow[0]["T6MaskFactor"].ToString());
                T6BinaryFactorLower = Convert.ToInt32(Drow[0]["T6LowerBinaryFactor"].ToString());
                T6BinaryFactorUpper = Convert.ToInt32(Drow[0]["T6UpperBinaryFactor"].ToString());
                T6MaskingLevel = Convert.ToInt32(Drow[0]["T6MaskingLevel"].ToString());
                T6PixelThreshold = Convert.ToInt32(Drow[0]["T6PixelThreshold"].ToString());
                T6PixelScanRange = Convert.ToInt32(Drow[0]["T6PixelScanRange"].ToString());
                T6CutoffPixels = Convert.ToInt32(Drow[0]["T6CutoffPixels"].ToString());
                T6JumpThreshold = Convert.ToInt32(Drow[0]["T6JumpThreshold"].ToString());
                T6ApplyDirectionalScan.IsChecked = Convert.ToBoolean(Drow[0]["T6ApplyDirectionalScan"].ToString());

                LoadPresets();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_006", ex);
            }
        }

        void LoadPresets()
        {
            try
            {
                PresetValues = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("SFG_Presets");
                PresetListBox.Items.Clear();
                for (int i = 0; i < PresetValues.Length; i++)
                {
                    ListBoxItem lb = new ListBoxItem();
                    lb.Style = (Style)FindResource("Lb_Item_2Mer");
                    lb.Height = 44;
                    lb.Content = PresetValues[i]["Preset_Name"];
                    PresetListBox.Items.Add(lb);
                }
                PresetListBox.SelectedIndex = -1;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_007", ex);
            }
        }
        #endregion

        #region Window Closing
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            SaveParameters();
            _MYInstance = null;
        }
        #endregion

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\SurfaceFrameGrabWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("T6MAskFactor", T6MAskFactorSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6BinaryFactorLowerSlider", T6BinaryFactorLowerSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6BinaryFactorUpperSlider", T6BinaryFactorUpperSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6MaskingLevelSlider", T6MaskingLevelSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6PixelThresholdSlider", T6PixelThresholdSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6PixelScanRangeSlider", T6PixelScanRangeSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6CutoffPixelsSlider", T6CutoffPixelsSlider.Value.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("T6JumpThresholdSlider", T6JumpThresholdSlider.Value.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "T6MAskFactor")
                                {
                                    T6MAskFactorSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6BinaryFactorLowerSlider")
                                {
                                    T6BinaryFactorLowerSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6BinaryFactorUpperSlider")
                                {
                                    T6BinaryFactorUpperSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6MaskingLevelSlider")
                                {
                                    T6MaskingLevelSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6PixelThresholdSlider")
                                {
                                    T6PixelThresholdSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6PixelScanRangeSlider")
                                {
                                    T6PixelScanRangeSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6CutoffPixelsSlider")
                                {
                                    T6CutoffPixelsSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "T6JumpThresholdSlider")
                                {
                                    T6JumpThresholdSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_008", ex);

            }
        }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\SurfaceFrameGrabWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("T6MAskFactor", T6MAskFactorSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6BinaryFactorLowerSlider", T6BinaryFactorLowerSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6BinaryFactorUpperSlider", T6BinaryFactorUpperSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6MaskingLevelSlider", T6MaskingLevelSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6PixelThresholdSlider", T6PixelThresholdSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6PixelScanRangeSlider", T6PixelScanRangeSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6CutoffPixelsSlider", T6CutoffPixelsSlider.Value.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("T6JumpThresholdSlider", T6JumpThresholdSlider.Value.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    //ListOfParameters = null;
                    //ListOfParameters.Clear();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_009", ex);
            }
        }
        #endregion

        private void FilterTypeChanged(object sender, RoutedEventArgs e)
        {
            NotifyFramework();
        }

        private void ChannelChanged(object sender, RoutedEventArgs e)
        {
            if ((bool)rbRed.IsChecked)
                _ChannelNo = 0;
            else if ((bool)rbBlue.IsChecked)
                _ChannelNo = 2;
            else
                _ChannelNo = 1;
            RWrapper.RW_MainInterface.MYINSTANCE().SetSurfaceEDSettings(_ChannelNo);
        }

        private void ApplyChosenPreset(object sender, SelectionChangedEventArgs e)
        {
            int i = PresetListBox.SelectedIndex;
            if (i >= 0)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().SetAlgo6Param(Convert.ToInt32(PresetValues[i]["MaskFactor"]),
                                                                     Convert.ToInt32(PresetValues[i]["Binary_Lower"]),
                                                                     Convert.ToInt32(PresetValues[i]["Binary_Upper"]),
                                                                     Convert.ToInt32(PresetValues[i]["Threshold"]),
                                                                     Convert.ToInt32(PresetValues[i]["MaskingLevel"]),
                                                                     T6PixelScanRange, T6CutoffPixels, 3, T6JumpThreshold, (bool)rbAverage.IsChecked);
            }
        }

        private void SaveNewPresetValue(object sender, RoutedEventArgs e)
        {
            try
            {
                DataTable dt = GlobalSettings.SettingsReader.GetTable("SFG_Presets");
                for (int i = 0; i < dt.Rows.Count; i++)
                {
                    if (dt.Rows[i]["Preset_Name"].ToString() == txtPresetName.Text)
                    {
                        MessageBox.Show("This Preset Name is already present. Please give another name and save!", "Rapid-I");
                        return;
                    }
                    if (Convert.ToInt32(dt.Rows[i]["MaskFactor"]) == T6MAskFactor &&
                        Convert.ToInt32(dt.Rows[i]["Threshold"]) == T6PixelThreshold &&
                        Convert.ToInt32(dt.Rows[i]["MaskingLevel"]) == T6MaskingLevel &&
                        Convert.ToInt32(dt.Rows[i]["Binary_Lower"]) == T6BinaryFactorLower &&
                        Convert.ToInt32(dt.Rows[i]["Binary_Upper"]) == T6BinaryFactorUpper)
                    {
                        MessageBoxResult mbr = MessageBox.Show("The Preset " + dt.Rows[i]["Preset_Name"].ToString() + " has the same values you have chosen now. Do you still want to save this?", "Rapid-I", MessageBoxButton.YesNo);
                        if (mbr != MessageBoxResult.Yes)
                            return;
                        else
                            break;
                    }
                }
                DataRow dr = dt.NewRow();
                dr["MachineNo"] = GlobalSettings.MachineNo;
                dr["Preset_Name"] = txtPresetName.Text;
                dr["MaskFactor"] = T6MAskFactor;
                dr["Threshold"] = T6PixelThreshold;
                dr["Binary_Lower"] = T6BinaryFactorLower;
                dr["Binary_Upper"] = T6BinaryFactorUpper;
                dr["MaskingLevel"] = T6MaskingLevel;
                dt.Rows.Add(dr);
                GlobalSettings.SettingsReader.UpdateTable("SFG_Presets");
                LoadPresets();
                //btnOK_Click(sender, e);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("SFG_010", ex);
            }
        }
    }
}