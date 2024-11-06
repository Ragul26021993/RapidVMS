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
using CalibrationModule;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for ShapeInfoWin.xaml
	/// </summary>
	public partial class DriveSelectionWin : Window
	{
		public event RoutedEventHandler WinClosing;
		DB Settings_DataB;
		System.Data.DataRow CurrentSettingsRow;
		public bool Result { get; set; }
		public string SelectedProgramPath, SelectedReportPath;
		public DriveSelectionWin()
		{
			this.InitializeComponent();
			
			// Insert code required on object creation below this point.
			init();
		}
		private void init()
		{
			//SystemDriveCBx.Items.Clear();
			//System.IO.DriveInfo[] DirectoryList = System.IO.DriveInfo.GetDrives();
			//foreach (System.IO.DriveInfo D in DirectoryList)
			//    if (D.DriveType == System.IO.DriveType.Fixed)
			//        SystemDriveCBx.Items.Add(D.Name);
			//SelectedDrive = SystemDriveCBx.Items[0].ToString();
			//SystemDriveCBx.SelectedIndex = 0;
			//this.PreviewKeyDown += new KeyEventHandler(txtNewName_PreviewKeyDown);

			Settings_DataB = new DB(RWrapper.RW_MainInterface.DBConnectionString);
			Settings_DataB.FillTable("CurrentMachine");
			Settings_DataB.FillTable("UserSettings");
			CurrentSettingsRow = Settings_DataB.Select_ChildControls("UserSettings", "MachineNo", GlobalSettings.MachineNo)[0];
		}
		//On ok click just return the dialog result as true.
		private void btnOK_Click(object sender, RoutedEventArgs e)
		{
			this.Result = true;
			SelectedProgramPath = tblock_ProgramLocator.Text;//SystemDriveCBx.SelectedItem.ToString();
			UpdateDrive();
			this.Close();
		}
		//if user presses enter key in textbox just make the dialog result to be true.
		private void txtNewName_PreviewKeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter)
			{
				this.Result = true;
				SelectedProgramPath = tblock_ProgramLocator.Text;  //SystemDriveCBx.SelectedItem.ToString();
				SelectedReportPath = tblock_ReportLocator.Text;  //SystemDriveCBx.SelectedItem.ToString();
				UpdateDrive();
				e.Handled = true;
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
			e.Cancel = true;
		}
		private void UpdateDrive()
		{
			CurrentSettingsRow["SelectProgramDrive"] = SelectedProgramPath;
			CurrentSettingsRow["SelectReportPath"] = SelectedProgramPath;
			Settings_DataB.Update_Table("UserSettings");
		}
		public bool CheckProgramDrive(string FilePath)
		{
			//FilePath = FilePath.Substring(0, FilePath.IndexOf("\\") + 1);
			if (System.IO.Directory.Exists(FilePath)) return true;
			//if (tblock_ProgramLocator.Text !="") //SystemDriveCBx.Items.Contains(FilePath))
			//    return true;
			return false;
		}

		private void Btn_ProgramLocator_Click(object sender, RoutedEventArgs e)
		{
			System.Windows.Forms.FolderBrowserDialog fbd = new System.Windows.Forms.FolderBrowserDialog();
			fbd.ShowDialog();
			Button btn = (Button)sender;
			TextBlock tb = tblock_ProgramLocator;
			if (btn.Name.Contains("Report"))
				tb = tblock_ReportLocator;
			
			tb.Text = fbd.SelectedPath;
			if (!tb.Text.EndsWith(Convert.ToString(Microsoft.VisualBasic.Strings.Chr(92))))
				tb.Text += Microsoft.VisualBasic.Strings.Chr(92);
			if (!(bool)chkDecoupleReportPath.IsChecked)
				tblock_ReportLocator.Text = tb.Text;
		}
	}
}