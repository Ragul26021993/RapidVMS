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
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ShapeInfoWin.xaml
    /// </summary>
    public partial class CalibInfoWin : Window, ISaveWindowsParameters
    {
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        public CalibInfoWin()
        {
            this.InitializeComponent();
            ReadParameters();
            // Insert code required on object creation below this point.
        }
        //On ok click just return the dialog result as true.
        private void OKBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().SetPLaceCalibrationDeatil(McNumTxtBx.Text, CustNameTxtBx.Text, VerificationByTxtBx.Text, DateTxtBx.Text);
            this.Result = true;
            this.Close();
        }
        private void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Result = false;
            this.Close();
        }
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            this.Hide();
            SaveParameters();
            e.Cancel = true;
        }
        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CalibInfoWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("McNum", McNumTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CustName", CustNameTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("VerificationBy", VerificationByTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("DateTxt", DateTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "McNum")
                                {
                                    McNumTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "CustName")
                                {
                                    CustNameTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "VerificationBy")
                                {
                                    VerificationByTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "DateTxt")
                                {
                                    DateTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\CalibInfoWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("McNum", McNumTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CustName", CustNameTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("VerificationBy", VerificationByTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("DateTxt", DateTxtBx.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
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