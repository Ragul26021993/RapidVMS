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
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class ProfileScanWidthWin : Window
    {
        public ProfileScanWidthWin()
        {
            this.InitializeComponent();
            // Insert code required on object creation below this point.
            this.Loaded += new RoutedEventHandler(ProfileScanWidthWin_Loaded);
        }
        void ProfileScanWidthWin_Loaded(object sender, RoutedEventArgs e)
        {
            WidthTxtBx.Text = "0";
            BFPtsBundleTxtBx.Text = "4";
            ScanJumpTxtBx.Text = "30";
            MeasurementCountTxtBx.Text = "0";
            ReadParameters();
        }
        void OkBtn_Click(object sender, RoutedEventArgs e)
        {
            double Width;
            int BFPtsBundle, ScanJump, MeasurementCount;
            bool ArcBFType, AutoContinueForOuterEdge;
            if (WidthTxtBx.Text.Length > 0)
                Width = Convert.ToDouble(WidthTxtBx.Text);
            else Width = 0;
            if (BFPtsBundleTxtBx.Text.Length > 0)
                BFPtsBundle = Convert.ToInt32(BFPtsBundleTxtBx.Text);
            else BFPtsBundle = 0;
            if (ScanJumpTxtBx.Text.Length > 0)
                ScanJump = Convert.ToInt32(ScanJumpTxtBx.Text);
            else ScanJump = 0;
            if (MeasurementCountTxtBx.Text.Length > 0)
                MeasurementCount = Convert.ToInt32(MeasurementCountTxtBx.Text);
            else MeasurementCount = 0;
            ArcBFType = (bool)ArcBFTypeRBtn.IsChecked;
            AutoContinueForOuterEdge = (bool)AutoContinueOuterChkBx.IsChecked;
            RWrapper.RW_AutoProfileScanMeasure.MYINSTANCE().StartAutoProfileScan(Width, BFPtsBundle, ScanJump, MeasurementCount, ArcBFType, AutoContinueForOuterEdge);
            this.Close();
        }
        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            SaveParameters();
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ProfileScanWidthWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("WidthTxtBx", WidthTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("BFPtsBundle", BFPtsBundleTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ScanJump", ScanJumpTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MeasurementCount", MeasurementCountTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("LineBFType", LineBFTypeRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ArcBFType", ArcBFTypeRBtn.IsChecked.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("AutoContinueOuter", AutoContinueOuterChkBx.IsChecked.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "WidthTxtBx")
                                {
                                    WidthTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "BFPtsBundle")
                                {
                                    BFPtsBundleTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "ScanJump")
                                {
                                    ScanJumpTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "MeasurementCount")
                                {
                                    MeasurementCountTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "LineBFType")
                                {
                                    LineBFTypeRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "ArcBFType")
                                {
                                    ArcBFTypeRBtn.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                if (ListOfParameters[i].Name == "AutoContinueOuter")
                                {
                                    AutoContinueOuterChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ProfileScanWidthWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("WidthTxtBx", WidthTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("BFPtsBundle", BFPtsBundleTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ScanJump", ScanJumpTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MeasurementCount", MeasurementCountTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("LineBFType", LineBFTypeRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ArcBFType", ArcBFTypeRBtn.IsChecked.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("AutoContinueOuter", AutoContinueOuterChkBx.IsChecked.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                    ListOfParameters = null;
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion
    }
}