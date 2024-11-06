using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Input;
using Rapid.Interface;
using Rapid.IO;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ShapeInfoWin.xaml
    /// </summary>
    public partial class AutoProbePathPropWin : Window, ISaveWindowsParameters
    {
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        public double RadiusVal, ToleranceVal;

        public AutoProbePathPropWin()
        {
            this.InitializeComponent();
            ReadParameters();
            // Insert code required on object creation below this point.
        }

        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            this.Result = true;
            UpdateValues();
            this.Close();
        }
        //On cancel click just return the dialog result as false.
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            this.Result = false;
            this.Close();
        }
        //if user presses enter key in textbox just make the dialog result to be true.
        private void txtNewName_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                this.Result = true;
                UpdateValues();
                e.Handled = true;
                this.Close();
            }
            else if (e.Key == Key.Cancel)
            {
                this.Result = false;
                this.Close();
            }
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

        private void UpdateValues()
        {
            RadiusVal = 0; ToleranceVal = 0;
            double.TryParse(RadiusTxtBx.Text, out RadiusVal);
            double.TryParse(ToleranceTxtBx.Text, out ToleranceVal);
        }

        #region To save the Windows Parameters 
        public string TextFilePath { get; set; }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AutoProbePathPropWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("Radius", RadiusTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Tolerance", ToleranceTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ShwMeasureWithDeviation", ((bool)ShwMeasureWithDeviationChkBx.IsChecked).ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CompanyName", CompanyNameTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CompDrawingNum", CompDrawingNumTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CompSerialNum", CompSerialNumTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CompRevisionNumTxtBx", CompRevisionNumTxtBx.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\AutoProbePathPropWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("Radius", RadiusTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Tolerance", ToleranceTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ShwMeasureWithDeviation", ((bool)ShwMeasureWithDeviationChkBx.IsChecked).ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CompanyName", CompanyNameTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CompDrawingNum", CompDrawingNumTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CompSerialNum", CompSerialNumTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CompRevisionNumTxtBx", CompRevisionNumTxtBx.Text.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "Radius")
                                {
                                    RadiusTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "Tolerance")
                                {
                                    ToleranceTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "ShwMeasureWithDeviation")
                                {
                                    ShwMeasureWithDeviationChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
                                }
                                else if (ListOfParameters[i].Name == "CompanyName")
                                {
                                    CompanyNameTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "CompDrawingNum")
                                {
                                    CompDrawingNumTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "CompSerialNum")
                                {
                                    CompSerialNumTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "CompRevisionNumTxtBx")
                                {
                                    CompRevisionNumTxtBx.Text = ListOfParameters[i].Value.ToString();
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
        #endregion
    }
}