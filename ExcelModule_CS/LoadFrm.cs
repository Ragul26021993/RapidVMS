using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Microsoft.VisualBasic;
using Microsoft.VisualBasic.CompilerServices;

namespace rrm
{
    public partial class LoadFrm
    {
        static LoadFrm()
        {
            dgTitles = new DataGridView();
        }

        public LoadFrm()
        {
            // This call is required by the Windows Form Designer.
            InitializeComponent();
            // Add any initialization after the InitializeComponent() call.
            AddTitleDatagridViewToForm();
            // Fill the tables in the dataset
            Data.FillTableRange(DatabaseConstants.TableNames);
            __okButt.Name = "_okButt";
            __cancelButton.Name = "_cancelButton";
            _chooseProgram.Name = "chooseProgram";
            _ChooseReport.Name = "ChooseReport";
            _btnDeleteProgram.Name = "btnDeleteProgram";
            _btnRename.Name = "btnRename";
            __reportStyleBox.Name = "_reportStyleBox";
            __ReportBox.Name = "_ReportBox";
            _btnMakeNewReport.Name = "btnMakeNewReport";
            _btnClose.Name = "btnClose";
            _btnSet.Name = "btnSet";
            _txtPgInitials.Name = "txtPgInitials";
            _rdbShowDateWise.Name = "rdbShowDateWise";
            _rdbShowNameWise.Name = "rdbShowNameWise";
        }

        private string SelectedReportStr, SelectedProgramStr, SelectedReportStyleStr;
        private xl_DB Data = new xl_DB();
        private int Count1, Count2;
        private List<string> ProgramNameList = new List<string>();
        private bool TitleValuesChanged = false;

        public delegate void LoadForm_Handler(string Str);

        public static event LoadForm_Handler OnLoadClicked;

        public static DataGridView dgTitles;

