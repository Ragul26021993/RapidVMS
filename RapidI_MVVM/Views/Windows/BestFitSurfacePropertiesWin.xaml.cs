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
	/// Interaction logic for ShapeInfoWin.xaml
	/// </summary>
	public partial class BestFitSurfacePropertiesWin : Window,ISaveWindowsParameters
	{
        //Constructor
        public BestFitSurfacePropertiesWin()
		{
            try
            {
                this.InitializeComponent();
                ReadParameters();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:BFWIN01", ex); }
		}
        
        //Function to Set the initial Values
        public void SetInitialValues(double PtsFactor, bool BFSurfaceAroundXYPln)
        {
            try
            {
                PtsFactorTxtBx.Text = PtsFactor.ToString();
                BFSurfaceAroundXYPlnChkBx.IsChecked = BFSurfaceAroundXYPln;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:BFWIN02", ex); }
        }
        //On ok click
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().BestFitWithDifferentParam(double.Parse(PtsFactorTxtBx.Text), (bool)BFSurfaceAroundXYPlnChkBx.IsChecked, false);
                this.Close();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:BFWIN03", ex); }
        }
        //On apply click - apply the values to framework
        private void btnApply_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().BestFitWithDifferentParam(double.Parse(PtsFactorTxtBx.Text), (bool)BFSurfaceAroundXYPlnChkBx.IsChecked, true);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:BFWIN04", ex); }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            SaveParameters();
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void SaveParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\BestFitSurfacePropertiesWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("PtsFactor", PtsFactorTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("BFSurfaceAroundXYPln", (BFSurfaceAroundXYPlnChkBx.IsChecked).ToString()));
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\BestFitSurfacePropertiesWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("PtsFactor", PtsFactorTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("BFSurfaceAroundXYPln", (BFSurfaceAroundXYPlnChkBx.IsChecked).ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "PtsFactor")
                                {
                                    PtsFactorTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                else if (ListOfParameters[i].Name == "BFSurfaceAroundXYPln")
                                {
                                    BFSurfaceAroundXYPlnChkBx.IsChecked = Convert.ToBoolean(ListOfParameters[i].Value.ToString());
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