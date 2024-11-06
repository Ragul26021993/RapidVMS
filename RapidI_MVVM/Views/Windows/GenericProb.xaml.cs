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
using System.Data.OleDb;
using System.Data;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CloudPointMeasurement.xaml
    /// </summary>
    public partial class GenericProb : Window
    {
        public event RoutedEventHandler WinClosing;
        private ObservableCollection<UserDefinedProbType> _PTypeColl;
        public ObservableCollection<UserDefinedProbType> PTypeColl
        {
            get
            {
                return _PTypeColl;
            }
            set
            {
                _PTypeColl = value;
            }
        }
        //public ObservableCollection<Probedetails> ProbeDetails;        
        public bool WindowOpen = false;
        static GenericProb instance;

        public static GenericProb Myinstance()
        {
            if (instance == null)
            {
                instance = new GenericProb();
            }
            return instance;
        }

        public GenericProb()
        {
            this.InitializeComponent();
            this.Loaded += new RoutedEventHandler(GenericProb_Loaded);
            init();
            //ProbDetailsDG.ItemsSource = ProbeDetails;
        }

        void GenericProb_Loaded(object sender, RoutedEventArgs e)
        {

        }

        //This function is made public because when values get updated in settings then, at the same time values in the grid here should also gets updated.
        //Call this method through the object of main Win....Shweytank

        public void init()
        {
            try
            {
                //DataRow[] DetailsRow = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("ProbAssemblyTypeDetails");
                PTypeColl = new ObservableCollection<UserDefinedProbType>();
                DataTable DetailsDt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                List<string> UniqueProbeTypeList = new List<string>();
                for (int i = 0; i < DetailsDt.Rows.Count; i++)
                {
                    string ProbetypeString = DetailsDt.Rows[i]["ProbType"].ToString();
                    if (ProbetypeString != "")
                    {
                        if (!UniqueProbeTypeList.Contains(ProbetypeString))
                        {
                            UniqueProbeTypeList.Add(ProbetypeString);
                        }
                        else
                            continue;
                    }
                }
                foreach (string Probetype in UniqueProbeTypeList)
                {
                    DataRow[] DetailsRow = DetailsDt.Select("ProbType = '" + Probetype + "'");
                    UserDefinedProbType probetype = new UserDefinedProbType();
                    probetype.ProbtypeName = Probetype;
                    probetype.ProbeDetails = new ObservableCollection<Probedetails>();
                    //ProbDetailsDG.ItemsSource = ProbeDetails;
                    if (DetailsRow.Count() > 0)
                    {
                        foreach (DataRow DRows in DetailsRow)
                        {
                            Probedetails pd = new Probedetails();
                            pd.ProbtypeName = DRows["ProbType"].ToString();
                            pd.OffsetX = DRows["OffsetX"].ToString();
                            pd.OffsetY = DRows["OffsetY"].ToString();
                            pd.OffsetZ = DRows["OffsetZ"].ToString();
                            pd.Comment = DRows["Comment"].ToString();
                            pd.Orientation = DRows["Orientation"].ToString();
                            pd.ID = Convert.ToInt32(DRows["ID"].ToString());
                            pd.Radius = Convert.ToInt32(DRows["Radius"].ToString());
                            pd.MachineNo = DRows["MachineNo"].ToString();
                            probetype.ProbeDetails.Add(pd);
                        }
                    }
                    PTypeColl.Add(probetype);
                }
                for (int i = 0; i < PTypeColl.Count; i++)
                {
                    if (PTypeColl[i].ProbtypeName == GlobalSettings.CurrentGenericProbeType)
                    {
                        ProbeType.SelectedItem = PTypeColl[i];
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:GP01", ex);
            }
        }

        protected void AddBtn_Click(object Sender, RoutedEventArgs e)
        {
            try
            {
                string PTNAME = ProbeType.Text;
                bool AlreadyExist = false;
                if (PTNAME == "")
                {
                    return;
                }
                foreach (UserDefinedProbType UDPT in PTypeColl)
                {
                    if (PTNAME == UDPT.ProbtypeName)
                    {
                        AlreadyExist = true;
                        break;
                    }
                }
                if (AlreadyExist == true)
                {
                    MessageBox.Show("Probe Type  already exist ");
                }
                else
                {
                    DataTable Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                    int MaxId = 0;
                    if (Dt.Rows.Count != 0)
                    {
                        MaxId = Convert.ToInt32(Dt.Compute("MAX(ID)", ""));
                    }

                    DataRow drow = Dt.NewRow();
                    drow["ProbType"] = PTNAME;
                    drow["OffsetX"] = 0;
                    drow["OffsetY"] = 0;
                    drow["OffsetZ"] = 0;
                    drow["Comment"] = "None";
                    drow["IsValid"] = true;
                    drow["Orientation"] = "None";
                    drow["MachineNo"] = GlobalSettings.MachineNo;
                    drow["Radius"] = 2;
                    Dt.Rows.Add(drow);
                    GlobalSettings.SettingsReader.UpdateTable("ProbAssemblyTypeDetails");

                    UserDefinedProbType Ptype = new UserDefinedProbType();
                    Ptype.ProbtypeName = PTNAME;
                    Ptype.ProbeDetails = new ObservableCollection<Probedetails>();
                    Probedetails pd = new Probedetails();
                    pd.ProbtypeName = PTNAME;
                    pd.OffsetX = "0";
                    pd.OffsetY = "0";
                    pd.OffsetZ = "0";
                    pd.Comment = "None";
                    pd.Orientation = "None";
                    pd.ID = ++MaxId;
                    Ptype.ProbeDetails.Add(pd);
                    PTypeColl.Add(Ptype);
                    ProbeType.SelectedItem = Ptype;
                    //ProbeType.SelectedIndex = PTypeColl.Count - 1;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:GP02", ex);
            }
        }

        protected void UpdateBtn_Click(object Sender, RoutedEventArgs e)
        {
            try
            {
                ObservableCollection<UserDefinedProbType> ProbelistColl = PTypeColl;
                if (ProbelistColl == null)
                    return;

                DataTable Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                int MaxId = Convert.ToInt32(Dt.Compute("MAX(ID)", ""));
                DataRow[] DetailsRow = Dt.Select("MachineNo= '" + GlobalSettings.MachineNo + "'");

                //To Update and Delete the Rows

                int Rowcount = DetailsRow.Count();
                for (int i = 0; i < Rowcount; i++)
                {
                    bool RowDeleted = true;
                    for (int Pcount = 0; Pcount < ProbelistColl.Count; Pcount++)
                    {
                        for (int j = 0; j < ProbelistColl[Pcount].ProbeDetails.Count; j++)
                        {
                            try
                            {
                                if (Convert.ToInt32(DetailsRow[i]["ID"]) == ProbelistColl[Pcount].ProbeDetails[j].ID)
                                {
                                    DetailsRow[i]["OffsetX"] = ProbelistColl[Pcount].ProbeDetails[j].OffsetX;
                                    DetailsRow[i]["OffsetY"] = ProbelistColl[Pcount].ProbeDetails[j].OffsetY;
                                    DetailsRow[i]["OffsetZ"] = ProbelistColl[Pcount].ProbeDetails[j].OffsetZ;
                                    DetailsRow[i]["Comment"] = ProbelistColl[Pcount].ProbeDetails[j].Comment;
                                    DetailsRow[i]["Orientation"] = ProbelistColl[Pcount].ProbeDetails[j].Orientation;
                                    DetailsRow[i]["Radius"] = ProbelistColl[Pcount].ProbeDetails[j].Radius;
                                    RowDeleted = false;
                                    break;
                                }
                            }
                            catch (DeletedRowInaccessibleException)
                            {

                            }
                        }
                    }
                    if (RowDeleted)
                    {
                        DetailsRow[i].Delete();
                        GlobalSettings.SettingsReader.UpdateTable("ProbAssemblyTypeDetails");
                        Dt = GlobalSettings.SettingsReader.GetTable("ProbAssemblyTypeDetails");
                        DetailsRow = Dt.Select("MachineNo= '" + GlobalSettings.MachineNo + "'");
                        Rowcount--;
                        i--;
                    }
                }
                //To Add new rows
                for (int Pcount = 0; Pcount < ProbelistColl.Count; Pcount++)
                {
                    for (int i = 0; i < ProbelistColl[Pcount].ProbeDetails.Count; i++)
                    {
                        if (ProbelistColl[Pcount].ProbeDetails[i].ID == 0)
                        {
                            DataRow drow = Dt.NewRow();
                            drow["ProbType"] = ProbelistColl[Pcount].ProbtypeName;
                            drow["OffsetX"] = ProbelistColl[Pcount].ProbeDetails[i].OffsetX;
                            drow["OffsetY"] = ProbelistColl[Pcount].ProbeDetails[i].OffsetY;
                            drow["OffsetZ"] = ProbelistColl[Pcount].ProbeDetails[i].OffsetZ;
                            drow["Comment"] = ProbelistColl[Pcount].ProbeDetails[i].Comment;
                            drow["Orientation"] = ProbelistColl[Pcount].ProbeDetails[i].Orientation;
                            drow["Radius"] = ProbelistColl[Pcount].ProbeDetails[i].Radius;
                            drow["IsValid"] = true;
                            drow["MachineNo"] = GlobalSettings.MachineNo;
                            ProbelistColl[Pcount].ProbeDetails[i].ID = ++MaxId;
                            Dt.Rows.Add(drow);
                        }
                    }
                }
                GlobalSettings.SettingsReader.UpdateTable("ProbAssemblyTypeDetails");
                MessageBox.Show("Values Updated");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:GP03", ex);
            }
        }

        private void ProbTypeCBk_SelectionChanged(object sender, TextChangedEventHandler e)
        {
            try
            {
                ObservableCollection<UserDefinedProbType> UpdatedCollection = PTypeColl;
            }
            catch (Exception)
            {
                throw;
            }
        }

        private void ProbDetailsDG_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            int indx = ProbDetailsDG.SelectedIndex;
            if (indx != -1)
            {
                if (!WindowOpen)
                {
                    Probedetails SelectedObj = (Probedetails)(ProbDetailsDG.SelectedItem);
                    RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch((int)SelectedObj.ID);
                    WindowOpen = true;
                    this.Hide();
                    this.Show();
                }
            }
        }

        public void SendStylusData()
        {
            if (ProbDetailsDG.SelectedIndex != -1)
            {
                Probedetails SelectedObj = (Probedetails)(ProbDetailsDG.SelectedItem);
                RWrapper.RW_DRO.MYINSTANCE().GetProbePositionInTouch((int)SelectedObj.ID);
            }
        }

        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            ProbDetailsDG.SelectedIndex = -1;
            WindowOpen = false;
            RaiseEvent();
            instance = null;
            //this.Hide();
            //e.Cancel = true;
        }
    }

    public class ProbeList : ObservableCollection<UserDefinedProbType> { }
    public class UserDefinedProbType
    {
        public UserDefinedProbType()
        {
        }
        string _ProbeTypeName;
        public string ProbtypeName
        {
            get
            {
                return _ProbeTypeName;
            }
            set
            {
                _ProbeTypeName = value;
            }
        }
        public ObservableCollection<Probedetails> ProbeDetails { get; set; }
    }
    public class Probedetails : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        private string _orientation;
        private string _offsetX;
        private string _offsetY;
        private string _offsetZ;
        private string _Comment;
        private double _radius = 2;
        private string _ProbeTypeName;
        private string _MachineNo;
        public long ID = 0;

        public string ProbtypeName
        {
            get
            {
                return _ProbeTypeName;
            }
            set
            {
                _ProbeTypeName = value;
            }
        }

        public string Orientation
        {
            get
            {
                return _orientation;
            }
            set
            {
                if (this._orientation == value) return;
                _orientation = value;
                Notify("Orientation");
            }
        }

        public string OffsetX
        {
            get
            {
                return _offsetX;
            }
            set
            {
                if (this._offsetX == value) return;
                _offsetX = value;
                Notify("OffsetX");
            }
        }

        public string OffsetY
        {
            get
            {
                return _offsetY;
            }
            set
            {
                if (this._offsetY == value) return;
                _offsetY = value;
                Notify("Offsety");
            }
        }

        public string OffsetZ
        {
            get
            {
                return _offsetZ;
            }
            set
            {
                if (this._offsetZ == value) return;
                _offsetZ = value;
                Notify("OffsetZ");
            }
        }

        public string Comment
        {
            get
            {
                return _Comment;
            }
            set
            {
                if (this._Comment == value) return;
                _Comment = value;
                Notify("Comment");
            }
        }

        public double Radius
        {
            get
            {
                return _radius;
            }
            set
            {
                if (this._radius == value) return;
                _radius = value;
                Notify("Radius");
            }
        }

        public string MachineNo
        {
            get
            {
                return _MachineNo;
            }
            set
            {
                if (this._MachineNo == value) return;
                _MachineNo = value;
                Notify("MachineNo");
            }
        }
    }
}