        private void AddTitleDatagridViewToForm()
        {
            dgTitles.BorderStyle = BorderStyle.Fixed3D;
            dgTitles.CellBorderStyle = DataGridViewCellBorderStyle.Sunken;
            dgTitles.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.None;
            dgTitles.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dgTitles.Columns.AddRange(new DataGridViewColumn[] { TitleName, TitleValue });
            dgTitles.Location = new Point(5, 531);
            dgTitles.Name = "dgTitles";
            dgTitles.ScrollBars = ScrollBars.Vertical;
            dgTitles.Size = new Size(736, 84);
            dgTitles.TabIndex = 17;
            Controls.Add(dgTitles);
        }
        // The form load function
        private void LoadFrm_Load(object sender, EventArgs e)
        {
            try
            {
                GetAllProgramNamesFilled("Program_Id ASC");
                dgTitles.AllowUserToAddRows = false;
                dgTitles.AllowUserToResizeRows = false;
                dgTitles.AllowUserToResizeColumns = false;
                dgTitles.Columns[0].Width = (int)Math.Round((dgTitles.Width - 10) / 2d);
                dgTitles.Columns[1].Width = (int)Math.Round((dgTitles.Width - 10) / 2d);
                dgTitles.CellEndEdit += Title_TxtChanged;
                // RenameEnabled = False
                // DeleteEnabled = False

                dgPrograms.SelectionChanged += _programBox_SelectedIndexChanged;
                rdbShowDateWise.Checked = true;
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0027:" + ex.Message);
            }
        }
        // Handle selction of programs 
        private void GetAllProgramNamesFilled(string sort, string Filter = "")
        {
            // Make datagrid View to its original state
            dgPrograms.DataSource = null;
            dgPrograms.Rows.Clear();
            DataView D;
            if (string.IsNullOrEmpty(Filter))
            {
                D = new DataView(Data.GetTable("ProgramInformation"), "Program_Id >= 1", sort, DataViewRowState.CurrentRows);
            }
            else
            {
                D = new DataView(Data.GetTable("ProgramInformation"), Filter, sort, DataViewRowState.CurrentRows);
            }
            // Get the table as per query given
            var Dt = D.ToTable();
            // now make an formatted table as to be shown in Frontend.
            var ProgramT = new DataTable();
            ProgramT.Columns.Add("Program Name", typeof(string));
            ProgramT.Columns.Add("Program Location", typeof(string));
            ProgramT.Columns.Add("Username", typeof(string));
            for (int i = 0, loopTo = Dt.Rows.Count - 1; i <= loopTo; i++)
            {
                string FullProgramName = Conversions.ToString(D.ToTable().Rows[i][1]);
                string Path = FullProgramName.Substring(0, FullProgramName.LastIndexOf(@"\") + 1);
                string OnlyProgramName = FullProgramName.Substring(FullProgramName.LastIndexOf(@"\") + 1);
                var dr = ProgramT.NewRow();
                dr.ItemArray = new object[] { OnlyProgramName, Path, Dt.Rows[i][2] };
                ProgramT.Rows.Add(dr);
            }
            // Set the datasource for the datagrid view
            dgPrograms.DataSource = ProgramT;
            // format the table
            dgPrograms.Columns[0].Width = 200;
            dgPrograms.Columns[1].Width = 290;
            dgPrograms.Columns[2].Width = 200;
            // Select the last row
            if (dgPrograms.Rows.Count >= 1)
            {
                dgPrograms.Rows[dgPrograms.Rows.Count - 1].Selected = true;
            }

            if (dgPrograms.Rows.Count - dgPrograms.Height / (double)dgPrograms.RowTemplate.Height + 1d >= 0d)
            {
                dgPrograms.FirstDisplayedScrollingRowIndex = (int)Math.Round(dgPrograms.Rows.Count - dgPrograms.Height / (double)dgPrograms.RowTemplate.Height + 1d);
            }
        }
        // Handling the Choose Program button Click action
        private void chooseProgram_Click(object sender, EventArgs e)
        {
            try
            {
                var OpenFD = new OpenFileDialog();
                bool b;
                OpenFD.Filter = "(*.rpp)Only PartProgram files|*.rpp;*.RPP";
            Repeat:
                ;
                var dailogR = OpenFD.ShowDialog();
                if (dailogR == DialogResult.OK)
                {
                    b = CheckFile(OpenFD.FileName, "ProgramInformation", 1);
                    if (b == true)
                    {
                        MessageBox.Show("This Program does not have an entry into the database!!" + Constants.vbCrLf + "You can open those programs which are made exclusively with this version of ExcelModule only." + Constants.vbCrLf + "Press 'Ok' to choose another program name." + Constants.vbCrLf + Constants.vbCrLf + "[ Currently Choosen Program Name : " + OpenFD.FileName + " ]", "Rapid-I", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                        goto Repeat;
                    }
                    else
                    {
                        for (int i = 0, loopTo = dgPrograms.Rows.Count - 1; i <= loopTo; i++)
                        {
                            if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(OpenFD.FileName, Operators.ConcatenateObject(dgPrograms.Rows[i].Cells[1].Value, dgPrograms.Rows[i].Cells[0].Value), false)))
                            {
                                dgPrograms.Rows[i].Selected = true;
                                break;
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0024:" + ex.Message);
            }
        }
        // Handling the Report Choosing by the user. 
        private void ChooseReport_Click(object sender, EventArgs e)
        {
            try
            {
                var OpenFD = new OpenFileDialog();
                bool b;
                OpenFD.Filter = "(*.xls,*.xls)Only Excel files|*.xls;*.xls";
                var dailogR = OpenFD.ShowDialog();
                if (dailogR == DialogResult.OK) // 'Windows.Forms.
                {
                    b = CheckFile(OpenFD.FileName, "ReportInfo");
                    if (b == true)
                    {
                        _ReportBox.Items.Add(OpenFD.FileName);
                        _ReportBox.SelectedItem = OpenFD.FileName;
                    }
                    else
                    {
                        Interaction.MsgBox("File Already Exists; kindly choose a different file name!!");
                    }
                }
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0033:" + ex.Message);
            }
        }
        // Function to check the status of the file choosen into the open file dailog 
        private bool CheckFile(string str, string TableName, int ItemIndex = 0)
        {
            try
            {
                string str1;
                for (int i = 0, loopTo = Data.GetTable(TableName).Rows.Count - 1; i <= loopTo; i++)
                {
                    if (ItemIndex != 0)
                    {
                        str1 = Conversions.ToString(Data.GetTable(TableName).Rows[i][ItemIndex]);
                    }
                    else
                    {
                        str1 = Conversions.ToString(Data.GetTable(TableName).Rows[i][2]);
                    }

                    if ((str1 ?? "") == (str ?? ""))
                    {
                        return false;
                    }
                }

                return true;
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0025:" + ex.Message);
            }

            return default;
        }
        // Handling the Selected program from the combobox of Programs   
        private void _programBox_SelectedIndexChanged(object sender, EventArgs e) // Handles _programBox.SelectedIndexChanged
        {
            try
            {
                DataGridView _listB = (DataGridView)sender;
                Reset_All();
                string FullProgramName;
                if (_listB.SelectedRows.Count < 1)
                    return;
                // Try
                FullProgramName = Conversions.ToString(Operators.ConcatenateObject(_listB.SelectedRows[0].Cells[1].Value, _listB.SelectedRows[0].Cells[0].Value));
                SelectedProgramStr = FullProgramName;
                // Catch ex As Exception

                // End Try

                // Try
                // Dim dataR As DataRow = Data.Select_Rows("ProgramInformation", "Program_Name", SelectedProgramStr)(0)
                // lblUsername.Text = dataR.Item(2)
                // Catch ex As Exception

                // End Try
                var Ar = new ArrayList();
                Ar = Data.GetAllReportsStyleNames(Conversions.ToInteger(FullProgramName));
                _reportLabel.Visible = true;
                _reportStyleBox.Visible = true;
                _reportStyleBox.Items.Clear();
                for (int i = 0, loopTo = Ar.Count - 1; i <= loopTo; i++)
                    _reportStyleBox.Items.Add(Ar[i]);
                _ReportBox.Items.Clear();
                _ReportBox.Text = "";
                if (_reportStyleBox.Items.Count > 0)
                    _reportStyleBox.SelectedIndex = 0;
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0026:" + ex.Message);
            }
        }
        // Handling the cancel button click action
        private void _cancelButton_Click(object sender, EventArgs e)
        {
            ReturnProgramName = "";
            ReturnReportName = "";
            ReturnReportStyleName = "";
            OnLoadClicked?.Invoke("Cancel");
            Hide();
        }
        // At Run Time as per need adding the necessary controls to the form
        private void _reportStyleBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            try
            {
                dgTitles.Rows.Clear();
                // Getting the report Style id for the report Style Name.
                int Report_Style_ID = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", _reportStyleBox.SelectedItem)[0][0]);
                // Getting all the rows from the title information containing the title name corresponding to the given Report_Style_ID.
                var dataR = Data.Select_Rows("TitleInformation", "Report_Style_Id", Report_Style_ID);
                // Calling the function to populate the Report Names from the Report_Style_ID.
                PopulateReportNames(Report_Style_ID);
                for (int a = 0, loopTo = dataR.Length - 1; a <= loopTo; a++)
                {
                    var R = new DataGridViewRow();
                    R.CreateCells(dgTitles, new object[] { dataR[a][2].ToString(), dataR[a][4].ToString() });
                    dgTitles.Rows.Add(R);
                }
                // making the load button visible.
                _okButt.Visible = true;
                _okButt.Enabled = true;
                SelectedReportStyleStr = _reportStyleBox.Text;
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0028:" + ex.Message);
            }
        }
        // Handling the changes made to the titles values in the datagrid view.
        private void Title_TxtChanged(object sender, DataGridViewCellEventArgs e)
        {
            int Report_Style_ID = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", _reportStyleBox.SelectedItem)[0][0]);
            // Getting all the rows from the title information containing the title name corresponding to the given Report_Style_ID.
            var dataR = Data.Select_Rows("TitleInformation", "Report_Style_Id", Report_Style_ID);
            if ((dgTitles.Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString() ?? "") != (Conversions.ToString(dataR[e.RowIndex][4]) ?? ""))
            {
                TitleValuesChanged = true;
            }
            else
            {
                TitleValuesChanged = false;
            }
        }
        // This Adds all report names for the given report style 
        private void PopulateReportNames(int Id)
        {
            try
            {
                var DataRN = Data.DataS.Tables["ReportInformation"].Select("Report_Style_Id=" + "'" + Id + "'", "Report_Id DESC");
                _ReportNamelabel.Visible = true;
                _ReportBox.Visible = true;
                _ReportBox.Items.Clear();
                ChooseReport.Visible = true;
                for (int i = 0, loopTo = DataRN.Length - 1; i <= loopTo; i++)
                {
                    // Adding the names of the reports 
                    string FullReportName = Conversions.ToString(DataRN[i][2]);
                    _ReportBox.Items.Add(FullReportName);
                }

                if (_ReportBox.Items.Count > 0)
                    _ReportBox.SelectedItem = _ReportBox.Items[0].ToString();
                SelectedReportStr = _ReportBox.Items[0].ToString();
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("Error Code E0030:" + ex.Message);
            }
        }
        // handling the change of report names in list box selection.
        private void Handle_SelectedReport(object sender, EventArgs e)
        {
            // SelectedReport = CType(sender, ListBox).SelectedItem.ToString
            SelectedReportStr = ((ListBox)sender).SelectedItem.ToString();
        }
        // Handling the Load button click.
        private void _okButt_Click(object sender, EventArgs e)
        {
            try
            {
                if (TitleValuesChanged & IsReportNameExist())
                {
                    MessageBox.Show("You have changed the title values.Please give another name for the report.", "ExcelModule", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    return;
                }
                else
                {
                    // Checking the report name 
                    var PreviousReportRow = Data.Select_Rows("ReportStyleInformation", "ReportStyleName", _reportStyleBox.SelectedItem)[0];
                    int ReportStyle_ID = Conversions.ToInteger(PreviousReportRow[0]);
                    // now check the boolean flag and proceed 
                    if (!IsReportNameExist())
                    {
                        // Create the program 
                        // Select the program template and copy and paste it to the report location.
                        var ReportInformationRow = Data.GetTable("ReportInformation").Select("R_ReportName=" + "'" + _ReportBox.Items[0].ToString() + "'")[0];
                        string Template_location = Conversions.ToString(PreviousReportRow[4]);
                    CopyFile:
                        try
                        {
                            ;
                            if (File.Exists(_ReportBox.Text))
                            {
                                // File already exists , so showing a message box to give option to overwrite
                                var M = MessageBox.Show("The file already exists in the report folder." + "Click 'Ok' to overwrite or 'Cancel' to give another name to the Report.", "Excel Module", MessageBoxButtons.OKCancel, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1);
                                if (M == DialogResult.OK) // 'Windows.Forms.
                                {
                                    File.Copy(Template_location, _ReportBox.Text, true);
                                }
                                else
                                {
                                    return;
                                }
                            }
                            else
                            {
                                File.Copy(Template_location, _ReportBox.Text, true);
                            }
                        }
                        // My.Computer.FileSystem.CopyFile(Template_location, _ReportBox.Text)
                        catch (Exception ex)
                        {
                            var M = MessageBox.Show("Caught following Excption in handling this file." + Constants.vbCrLf + ex.ToString(), "Excel Module", MessageBoxButtons.RetryCancel, MessageBoxIcon.Asterisk);
                            if (M == DialogResult.Retry) // 'Windows.Forms.
                            {
                                goto CopyFile;
                            }
                        }
                        // Update the database about this new report name.
                        var NewReportRow = Data.GetTable("ReportInformation").NewRow();
                        NewReportRow[1] = ReportStyle_ID;
                        NewReportRow[2] = _ReportBox.Text;
                        for (int i = 3; i <= 24; i++)
                            NewReportRow[i] = ReportInformationRow[i];
                        Data.GetTable("ReportInformation").Rows.Add(NewReportRow);
                        Data.Update_Table("ReportInformation");
                    }
                }

                Hide();
                OnLoadClicked?.Invoke("Load");
            }
            catch (Exception ex)
            {
                Interaction.MsgBox(ex.Message);
                Hide();
            }
        }
        // For resetting the form to its original condition.
        public bool Reset_All()
        {
            try
            {
                Data.DataS.Clear();
                Data.FillTableRange(DatabaseConstants.TableNames);
                TitleValuesChanged = false;
                _reportStyleBox.Visible = false;
                _ReportBox.Visible = false;
                _okButt.Visible = false;
                _ReportNamelabel.Visible = false;
                _reportLabel.Visible = false;
                ChooseReport.Visible = false;
                dgTitles.Rows.Clear();
                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }
        // Checking if report name already exist in databse or not.
        private bool IsReportNameExist()
        {
            int ReportStyle_ID = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", _reportStyleBox.SelectedItem)[0][0]);
            var DataRN = Data.Select_Rows("ReportInformation", "Report_Style_Id", ReportStyle_ID);
            for (int i = 0, loopTo = DataRN.Length - 1; i <= loopTo; i++)
            {
                if ((_ReportBox.Text ?? "") == (Conversions.ToString(DataRN[i][2]) ?? ""))
                {
                    return true;
                }
            }

            return false;
        }
        // Function to handle the deleting the already made program.
        private void btnDeleteProgram_Click(object sender, EventArgs e)
        {
            if (dgPrograms.SelectedRows.Count < 1)
                return;
            var D = MessageBox.Show("Are you sure, you want to delete this Program Name?" + Constants.vbCrLf + "NOTE : This will also delete all the related Reports made with this Program name.", "ExcelModule", MessageBoxButtons.OKCancel, MessageBoxIcon.Warning, MessageBoxDefaultButton.Button1);
            if (D == DialogResult.OK) // 'Windows.Forms.
            {
                try
                {
                    string ProgramN = Conversions.ToString(Operators.ConcatenateObject(dgPrograms.SelectedRows[0].Cells[1].Value, dgPrograms.SelectedRows[0].Cells[0].Value));
                    int P_ID = Conversions.ToInteger(Data.DataS.Tables["ProgramInformation"].Select("Program_Name=" + "'" + ProgramN + "'")[0][0]);
                    var ReportStyleRows = Data.DataS.Tables["ReportStyleInformation"].Select("Program_Id=" + "'" + P_ID + "'");
                    string TemplateLocation = Conversions.ToString(ReportStyleRows[0][4]);
                    int RepS_ID = Conversions.ToInteger(ReportStyleRows[0][0]);
                    var ReportRows = Data.DataS.Tables["ReportInformation"].Select("Report_Style_Id=" + "'" + RepS_ID + "'");
                    var FieldRows = Data.DataS.Tables["FieldInformation"].Select("Report_Style_Id=" + "'" + RepS_ID + "'");
                    try               // Now Delete the files from the harddisk.
                    {
                        File.Delete(ProgramN); // ' My.Computer.FileSystem.DeleteFile(ProgramN)
                        File.Delete(TemplateLocation); // 'My.Computer.FileSystem.DeleteFile(TemplateLocation)
                    }
                    catch (Exception ex)
                    {
                        Interaction.MsgBox("Following Error occured in deleting files" + Constants.vbCrLf + ex.Message);
                    }

                    Data.DataS.Tables["ProgramInformation"].Select("Program_Name=" + "'" + ProgramN + "'")[0].Delete();     // Now delete the database entries.
                    for (int i = 0, loopTo = ReportStyleRows.Length - 1; i <= loopTo; i++)
                        ReportStyleRows[i].Delete();
                    for (int i = 0, loopTo1 = ReportRows.Length - 1; i <= loopTo1; i++)
                    {
                        try
                        {
                            File.Delete(ReportRows[i][2].ToString()); // ' My.Computer.FileSystem.DeleteFile(ReportRows(i)(2).ToString)
                        }
                        catch (IOException ex)
                        {
                            // This can only occur if file is already being opened or not exis at he location itself in this case just continue the loop.
                        }

                        ReportRows[i].Delete();
                    }

                    for (int i = 0, loopTo2 = FieldRows.Length - 1; i <= loopTo2; i++)
                        FieldRows[i].Delete();
                    Data.Update_Table("ProgramInformation");
                    Data.Update_Table("ReportStyleInformation");
                    Data.Update_Table("ReportInformation");
                    Data.Update_Table("FieldInformation");
                    Reset_All();     // Refill the program names.
                    GetAllProgramNamesFilled("Program_Id ASC");
                }
                catch (Exception ex)
                {
                    Interaction.MsgBox("Error in deleting the entries " + Constants.vbCrLf + ex.Message);
                }
            }
        }
        // Handling the renaming of the programs.
        private void Rename_Click(object sender, EventArgs e)
        {
            if (dgPrograms.SelectedRows.Count < 1)
                return;
            string NewProgramName = Interaction.InputBox("Enter New Program Name and click 'Ok'", "Excel Module", Conversions.ToString(dgPrograms.SelectedRows[0].Cells[0].Value));
            if ((NewProgramName is object | !string.IsNullOrEmpty(NewProgramName)) & (NewProgramName.Contains(".rpp") | NewProgramName.Contains(".RPP")))
            {
                // start the procedure of renaming the program name.
                string ProgramN = Conversions.ToString(Operators.ConcatenateObject(dgPrograms.SelectedRows[0].Cells[1].Value, dgPrograms.SelectedRows[0].Cells[0].Value));
                int P_ID = Conversions.ToInteger(Data.DataS.Tables["ProgramInformation"].Select("Program_Name=" + "'" + ProgramN + "'")[0][0]);
                var ReportStyleRows = Data.DataS.Tables["ReportStyleInformation"].Select("Program_Id=" + "'" + P_ID + "'");
                int RepS_ID = Conversions.ToInteger(ReportStyleRows[0][0]);
                // Now Rename the file from the harddisk.
                try
                {
                    File.Move(ProgramN, NewProgramName); // ' My.Computer.FileSystem.RenameFile(ProgramN, NewProgramName)
                                                         // Now Rename the database entries.
                    Data.DataS.Tables["ProgramInformation"].Select("Program_Name=" + "'" + ProgramN + "'")[0][1] = Operators.ConcatenateObject(dgPrograms.SelectedRows[0].Cells[1].Value, NewProgramName);
                    Data.Update_Table("ProgramInformation");
                }
                catch (Exception ex)
                {
                    Interaction.MsgBox("Following Error occured in Renaming files" + Constants.vbCrLf + ex.Message);
                    return;
                }
                // Refill the program names.
                GetAllProgramNamesFilled("Program_Id ASC");
            }
        }

        private string[] MonthNames = new string[] { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        // Making visible the panel for giving the new report name.
        private void ShowNewReportGroupBox(object sender, EventArgs e)
        {
            try
            {
                grpMakeNewReport.Visible = true;
                if (!SetReportNameWithDate)
                {
                    txtNewReportName.Text = _ReportBox.Text;
                }
                else
                {
                    string _programName = Conversions.ToString(dgPrograms.SelectedRows[0].Cells[0].Value);
                    string NewreportName = _ReportBox.Text.Substring(0, _ReportBox.SelectedItem.ToString().LastIndexOf(@"\")) + @"\" + _programName.Substring(0, _programName.LastIndexOf(".")) + "_";
                    NewreportName = NewreportName + DateAndTime.Today.Day + MonthNames[DateAndTime.Today.Month - 1] + DateAndTime.Today.Year + ".xls";
                    txtNewReportName.Text = NewreportName;
                }
            }
            catch (Exception ex)
            {
            }
        }
        // Closing the panel to make new report based on old report style.
        private void btnClose_Click(object sender, EventArgs e)
        {
            try
            {
                grpMakeNewReport.Visible = false;
            }
            catch (Exception ex)
            {
            }
        }
        // Setting the new name for the report file.
        private void btnSet_Click(object sender, EventArgs e)
        {
            try
            {
                grpMakeNewReport.Visible = false;
                if (!_ReportBox.Items.Contains(txtNewReportName.Text))
                {
                    _ReportBox.Items.Add(txtNewReportName.Text);
                    _ReportBox.SelectedItem = txtNewReportName.Text;
                }
            }
            catch (Exception ex)
            {
            }
        }
        // Property to return the Program Name 
        public string ReturnProgramName
        {
            get
            {
                return SelectedProgramStr;
                // Dim index As Integer = _programBox.SelectedIndex
                // Return ProgramNameList(index) & _programBox.SelectedItem.ToString
            }

            set
            {
                // _programBox.Text = value
                SelectedProgramStr = value;
            }
        }
        // Property to return Report Name 
        public string ReturnReportName
        {
            get
            {
                return SelectedReportStr;
                // Dim str As String = _ReportBox.Text
                // Return str

            }

            set
            {
                // _ReportBox.SetSelected(_ReportBox.SelectedIndex, False)
                // _ReportBox.Text = value
                SelectedReportStr = value;
            }
        }
        // Property for giving the report Style name.
        public string ReturnReportStyleName
        {
            get
            {
                return SelectedReportStyleStr;
                // Dim str As String = _reportStyleBox.Text
                // Return str
            }

            set
            {
                // _reportStyleBox.Text = value
                SelectedReportStyleStr = value;
            }
        }
        // Should be called When Load is clicked :: It returns the arraylist containing the all titles Values changed by the user. 
        public static ArrayList ReturnTitleValuesWhenLoadCLicked()
        {
            var ar = new ArrayList();
            if (dgTitles is object)
            {
                for (int i = 0, loopTo = dgTitles.Rows.Count - 1; i <= loopTo; i++)
                {
                    if (!ReferenceEquals(dgTitles.Rows[i].Cells[1].Value, DBNull.Value) && dgTitles.Rows[i].Cells[1].Value is object)
                    {
                        ar.Add(dgTitles.Rows[i].Cells[1].Value.ToString());
                    }
                }
            }

            return ar;
        }
        // Property stating whether picture is o be inserted for currently selected report or not.
        public bool IsPictureToBeInserted
        {
            get
            {
                try
                {
                    var ReportInformationRow = Data.GetTable("ReportInformation").Select("R_ReportName=" + "'" + _ReportBox.Text + "'")[0];
                    return Conversions.ToBoolean(ReportInformationRow[16]);
                }
                catch (Exception ex)
                {
                    return false;
                }
            }
        }
        // Property giving the size of the image
        public Size SizeOfPicture
        {
            get
            {
                try
                {
                    var ReportInformationRow = Data.GetTable("ReportInformation").Select("R_ReportName=" + "'" + _ReportBox.Text + "'")[0];
                    return new Size(Conversions.ToInteger(ReportInformationRow[17]), Conversions.ToInteger(ReportInformationRow[18]));
                }
                catch (Exception ex)
                {
                    return new Size(0, 0);
                }
            }
        }

        private bool _IsReportNameWithDate = false;

        public bool SetReportNameWithDate
        {
            get
            {
                return _IsReportNameWithDate;
            }

            set
            {
                _IsReportNameWithDate = value;
            }
        }

        private void ProgarmSort_CheckedChanged(object sender, EventArgs e)
        {
            if (rdbShowDateWise.Checked)
            {
                GetAllProgramNamesFilled("Program_ID ASC");
            }
            else
            {
                GetAllProgramNamesFilled("Program_Name ASC");
            }
        }

        private void txtPgInitials_TextChanged(object sender, EventArgs e)
        {
            GetAllProgramNamesFilled("Program_ID ASC", "Program_Name LIKE '%" + txtPgInitials.Text + "%'");
            txtPgInitials.Focus();
        }

        public bool RenameEnabled
        {
            get
            {
                return btnRename.Visible;
            }

            set
            {
                btnRename.Visible = value;
            }
        }

        public bool DeleteEnabled
        {
            get
            {
                return btnDeleteProgram.Visible;
            }

            set
            {
                btnDeleteProgram.Visible = value;
            }
        }

        public DataView GetAllProgramNames(string sort, string Filter = "")
        {
            DataView D;
            if (string.IsNullOrEmpty(Filter))
            {
                D = new DataView(Data.GetTable("ProgramInformation"), "Program_Id >= 1", sort, DataViewRowState.CurrentRows);
            }
            else
            {
                D = new DataView(Data.GetTable("ProgramInformation"), Filter, sort, DataViewRowState.CurrentRows);
            }

            return D;
        }

        public void UpdateNewStyle()
        {
            Data.DataS.Tables.Remove("ReportStyleInformation");
            Data.DataS.Tables.Remove("ReportInformation");
            Data.FillTable("ReportStyleInformation");
            Data.FillTable("ReportInformation");
        }

        public ArrayList GetAllreportStyles(int ProgID) // 'ByVal SelectedProgramName As String
        {
            return Data.GetAllReportsStyleNames(ProgID); // ' SelectedProgramName)
        }

        public ArrayList GetAllReportNames(string SelectedReportStyle)
        {
            var Ar = new ArrayList();
            // Get the Report Style id from selected report style.
            int Report_Style_ID = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", SelectedReportStyle)[0][0]);

            // Calling the function to populate the Report Names from the Report_Style_ID.
            var DataRN = Data.DataS.Tables["ReportInformation"].Select("Report_Style_Id=" + "'" + Report_Style_ID + "'", "Report_Id DESC");
            for (int i = 0, loopTo = DataRN.Length - 1; i <= loopTo; i++)
            {
                string FullReportName = Conversions.ToString(DataRN[i][2]);
                Ar.Add(FullReportName);
            }

            return Ar;
        }

        public ArrayList GetAllReportTitles(string SelectedReportStyle)
        {
            var Ar = new ArrayList();
            // Get the Report Style id from selected report style.
            int Report_Style_ID = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", SelectedReportStyle)[0][0]);
            // Getting all the rows from the title information containing the title name corresponding to the given Report_Style_ID.
            var dataR = Data.Select_Rows("TitleInformation", "Report_Style_Id", Report_Style_ID);
            for (int i = 0, loopTo = dataR.Length - 1; i <= loopTo; i++)
            {
                var FullReportName = new[] { dataR[i][2], dataR[i][4] };
                Ar.Add(FullReportName);
            }

            return Ar;
        }

        public bool CheckTitlesChange(string SeletedReportStyle, string Colvalue, int currentIndex)
        {
            int Report_Style_ID = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", SeletedReportStyle)[0][0]);
            // Getting all the rows from the title information containing the title name corresponding to the given Report_Style_ID.
            var dataR = Data.Select_Rows("TitleInformation", "Report_Style_Id", Report_Style_ID);
            if ((Colvalue ?? "") != (Conversions.ToString(dataR[currentIndex][4]) ?? ""))
            {
                return true;
            }

            return default;
        }

        public bool InsertNewReport(string SelectedProgramName, string SelectedReportStyle, string NewReportName, bool IsTitleChnanged)
        {
            try
            {
                Data.DataS.Clear();
                Data.FillTableRange(DatabaseConstants.TableNames);
                bool ReportExists = false;
                var PreviousReportRow = Data.Select_Rows("ReportStyleInformation", "ReportStyleName", SelectedReportStyle)[0];
                int ReportStyle_ID = Conversions.ToInteger(PreviousReportRow[0]);
                var DataRN = Data.Select_Rows("ReportInformation", "Report_Style_Id", ReportStyle_ID);
                // If DataRN(0).Item(32) Then
                // 'Return False
                // End If
                for (int i = 0, loopTo = DataRN.Length - 1; i <= loopTo; i++)
                {
                    if ((NewReportName ?? "") == (Conversions.ToString(DataRN[i][2]) ?? ""))
                    {
                        ReportExists = true;
                        break;
                    }
                }

                if (IsTitleChnanged & ReportExists)
                {
                    MessageBox.Show("You have changed the title values.Please give another name for the report.", "ExcelModule", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    return false;
                }
                // now check the boolean flag and proceed 
                else if (!ReportExists)
                {
                    // Create the program 
                    // Select the program template and copy and paste it to the report location.
                    var ReportInformationRow = Data.GetTable("ReportInformation").Select("Report_Style_Id=" + "'" + ReportStyle_ID + "'")[0];
                    string Template_location = Conversions.ToString(PreviousReportRow[4]);
                CopyFile:
                    try
                    {
                        if (File.Exists(NewReportName))
                        {
                            // File already exists , so showing a message box to give option to overwrite
                            var M = MessageBox.Show("The file already exists in the report folder." + "Click 'Ok' to overwrite or 'Cancel' to give another name to the Report.", "Excel Module", MessageBoxButtons.OKCancel, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1);
                            if (M == DialogResult.OK) // 'Windows.Forms.
                            {
                                File.Copy(Template_location, NewReportName, true); // '  My.Computer.FileSystem.CopyFile(Template_location, NewReportName, True)
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            File.Copy(Template_location, NewReportName, true);
                        } // 'My.Computer.FileSystem.CopyFile(Template_location, NewReportName, True)
                    }
                    // My.Computer.FileSystem.CopyFile(Template_location, _ReportBox.Text)
                    catch (Exception ex)
                    {
                        var M = MessageBox.Show("Caught following Exception in handling this file." + Constants.vbCrLf + ex.ToString(), "Excel Module", MessageBoxButtons.RetryCancel, MessageBoxIcon.Asterisk);
                        if (M == DialogResult.Retry) // 'Windows.Forms.
                        {
                            goto CopyFile;
                        }
                    }
                    // Update the database about this new report name.
                    var NewReportRow = Data.GetTable("ReportInformation").NewRow();
                    if (Data.DataS.Tables["ReportInformation"].Rows.Count != 0)
                    {
                        NewReportRow[0] = Data.ReturnsID("select max([Report_Id]) from ReportInformation") + 1;
                    }
                    else
                    {
                        return false;
                    }
                    
                    string reportnamecsv = "Nothing";
                    if (Conversions.ToBoolean(ReportInformationRow[38]))
                    {
                        int ind = NewReportName.LastIndexOf(".");
                        reportnamecsv = NewReportName.Substring(0, ind) + ".csv";
                        var FieldRN = Data.Select_Rows("FieldInformation", "Report_Style_Id", ReportStyle_ID);
                        var sWriter = new StreamWriter(reportnamecsv, true); // ' My.Computer.FileSystem.OpenTextFileWriter(reportnamecsv, True)
                        string headerline = "S.No.";
                        for (int i = 0, loopTo1 = FieldRN.Count() - 1; i <= loopTo1; i++)
                            headerline += "," + FieldRN[i][3].ToString();
                        sWriter.WriteLine(headerline);
                        sWriter.Close();
                    }

                    NewReportRow[1] = ReportStyle_ID;
                    NewReportRow[2] = NewReportName;
                    for (int i = 3; i <= 25; i++)
                        NewReportRow[i] = ReportInformationRow[i];
                    if (Conversions.ToBoolean(ReportInformationRow[4].Equals("Column")))
                    {
                        if (Conversions.ToBoolean(Operators.OrObject(Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], 1, false), Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], 2, false))))
                        {
                            NewReportRow[26] = Operators.AddObject(ReportInformationRow[21], 2);
                        }
                        else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], 3, false)))
                        {
                            NewReportRow[26] = Operators.AddObject(ReportInformationRow[21], 4);
                        }
                        else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], -1, false)))
                        {
                            NewReportRow[26] = ReportInformationRow[21];
                        }
                        else
                        {
                            NewReportRow[26] = Operators.AddObject(ReportInformationRow[21], 1);
                        }

                        if (Conversions.ToBoolean(ReportInformationRow[30]))
                        {
                            NewReportRow[26] = (string)NewReportRow[26] + (string)NewReportRow[26];
                        }

                        if (Conversions.ToBoolean(NewReportRow[16]))
                        {
                            NewReportRow[26] = (string)NewReportRow[26] + (string)NewReportRow[26];
                        }

                        NewReportRow[27] = ReportInformationRow[22];
                    }
                    else
                    {
                        var f1 = new Form1();
                        if (Conversions.ToBoolean(Operators.OrObject(Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], 1, false), Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], 2, false))))
                        {
                            NewReportRow[27] = f1.GetColumnName(f1.GetColIndex(Conversions.ToString(ReportInformationRow[22])) + 2);
                        }
                        else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], 3, false)))
                        {
                            NewReportRow[27] = f1.GetColumnName(f1.GetColIndex(Conversions.ToString(ReportInformationRow[22])) + 4);
                        }
                        else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(ReportInformationRow[6], -1, false)))
                        {
                            NewReportRow[27] = f1.GetColumnName(f1.GetColIndex(Conversions.ToString(ReportInformationRow[22])));
                        }
                        else
                        {
                            NewReportRow[27] = f1.GetColumnName(f1.GetColIndex(Conversions.ToString(ReportInformationRow[22])) + 1);
                        }

                        if (Conversions.ToBoolean(ReportInformationRow[30]))
                        {
                            NewReportRow[27] = f1.GetColumnName(f1.GetColIndex(Conversions.ToString(NewReportRow[27])) + 1);
                        }

                        if (Conversions.ToBoolean(NewReportRow[16]))
                        {
                            NewReportRow[27] = f1.GetColumnName(f1.GetColIndex(Conversions.ToString(NewReportRow[27])) + 1);
                        }

                        NewReportRow[26] = ReportInformationRow[21];
                        f1.Dispose();
                        f1 = null;
                    }

                    NewReportRow[28] = ReportInformationRow[9];
                    NewReportRow[29] = ReportInformationRow[13];
                    NewReportRow[30] = ReportInformationRow[30];
                    NewReportRow[31] = ReportInformationRow[31];
                    NewReportRow[32] = ReportInformationRow[32];
                    NewReportRow[33] = ReportInformationRow[33];
                    NewReportRow[34] = ReportInformationRow[34];
                    NewReportRow[35] = ReportInformationRow[35];
                    NewReportRow[36] = ReportInformationRow[36];
                    NewReportRow[37] = ReportInformationRow[37];
                    NewReportRow[38] = ReportInformationRow[38];
                    NewReportRow[39] = reportnamecsv;
                    NewReportRow[40] = ReportInformationRow[40];
                    NewReportRow[41] = ReportInformationRow[41];
                    Data.GetTable("ReportInformation").Rows.Add(NewReportRow);
                    Data.Update_Table("ReportInformation");
                    // 'If we have multiprogram Report, check for all entries in Multiprogram report and make new entries for each of those....
                    if (Conversions.ToBoolean(ReportInformationRow[32]))
                    {
                        // Data.FillTable("MultiProgramReport")
                        var ProgramList = Data.GetTable("MultiProgramReport").Select("Report_Id = " + ReportInformationRow[0].ToString());
                        for (int ii = 0, loopTo2 = ProgramList.Length - 1; ii <= loopTo2; ii++)
                        {
                            var NewMultiEntry = Data.GetTable("MultiProgramReport").NewRow();
                            NewMultiEntry[0] = NewReportRow[0];
                            // 'Copy all the entries as is in the first report generated... 
                            for (int jj = 1, loopTo3 = NewMultiEntry.ItemArray.Length - 1; jj <= loopTo3; jj++)
                                NewMultiEntry[jj] = ProgramList[ii][jj];
                            // 'Now replace the last column and last row used entries
                            NewMultiEntry[7] = NewMultiEntry[5];
                            NewMultiEntry[8] = NewMultiEntry[6];
                            // If ReportInformationRow.Item(4).Contains("Column") Then

                            // Else
                            // 'If NewMultiEntry.Item(5) =
                            // NewMultiEntry.Item(7) = NewMultiEntry.Item(5) - 1 Then
                            // End If
                            // 'Add the row to the table...
                            Data.GetTable("MultiProgramReport").Rows.Add(NewMultiEntry);
                        }

                        Data.Update_Table("MultiProgramReport");
                    }
                }

                return true;
            }
            catch (Exception ex)
            {
                Interaction.MsgBox(ex.Message);
                return false;
            }
        }

        public DataSet GetDataSet()
        {
            try
            {
                if (Data is null)
                {
                    Data = new xl_DB();
                    Data.DataS = Data.FillTableRange(DatabaseConstants.TableNames);
                }

                return Data.DataS;
            }
            catch (Exception ex)
            {
                return null;
            }
        }
    }
}