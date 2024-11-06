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
using System.Linq;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for CalibrationWin.xaml
	/// </summary>
	public partial class ImageCaptureGridWin : Window
	{
        public ImageCaptureGridWin()
		{
			this.InitializeComponent();
            ReadParameters();
            tbImageGridDSavePath.Text = GlobalSettings.ImageGridSavePath;
            RWrapper.RW_ImageGridProgram.MYINSTANCE().SetStoragePath(GlobalSettings.ImageGridSavePath);

            // Insert code required on object creation below this point.
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            RunPauseBtn.Click += new RoutedEventHandler(RunPauseBtn_Click);
            
            StopBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Stop", "Part Program", 32, 32);
            StopBtn.ToolTip = "Stop";
            StopBtn.Tag = "Stop";
            StopBtn.Click += new RoutedEventHandler(StopBtn_Click);

            ClearAllBtn.Click += new RoutedEventHandler(ClearAllBtn_Click);
            RWrapper.RW_ImageGridProgram.MYINSTANCE().CallPartProgramBuildEvent += new RWrapper.RW_ImageGridProgram.PartProgramBuildEventHandler(ImageCaptureGridWin_CallPartProgramBuildEvent);
        }
        void RunPauseBtn_Click(object sender, RoutedEventArgs e)
        {
            string BtnTag = RunPauseBtn.Tag as string;
            if (BtnTag == "Run" || BtnTag == "Continue")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Pause", "Pause", 32, 32);
                RunPauseBtn.ToolTip = "Pause";
                RunPauseBtn.Tag = "Pause";
                if (BtnTag == "Run")
                {
                    if (!System.IO.Directory.Exists(tbImageGridDSavePath.Text + "\\" + txtBatchName.Text))
                        System.IO.Directory.CreateDirectory(tbImageGridDSavePath.Text + "\\" + txtBatchName.Text);

                    RWrapper.RW_ImageGridProgram.MYINSTANCE().CreatePathListForImageAction(Convert.ToInt16(NoOfRowsTxtBx.Text), Convert.ToInt16(NoOfColsTxtBx.Text), Convert.ToDouble(CellHeightTxtBx.Text), Convert.ToDouble(CellWidthTxtBx.Text));
                    ClearAllBtn.IsEnabled = false;
                }
                else
                    RWrapper.RW_ImageGridProgram.MYINSTANCE().Continue_PauseImageAction(true);
            }
            else if (BtnTag == "Pause")
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Continue", "Continue", 32, 32);
                RunPauseBtn.ToolTip = "Continue";
                RunPauseBtn.Tag = "Continue";
                RWrapper.RW_ImageGridProgram.MYINSTANCE().Continue_PauseImageAction(false);
            }
        }
        void StopBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_ImageGridProgram.MYINSTANCE().StopImageAction();
            RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
            RunPauseBtn.ToolTip = "Run";
            RunPauseBtn.Tag = "Run";
            ClearAllBtn.IsEnabled = true;
        }
        void ClearAllBtn_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_ImageGridProgram.MYINSTANCE().ClearAll();
        }
        void ImageCaptureGridWin_CallPartProgramBuildEvent()
        {
            if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
            {
                RunPauseBtn.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Run", "Run", 32, 32);
                RunPauseBtn.ToolTip = "Run";
                RunPauseBtn.Tag = "Run";
                ClearAllBtn.IsEnabled = true;
                Button BldBtn = (Button)GlobalSettings.MainWin.St.Children.OfType<Button>().Where(T => ((string[])T.Tag)[2] == "Build").First();
                GlobalSettings.MainWin.HandleLowerPanelsButtonsClick(BldBtn, null);
            }
            else
            {
                this.Dispatcher.Invoke(new RWrapper.RW_ImageGridProgram.PartProgramBuildEventHandler(ImageCaptureGridWin_CallPartProgramBuildEvent));
            }
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ImageCaptureGridWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfRows", NoOfRowsTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NoOfCols", NoOfColsTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CellWidth", CellWidthTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("CellHeight", CellHeightTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "NoOfRows")
                                {
                                    NoOfRowsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "NoOfCols")
                                {
                                    NoOfColsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "CellWidth")
                                {
                                    CellWidthTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "CellHeight")
                                {
                                    CellHeightTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\ImageCaptureGridWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfRows", NoOfRowsTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NoOfCols", NoOfColsTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CellWidth", CellWidthTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("CellHeight", CellHeightTxtBx.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
                    ListOfParameters.Clear();
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion

        private void BtnFolder_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog fbd = new System.Windows.Forms.FolderBrowserDialog();
            fbd.ShowNewFolderButton = true;
            fbd.ShowDialog();

            if (fbd.SelectedPath != "")
            {
                //GlobalSettings.ImageGridSavePath = fbd.SelectedPath;
                tbImageGridDSavePath.Text = fbd.SelectedPath;
            }

        }

        private void txtBatchName_TextChanged(object sender, TextChangedEventArgs e)
        {
            RWrapper.RW_ImageGridProgram.MYINSTANCE().SetStoragePath(tbImageGridDSavePath.Text + "\\" + txtBatchName.Text);
        }

        private void btnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void BtnOpenFolder_Click(object sender, RoutedEventArgs e)
        {
            System.Diagnostics.Process.Start("explorer.exe", tbImageGridDSavePath.Text + "\\" + txtBatchName.Text);
        }
    }
}