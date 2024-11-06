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

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for PPBuildWin.xaml
    /// </summary>
    public partial class PPBuildWin : OpenGlWin
    {
        public bool ExportToCsv = false;
        public PPBuildWin()
        {
            this.InitializeComponent();
            // Insert code required on object creation below this point.
            init();
            ExportToCsv = Convert.ToBoolean(GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("UserSettings")[0]["EnableCsvExport"]);
        }
        void init()
        {
            AllExistingProgramCBx.SelectionChanged += new SelectionChangedEventHandler(AllExistingProgramCBx_SelectionChanged);
            this.txtProgramName.TextChanged += new TextChangedEventHandler(txtProgramName_TextChanged);
        }

        void txtProgramName_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (txtReportName.Text.Contains("_"))
            {
                txtReportName.Text = txtProgramName.Text + "_" + txtReportName.Text.Substring(txtReportName.Text.LastIndexOf('_') + 1, txtReportName.Text.Length - txtReportName.Text.LastIndexOf('_') - 1);
                if (AllExistingProgramCBx.SelectedIndex == -1)
                {
                    txtReportStyle.Text = txtProgramName.Text + "_" + txtReportStyle.Text.Substring(txtReportStyle.Text.LastIndexOf('_') + 1, txtReportStyle.Text.Length - txtReportStyle.Text.LastIndexOf('_') - 1);
                    GlobalSettings.MainExcelInstance.UseExistingReport = false;
                }
                else
                {
                    string RptName = AllExistingProgramCBx.SelectedItem.ToString();
                    AllExistingProgramCBx.Text = RptName;
                    txtReportName.Text = RptName.Substring(0, RptName.LastIndexOf(".xls"));
                    GlobalSettings.MainExcelInstance.UseExistingReport = true;
                }
            }
        }
        public void SetProgramName(string ProgramName)
        {
            txtProgramName.Text = ProgramName;
        }
        public void SetProgramPath(string ProgramPath)
        {
            txtProgramPath.Text = ProgramPath;
        }
        public void SetReportName(string ReportName)
        {
            if (AllExistingProgramCBx.SelectedIndex == -1)
                txtReportName.Text = ReportName;
            else
            {
                string RptName = AllExistingProgramCBx.SelectedItem.ToString();
                AllExistingProgramCBx.Text = RptName;
                txtReportName.Text = RptName.Substring(0, RptName.LastIndexOf(".xls"));
                GlobalSettings.MainExcelInstance.UseExistingReport = true;
            }
        }
        public void SetReportPath(string ReportPath)
        {
            txtReportPath.Text = ReportPath;
        }
        public void SetReportStyle(string ReportStyle)
        {
            txtReportStyle.Text = ReportStyle;
        }
        public string GetProgramName
        {
            get
            { return txtProgramPath.Text + "\\" + txtProgramName.Text + ".rppx"; }

        }
        public string GetReportName
        {
            get
            { return txtReportPath.Text + "\\" + txtReportName.Text + ".xls"; }
        }
        public string GetReportStyle
        {
            get
            { return txtReportStyle.Text; }
        }

        public void ResetAllReportCBx()
        {
            AllExistingProgramCBx.ItemsSource = null;
            AllExistingProgramCBx.ItemsSource = GlobalSettings.AllExistingPartProgramNames;
            AllExistingProgramCBx.Text = "";
            GlobalSettings.MainExcelInstance.UseExistingReport = false;
            this.UpdateLayout();
        }
        void AllExistingProgramCBx_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            GlobalSettings.MainExcelInstance.UseExistingReport = false;
            if (AllExistingProgramCBx.SelectedIndex > -1)
            {
                string RptName = AllExistingProgramCBx.SelectedItem.ToString();
                AllExistingProgramCBx.Text = RptName;
                txtReportName.Text = RptName.Substring(0, RptName.LastIndexOf(".xls"));
                GlobalSettings.MainExcelInstance.UseExistingReport = true;
            }
        }
    }
}