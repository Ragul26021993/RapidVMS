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
using System.Collections.ObjectModel;
using System.ComponentModel;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for TouchVisionCaliberationWin.xaml
    /// </summary>
    public partial class TouchVisionCaliberationWin : Window, INotifyPropertyChanged
    {
        #region Variable and Event Deleclaration
        
        private static TouchVisionCaliberationWin _MyInstance;        
        public static TouchVisionCaliberationWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new TouchVisionCaliberationWin();
            }
            return _MyInstance;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string popertyname)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(popertyname));
            }
        }

        public EventHandler OnClosingEvent;

        private double _SphereRadius = 1;
        private double _ProbeRadius = 1;
        private string _SelectedProbeType;

        public double SphereRadius
        {
            get
            {
                return _SphereRadius;
            }
            set
            {
                _SphereRadius = value;
                Notify("SphereRadius");
            }
        }
        public double ProbeRadius
        {
            get
            {
                return _ProbeRadius;
            }
            set
            {
                _ProbeRadius = value;
                Notify("ProbeRadius");
            }
        }
        public string ProbeType
        {
            get
            {
                return _SelectedProbeType;
            }
            set
            {
                _SelectedProbeType = value;
                Notify("ProbeType");
            }
        }

        #endregion

        private TouchVisionCaliberationWin()
        {
            InitializeComponent();
            init();
            ReadParameters();
        }

        void init()
        {
            FillCombobox(ref ProbeTypeCBx, "Probe", "ProbeType", "Frequency");
        }

        private void FillCombobox(ref ComboBox Combobx, string tablename, string column, string OrderByColumn = "", string filter = "")
        {
            try
            {
                Combobx.Items.Clear();
                System.Data.OleDb.OleDbConnection con = new System.Data.OleDb.OleDbConnection();
                con.ConnectionString = RWrapper.RW_MainInterface.DBConnectionString;
                string query = "";
                if (filter == "" || filter == null)
                {
                    query = "Select (" + column + ") from " + tablename;
                }
                else
                {
                    query = "Select (" + column + ") from " + tablename + " where " + filter;
                }

                if (OrderByColumn != "")
                {
                    //This is because in SQL Query Order by Clause should always be at the last .
                    query += " Order By " + OrderByColumn + " ASC";
                }

                System.Data.OleDb.OleDbDataAdapter adapter = new System.Data.OleDb.OleDbDataAdapter(query, con);
                System.Data.DataTable DT = new System.Data.DataTable(tablename);
                adapter.Fill(DT);
                foreach (System.Data.DataRow Drow in DT.Rows)
                {
                    Combobx.Items.Add(Drow[0]);
                }
                if (Combobx.Items.Count > 0)
                {
                    Combobx.SelectedIndex = 0;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("StarProbAutoCalib:EX03", ex);
            }
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (OnClosingEvent != null)
            {
                OnClosingEvent(null, null);
            }
            SaveParameters();
            _MyInstance = null;
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\TouchVisionCaliberationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("SphereRadius", SphereRadiusTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ProbeRadius", ProbeRadiusTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ProbeType", ProbeTypeCBx.Text.ToString()));
                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "SphereRadius")
                                {
                                    SphereRadiusTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "ProbeRadius")
                                {
                                    ProbeRadiusTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "ProbeType")
                                {
                                    ProbeTypeCBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\TouchVisionCaliberationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("SphereRadius", SphereRadiusTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ProbeRadius", ProbeRadiusTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ProbeType", ProbeTypeCBx.Text.ToString()));
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