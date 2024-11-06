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
    public partial class TransparencySettingWin : Window,ISaveWindowsParameters
    {
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        public TransparencySettingWin()
        {
            this.InitializeComponent();
            ReadParameters();
            this.TransparencyValueSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(TransparencyValueSlider_ValueChanged);
            // Insert code required on object creation below this point.
        }
        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        { this.Result = true; this.Close(); }
        //On cancel click  just return the dialog result as false.
        private void btnCancel_Click(object sender, RoutedEventArgs e)
        { this.Result = false; this.Close(); }
        //if user presses enter key in textbox just make the dialog result to be true.
        private void txtNewName_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) { this.Result = true; e.Handled = true; this.Close(); }
            else if (e.Key == Key.Escape) { this.Result = false; e.Handled = true; this.Close(); }
        }
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            SaveParameters();
            this.Hide();
            e.Cancel = true;
        }
        void TransparencyValueSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().SetEntityTransparency(TransparencyValueSlider.Value / 100);
        }
        public void ClearAll()
        {
            TransparencyValueSlider.Value = 40;
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\TransparencySettingWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("TransparencyValueSlider", TransparencyValueSlider.Value.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "TransparencyValueSlider")
                                {
                                    TransparencyValueSlider.Value = Convert.ToInt32(ListOfParameters[i].Value.ToString());
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\TransparencySettingWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("TransparencyValueSlider", TransparencyValueSlider.Value.ToString()));
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