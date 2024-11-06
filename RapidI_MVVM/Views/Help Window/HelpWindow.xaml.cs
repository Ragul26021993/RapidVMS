using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media.Imaging;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Data.OleDb;
using System.Data;
using Rapid;

namespace RapidI.Help
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class HelpWindow : Window, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }
        private bool _EditModeBtn = false;
        public bool EditModeProp
        {
            get
            {
                return _EditModeBtn;
            }
            set
            {
                _EditModeBtn = value;
                if (_EditModeBtn)
                {
                    LblDescription.Visibility = System.Windows.Visibility.Collapsed;
                }
                else
                    LblDescription.Visibility = System.Windows.Visibility.Visible;
                Notify("EditModeProp");
            }
        }

        private bool _IsAdmminMode = false;
        public bool IsAdmminMode
        {
            get
            {
                return _IsAdmminMode;
            }
            set
            {
                _IsAdmminMode = value;
                if (!_IsAdmminMode)
                {
                    EditModeProp = false;
                }
                Notify("IsAdmminMode");
            }
        }

        #region Decleration Variable and Enums

        public enum SearchUsing { TopicId, TopicDisplayName, TopicName };
        public event RoutedEventHandler WinClosing;
        DisplayOnScreen DOS;
        public static string dbPath;
        private static HelpWindow MyObj = null;

        #endregion

        #region Construction and Window Load

        public static HelpWindow GetObj()
        {
            try
            {
                if (MyObj == null)
                    MyObj = new HelpWindow();
                MyObj.Owner = GlobalSettings.MainWin;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError01", ex);
            }
            return MyObj;
        }
        private HelpWindow()
        {
            InitializeComponent();
            init();
            this.Loaded += new RoutedEventHandler(HelpWindow_Loaded);
            this.EditMode.Click += new RoutedEventHandler(EditMode_Click);
        }
        protected void HelpWindow_Loaded(object sender, RoutedEventArgs e)
        {
        }
        protected void EditMode_Click(object sender, RoutedEventArgs e)
        {
            EditModeProp = (bool)EditMode.IsChecked;
        }

        void init()
        {
            try
            {
                DOS = new DisplayOnScreen(dbPath); //@"Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + dbpath
                DOS.TopicDisplayName = "Rapid I Help";
                DisplayHelpContainer.DataContext = DOS.Branches;
                ShowMain();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError03", ex);
            }
        }

        public void ShowMain()
        {
            DetailsGrpBx.Visibility = Visibility.Collapsed;
            DisplayNonLeafNode.Visibility = Visibility.Visible;
            DisplayNonLeafNode.DataContext = DOS;
        }

        #endregion

        void TreeEntity_Click(object Sender, RoutedEventArgs e)
        {
            object CurrentButton = ((Hyperlink)Sender).CommandParameter;
            TopicEntity FoundEntity = (TopicEntity)CurrentButton;
            ShowTreeNodeDetails(FoundEntity);
        }

        void ShowTreeNodeDetails(TopicEntity CurrentNode)
        {
            if (CurrentNode.IsLeafNode)
            {
                DisplayNonLeafNode.Visibility = Visibility.Collapsed;
                DetailsGrpBx.Visibility = Visibility.Visible;
                ((DetailedTopicEntity)CurrentNode).FillDetails(dbPath); // @"Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + dbPath);
                DetailsGrpBx.DataContext = CurrentNode;
                return;
            }
            else
            {
                DetailsGrpBx.Visibility = Visibility.Collapsed;
                DisplayNonLeafNode.Visibility = Visibility.Visible;
                DisplayNonLeafNode.DataContext = CurrentNode;
            }
        }

        TopicEntity FindTreeNode(string Name, TopicEntity CurrentEntity, SearchUsing Search)
        {
            if (Search == SearchUsing.TopicDisplayName)
            {
                if (CurrentEntity.TopicDisplayName == Name)
                    return CurrentEntity;
                if (CurrentEntity.Branches.Count == 0)
                    return null;
            }
            if (Search == SearchUsing.TopicName)
            {
                if (CurrentEntity.TopicName == Name)
                    return CurrentEntity;
                if (CurrentEntity.Branches.Count == 0)
                    return null;
            }
            for (int i = 0; i < CurrentEntity.Branches.Count; i++)
            {
                TopicEntity TmpEntity = FindTreeNode(Name, CurrentEntity.Branches[i], Search);
                if (TmpEntity != null)
                    return TmpEntity;
            }
            return null;
        }

        public bool ShowTopic(string TopicLocation, string TopicName)
        {
            bool a = false;
            TopicEntity FoundEntity;
            try
            {
                for (int i = 0; i < DOS.Branches.Count; i++)
                {
                    FoundEntity = FindTreeNode(TopicName, DOS.Branches[i], SearchUsing.TopicName);
                    if (FoundEntity != null)
                    {
                        if (FoundEntity.IsLeafNode)
                        {
                            ((DetailedTopicEntity)FoundEntity).FillDetails(dbPath); // @"Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + dbPath);
                            DisplayNonLeafNode.Visibility = Visibility.Collapsed;
                            DetailsGrpBx.Visibility = Visibility.Visible;
                            DetailsGrpBx.DataContext = FoundEntity;
                            a = true;
                            return a;
                        }
                        else
                        {
                            FoundEntity = FindTreeNode(TopicName, DOS.Branches[i], SearchUsing.TopicDisplayName);
                            if (FoundEntity != null)
                            {
                                if (FoundEntity.IsLeafNode)
                                {
                                    ((DetailedTopicEntity)FoundEntity).FillDetails(dbPath); // @"Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + dbPath);
                                    DisplayNonLeafNode.Visibility = Visibility.Collapsed;
                                    DetailsGrpBx.Visibility = Visibility.Visible;
                                    DetailsGrpBx.DataContext = FoundEntity;
                                    a = true;
                                    return a;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError02", ex);
            }
            return a;
        }

        void btn_click(object sender, RoutedEventArgs e)
        {
            object obj = ((Hyperlink)sender).CommandParameter;
            TopicEntity obj1 = (TopicEntity)obj;
            ShowTreeNodeDetails(obj1);
        }

        protected void UpdateBtn_Click(object Sender, RoutedEventArgs e)
        {
            DataSet ds = new DataSet();
            OleDbCommandBuilder Commb;
            try
            {
                object CurrentButton = ((Button)Sender).DataContext;
                TopicEntity FoundEntity = (TopicEntity)CurrentButton;
                OleDbConnection con = new OleDbConnection(dbPath); // @"Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + dbPath);
                OleDbDataAdapter adapter;
                if (FoundEntity.IsLeafNode)
                {
                    adapter = new OleDbDataAdapter("Select * from ButtonDescription", con);
                    adapter.Fill(ds, "ButtonDescription");
                    adapter = new OleDbDataAdapter("Select * from ToolStripButton", con);
                    adapter.Fill(ds, "ToolStripButton");
                    DataRow[] dr = ds.Tables["ButtonDescription"].Select("ButtonID ='" + FoundEntity.TopicID + "'");
                    if (dr.Length > 0)
                    {
                        dr[0]["Description" + GlobalSettings.Language] = FoundEntity.TopicDescription;
                        dr[0]["Clicks"] = ((DetailedTopicEntity)FoundEntity).Clicks;
                        dr[0]["Comments"] = ((DetailedTopicEntity)FoundEntity).TopicComments;
                    }
                    else
                    {
                        DataRow NewDataRow = ds.Tables["ButtonDescription"].NewRow();
                        NewDataRow["DescriptionEnglish"] = FoundEntity.TopicDescription;
                        NewDataRow["DescriptionGerman"] = FoundEntity.TopicDescription;
                        NewDataRow["DescriptionArabic"] = FoundEntity.TopicDescription;
                        NewDataRow["DescriptionTurkish"] = FoundEntity.TopicDescription;
                        NewDataRow["ButtonID"] = FoundEntity.TopicID;
                        NewDataRow["Clicks"] = ((DetailedTopicEntity)FoundEntity).Clicks;
                        NewDataRow["Comments"] = ((DetailedTopicEntity)FoundEntity).TopicComments;
                        ds.Tables["ButtonDescription"].Rows.Add(NewDataRow);
                    }
                    dr = ds.Tables["ToolStripButton"].Select("ButtonID ='" + FoundEntity.TopicID + "'");
                    dr[0]["ButtonToolTip" + GlobalSettings.Language] = FoundEntity.TopicDisplayName;
                    adapter = new OleDbDataAdapter("Select * from Procedure_Steps where(TopicID=" + FoundEntity.TopicID + ") Order by StepNumber", con);
                    adapter.Fill(ds, "Procedure_Steps");

                    #region For Procedure Add/Update/Delete

                    int MaxId = 0;
                    if (ds.Tables["Procedure_Steps"].Rows.Count > 0)
                    {
                        MaxId = Convert.ToInt32(ds.Tables["Procedure_Steps"].Compute("MAX(ProcedureID)", ""));
                    }
                    //To Update and Delete the Rows

                    int Rowcount = ds.Tables["Procedure_Steps"].Rows.Count;
                    for (int i = 0; i < Rowcount; i++)
                    {
                        bool RowDeleted = true;
                        for (int j = 0; j < ((DetailedTopicEntity)FoundEntity).Procedureitems.Count; j++)
                        {
                            if (Convert.ToInt32(ds.Tables["Procedure_Steps"].Rows[i]["ProcedureID"]) == ((DetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureID)
                            {
                                ds.Tables["Procedure_Steps"].Rows[i]["StepNumber"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureStepNumber;
                                ds.Tables["Procedure_Steps"].Rows[i]["TopicID"] = ((DetailedTopicEntity)FoundEntity).TopicID;
                                ds.Tables["Procedure_Steps"].Rows[i]["Comments"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureComment;
                                //ds.Tables["Procedure_Steps"].Rows[i]["ProcedureImage"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureImage;
                                ds.Tables["Procedure_Steps"].Rows[i]["ProcedureText" + GlobalSettings.Language] = ((DetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureText;
                                RowDeleted = false;
                                break;
                            }
                        }
                        if (RowDeleted)
                        {
                            ds.Tables["Procedure_Steps"].Rows[i].Delete();
                            Rowcount--;
                            i--;
                        }
                    }

                    //To Add new rows
                    for (int i = 0; i < ((DetailedTopicEntity)FoundEntity).Procedureitems.Count; i++)
                    {
                        if (((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureID == -1)
                        {
                            DataRow drow = ds.Tables["Procedure_Steps"].NewRow();
                            drow["StepNumber"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureStepNumber;
                            drow["TopicID"] = ((DetailedTopicEntity)FoundEntity).TopicID;
                            drow["ProcedureTextEnglish"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText;
                            drow["ProcedureTextGerman"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText;
                            //drow["ProcedureImage"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureImage;
                            drow["Comments"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureComment;
                            drow["ProcedureTextArabic"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText; ;
                            drow["ProcedureTextTurkish"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText;
                            drow["ProcedureID"] = ++MaxId;
                            ds.Tables["Procedure_Steps"].Rows.Add(drow);
                        }
                    }
                    #endregion

                    adapter = new OleDbDataAdapter("Select * from Applications where(TopicID=" + FoundEntity.TopicID + ")", con);
                    adapter.Fill(ds, "Applications");

                    #region For Application Add/Update/Delete

                    MaxId = 0;
                    if (ds.Tables["Applications"].Rows.Count > 0)
                    {
                        MaxId = Convert.ToInt32(ds.Tables["Applications"].Compute("MAX(AppID)", ""));
                    }
                    //To Update and Delete the Rows
                    Rowcount = ds.Tables["Applications"].Rows.Count;
                    for (int i = 0; i < Rowcount; i++)
                    {
                        bool RowDeleted = true;
                        for (int j = 0; j < ((DetailedTopicEntity)FoundEntity).Applicationitems.Count; j++)
                        {
                            if (Convert.ToInt32(ds.Tables["Applications"].Rows[i]["AppID"]) == ((DetailedTopicEntity)FoundEntity).Applicationitems[j].AppID)
                            {
                                ds.Tables["Applications"].Rows[i]["TopicID"] = ((DetailedTopicEntity)FoundEntity).TopicID;
                                //ds.Tables["Applications"].Rows[i]["AppImage"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[j].AppImage;
                                ds.Tables["Applications"].Rows[i]["AppText" + GlobalSettings.Language] = ((DetailedTopicEntity)FoundEntity).Applicationitems[j].AppText;
                                ds.Tables["Applications"].Rows[i]["Comments" + GlobalSettings.Language] = ((DetailedTopicEntity)FoundEntity).Applicationitems[j].AppComment;
                                RowDeleted = false;
                                break;
                            }
                        }
                        if (RowDeleted)
                        {
                            ds.Tables["Applications"].Rows[i].Delete();
                            Rowcount--;
                            i--;
                        }
                    }

                    //To Add new rows
                    for (int i = 0; i < ((DetailedTopicEntity)FoundEntity).Applicationitems.Count; i++)
                    {
                        if (((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppID == -1)
                        {
                            DataRow drow = ds.Tables["Applications"].NewRow();
                            drow["TopicID"] = ((DetailedTopicEntity)FoundEntity).TopicID;
                            //drow["AppImage"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppImage;
                            drow["AppTextEnglish"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppText;
                            drow["AppTextGerman"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppText;
                            drow["AppTextArabic"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppText;
                            drow["AppTextTurkish"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppText;
                            drow["CommentsEnglish"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppComment;
                            drow["CommentsGerman"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppComment;
                            drow["CommentsArabic"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppComment;
                            drow["CommentsTurkish"] = ((DetailedTopicEntity)FoundEntity).Applicationitems[i].AppComment;
                            drow["AppID"] = ++MaxId;
                            ds.Tables["Applications"].Rows.Add(drow);
                        }
                    }

                    #endregion

                    adapter = new OleDbDataAdapter("Select * from SpecialNotes where(TopicID=" + FoundEntity.TopicID + ")", con);
                    adapter.Fill(ds, "SpecialNotes");

                    #region For SpecialNotes Add/Update/Delete

                    MaxId = 0;
                    if (ds.Tables["SpecialNotes"].Rows.Count > 0)
                    {
                        MaxId = Convert.ToInt32(ds.Tables["SpecialNotes"].Compute("MAX(SpID)", ""));
                    }
                    //To Update and Delete the Rows                        
                    Rowcount = ds.Tables["SpecialNotes"].Rows.Count;
                    for (int i = 0; i < Rowcount; i++)
                    {
                        bool RowDeleted = true;
                        for (int j = 0; j < ((DetailedTopicEntity)FoundEntity).SpecialNotesitems.Count; j++)
                        {
                            if (Convert.ToInt32(ds.Tables["SpecialNotes"].Rows[i]["SpID"]) == ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[j].SPID)
                            {
                                ds.Tables["SpecialNotes"].Rows[i]["TopicID"] = ((DetailedTopicEntity)FoundEntity).TopicID;
                                //ds.Tables["SpecialNotes"].Rows[i]["SpImage"] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[j].SPImage;
                                ds.Tables["SpecialNotes"].Rows[i]["SpText" + GlobalSettings.Language] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[j].SPText;
                                ds.Tables["SpecialNotes"].Rows[i]["WorkAround" + GlobalSettings.Language] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[j].SPWorkAround;
                                RowDeleted = false;
                                break;
                            }
                        }
                        if (RowDeleted)
                        {
                            ds.Tables["SpecialNotes"].Rows[i].Delete();
                            Rowcount--;
                            i--;
                        }
                    }

                    //To Add new rows
                    for (int i = 0; i < ((DetailedTopicEntity)FoundEntity).SpecialNotesitems.Count; i++)
                    {
                        if (((DetailedTopicEntity)FoundEntity).SpecialNotesitems[i].SPID == -1)
                        {
                            DataRow drow = ds.Tables["SpecialNotes"].NewRow();
                            drow["TopicID"] = ((DetailedTopicEntity)FoundEntity).TopicID;
                            //drow["SpImage"] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[i].SPImage;
                            drow["SpTextEnglish"] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[i].SPText;
                            drow["SpTextGerman"] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[i].SPText;
                            drow["WorkAroundEnglish"] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[i].SPWorkAround;
                            drow["WorkAroundGerman"] = ((DetailedTopicEntity)FoundEntity).SpecialNotesitems[i].SPWorkAround;
                            drow["SpID"] = ++MaxId;
                            ds.Tables["SpecialNotes"].Rows.Add(drow);
                        }
                    }
                    #endregion

                    #region UpdateDatabase
                    try
                    {
                        string[] Tables = { "SpecialNotes", "ButtonDescription", "Procedure_Steps", "Applications", "ToolStripButton" };
                        foreach (string str in Tables)
                        {
                            string Querystring = "Select * from " + str + "";
                            adapter = new OleDbDataAdapter(Querystring, con);
                            Commb = new OleDbCommandBuilder(adapter);
                            adapter.Update(ds.Tables[str]);
                        }
                    }
                    catch (Exception ex)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError54", ex);
                    }
                    MessageBox.Show("Values Updated");
                    #endregion
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError41", ex);
            }
        }

        /*private void mediaElement_Drop(object sender, System.Windows.DragEventArgs e)
        {
            try
            {
                String[] FileName = (String[])e.Data.GetData(System.Windows.DataFormats.FileDrop, true);
                if (FileName.Length > 0)
                {
                    String VideoPath = FileName[0].ToString();
                    mediaElement.Source = new Uri(VideoPath);
                    mediaElement.Play();
                }
                e.Handled = true;
            }
            catch (Exception Ex)
            {
            }
        }
        protected void Play_Click(object sender, RoutedEventArgs e)
        {
            if (mediaElement.Source != null)
            {
                mediaElement.Play();
            }
        }
        protected void Pause_Click(object sender, RoutedEventArgs e)
        {
            if (mediaElement.Source != null)
            {
                mediaElement.Pause();
            }
        }
        protected void Stop_Click(object sender, RoutedEventArgs e)
        {
            if (mediaElement.Source != null)
            {
                mediaElement.Stop();
            }
        }*/

        #region Window Close/Hide

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

        private void Button_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            RapidiHelpWin.dbPath = System.Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Rapid-I 5.0\Database\Rapid-I HelpFiles.mdb";
            RapidiHelpWin.MYInstance().Show();
        }

        #endregion

    }

    public class DisplayOnScreen : TopicEntity
    {
        DataSet ds = new DataSet();
        public DisplayOnScreen(string ConnectionStr)
        {
            try
            {
                OleDbConnection con = new OleDbConnection(ConnectionStr);
                OleDbDataAdapter adapter;
                adapter = new OleDbDataAdapter("Select * from ButtonCategory", con);
                adapter.Fill(ds, "ButtonCategory");
                adapter = new OleDbDataAdapter("Select * from ButtonDescription", con);
                adapter.Fill(ds, "ButtonDescription");
                adapter = new OleDbDataAdapter("Select * from ToolStripButton", con);
                adapter.Fill(ds, "ToolStripButton");

                this.Branches = new ObservableCollection<TopicEntity>();
                for (int j = 0; j < ds.Tables["ButtonCategory"].Rows.Count; j++)
                {
                    if ((int)ds.Tables["ButtonCategory"].Rows[j]["ParentID"] == -1)
                    {
                        TopicEntity TmpTopicEntity = GetFilledCategoryObject(ds.Tables["ButtonCategory"].Rows[j], ConnectionStr);
                        FillTreeLeaves(ds.Tables["ButtonCategory"], ref TmpTopicEntity, ConnectionStr);
                        Branches.Add(TmpTopicEntity);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError04", ex);
            }
        }
        public void FillTreeLeaves(DataTable Source, ref TopicEntity Me, string DBconnectorStr)
        {
            for (int i = 0; i < Source.Rows.Count; i++)
            {
                if (((int)Source.Rows[i]["ParentID"]) == Me.CategoryID)
                {
                    TopicEntity TmpTopicEntity = GetFilledCategoryObject(Source.Rows[i], DBconnectorStr);
                    FillTreeLeaves(Source, ref TmpTopicEntity, DBconnectorStr);
                    Me.Branches.Add(TmpTopicEntity);
                }
            }
        }
        private TopicEntity GetFilledCategoryObject(DataRow SourceRow, string DBconnectorStr)
        {
            TopicEntity TmpTopicEntity = new TopicEntity();
            TmpTopicEntity.TopicID = -1;
            TmpTopicEntity.TopicName = SourceRow["CategoryName"].ToString();
            TmpTopicEntity.TopicDisplayName = SourceRow["ToolTip" + GlobalSettings.Language].ToString();
            TmpTopicEntity.IsLeafNode = false;
            TmpTopicEntity.CategoryID = (int)SourceRow["CategoryID"];
            DataRow[] Drow = ds.Tables["ToolStripButton"].Select("Alignment = '" + SourceRow["CategoryName"].ToString() + "'");
            for (int i = 0; i < Drow.Length; i++)
            {
                TopicEntity ChildEntity = new DetailedTopicEntity(DBconnectorStr);
                ChildEntity.TopicID = Convert.ToInt32(Drow[i]["ButtonId"].ToString());
                System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                if (Drow[i]["ButtonIcon"] != System.DBNull.Value)
                {
                    try
                    {
                        byte[] Source = (byte[])Drow[i]["ButtonIcon"];
                        bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                        bmp.BeginInit();
                        bmp.StreamSource = (new System.IO.MemoryStream(Source));
                        bmp.EndInit();
                        ((DetailedTopicEntity)ChildEntity).ButtonImage = bmp;
                    }
                    catch (Exception ex)
                    {
                        //MessageBox.Show(Drow[i]["ButtonName"].ToString());
                        RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError53", ex);
                    }
                }
                ((DetailedTopicEntity)ChildEntity).Clicks = null;
                DataRow[] DescriptionRow = ds.Tables["ButtonDescription"].Select("ButtonID = '" + Convert.ToInt32(Drow[i]["ButtonId"].ToString()) + "'");
                if (DescriptionRow.Length > 0)
                {
                    if (DescriptionRow[0]["clicks"] != DBNull.Value)
                        ((DetailedTopicEntity)ChildEntity).Clicks = Convert.ToInt32(DescriptionRow[0]["clicks"].ToString());
                    ((DetailedTopicEntity)ChildEntity).TopicComments = (DescriptionRow[0]["Comments"].ToString());
                    ChildEntity.TopicDescription = DescriptionRow[0]["Description" + GlobalSettings.Language].ToString();
                }

                ChildEntity.TopicName = Drow[i]["ButtonName"].ToString();
                ChildEntity.TopicDisplayName = Drow[i]["ButtonToolTip" + GlobalSettings.Language].ToString();
                ChildEntity.IsLeafNode = true;
                TmpTopicEntity.Branches.Add(ChildEntity);
            }
            return TmpTopicEntity;
        }
        public void FillTree(DataTable Source, ref TopicEntity Me, string DBconnectorStr)
        {
            try
            {
                if (Me.IsLeafNode) return;

                for (int i = 0; i < Source.Rows.Count; i++)
                {
                    if (((int)Source.Rows[i]["ParentID"]) == Me.TopicID)
                    {
                        TopicEntity TmpTopicEntity = GetFilledObject(Source.Rows[i], DBconnectorStr);
                        FillTree(Source, ref TmpTopicEntity, DBconnectorStr);
                        Me.Branches.Add(TmpTopicEntity);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError08", ex);
            }
        }
        private TopicEntity GetFilledObject(DataRow SourceRow, string DBconnectorStr)
        {
            TopicEntity TmpTopicEntity;
            bool IsLeafNode = (bool)SourceRow["IsLeafNode"];
            if (IsLeafNode)
            {
                TmpTopicEntity = new DetailedTopicEntity(DBconnectorStr);

                System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                if (SourceRow["ButtonIcon"] != System.DBNull.Value)
                {
                    byte[] Source = (byte[])SourceRow["ButtonIcon"];
                    bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                    bmp.BeginInit();
                    bmp.StreamSource = (new System.IO.MemoryStream(Source));
                    bmp.EndInit();
                    ((DetailedTopicEntity)TmpTopicEntity).ButtonImage = bmp;
                }
                ((DetailedTopicEntity)TmpTopicEntity).Clicks = null;
                if (SourceRow["clicks"] != DBNull.Value)
                    ((DetailedTopicEntity)TmpTopicEntity).Clicks = (int)SourceRow["clicks"];
                ((DetailedTopicEntity)TmpTopicEntity).TopicComments = SourceRow["Comments"].ToString();
            }
            else
            {
                TmpTopicEntity = new TopicEntity();
            }
            TmpTopicEntity.TopicID = (int)SourceRow["TopicID"];
            TmpTopicEntity.TopicName = SourceRow["TopicName"].ToString();
            TmpTopicEntity.TopicDisplayName = SourceRow["TopicDisplayName" + GlobalSettings.Language].ToString();
            TmpTopicEntity.TopicDescription = SourceRow["Description" + GlobalSettings.Language].ToString();
            TmpTopicEntity.IsLeafNode = IsLeafNode;
            return TmpTopicEntity;
        }
    }
    public class DetailedTopicEntity : TopicEntity
    {
        string dbConnectionStr;
        OleDbConnection con;
        OleDbDataAdapter adapter;
        DataSet ds = new DataSet();
        public ObservableCollection<Procedures> Procedureitems { get; set; }
        public ObservableCollection<Applications> Applicationitems { get; set; }
        public ObservableCollection<specialNotes> SpecialNotesitems { get; set; }
        public ObservableCollection<TutorialVideos> TutorialVideositems { get; set; }
        public int? Clicks { get; set; }
        public string TopicComments { get; set; }
        public BitmapImage ButtonImage { get; set; }
        private bool _HasTutorials = false;
        public bool HasTutorials
        {
            get
            {
                return _HasTutorials;
            }
            set
            {
                _HasTutorials = value;
            }
        }
        public DetailedTopicEntity(string ConnectionStr)
        {
            dbConnectionStr = ConnectionStr;
            this.Applicationitems = new ObservableCollection<Applications>();
            this.SpecialNotesitems = new ObservableCollection<specialNotes>();
            this.Procedureitems = new ObservableCollection<Procedures>();
            this.TutorialVideositems = new ObservableCollection<TutorialVideos>();
        }
        public void FillDetails(string ConnectionStr)
        {
            this.dbConnectionStr = ConnectionStr;
            ForProcedure(TopicID);
            ForApplication(TopicID);
            ForSpecialNotes(TopicID);
            ForTutorials(TopicID);
        }
        public void ForProcedure(int TopicIDs)
        {
            try
            {
                Procedureitems.Clear();
                DataSet ds = new DataSet();
                con = new OleDbConnection(dbConnectionStr);
                con.Open();
                adapter = new OleDbDataAdapter("Select ProcedureID,StepNumber,ProcedureText" + GlobalSettings.Language + ",ProcedureImage,Comments from Procedure_Steps where(TopicID=" + TopicIDs + ") Order by StepNumber", con);
                adapter.Fill(ds, "Procedure");
                con.Close();
                for (int i = 0; i < ds.Tables[0].Rows.Count; i++)
                {
                    string StepNumber, proced, ProcedureText, Comments;
                    int? Procedure_Number = null, Step_Number = null;
                    byte[] ImageByteArray = null;
                    System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                    if (ds.Tables[0].Rows[i]["ProcedureImage"].ToString() != "")
                    {
                        byte[] Source = (byte[])ds.Tables[0].Rows[i]["ProcedureImage"];
                        bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                        bmp.BeginInit();
                        bmp.StreamSource = (new System.IO.MemoryStream(Source));
                        bmp.EndInit();
                        ImageByteArray = Source;
                    }
                    proced = ds.Tables[0].Rows[i].ItemArray[0].ToString();
                    StepNumber = ds.Tables[0].Rows[i].ItemArray[1].ToString();
                    ProcedureText = ds.Tables[0].Rows[i].ItemArray[2].ToString();
                    Comments = ds.Tables[0].Rows[i].ItemArray[4].ToString();
                    if (StepNumber != "")
                    {
                        Step_Number = Convert.ToInt32(StepNumber);
                    }
                    if (proced != "")
                    {
                        Procedure_Number = Convert.ToInt32(proced);
                    }
                    this.Procedureitems.Add(new Procedures(Procedure_Number, Step_Number, ProcedureText, bmp, Comments, ImageByteArray));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError05", ex);
            }
        }
        public void ForApplication(int TopicIDs)
        {
            try
            {
                Applicationitems.Clear();
                DataSet ds = new DataSet();
                con = new OleDbConnection(dbConnectionStr);
                con.Open();
                adapter = new OleDbDataAdapter("Select AppID,AppText" + GlobalSettings.Language + ",AppImage,Comments" + GlobalSettings.Language + " from Applications where(TopicID=" + TopicIDs + ")", con);
                adapter.Fill(ds, "Applications");
                con.Close();
                for (int i = 0; i < ds.Tables["Applications"].Rows.Count; i++)
                {
                    string appid, AppText, Comments;
                    int? AppID = null;
                    System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                    if (ds.Tables[0].Rows[i]["AppImage"].ToString() != "")
                    {
                        byte[] Source = (byte[])ds.Tables["Applications"].Rows[i]["AppImage"];
                        bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                        bmp.BeginInit();
                        bmp.StreamSource = (new System.IO.MemoryStream(Source));
                        bmp.EndInit();
                    }
                    appid = ds.Tables[0].Rows[i]["AppID"].ToString();
                    AppText = ds.Tables[0].Rows[i]["AppText" + GlobalSettings.Language].ToString();
                    Comments = ds.Tables[0].Rows[i]["Comments" + GlobalSettings.Language].ToString();
                    if (appid != "")
                    {
                        AppID = Convert.ToInt32(appid);
                    }
                    this.Applicationitems.Add(new Applications(AppID, AppText, bmp, Comments));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError06", ex);
            }
        }
        public void ForSpecialNotes(int TopicIDs)
        {
            try
            {
                try
                {
                    SpecialNotesitems.Clear();
                    DataSet ds = new DataSet();
                    con = new OleDbConnection(dbConnectionStr);
                    con.Open();
                    adapter = new OleDbDataAdapter("Select SpID,SpText" + GlobalSettings.Language + ",SpImage,Comments,WorkAround" + GlobalSettings.Language + " from SpecialNotes where(TopicID=" + TopicIDs + ")", con);
                    adapter.Fill(ds, "SpecialNotes");

                    con.Close();

                    for (int i = 0; i < ds.Tables[0].Rows.Count; i++)
                    {
                        string sppid, SpText, Comments, WorkAround;

                        int? SpID = null;
                        System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                        if (ds.Tables[0].Rows[i]["SpImage"].ToString() != "")
                        {
                            byte[] Source = (byte[])ds.Tables[0].Rows[i]["SpImage"];
                            bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                            bmp.BeginInit();
                            bmp.StreamSource = (new System.IO.MemoryStream(Source));
                            bmp.EndInit();
                        }
                        sppid = ds.Tables[0].Rows[i]["SpID"].ToString();
                        SpText = ds.Tables[0].Rows[i]["SpText" + GlobalSettings.Language].ToString();
                        Comments = ds.Tables[0].Rows[i]["Comments"].ToString();
                        WorkAround = ds.Tables[0].Rows[i]["WorkAround" + GlobalSettings.Language].ToString();
                        if (sppid != "")
                        {
                            SpID = Convert.ToInt32(sppid);
                        }
                        this.SpecialNotesitems.Add(new specialNotes(SpID, SpText, bmp, Comments, WorkAround));
                    }
                    ds.Clear();
                }
                finally
                {
                    con.Close();
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError07", ex);
            }
        }
        public void ForTutorials(int TopicIDs)
        {
            try
            {
                TutorialVideositems.Clear();
                DataSet ds = new DataSet();
                con = new OleDbConnection(dbConnectionStr);
                adapter = new OleDbDataAdapter("Select TutID,Description" + GlobalSettings.Language + ",Title" + GlobalSettings.Language + ",Tutorial" + GlobalSettings.Language + ",RelatedGroup from HelpTutorials where(TopicID=" + TopicIDs + ")", con);
                adapter.Fill(ds, "HelpTutorials");
                for (int i = 0; i < ds.Tables[0].Rows.Count; i++)
                {
                    this.HasTutorials = true;
                    string Description, Title;
                    int RelatedGroup, TutID;
                    byte[] Source = null;
                    if (ds.Tables[0].Rows[i]["Tutorial" + GlobalSettings.Language] != DBNull.Value)
                    {
                        Source = (byte[])ds.Tables[0].Rows[i]["Tutorial" + GlobalSettings.Language];
                    }
                    TutID = Convert.ToInt32(ds.Tables[0].Rows[i]["TutID"].ToString());
                    RelatedGroup = Convert.ToInt32(ds.Tables[0].Rows[i]["RelatedGroup"].ToString());
                    Description = ds.Tables[0].Rows[i]["Description" + GlobalSettings.Language].ToString();
                    Title = ds.Tables[0].Rows[i]["Title" + GlobalSettings.Language].ToString();
                    this.TutorialVideositems.Add(new TutorialVideos(TutID, Description, Title, Source, RelatedGroup));
                }
                ds.Clear();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError55", ex);
            }
        }
    }
    public class TopicEntity
    {
        private int _TopicID;
        private string _TopicName;
        private string _TopicDisplayName;
        private string _TopicDescription;
        private bool _IsLeafNode = false;
        private int _CategoryID;

        public int CategoryID
        {
            get
            {
                return _CategoryID;
            }
            set
            {
                _CategoryID = value;
            }
        }
        public int TopicID
        {
            get
            {
                return _TopicID;
            }
            set
            {
                _TopicID = value;
            }
        }
        public string TopicName
        {
            get
            {
                return _TopicName;
            }
            set
            {
                _TopicName = value;
            }
        }
        public string TopicDisplayName
        {
            get
            {
                return _TopicDisplayName;
            }
            set
            {
                _TopicDisplayName = value;
            }
        }
        public string TopicDescription
        {
            get
            {
                return _TopicDescription;
            }
            set
            {
                _TopicDescription = value;
            }
        }
        public bool IsLeafNode
        {
            get
            {
                return _IsLeafNode;
            }
            set
            {
                _IsLeafNode = value;
            }
        }
        public bool IsCategoryNode { get; set; }
        public ObservableCollection<TopicEntity> Branches { get; set; }
        public TopicEntity()
        {
            Branches = new ObservableCollection<TopicEntity>();
        }
    }
    public class Procedures : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        private ObservableCollection<string> _ProcedureCommentList = new ObservableCollection<string>();
        public ObservableCollection<string> ProcedureCommentList
        {
            get
            {
                return _ProcedureCommentList;
            }
            set
            {
                _ProcedureCommentList = value;
                Notify("ProcedureCommentList");
            }
        }

        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        string procedurecomment = "";
        public string ProcedureComment
        {
            get
            {
                return procedurecomment;
            }
            set
            {
                procedurecomment = value;
                UpdateProcedureList(procedurecomment);
                Notify("procedurecomment");
            }
        }

        int? procedureID = -1;
        public int? ProcedureID
        {
            get
            {
                return procedureID;
            }
            set
            {
                this.procedureID = value;
                Notify("procedureID");
            }
        }

        string proceduretext = "";
        public string ProcedureText
        {
            get
            {
                return proceduretext;
            }
            set
            {
                this.proceduretext = value;
                Notify("proceduretext");
            }
        }

        int? procedureStepNumber = 0;
        public int? ProcedureStepNumber
        {
            get { return procedureStepNumber; }
            set
            {
                this.procedureStepNumber = value; Notify("procedureStepNumber");
            }
        }

        private byte[] _ImageBinarySource;
        public byte[] ImageBinarySource
        {
            get
            {
                return _ImageBinarySource;
            }
            set
            {
                _ImageBinarySource = value;
            }
        }
        BitmapImage procedureimg = null;
        public BitmapImage ProcedureImage
        {
            get
            {
                return procedureimg;
            }
            set
            {
                //PropertyChanged.ChangeAndNotify(ref procedureimg, value, () => ProcedureImage);
                this.procedureimg = value;
                Notify("procedureimg");
            }
        }

        void UpdateProcedureList(string ProcedureComments)
        {
            string[] Nolines = ProcedureComments.Split(new string[] { "\r\n" }, StringSplitOptions.RemoveEmptyEntries);
            _ProcedureCommentList.Clear();
            for (int i = 0; i < Nolines.Count(); i++)
            {
                ProcedureCommentList.Add(Nolines[i]);
            }
        }

        public Procedures(int? procedureid, int? stepnumber, string proceduretext, BitmapImage img, string procedurecomment, byte[] ImageInformOfByteArray)
        {
            this.ProcedureStepNumber = stepnumber;
            this.procedureID = procedureid;
            this.proceduretext = proceduretext;
            this.procedurecomment = procedurecomment;
            this.procedureimg = img;
            ImageBinarySource = ImageInformOfByteArray;
            UpdateProcedureList(procedurecomment);
        }
        public Procedures()
        {
        }
    }
    public class Applications : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        int? appId = -1;
        public int? AppID
        {
            get
            {
                return appId;
            }
            set
            {
                appId = value;
                Notify("appId");
            }
        }

        string appText = "";
        public string AppText
        {
            get
            {
                return appText;
            }
            set
            {
                appText = value;
                Notify("appText");
            }
        }

        string appComment = "";
        public string AppComment
        {
            get
            {
                return appComment;
            }
            set
            {
                appComment = value;
                Notify("appComment");
            }
        }

        BitmapImage appImg = null;
        public BitmapImage AppImage
        {
            get
            {
                return appImg;
            }
            set
            {
                appImg = value; Notify("appImg");
            }
        }

        public Applications(int? appid, string apptext, BitmapImage appimg, string appcomment)
        {
            this.appId = appid;
            this.appText = apptext;
            this.appImg = appimg;
            this.appComment = appcomment;
        }
        public Applications()
        {
        }
    }
    public class specialNotes : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }
        int? spId = -1;
        public int? SPID
        {
            get
            {
                return spId;
            }
            set
            {
                spId = value;
                Notify("spId");
            }
        }
        string spText = "";
        public string SPText
        {
            get
            {
                return spText;
            }
            set
            {
                spText = value;
                Notify("SPText");
            }
        }
        string spComment = "";
        public string SPComent
        {
            get
            {
                return spComment;
            }
            set
            {
                spComment = value;
                Notify("SPComent");
            }
        }
        //BitmapImage spImg=null;
        //public BitmapImage SPImage
        //{
        //    get
        //    {
        //        return spImg;
        //    }
        //    set
        //    {
        //        spImg = value;
        //        Notify("SPImage");
        //    }
        //}
        string spwork = "";
        public string SPWorkAround
        {
            get
            {
                return spwork;
            }
            set
            {
                spwork = value;
                Notify("SPWorkAround");
            }
        }
        public specialNotes(int? spid, string sptext, BitmapImage spimage, string spcomments, string spworkaround)
        {
            this.spId = spid;
            this.spText = sptext;
            //this.spImg = spimage;
            this.spComment = spcomments;
            this.spwork = spworkaround;
        }
        public specialNotes()
        {
        }
    }
    public class TutorialVideos : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }
        //int _Id;
        //string _Description;
        //string _Title;
        //byte[] _TutorialSource;
        //int _RelatedGroup;

        public int ID { get; set; }
        public string Description { get; set; }
        public string Title { get; set; }
        public byte[] TutorialSource { get; set; }
        public int RelatedGroup { get; set; }

        public TutorialVideos()
        {
        }
        public TutorialVideos(int TutId, string TutDescription, string TutTitle, Byte[] Tutorial, int TutGroup)
        {
            ID = TutId;
            Description = TutDescription;
            Title = TutTitle;
            TutorialSource = Tutorial;
            RelatedGroup = TutGroup;
        }
    }
}