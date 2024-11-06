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
	/// Interaction logic for CalibrationWin.xaml
	/// </summary>
	public partial class XYCorrection : Window
	{
        double dx=0, dy=0;
        public XYCorrection()
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
        }

        void SaveBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                dx = Convert.ToDouble(XCTxtBx.Text);
                dy = Convert.ToDouble(YCTxtBx.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return;
            }
            if (saveCorrectionvalues())
            {
                MessageBox.Show("Correction Values Saved");
            }
        }       
        private bool saveCorrectionvalues()
        {
            bool IsvalueUpdated=false;
            string Machineno = "";
            System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection();
            con.ConnectionString = RWrapper.RW_MainInterface.DBConnectionString;
            System.Data.OleDb.OleDbCommand cmd = new System.Data.OleDb.OleDbCommand();
            cmd.Connection = con;
            string query1 = "select * from CurrentMachine";
            System.Data.OleDb.OleDbDataAdapter adapter = new System.Data.OleDb.OleDbDataAdapter(query1, con);
            System.Data.DataTable DT = new System.Data.DataTable();
            con.Close();
            adapter.Fill(DT);
            Machineno = DT.Rows[0]["MachineNo"].ToString();
            string query = "update MeasurementSettings set X_Shift_FocusCalib=800 , Y_Shift_FocusCalib = 600 , Deviation1_FocusCalib = "+ dx + ", Deviation2_FocusCalib = " + dy + " where MachineNo = '" + Machineno + "'";
            cmd.CommandText = query;
            con.Open();
            try
            {
                cmd.ExecuteNonQuery();
                IsvalueUpdated = true;
                con.Close();
            }
            catch (Exception ex)
            {
                con.Close();
                IsvalueUpdated = false;
                MessageBox.Show(ex.Message);
            }
            return IsvalueUpdated;
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\XYCorrection.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("XCorrection", XCTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("YCorrection", YCTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "XCorrection")
                                {
                                    XCTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "YCorrection")
                                {
                                    YCTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\XYCorrection.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("XCorrection", XCTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("YCorrection", YCTxtBx.Text.ToString()));
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