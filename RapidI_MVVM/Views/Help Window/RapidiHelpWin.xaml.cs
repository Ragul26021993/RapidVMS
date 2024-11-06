using System;
using System.Windows.Controls;
using System.Windows;
using System.Windows.Documents;
using System.Windows.Forms.Integration;
using System.Windows.Media.Imaging;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Drawing;
using System.Data;
using System.Data.OleDb;
using System.Collections.Generic;
using System.Collections;
using Rapid;
using Rapid.Utilities;
using System.Xml;


namespace RapidI.Help
{
    /// <summary>
    /// Interaction logic for RapidiHelpWin.xaml
    /// </summary>
    public partial class RapidiHelpWin : Window, INotifyPropertyChanged
    {
        #region Variable and Enums Decleration

        public enum SearchUsing { TopicId, TopicDisplayName, TopicName };
        HelpDisplayOnScreen DOS;
        public static string dbPath;
        Procedures CurrentSelectedObject;
        private static RapidiHelpWin _MYInstance;
        private bool _EditMode = false;
        private string FirstHalfConnectionString;
        public bool EditMode
        {
            get
            {
                return _EditMode;
            }
            set
            {
                _EditMode = value;
                if (_EditMode)
                {
                    UploadImageCol.Visibility = System.Windows.Visibility.Visible;
                    DoThisCol.IsReadOnly = !_EditMode;
                    ThingsToConsiderCol.IsReadOnly = !_EditMode;
                }
                else
                {
                    UploadImageCol.Visibility = System.Windows.Visibility.Hidden;
                    DoThisCol.IsReadOnly = _EditMode;
                    ThingsToConsiderCol.IsReadOnly = _EditMode;
                }
                Notify("EditMode");
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
        #endregion

        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        public static RapidiHelpWin MYInstance()
        {
            if (_MYInstance == null)
            {
                _MYInstance = new RapidiHelpWin();
                _MYInstance.Owner = GlobalSettings.MainWin;
            }
            return _MYInstance;
        }
        private RapidiHelpWin()
        {
            InitializeComponent();
            ImageHost.GetPictureBox().MouseDown += new System.Windows.Forms.MouseEventHandler(ImageHost_MouseDown);
            ImageHost.GetPictureBox().MouseMove += new System.Windows.Forms.MouseEventHandler(ImageHost_MouseMove);
            ImageHost.GetPictureBox().Paint += new System.Windows.Forms.PaintEventHandler(ImageHost_Paint);
            this.WindowState = System.Windows.WindowState.Maximized;
            init();
        }
        void init()
        {
            try
            {
                if (System.Environment.Is64BitOperatingSystem)
                    FirstHalfConnectionString = "Microsoft.Ace.OLEDB.12.0; Data Source=";
                else
                    FirstHalfConnectionString = "Microsoft.Jet.OLEDB.4.0;Data Source=";

                DOS = new HelpDisplayOnScreen(@"Provider=" + FirstHalfConnectionString + dbPath);

                DOS.TopicDisplayName = "Rapid I Help";
                TreeViewGrid.DataContext = DOS.Branches;
                ShowMain();
                EditMode = false;
                HelpDG.SizeChanged += new SizeChangedEventHandler(HelpDG_SizeChanged);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError03", ex);
            }
        }

        void ShowTreeNodeDetails(TopicDetails CurrentNode)
        {
            if (CurrentNode.IsLeafNode)
            {
                DisplayNonLeafNodeGrpBx.Visibility = Visibility.Collapsed;
                CaptureImageGrpBx.Visibility = System.Windows.Visibility.Collapsed;
                DetailsGrpBx.Visibility = Visibility.Visible;
                ((HelpDetailedTopicEntity)CurrentNode).FillDetails(@"Provider=" + FirstHalfConnectionString + dbPath);
                DetailsGrpBx.DataContext = CurrentNode;
                if (EditMode)
                {
                    bool exit = HelpDG.CommitEdit(DataGridEditingUnit.Row, true);

                }
                HelpDG.Items.Refresh();
                return;
            }
            else
            {
                CaptureImageGrpBx.Visibility = System.Windows.Visibility.Collapsed;
                DetailsGrpBx.Visibility = Visibility.Collapsed;
                DisplayNonLeafNodeGrpBx.Visibility = Visibility.Visible;
                DisplayNonLeafNodeGrpBx.DataContext = CurrentNode;
            }
        }
        TopicDetails FindTreeNode(string Name, TopicDetails CurrentEntity, SearchUsing Search)
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
                TopicDetails TmpEntity = FindTreeNode(Name, CurrentEntity.Branches[i], Search);
                if (TmpEntity != null)
                    return TmpEntity;
            }
            return null;
        }
        public void ShowMain()
        {
            DetailsGrpBx.Visibility = Visibility.Collapsed;
            DisplayNonLeafNodeGrpBx.Visibility = Visibility.Visible;
            DisplayNonLeafNodeGrpBx.DataContext = DOS;
        }
        public void ShowGroup(string TopicLocation)
        {

        }
        public bool ShowTopic(string TopicLocation, string TopicName)
        {
            bool a = false;
            TopicDetails FoundEntity;
            try
            {
                for (int i = 0; i < DOS.Branches.Count; i++)
                {
                    FoundEntity = FindTreeNode(TopicName, DOS.Branches[i], SearchUsing.TopicName);
                    if (FoundEntity != null)
                    {
                        if (FoundEntity.IsLeafNode)
                        {
                            ((HelpDetailedTopicEntity)FoundEntity).FillDetails(@"Provider=" + FirstHalfConnectionString + dbPath);
                            DisplayNonLeafNodeGrpBx.Visibility = Visibility.Collapsed;
                            DetailsGrpBx.Visibility = Visibility.Visible;
                            DetailsGrpBx.DataContext = FoundEntity;
                            a = true;
                            return a;
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

        #region Button Click Events

        protected void TreeEntity_Click(object sender, RoutedEventArgs e)
        {
            object CurrentButton = ((Hyperlink)sender).CommandParameter;
            TopicDetails FoundEntity = (TopicDetails)CurrentButton;
            ShowTreeNodeDetails(FoundEntity);

        }

        protected void NonLeafeNodeLinkBtn_click(object sender, RoutedEventArgs e)
        {
            object obj = ((Hyperlink)sender).CommandParameter;
            TopicDetails obj1 = (TopicDetails)obj;
            ShowTreeNodeDetails(obj1);
        }

        protected void UpdateBtn_Click(object Sender, RoutedEventArgs e)
        {
            DataTable TableTopics = new DataTable();
            OleDbCommandBuilder Commb;
            try
            {
                object CurrentButton = ((Button)Sender).DataContext;
                TopicDetails FoundEntity = (TopicDetails)CurrentButton;
                OleDbConnection con = new OleDbConnection(@"Provider=" + FirstHalfConnectionString + dbPath); //Microsoft.Ace.OLEDB.12.0;Data Source=
                OleDbDataAdapter adapter;
                if (FoundEntity.IsLeafNode)
                {
                    adapter = new OleDbDataAdapter("Select * from Topics", con);
                    adapter.Fill(TableTopics);
                    Commb = new OleDbCommandBuilder(adapter);
                    DataRow[] dr = TableTopics.Select("TopicID ='" + FoundEntity.TopicID + "'");
                    if (dr.Length > 0)
                    {
                        dr[0]["Description" + GlobalSettings.Language] = FoundEntity.TopicDescription;
                        //dr[0]["Clicks"] = ((HelpDetailedTopicEntity)FoundEntity).Clicks;
                        dr[0]["Comments"] = ((HelpDetailedTopicEntity)FoundEntity).TopicComments;
                        adapter.Update(TableTopics);
                    }
                    else
                    {
                        return;
                    }

                    #region For Procedure Add/Update/Delete

                    //To Update and Delete the Rows
                    adapter = new OleDbDataAdapter("Select * from Procedure_Steps", con);
                    DataTable Dt = new DataTable();
                    adapter.Fill(Dt);
                    Commb = new OleDbCommandBuilder(adapter);

                    int MaxId = 0;
                    if (Dt.Rows.Count > 0)
                    {
                        MaxId = Convert.ToInt32(Dt.Compute("MAX(ProcedureID)", ""));
                    }
                    DataRow[] DetailsRow = Dt.Select("TopicID= '" + FoundEntity.TopicID + "'");
                    int Rowcount = DetailsRow.Length;
                    for (int i = 0; i < Rowcount; i++)
                    {
                        bool RowDeleted = true;
                        for (int j = 0; j < ((HelpDetailedTopicEntity)FoundEntity).Procedureitems.Count; j++)
                        {
                            try
                            {
                                if (Convert.ToInt32(DetailsRow[i]["ProcedureID"]) == ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureID)
                                {
                                    DetailsRow[i]["StepNumber"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureStepNumber;
                                    DetailsRow[i]["TopicID"] = ((HelpDetailedTopicEntity)FoundEntity).TopicID;
                                    DetailsRow[i]["Comments" + GlobalSettings.Language] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureComment;
                                    if (((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureImage != null)
                                    {
                                        System.IO.MemoryStream MemStrem = (System.IO.MemoryStream)((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureImage.StreamSource;
                                        if (MemStrem != null)
                                        {
                                            //byte[] byts = ImageToByte(((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureImage);
                                            DetailsRow[i]["ProcedureImage"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ImageBinarySource;
                                        }
                                    }
                                    DetailsRow[i]["ProcedureText" + GlobalSettings.Language] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[j].ProcedureText;
                                    DetailsRow[i]["NewlyAddedOrUpdated"] = true;
                                    RowDeleted = false;
                                    break;
                                }
                            }
                            catch (DeletedRowInaccessibleException)
                            {

                            }
                        }
                        if (RowDeleted)
                        {
                            DetailsRow[i].Delete();
                            adapter.Update(Dt);
                            adapter = new OleDbDataAdapter("Select * from Procedure_Steps", con);
                            Dt = new DataTable();
                            adapter.Fill(Dt);
                            Commb = new OleDbCommandBuilder(adapter);
                            DetailsRow = Dt.Select("TopicID= '" + FoundEntity.TopicID + "'");
                            Rowcount--;
                            i--;
                        }
                    }

                    //To Add new rows
                    for (int i = 0; i < ((HelpDetailedTopicEntity)FoundEntity).Procedureitems.Count; i++)
                    {
                        if (((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureID == -1)
                        {
                            DataRow drow = Dt.NewRow();
                            drow["StepNumber"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureStepNumber;
                            drow["TopicID"] = ((HelpDetailedTopicEntity)FoundEntity).TopicID;
                            drow["ProcedureTextEnglish"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText;
                            drow["ProcedureTextGerman"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText;
                            //drow["ProcedureImage"] = ((DetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureImage;
                            drow["CommentsEnglish"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureComment;
                            drow["CommentsGerman"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ProcedureText;
                            drow["ProcedureID"] = ++MaxId;
                            drow["NewlyAddedOrUpdated"] = true;
                            if (((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ImageBinarySource != null && ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ImageBinarySource.Length > 0)
                            {
                                drow["ProcedureImage"] = ((HelpDetailedTopicEntity)FoundEntity).Procedureitems[i].ImageBinarySource;
                            }
                            Dt.Rows.Add(drow);
                        }
                    }
                    #endregion

                    #region UpdateDatabase
                    try
                    {
                        adapter.Update(Dt);
                        MessageBox.Show("Values Updated Successfully");
                    }
                    catch (Exception ex)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError56", ex);
                        MessageBox.Show("Error in Updating Values");
                    }

                    #endregion
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError52", ex);
                MessageBox.Show("Error in Updating Values");
            }
        }

        protected void OnUploadBtnClicked(object Sender, RoutedEventArgs e)
        {
            try
            {
                object CurrentButton = ((Button)Sender).CommandParameter;
                Procedures FoundEntity = (Procedures)CurrentButton;
                CurrentSelectedObject = FoundEntity;

                DetailsGrpBx.Visibility = System.Windows.Visibility.Hidden;
                DisplayNonLeafNodeGrpBx.Visibility = System.Windows.Visibility.Hidden;
                CaptureImageGrpBx.Visibility = System.Windows.Visibility.Visible;

                this.WindowState = System.Windows.WindowState.Minimized;
                System.Threading.Thread.Sleep(500);
                this.SaveScreen();
                Bitmap cropBmp = new Bitmap("screen.bmp");
                ImageHost.GetPictureBox().Image = cropBmp.Fit2PictureBox(ImageHost.GetPictureBox());
                this.WindowState = System.Windows.WindowState.Normal;
            }
            catch (Exception)
            {

            }
        }

        protected void ExportBtn_Click(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog();
            System.Windows.Forms.DialogResult result = dlg.ShowDialog();
            if (dlg.SelectedPath != null && dlg.SelectedPath != "")
            {
                ExportAll(dlg.SelectedPath);
            }
        }

        protected void ImportBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ImportHelp();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError53aa", ex);
            }
        }

        private void UploadImageBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.FileName = "Document"; // Default file name
            dlg.DefaultExt = ".png"; // Default file extension
            dlg.Filter = "Image file (.jpg)|*.jpg |(.png)|*.png"; // Filter files by extension 

            // Show open file dialog box
            Nullable<bool> result = dlg.ShowDialog();

            // Process open file dialog box results 
            if (result == true)
            {
                // Open document 
                string filename = dlg.FileName;
                string Extention = filename.Substring(filename.LastIndexOf("."));
                Bitmap cropableImage = new Bitmap(filename);
                ImageHost.GetPictureBox().Image = null;

                ImageHost.GetPictureBox().Image = cropableImage;
                CurrentCropedImage = cropableImage;
                MagValSlider.Value = 1;

                System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                byte[] Source = this.ImageToByte(cropableImage);
                bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                bmp.BeginInit();
                bmp.StreamSource = (new System.IO.MemoryStream(Source));
                bmp.CacheOption = BitmapCacheOption.OnLoad;
                bmp.EndInit();
                CurrentSelectedObject.ProcedureImage = bmp;
                CurrentSelectedObject.ImageBinarySource = Source;
            }

        }
        #endregion

        #region Capture Image Related Code

        private bool _selecting;
        private System.Drawing.Rectangle _selection;
        private System.Drawing.Rectangle TempRectangle;
        System.Drawing.Image CurrentCropedImage;
        private void CaptureImageBtnClick(object sender, RoutedEventArgs e)
        {
            this.WindowState = System.Windows.WindowState.Minimized;
            System.Threading.Thread.Sleep(1000);
            this.SaveScreen();
            Bitmap cropBmp = new Bitmap("screen.bmp");
            ImageHost.GetPictureBox().Image = cropBmp.Fit2PictureBox(ImageHost.GetPictureBox());
            this.WindowState = System.Windows.WindowState.Normal;
        }

        private void CropImageBtnClick(object Sender, RoutedEventArgs e)
        {
            if (!_selecting)
            {
                if (_selection != null && _selection.Width > 0 && _selection.Height > 0)
                {
                    int PictureBoxImageWidth = ImageHost.GetPictureBox().Image.Width;
                    int PictureBoxImageHeight = ImageHost.GetPictureBox().Image.Height;
                    if (PictureBoxImageWidth < _selection.X || PictureBoxImageHeight < _selection.Y)
                        return;
                    if (PictureBoxImageWidth < (_selection.X + _selection.Width))
                        _selection.Width = PictureBoxImageWidth - _selection.X;
                    if (PictureBoxImageHeight < (_selection.Y + _selection.Height))
                        _selection.Height = PictureBoxImageHeight - _selection.Y;

                    System.Drawing.Image img = ImageHost.GetPictureBox().Image.Crop(_selection);
                    // Fit image to the picturebox:              
                    ImageHost.GetPictureBox().Image = img;
                    CurrentCropedImage = img;
                    MagValSlider.Value = 1;

                    System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                    byte[] Source = this.ImageToByte(img);
                    bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                    bmp.BeginInit();
                    bmp.StreamSource = (new System.IO.MemoryStream(Source));
                    bmp.CacheOption = BitmapCacheOption.OnLoad;
                    bmp.EndInit();
                    CurrentSelectedObject.ProcedureImage = bmp;
                    CurrentSelectedObject.ImageBinarySource = Source;
                }
            }
        }

        private void MAGSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            int MagLevel = (int)(MagValSlider.Value);
            if (ImageHost != null)
            {
                if (CurrentCropedImage != null)
                {
                    System.Drawing.Image img = CurrentCropedImage;
                    ImageHost.GetPictureBox().Image = img.ScaleImage(MagLevel, 75);
                }
            }
        }

        private void ImageHost_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            // Starting point of the selection:
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                if (!_selecting)
                {
                    _selecting = true;
                    System.Drawing.Point Pts = new System.Drawing.Point(e.X, e.Y);
                    _selection = new System.Drawing.Rectangle(Pts, new System.Drawing.Size());
                    TempRectangle = new System.Drawing.Rectangle(Pts, new System.Drawing.Size());
                    TempRectangle.X = _selection.X;
                    TempRectangle.Y = _selection.Y;
                    TempRectangle.Width = _selection.Width;
                    TempRectangle.Height = _selection.Height;
                }
                else
                {
                    _selecting = false;

                    //System.Drawing.Image img = ImageHost.GetOglHost().Image.Crop(_selection);

                    //ImageHost.GetOglHost().Image = img;
                }
            }
        }

        private void ImageHost_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            // Update the actual size of the selection:
            if (_selecting)
            {
                int tempWidth = 0, tempheight = 0;
                tempWidth = Math.Abs(e.X - TempRectangle.X);
                tempheight = Math.Abs(e.Y - TempRectangle.Y);

                if (TempRectangle.X > e.X)
                    _selection.X = e.X;
                if (TempRectangle.Y > e.Y)
                    _selection.Y = e.Y;

                _selection.Width = tempWidth;
                _selection.Height = tempheight;

                // Redraw the picturebox:
                ImageHost.GetPictureBox().Refresh();
            }
        }

        private void ImageHost_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            if (_selecting)
            {
                // Draw a rectangle displaying the current selection
                System.Drawing.Color customColor = System.Drawing.Color.FromArgb(80, System.Drawing.Color.White);
                SolidBrush shadowBrush = new SolidBrush(customColor);
                System.Drawing.Pen pen = new System.Drawing.Pen(System.Drawing.Color.Black, 1);
                e.Graphics.DrawRectangle(pen, _selection);
                e.Graphics.FillRectangle(shadowBrush, _selection);
            }
        }

        private void CloseCapturingImageUI(object Sender, RoutedEventArgs e)
        {
            CaptureImageGrpBx.Visibility = System.Windows.Visibility.Hidden;
            DetailsGrpBx.Visibility = System.Windows.Visibility.Visible;
        }

        #endregion

        public Byte[] ImageToByte(BitmapImage imageSource)
        {
            System.IO.Stream stream = imageSource.StreamSource;
            Byte[] buffer = null;
            if (stream != null && stream.Length > 0)
            {
                using (System.IO.BinaryReader br = new System.IO.BinaryReader(stream))
                {
                    buffer = br.ReadBytes((Int32)stream.Length);
                }
            }

            return buffer;
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
        }

        void ExportAll(string FolderPath)
        {
            try
            {
                FolderPath += "\\Help Files";
                System.IO.Directory.CreateDirectory(FolderPath);
                FolderPath += "\\Images";
                System.IO.Directory.CreateDirectory(FolderPath);

                List<string> TableNames = new List<string>();
                //Written on 28-oct2012
                string Connection = @"Provider=" + FirstHalfConnectionString + dbPath; //Microsoft.Ace.OLEDB.12.0;Data Source=
                System.Data.OleDb.OleDbConnection dbcon = new System.Data.OleDb.OleDbConnection(Connection);


                // Save document 
                string filename = FolderPath.Substring(0, FolderPath.LastIndexOf("\\")) + "\\HelpFiles.XML";
                DataTable DataT = new DataTable();
                DataT = GetTable("select * from Procedure_Steps where NewlyAddedOrUpdated= true", Connection);
                DataT.Columns.Add("TopicName");
                DataT.Columns.Add("DescriptionEnglish");
                DataT.Columns.Add("DescriptionGerman");
                for (int i = 0; i < DataT.Rows.Count; i++)
                {
                    DataT.Rows[i]["TopicName"] = GetSingleValue("Select TopicName from Topics where TopicID = " + Convert.ToInt32(DataT.Rows[i]["TopicID"]));
                    DataT.Rows[i]["DescriptionEnglish"] = GetSingleValue("Select DescriptionEnglish from Topics where TopicID = " + Convert.ToInt32(DataT.Rows[i]["TopicID"]));
                    DataT.Rows[i]["DescriptionGerman"] = GetSingleValue("Select DescriptionGerman from Topics where TopicID = " + Convert.ToInt32(DataT.Rows[i]["TopicID"]));
                }
                string XmlRootName = "HelpNotes";
                XmlDocument doc = new XmlDocument();
                XmlNode root = doc.CreateElement(XmlRootName);
                doc.AppendChild(root);
                XmlNode version = doc.CreateXmlDeclaration("1.0", "utf-8", "yes");
                doc.InsertBefore(version, root);
                AddChildNodes(root, "Help Files", DataT);
                for (int i = 0; i < DataT.Rows.Count; i++)
                {
                    if (DataT.Rows[i]["ProcedureImage"] != DBNull.Value)
                    {
                        try
                        {
                            byte[] Source = (byte[])DataT.Rows[i]["ProcedureImage"];
                            System.IO.MemoryStream Ms = new System.IO.MemoryStream(Source, 0, Source.Length);
                            Ms.Write(Source, 0, Source.Length);
                            System.Drawing.Image Img = null;
                            Img = System.Drawing.Image.FromStream(Ms);
                            Img.Save(FolderPath + "\\" + DataT.Rows[i]["ProcedureID"].ToString() + ".png", System.Drawing.Imaging.ImageFormat.Png);
                        }
                        catch (Exception)
                        {
                        }
                    }
                }
                doc.Save(filename);
                MessageBox.Show("Database values exported successfully");

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX01", ex);
                MessageBox.Show(ex + "Export : 1");
            }
        }

        //Import Partprogram
        public bool ImportHelp()
        {
            DataSet XmlDs = new DataSet();
            DataSet XLDS = new DataSet();
            Hashtable RStyleId = new Hashtable();

            System.Windows.Forms.FolderBrowserDialog dlg = new System.Windows.Forms.FolderBrowserDialog();
            System.Windows.Forms.DialogResult result = dlg.ShowDialog();
            string ProgramNewName = dlg.SelectedPath;
            if (ProgramNewName != "")
            {
                string xmlPath = ProgramNewName + "\\HelpFiles" + ".xml";
                string ImageFolderPath = ProgramNewName + "\\Images\\";
                if (System.IO.File.Exists(xmlPath))
                {
                    #region Enter Values in Database
                    if (readXML(xmlPath, ref XmlDs))
                    {
                        try
                        {
                            DataRow[] DrowFromRapidSettings;
                            DataTable ImportDatatable = XmlDs.Tables[0];
                            string Connection = @"Provider=" + FirstHalfConnectionString + dbPath; //Microsoft.Ace.OLEDB.12.0;Data Source=
                            List<string> DeletedTopicsId = new List<string>();
                            for (int i = 0; i < ImportDatatable.Rows.Count; i++)
                            {
                                string ImagePath = ImageFolderPath + ImportDatatable.Rows[i]["ProcedureID"].ToString() + ".png";
                                string TopicsName = ImportDatatable.Rows[i]["TopicName"].ToString();
                                DrowFromRapidSettings = GlobalSettings.SettingsReader.GetRowsAccordingToFilter("ToolStripButton", "ButtonName", TopicsName);
                                if (DrowFromRapidSettings.Length > 0)
                                {
                                    DataTable ProcedureDataT = new DataTable();
                                    DataTable TopicsDataT = new DataTable();
                                    TopicsDataT = GetTable("select * from Topics Where TopicName ='" + TopicsName + "' AND ParentID<>-1", Connection);
                                    TopicsDataT.Rows[0]["DescriptionEnglish"] = ImportDatatable.Rows[i]["DescriptionEnglish"].ToString();
                                    TopicsDataT.Rows[0]["DescriptionGerman"] = ImportDatatable.Rows[i]["DescriptionGerman"].ToString();
                                    UpdateTable("Topics", Connection, TopicsDataT);
                                    string TopicsId = TopicsDataT.Rows[0]["TopicID"].ToString();
                                    ProcedureDataT = GetTable("select * from Procedure_Steps where TopicID=" + TopicsId, Connection);
                                    if (ProcedureDataT.Rows.Count > 0)
                                    {
                                        if (!DeletedTopicsId.Contains(TopicsId))
                                        {
                                            DeletedTopicsId.Add(TopicsId);
                                            foreach (DataRow Rows in ProcedureDataT.Rows)
                                            {
                                                Rows.Delete();
                                            }
                                            UpdateTable("Procedure_Steps", Connection, ProcedureDataT);
                                        }
                                    }
                                    ProcedureDataT = GetTable("select * from Procedure_Steps ", Connection);
                                    DataRow Drow = ProcedureDataT.NewRow();
                                    Drow["StepNumber"] = ImportDatatable.Rows[i]["StepNumber"];
                                    Drow["TopicID"] = Convert.ToInt32(TopicsId);
                                    Drow["ProcedureTextEnglish"] = ImportDatatable.Rows[i]["ProcedureTextEnglish"];
                                    Drow["ProcedureTextGerman"] = ImportDatatable.Rows[i]["ProcedureTextGerman"];
                                    Drow["ProcedureImage"] = GetImageBytes(ImagePath);
                                    Drow["CommentsEnglish"] = ImportDatatable.Rows[i]["CommentsEnglish"];
                                    Drow["NewlyAddedOrUpdated"] = false;
                                    Drow["CommentsGerman"] = ImportDatatable.Rows[i]["CommentsGerman"];
                                    ProcedureDataT.Rows.Add(Drow);
                                    UpdateTable("Procedure_Steps", Connection, ProcedureDataT);
                                }
                            }

                            MessageBox.Show("Help Imported Successsfully");
                        }
                        catch (Exception ex)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("ImportPP Database:PPLast", ex);
                            return false;
                        }
                    }
                    #endregion

                }
            }
            return false;
        }

        //Read the Xml File
        int tablecount, rowcount;
        private bool readXML(string Filepath, ref DataSet XmlDs)
        {
            bool correctXml = false;
            try
            {
                tablecount = 0; rowcount = 0;
                {
                    XmlTextReader reader = new XmlTextReader(Filepath);
                    reader.WhitespaceHandling = WhitespaceHandling.None;
                    XmlDocument xmlDoc = new XmlDocument();
                    xmlDoc.Load(reader);
                    reader.Close();
                    XmlNode xnod = xmlDoc.DocumentElement;
                    if (xnod.Name == "HelpNotes")
                    {
                        AddXMLTablesToDs(ref XmlDs, xnod, 1);
                        correctXml = true;
                    }
                    else
                    {
                        MessageBox.Show("Your XML was probably bad...");
                        correctXml = false;
                    }
                }
            }
            catch (XmlException ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError57", ex);
                MessageBox.Show("Your XML was probably bad...");
                correctXml = false;
            }
            return correctXml;
        }

        //Transform Xml tables to System.data.dataset Tables
        private void AddXMLTablesToDs(ref DataSet XmlDs, XmlNode xnod, Int32 intLevel)
        {
            XmlNode xnodWorking;
            if (intLevel == 5)
            {
                if (XmlDs.Tables[tablecount - 1].Rows.Count < rowcount)
                    XmlDs.Tables[tablecount - 1].Rows.Add();
                XmlDs.Tables[tablecount - 1].Rows[rowcount - 1][xnod.ParentNode.Name] = xnod.Value;
            }
            if (xnod.NodeType == XmlNodeType.Element)
            {
                XmlNamedNodeMap mapAttributes = xnod.Attributes;
                if (xnod.Name == "DataRow")
                {
                    rowcount++;
                }
                foreach (XmlNode xnodAttribute in mapAttributes)
                {
                    if (xnod.Name == "Table")
                    {
                        DataTable dt = new DataTable(xnodAttribute.Value);
                        XmlDs.Tables.Add(dt);
                        tablecount++;
                        rowcount = 0;
                    }
                    else if (intLevel == 4)
                    {
                        if (rowcount == 1)
                        {
                            XmlDs.Tables[tablecount - 1].Columns.Add(xnod.Name, Type.GetType(xnodAttribute.Value));
                        }
                    }
                }
                if (xnod.HasChildNodes)
                {
                    xnodWorking = xnod.FirstChild;
                    while (xnodWorking != null)
                    {
                        AddXMLTablesToDs(ref XmlDs, xnodWorking, intLevel + 1);
                        xnodWorking = xnodWorking.NextSibling;
                    }
                }
            }
        }

        //Add child to XML
        private void AddChildNodes(XmlNode Root, string TableName, DataTable dt)
        {
            try
            {
                XmlNode Table = Root.OwnerDocument.CreateElement("Table");
                XmlAttribute attrib;
                attrib = Root.OwnerDocument.CreateAttribute("Name");
                attrib.Value = TableName;
                Table.Attributes.Append(attrib);
                Root.AppendChild(Table);
                for (int j = 0; j < dt.Rows.Count; j++)
                {
                    XmlNode Row = Table.OwnerDocument.CreateElement("DataRow");
                    Table.AppendChild(Row);
                    for (int col = 0; col < dt.Rows[j].ItemArray.Length; col++)
                    {
                        XmlNode Collumns = Row.OwnerDocument.CreateElement(dt.Columns[col].ColumnName.ToString());
                        XmlAttribute CollumnDatatype;
                        CollumnDatatype = Row.OwnerDocument.CreateAttribute("DataType");
                        CollumnDatatype.Value = dt.Columns[col].DataType.ToString();
                        Collumns.Attributes.Append(CollumnDatatype);
                        if (dt.Columns[col].DataType == Type.GetType("System.Byte[]"))
                        {
                            Collumns.InnerText = null;
                        }
                        else
                            Collumns.InnerText = dt.Rows[j][col].ToString();
                        Row.AppendChild(Collumns);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX02", ex);
            }
        }

        DataTable GetTable(string Query, string Connectionstring, string Tablename = "MyTable")
        {
            try
            {
                string query = Query;
                System.Data.OleDb.OleDbDataAdapter adapter = new System.Data.OleDb.OleDbDataAdapter(query, Connectionstring);
                DataTable DT = new DataTable(Tablename);
                adapter.Fill(DT);
                return DT;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:EX03", ex);
                return null;
            }
        }

        bool UpdateTable(string TableName, string Connectionstring, DataTable Dt)
        {
            try
            {
                string query = "Select * from " + TableName;
                System.Data.OleDb.OleDbDataAdapter adapter = new System.Data.OleDb.OleDbDataAdapter(query, Connectionstring);
                OleDbCommandBuilder CommB = new OleDbCommandBuilder(adapter);
                adapter.Update(Dt);
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:RH03", ex);
                return false;
            }
        }

        byte[] GetImageBytes(string ImagePath)
        {
            System.Drawing.Image CurrentImage = null;
            byte[] ReturnableBytes = null;
            if (System.IO.File.Exists(ImagePath))
            {
                CurrentImage = System.Drawing.Image.FromFile(ImagePath);
                ReturnableBytes = this.ImageToByte(CurrentImage);
                return ReturnableBytes;
            }
            else
                return ReturnableBytes;
        }

        private string GetSingleValue(string query)
        {
            string TopicName = "";
            string Connection = @"Provider=" + FirstHalfConnectionString + dbPath; //Microsoft.Ace.OLEDB.12.0;Data Source=
            OleDbConnection con = new OleDbConnection(Connection);
            con.Open();
            OleDbCommand cmd = new OleDbCommand(query);
            cmd.Connection = con;
            OleDbDataReader rdr = cmd.ExecuteReader();
            rdr.Read();
            TopicName = rdr.GetValue(0).ToString();
            con.Close();
            return TopicName;
        }

        void HelpDG_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            HelpDG.Items.Refresh();
        }
    }

    public class TopicDetails
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
        public ObservableCollection<TopicDetails> Branches { get; set; }
        public TopicDetails()
        {
            Branches = new ObservableCollection<TopicDetails>();
        }
    }
    public class HelpEntity
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
        public ObservableCollection<HelpEntity> ColHelpDetails { get; set; }
        public HelpEntity()
        {
            ColHelpDetails = new ObservableCollection<HelpEntity>();
        }
    }
    public class HelpDisplayOnScreen : TopicDetails
    {
        public HelpDisplayOnScreen(string ConnectionStr)
        {
            try
            {
                DataSet ds = new DataSet();
                OleDbConnection con = new OleDbConnection(ConnectionStr);
                OleDbDataAdapter adapter;

                con.Open();
                adapter = new OleDbDataAdapter("Select * from Topics", con);
                adapter.Fill(ds, "Topics");
                con.Close();
                this.Branches = new ObservableCollection<TopicDetails>();
                for (int i = 0; i < ds.Tables[0].Rows.Count; i++)
                {
                    if (((int)ds.Tables[0].Rows[i]["ParentID"]) == -1)
                    {
                        TopicDetails TmpTopicEntity = GetFilledObject(ds.Tables[0].Rows[i], ConnectionStr);
                        FillTree(ds.Tables[0], ref TmpTopicEntity, ConnectionStr);
                        Branches.Add(TmpTopicEntity);
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError04", ex);
            }
        }
        public void FillTree(DataTable Source, ref TopicDetails Me, string DBconnectorStr)
        {
            try
            {
                if (Me.IsLeafNode) return;

                for (int i = 0; i < Source.Rows.Count; i++)
                {
                    if (((int)Source.Rows[i]["ParentID"]) == Me.TopicID)
                    {
                        TopicDetails TmpTopicEntity = GetFilledObject(Source.Rows[i], DBconnectorStr);
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
        private TopicDetails GetFilledObject(DataRow SourceRow, string DBconnectorStr)
        {
            TopicDetails TmpTopicEntity;
            bool IsLeafNode = (bool)SourceRow["IsLeafNode"];
            if (IsLeafNode)
            {
                TmpTopicEntity = new HelpDetailedTopicEntity(DBconnectorStr);

                System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                if (SourceRow["ButtonIcon"] != System.DBNull.Value)
                {
                    byte[] Source = (byte[])SourceRow["ButtonIcon"];
                    bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                    bmp.BeginInit();
                    bmp.StreamSource = (new System.IO.MemoryStream(Source));
                    bmp.EndInit();
                    ((HelpDetailedTopicEntity)TmpTopicEntity).ButtonImage = bmp;
                }
                ((HelpDetailedTopicEntity)TmpTopicEntity).Clicks = null;
                if (SourceRow["clicks"] != DBNull.Value)
                    ((HelpDetailedTopicEntity)TmpTopicEntity).Clicks = (int)SourceRow["clicks"];
                ((HelpDetailedTopicEntity)TmpTopicEntity).TopicComments = SourceRow["Comments"].ToString();
            }
            else
            {
                TmpTopicEntity = new TopicDetails();
            }
            TmpTopicEntity.TopicID = (int)SourceRow["TopicID"];
            TmpTopicEntity.TopicName = SourceRow["TopicName"].ToString();
            TmpTopicEntity.TopicDisplayName = SourceRow["TopicDisplayName" + GlobalSettings.Language].ToString();
            TmpTopicEntity.TopicDescription = SourceRow["Description" + GlobalSettings.Language].ToString();
            TmpTopicEntity.IsLeafNode = IsLeafNode;
            return TmpTopicEntity;
        }
    }
    public class HelpDetailedTopicEntity : TopicDetails
    {
        string dbConnectionStr;
        OleDbConnection con;
        OleDbDataAdapter adapter;
        DataSet ds = new DataSet();
        public ObservableCollection<Procedures> Procedureitems { get; set; }
        private int? _Clicks = 0;
        public int? Clicks
        {
            get
            {
                return _Clicks;
            }
            set
            {
                _Clicks = value;
            }
        }
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
        public HelpDetailedTopicEntity(string ConnectionStr)
        {
            dbConnectionStr = ConnectionStr;
            this.Procedureitems = new ObservableCollection<Procedures>();
        }
        public void FillDetails(string ConnectionStr)
        {
            this.dbConnectionStr = ConnectionStr;
            ForProcedure(TopicID);
        }
        public void ForProcedure(int TopicIDs)
        {
            try
            {
                Procedureitems.Clear();
                DataSet ds = new DataSet();
                con = new OleDbConnection(dbConnectionStr);
                con.Open();
                adapter = new OleDbDataAdapter("Select ProcedureID,StepNumber,ProcedureText" + GlobalSettings.Language + ",ProcedureImage,Comments" + GlobalSettings.Language + " from Procedure_Steps where(TopicID=" + TopicIDs + ")", con);
                adapter.Fill(ds, "Procedure");
                con.Close();
                for (int i = 0; i < ds.Tables[0].Rows.Count; i++)
                {
                    string StepNumber, proced, ProcedureText, Comments;
                    int? Procedure_Number = null, Step_Number = null;
                    byte[] ImageBinaryData = null;
                    System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                    if (ds.Tables[0].Rows[i]["ProcedureImage"] != DBNull.Value)
                    {
                        try
                        {
                            byte[] Source = (byte[])ds.Tables[0].Rows[i]["ProcedureImage"];
                            bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                            bmp.BeginInit();
                            bmp.StreamSource = (new System.IO.MemoryStream(Source));
                            bmp.EndInit();
                            ImageBinaryData = Source;
                        }
                        catch (Exception)
                        {
                        }
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
                    this.Procedureitems.Add(new Procedures(Procedure_Number, Step_Number, ProcedureText, bmp, Comments, ImageBinaryData));
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("HelpfileError05", ex);
            }
        }
    }
    public class FormsHostPanel : WindowsFormsHost
    {
        //private System.ComponentModel.IContainer components = null;
        private System.Windows.Forms.PictureBox pictureBox;

        public FormsHostPanel()
            : base()
        {
            InitializeMyComponent();
            this.SizeChanged += new SizeChangedEventHandler(FormsHostPanel_SizeChanged);
            this.GotFocus += new RoutedEventHandler(FormsHostPanel_GotFocus);
        }
        private void InitializeMyComponent()
        {
            this.pictureBox = new System.Windows.Forms.PictureBox();
            // 
            // pictureBox
            // 
            this.pictureBox.Location = new System.Drawing.Point(0, 0);
            this.pictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.pictureBox.Name = "pictureBox";
            this.pictureBox.Size = new System.Drawing.Size(0, 0);
            this.pictureBox.TabIndex = 1;
            this.pictureBox.TabStop = false;
            // 
            // Form1
            // 
            this.Child = pictureBox;

            this.FocusVisualStyle = null;
        }

        public System.Windows.Forms.PictureBox GetPictureBox()
        {
            return pictureBox;
        }
        private void FormsHostPanel_SizeChanged(object sender, RoutedEventArgs e)
        {
            this.pictureBox.Size = new System.Drawing.Size((int)this.Width, (int)this.Height);
        }
        public void FormsHostPanel_GotFocus(object sender, RoutedEventArgs e)
        {
            pictureBox.Focus();
        }

        /// <summary>
        /// This provides the handle as IntPtr of the pic box.
        /// </summary>
        public IntPtr GetHandle
        {
            get
            {
                return pictureBox.Handle;
            }
        }
        public void SetCursor(System.Windows.Forms.Cursor Csr)
        {
            this.pictureBox.Cursor = Csr;
        }
    }
    public static class VirtualizingStackPanelBehaviors
    {
        public static bool GetIsPixelBasedScrolling(DependencyObject obj)
        {
            return (bool)obj.GetValue(IsPixelBasedScrollingProperty);
        }

        public static void SetIsPixelBasedScrolling(DependencyObject obj, bool value)
        {
            obj.SetValue(IsPixelBasedScrollingProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsPixelBasedScrolling.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsPixelBasedScrollingProperty =
            DependencyProperty.RegisterAttached("IsPixelBasedScrolling", typeof(bool), typeof(VirtualizingStackPanelBehaviors), new UIPropertyMetadata(false, OnIsPixelBasedScrollingChanged));

        private static void OnIsPixelBasedScrollingChanged(DependencyObject o, DependencyPropertyChangedEventArgs e)
        {
            var virtualizingStackPanel = o as VirtualizingStackPanel;
            if (virtualizingStackPanel == null)
                throw new InvalidOperationException();

            var isPixelBasedPropertyInfo = typeof(VirtualizingStackPanel).GetProperty("IsPixelBased", System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.DeclaredOnly | System.Reflection.BindingFlags.NonPublic);
            if (isPixelBasedPropertyInfo == null)
                throw new InvalidOperationException();

            isPixelBasedPropertyInfo.SetValue(virtualizingStackPanel, (bool)(e.NewValue), null);
        }
    }
    public class BindingProxy : Freezable
    {
        #region Overrides of Freezable

        protected override Freezable CreateInstanceCore()
        {
            return new BindingProxy();
        }

        #endregion

        public object Data
        {
            get { return (object)GetValue(DataProperty); }
            set { SetValue(DataProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Data.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty DataProperty =
            DependencyProperty.Register("Data", typeof(object), typeof(BindingProxy), new UIPropertyMetadata(null));
    }
}