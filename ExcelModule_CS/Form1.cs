using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using Microsoft.Office.Interop.Excel;
using Microsoft.VisualBasic;
using Microsoft.VisualBasic.CompilerServices;
using NPOI.SS.UserModel;

namespace rrm
{
    public partial class Form1
    {
        public Form1()
        {
            ColumnAfterWhich_PaginationShouldBeDone = ColumnMax;
            RowAfterWhich_PaginationShouldBeDone = RowMax;
            Invokingfunction = new Invokegridview(FillGridViews);

            // This call is required by the designer.
            InitializeComponent();
            if (Environment.Is64BitOperatingSystem)
            {
                DatabaseConstants.ConnectionString = "Provider=Microsoft.Ace.Oledb.12.0;Data Source=" + DatabaseConstants.AppDataFolderPath + @"\Database\ExcelDoc.mdb;Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl";
            }
            else
            {
                DatabaseConstants.ConnectionString = "Provider=Microsoft.Jet.Oledb.4.0;Data Source=" + DatabaseConstants.AppDataFolderPath + @"\Database\ExcelDoc.mdb;Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl";
            }

            __rowWise.Name = "_rowWise";
            __columnWise.Name = "_columnWise";
            __landScapeRadioButton.Name = "_landScapeRadioButton";
            __portraitRadioButton.Name = "_portraitRadioButton";
            _ChkShwFieldNum.Name = "ChkShwFieldNum";
            _Btn_TolerancePanelClose.Name = "Btn_TolerancePanelClose";
            __allowedRange.Name = "_allowedRange";
            __nominalMinMax.Name = "_nominalMinMax";
            __nomialPlusMinus.Name = "_nomialPlusMinus";
            __conditionalFormatting1.Name = "_conditionalFormatting1";
            __measurementListView.Name = "_measurementListView";
            _BtnMoveUp.Name = "BtnMoveUp";
            _BtnMoveDown.Name = "BtnMoveDown";
            _Button1.Name = "Button1";
            _addNewTitle.Name = "addNewTitle";
            _btnSetList.Name = "btnSetList";
            __extraInformationDataView.Name = "_extraInformationDataView";
            _filePassword.Name = "filePassword";
            _CheckBox1.Name = "CheckBox1";
            _UpdateRcadPic.Name = "UpdateRcadPic";
            _ProgramInNewSheet.Name = "ProgramInNewSheet";
            _DetailSelect.Name = "DetailSelect";
            _btnUpdatePicDetails.Name = "btnUpdatePicDetails";
            _chkInsertPicture.Name = "chkInsertPicture";
            _btnUpdateRange.Name = "btnUpdateRange";
            _ShowToolBar.Name = "ShowToolBar";
            _showExcelMenu.Name = "showExcelMenu";
            __saveButton.Name = "_saveButton";
            _blackWhite.Name = "blackWhite";
            _colorPrinter.Name = "colorPrinter";
            __cancelButton.Name = "_cancelButton";
            _Rawdataascol.Name = "Rawdataascol";
            // Add any initialization after the InitializeComponent() call.

        }

        private int HCMno = 0;                         // vinod.jakhar
        private bool IsHCM = false;                     // for DelphiTVS and HOB measurements
        private bool IsDTVSM = false;                   // 15/04/14
        private bool IsGrigImagecaptureM = false;       // we have to know IsGrigImagecaptureMeasurement Report..?
        private int GridImgCaptureRows = 0;            // For IsGrigImagecapture Rows
        private int GridImgCaptureCols = 0;            // For IsGrigImagecapture Cols
        private Type AxFramerControl;
        private object _dsoFramer;
        private ToolBar ToolBar1;
        private MenuBar MenuBar1;

        private void Form1_Load(object sender, EventArgs e)           // The Load Event of the form 
        {
            Location = new System.Drawing.Point(10, 20);                                                    // Initial Positioning of the Form 
            ShowInTaskbar = false;                                                                          // Dont show the form in Taskbar
            ContextMenuStrip FieldContextMenu = new ContextMenuStrip(), TitleContextMenu = new ContextMenuStrip();                                    // Assign the two context menus to the grid views.
            FieldContextMenu.Name = "Field";
            TitleContextMenu.Name = "Title";
            _measurementListView.ContextMenuStrip = FieldContextMenu;
            _extraInformationDataView.ContextMenuStrip = TitleContextMenu;
            FieldContextMenu.Items.Add("Convert to Title");                 // Adding the context MenuItems 
            FieldContextMenu.Items.Add("Insert");
            FieldContextMenu.Items.Add("Delete");
            TitleContextMenu.Items.Add("Convert to Field");
            TitleContextMenu.Items.Add("Insert");
            TitleContextMenu.Items.Add("Delete");
            FieldContextMenu.ItemClicked += Handle_ContextMenuClick;      // Handling the clicks of teh context menu items.
            TitleContextMenu.ItemClicked += Handle_ContextMenuClick;
            // Try
            string filePath = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles) + @"\Rapid-I 5.0";
            if (!Directory.Exists(filePath))
            {
                filePath = ((int)Environment.SpecialFolder.ProgramFilesX86).ToString() + @"\Rapid-I 5.0";
            }
            // Dim dsoFramerAssembly As Assembly = Assembly.LoadFrom(filePath & "\AxInterop.DSOFramer.dll")
            // If dsoFramerAssembly IsNot Nothing Then
            // AxFramerControl = dsoFramerAssembly.GetType("AxDSOFramer.AxFramerControl")
            // _dsoFramer = Activator.CreateInstance(AxFramerControl)
            // End If
            // Catch ex As Exception
            // 'MsgBox("Could Not Load DsoFramer")
            // lblNoDSOFramer.Visible = True
            // End Try

        }

        public void RefreshDatabase()
        {
            try
            {
                if (Data is null)
                    Data = new xl_DB();
                Data.FillTable("MasterTemplateSettings");
                var dT = Data.GetTable("MasterTemplateSettings");
                if (!dT.Columns.Contains("PartNameLabelLocation_Row"))
                {
                    Data.AddColumnToTable("MasterTemplateSettings", "PartNameLabelLocation_Row", typeof(string));
                    Data.AddColumnToTable("MasterTemplateSettings", "PartNameLabelLocation_Col", typeof(string));
                    Data.AddColumnToTable("MasterTemplateSettings", "PartName_Location_Row", typeof(string));
                    Data.AddColumnToTable("MasterTemplateSettings", "PartName_Location_Col", typeof(string));
                }

                Data.Update_Table("MasterTemplateSettings");
                Data.FillTable("MasterTemplateSettings");
            }
            catch (Exception ex)
            {
            }
        }
        // ______________________________________________________________Function to get the selected cell information _____________________________________________________________
        private void GetSelectedCellInformation(object sender, EventArgs e)
        {
            
            //try
            //{
            //    var SelectedRange = _dsoFramer.ActiveDocument.Application.Selection;
            //    _extraInformationDataView.SelectedRows[0].Cells[5].Value = GetColumnName(Conversions.ToInteger(SelectedRange.Column));
            //    _extraInformationDataView.SelectedRows[0].Cells[6].Value = SelectedRange.Row;
            //}
            //catch (IndexOutOfRangeException ex)
            //{
            //}     // it will catch exceptions only if there is no selected cell,so in that cases just exit the sub gracefuuly.
            //catch (Exception ex)
            //{
            //}      // This will only occur if there is no selected cell in the DSO framer (Due to a bug in DSO framer sometimes it gives the selected cell as nothing even when we have selected a particuler cell !!)
        }
        // _____________________________________________________________Function to handle the context menu for the Field Values.___________________________________________________
        private void Handle_ContextMenuClick(object sender, ToolStripItemClickedEventArgs e)
        {
            try
            {
                switch (((ContextMenuStrip)sender).Name ?? "")
                {
                    case "Field":
                        {
                            switch (e.ClickedItem.Text ?? "")
                            {
                                case "Convert to Title":
                                    {
                                        var NewTitleRow = addNewTitle_Click(null, null);
                                        NewTitleRow[2] = _measurementListView.SelectedRows[0].Cells[3].Value; // The field Name
                                        NewTitleRow[3] = _measurementListView.SelectedRows[0].Cells[4].Value; // The field Type
                                        NewTitleRow[4] = _measurementListView.SelectedRows[0].Cells[5].Value; // The field Value
                                        _measurementListView.Rows.Remove(_measurementListView.SelectedRows[0]);
                                        break;
                                    }

                                case "Insert":
                                    {
                                        _measurementListView_RowHeaderMouseDoubleClick(null, null);
                                        break;
                                    }

                                case "Delete":
                                    {
                                        if (_measurementListView.SelectedRows.Count != 0)
                                        {
                                            if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(_measurementListView.SelectedRows[0].Cells[4].Value, "Pass/Fail", false)))
                                            {
                                                AddPass = true;
                                                _measurementListView.Columns[10].Visible = false;
                                            }

                                            _measurementListView.Rows.Remove(_measurementListView.SelectedRows[0]);
                                        }

                                        RefreshFieldNumbers();
                                        break;
                                    }
                            }

                            break;
                        }

                    case "Title":
                        {
                            switch (e.ClickedItem.Text ?? "")
                            {
                                case "Convert to Field":
                                    {
                                        if (_measurementListView.SelectedRows.Count == 0)
                                            _measurementListView.Rows[_measurementListView.Rows.Count - 1].Selected = true;
                                        var NewFieldRow = _measurementListView_RowHeaderMouseDoubleClick(null, null);
                                        NewFieldRow[2] = _extraInformationDataView.SelectedRows[0].Cells[3].Value;
                                        NewFieldRow[5] = _extraInformationDataView.SelectedRows[0].Cells[4].Value;
                                        NewFieldRow[6] = _extraInformationDataView.SelectedRows[0].Cells[5].Value;
                                        _extraInformationDataView.Rows.Remove(_extraInformationDataView.SelectedRows[0]);
                                        RefreshFieldNumbers();
                                        break;
                                    }

                                case "Insert":
                                    {
                                        addNewTitle_Click(null, null);
                                        break;
                                    }

                                case "Delete":
                                    {
                                        if (_extraInformationDataView.SelectedRows.Count != 0)
                                        {
                                            _extraInformationDataView.Rows.Remove(_extraInformationDataView.SelectedRows[0]);
                                            No_of_New_TitleRows -= 1;
                                        }

                                        break;
                                    }
                            }

                            break;
                        }
                }
            }
            catch (IndexOutOfRangeException ex)
            {
            }            // This can only occur if there is no selected row ,then just escape the sub gracefully.
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E001", ex);
            }
        }
        // _______________________Function to have the combobox into the Title datagridview ,if title type is Predefined.________________________________________________________________
        private void Handle_PredefinedList(object sender, DataGridViewCellEventArgs e)
        {
            try
            {
                if (e.ColumnIndex == 4 & _extraInformationDataView.CurrentRow.Cells[3].Value.ToString().ToLower() == "predefined")
                {
                    var C = new DataGridViewComboBoxCell();
                    for (int i = 0, loopTo = PreDefinedList.Count - 1; i <= loopTo; i++)
                        C.Items.Add(PreDefinedList[i]);
                    _extraInformationDataView.CurrentRow.Cells[4] = C;
                }
                else if (_extraInformationDataView.CurrentRow.Cells[3].Value.ToString().ToLower() != "predefined")
                {
                    _extraInformationDataView.CurrentRow.Cells[4] = new DataGridViewTextBoxCell();
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E003", ex);
            }
        }
        // ____________________________________This Function assigns all the predefined items into the richtextbox.(Called in ResetAll function)_________________________________________________
        private void GetPredefinedListItems()
        {
            try
            {
                if (Data is null)
                {
                    Data = new xl_DB();
                    dataS = Data.FillTableRange(DatabaseConstants.TableNames);
                }

                Data.DataA = new OleDbDataAdapter("Select * From PreExistingFieldValue", DatabaseConstants.ConnectionString);
                Data.DataA.Fill(Data.DataS, "PreExistingFieldValue");
                PreDefinedList.Clear();
                for (int i = 0, loopTo = Data.DataS.Tables["PreExistingFieldValue"].Rows.Count - 1; i <= loopTo; i++)
                {
                    PreDefinedList.Add(Conversions.ToString(Data.DataS.Tables["PreExistingFieldValue"].Rows[i][1]));
                    if (i != Data.DataS.Tables["PreExistingFieldValue"].Rows.Count - 1)
                    {
                        txtPredefinedList.Text += PreDefinedList[i] + Microsoft.VisualBasic.Constants.vbCrLf;
                    }
                    else
                    {
                        txtPredefinedList.Text += PreDefinedList[i];
                    }
                }

                Data.DataS.Tables.Remove("PreExistingFieldValue");
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E004", ex);
            }
        }
        // _______________________________________________Handling the Radiobuttons click events for Changing the Report Type._________________________________________________________-
        protected void HandlingReportStylesForExcel(object sender, EventArgs e)
        {
            // 'If Not (CType(sender, RadioButton).Checked) Then Exit Sub
            // 'If (Not _landScapeRadioButton.Checked And Not _portraitRadioButton.Checked) Or (_landScapeRadioButton.Checked And _portraitRadioButton.Checked) Then Exit Sub
            if (string.IsNullOrEmpty(((RadioButton)sender).Text))
                return;
            try
            {
                if (_dsoFramer is object)
                    xL.workbook.Close(); // Closing the DSO Framer's already opened document before Opening New document 
                                        // -------------------------------------------Handling the User Interaction for changing the template styles----------------------------------------------------------
                IsColumnSelected = _columnWise.Checked;
                IsLandScapeSelected = _landScapeRadioButton.Checked;
                DefaultReportSelection(IsColumnSelected, IsLandScapeSelected);
            }
            catch (Exception ex)
            {
                // 'MsgBox("E005 :  " & ex.Message & vbCrLf & ex.StackTrace)
                OnErrorHappen?.Invoke("E005", ex);
            }
        }

        protected void PrinterHandling(object sender, EventArgs e)
        {
            try
            {
                if (colorPrinter.Checked == true)
                {
                    PrinterState = "Color";
                }
                else
                {
                    PrinterState = "Black/White";
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E006", ex);
            }
        }
        // ___________________________________________________________________________________________________________________
        private void CopyNPasteTemplates()
        {
            if (Directory.Exists(DatabaseConstants.TemporaryFilePath) == false)            // Checking thr directory existence ....Creating it only when it is not present
            {
                Directory.CreateDirectory(DatabaseConstants.TemporaryFilePath);
            }
            // ------------------------------------------Copying the required templates files to the Temp folder -------------------------------------------------------------------
            if (IsHCM)
            {
                File.Copy(DatabaseConstants.TemplatePath + @"\HobCheckerTemplate.xls", DatabaseConstants.TemporaryFilePath + @"\HobCheckerTemplate.xls", true);
            }

            if (IsDTVSM)
            {
                File.Copy(DatabaseConstants.TemplatePath + @"\DelphiTVSTemplate.xls", DatabaseConstants.TemporaryFilePath + @"\DelphiTVSTemplate.xls", true);
            }
            // If txtCustomTemplateLocation.Text = "" Then
            File.Copy(DatabaseConstants.TemplatePath + @"\Column_WiseWithLandscape.xls", DatabaseConstants.TemporaryFilePath + @"\Column_WiseWithLandscape.xls", true);
            File.Copy(DatabaseConstants.TemplatePath + @"\Column_WiseWithPortrait.xls", DatabaseConstants.TemporaryFilePath + @"\Column_WiseWithPortrait.xls", true);
            File.Copy(DatabaseConstants.TemplatePath + @"\Row_WiseWithLandscape.xls", DatabaseConstants.TemporaryFilePath + @"\Row_WiseWithLandscape.xls", true);
            File.Copy(DatabaseConstants.TemplatePath + @"\Row_WiseWithPortrait.xls", DatabaseConstants.TemporaryFilePath + @"\Row_WiseWithPortrait.xls", true);
            // Else
            // Dim tfile() As String = txtCustomTemplateLocation.Text.Split("\")
            // IO.File.Copy(txtCustomTemplateLocation.Text, TemporaryFilePath & "\" & tfile(tfile.Length - 1), True)
            // End If

        }
        // _____________________________________________________Method to Delete the Temporary directory made by the application.__________________________________________________________
        private void DeleteDirectory()
        {
            try
            {
                if (Directory.Exists(DatabaseConstants.TemporaryFilePath) == true)
                {
                    Directory.Delete(DatabaseConstants.TemporaryFilePath, true); // ' FileIO.DeleteDirectoryOption.DeleteAllContents)
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E007", ex);
            }
        }
        // __________________________________________________Filling The Measurement list and Title Information DatagridView._________________________________________________________
        protected void FillGridViews()
        {
            try
            {
                ChkShwFieldNum.Checked = false;
                AddPass = true; // check if its being used somewhere.
                if (_measurementListView.Rows.Count > 0)
                    _measurementListView.DataSource = null; // Clearing all the rows and columns from the datagridviews.
                if (_extraInformationDataView.Rows.Count > 0)
                    _extraInformationDataView.DataSource = null; // Clearing all the rows and columns from the datagridviews.
                                                                 // Filling the all required tables 
                                                                 // If Data Is Nothing Then
                Data = new xl_DB();
                dataS = Data.FillTableRange(DatabaseConstants.TableNames);
                // End If

                // -----------------clearing these two tables ,as we need only schema information for these tables ----------------------------------------------
                dataS.Tables["TitleInformation"].Rows.Clear();
                dataS.Tables["FieldInformation"].Rows.Clear();
                _measurementListView.DataSource = dataS.Tables["FieldInformation"];      // Assigning the table of dataset as datasource to the gridview 
                var HiddenColumns = new int[] { 0, 1, 2, 6, 7, 8, 9, 10, 11, 12, 13 };
                for (int i = 0, loopTo = HiddenColumns.Length - 1; i <= loopTo; i++)
                    _measurementListView.Columns[HiddenColumns[i]].Visible = false;                 // Hiding all unrequired columns from user of table FieldInformation.
                _measurementListView.Columns[7].ReadOnly = true;
                _measurementListView.Columns[8].ReadOnly = true;
                _measurementListView.Columns[9].ReadOnly = true;
                _measurementListView.Columns[2].Width = 25;
                _measurementListView.Columns[3].Width = 60;
                _measurementListView.Columns[4].Width = 60;
                _measurementListView.Columns[5].Width = 60;
                int a;
                // ------------------------------------populating  the measurement list given as arraylist to the Gridview --------------------------------------------------------
                for (int i = 0, loopTo1 = MeasurementStringList.Count - 1; i <= loopTo1; i++)
                {
                    a = 0;
                    var DataR = dataS.Tables["FieldInformation"].NewRow();
                    DataR[3] = MeasurementStringList[i][a];
                    a += 1;
                    DataR[4] = MeasurementStringList[i][a];
                    a += 1;
                    DataR[5] = MeasurementStringList[i][a];
                    a += 1;
                    DataR[6] = i;
                    string TempStr = MeasurementStringList[i][a].ToString();
                    string TempCmpStr = "0° 00' 00''";
                    if (TempStr.Contains(Conversions.ToString('°')))
                    {
                        if ((MeasurementStringList[i][a + 2].ToString() ?? "") != (TempCmpStr ?? "") | (MeasurementStringList[i][a + 1].ToString() ?? "") != (TempCmpStr ?? ""))
                        {
                            DataR[7] = MeasurementStringList[i][a];
                            a += 1;
                            DataR[8] = MeasurementStringList[i][a];
                            a += 1;
                            DataR[9] = MeasurementStringList[i][a];
                            a += 1;
                        }
                        else
                        {
                            a = a + 3;
                        }
                    }
                    else if (Conversions.ToBoolean(Operators.OrObject(Operators.ConditionalCompareObjectNotEqual(MeasurementStringList[i][a + 2], 0, false), Operators.ConditionalCompareObjectNotEqual(MeasurementStringList[i][a + 1], 0, false))))
                    {
                        DataR[7] = MeasurementStringList[i][a];
                        a += 1;
                        DataR[8] = MeasurementStringList[i][a];
                        a += 1;
                        DataR[9] = MeasurementStringList[i][a];
                        a += 1;
                    }
                    else
                    {
                        a = a + 3;
                    }

                    DataR[10] = false;
                    DataR[13] = MeasurementStringList[i][a];
                    a += 1;
                    dataS.Tables["FieldInformation"].Rows.Add(DataR);
                }

                Fieldrows = null;
                Fieldrows = Data.DataS.Tables["FieldInformation"].Select();
                _measurementListView.Columns[7].Width = 50;
                _measurementListView.Columns[8].Width = 50;
                _measurementListView.Columns[9].Width = 50;
                // -----------------------------Assigning the '_extraInformationDataView' datasource as Title Information data table.----------------------------------------------
                _extraInformationDataView.DataSource = dataS.Tables["TitleInformation"];
                _extraInformationDataView.Columns[0].Visible = false;
                _extraInformationDataView.Columns[1].Visible = false;
                _extraInformationDataView.Columns[5].Width = 50;
                _extraInformationDataView.Columns[6].Width = 50;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E008", ex);
            }
        }
        // __________________________________________________________________Function to Reset The form to its Original way._____________________________________________________________-
        public bool Reset_All(bool WriteAgain = false, bool ShowTemplate = false)
        {
            try
            {
                int R_index = 0;
                passStatus = PassFail.none;
                if (!NewStyle)
                {
                    if (!IsGrigImagecaptureM) // we dont need date time in Grid Program Report 
                    {
                        ArrayList Datearray = new ArrayList(), TimeArray = new ArrayList();
                        Datearray.Add("Date");
                        Datearray.Add("Date");
                        Datearray.Add(string.Format("{0:dd-MM-yyyy}", DateTime.Today));
                        Datearray.Add(0);
                        Datearray.Add(0);
                        Datearray.Add(0);
                        Datearray.Add("D1");
                        TimeArray.Add("Time");
                        TimeArray.Add("Time");
                        TimeArray.Add(DateTime.Now.Hour + ":" + DateTime.Now.Minute + ":" + DateTime.Now.Second);
                        TimeArray.Add(0);
                        TimeArray.Add(0);
                        TimeArray.Add(0);
                        TimeArray.Add("T1");
                        if (!IsHCM)
                        {
                            if (MeasurementStringList.Count > 0)
                            {
                                if (MeasurementStringList[0].GetType().ToString().Contains("ArrayList"))
                                {
                                    var ii = default(int);
                                    while (ii < MeasurementStringList.Count)
                                    {
                                        var tempAL = MeasurementStringList[0];
                                        if (tempAL[0].ToString() == "Date")
                                        {
                                            MeasurementStringList.Remove(tempAL);
                                            continue;
                                        }

                                        if (tempAL[1].ToString() == "Time")
                                        {
                                            MeasurementStringList.Remove(tempAL);
                                            continue;
                                        }

                                        ii += 1;
                                    }
                                }
                            }

                            MeasurementStringList.Insert(0, Datearray);
                            MeasurementStringList.Insert(1, TimeArray);
                        }
                    }
                }

                if (!WriteAgain)        // The Write Again Signifies that the ReportFile is already created and only Writing on it has to be done.
                {
                    SelectedSheet = "Report";   // make changes to read it from excel sheet_____________________________________________________________
                    if (!IsHCM)
                        SelectedDataSheet = "Data";
                    ShowDetail = false;
                    InsertImage = false;
                    ReportNameCsv = "";
                    if (!NewStyle)
                    {
                        CreateCsv = false;
                        // ToleranceFormat = 0
                    }

                    try
                    {
                        CopyNPasteTemplates();     // To copy the required templates files to a temporary folder temp 
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("The Required 'Templates' could not be found or are in use! Cannot Proceed Further." + Microsoft.VisualBasic.Constants.vbCrLf + Microsoft.VisualBasic.Constants.vbCrLf + ex.Message, "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                        return default;    // If exception come, Warn the user and exit the Sub
                        return false;
                    }

                    if (!UseExistingReport)
                    {
                        MultiProgramReport = false;
                        AllProgramInSingleReport = false;
                        DefaultReportSelection(IsColumnSelected, IsLandScapeSelected);
                    }
                    // If Not IsDTVSM Then
                    // CalculateMargins(ReportType)
                    // End If
                    else if (!AllProgramInSingleReport)
                    {
                        CalculateMargins(ReportType);
                    }

                    if (IsDTVSM)     // vinod jakhar 16/04/14
                    {
                    }
                    // FillGridViews()               'Filling the measurement datagridView and making only Those Fields visible that are meant to be Visible for the user
                    else if (IsGrigImagecaptureM)     // we dont need to fill grid .
                    {
                    }
                    // FillGridViews()
                    else
                    {
                        FillGridViews();
                    }

                    if (ShowTemplate)
                    {
                        var GotDSOFramer = default(bool);
                        if (UseExistingReport)
                        {
                            ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\Column_WiseWithLandscape.xls";
                        }

                        GetPredefinedListItems();                     // Get the all predefined values into the txtPredefined.
                        int P_index = ProgramName.LastIndexOf(@"\");              // Displaying the Program Name in proper format into the textbox 
                        if (ProgramName is null | string.IsNullOrEmpty(ProgramName))
                            goto ProgramNameNotExists; // Checking the Program Name given form the database.
                        _programNamePathBox.Text = ProgramName.Substring(0, P_index);
                        _programNameBox.Text = ProgramName.Substring(P_index + 1, ProgramName.Length - P_index - 1);
                        ProgramName_Exists = Data.Find_Record("ProgramInformation", ProgramName, 1);
                        R_index = ReportName.LastIndexOf(@"\");               // Displaying the report name in proper format  into the textbox 
                        _reportPathNameBox.Text = ReportName.Substring(0, R_index);
                        if (ProgramName_Exists)
                        {
                            var Ar = new ArrayList();
                            Ar = Data.GetAllReportsStyleNames(Conversions.ToInteger(ProgramName));
                            for (int i = 0, loopTo = Ar.Count - 1; i <= loopTo; i++)
                                _reportStyleNameBox.Items.Add(Ar[i]);
                        }

                        try
                        {
                            xL.Open_File(ReportTemplateName);
                            GotDSOFramer = true;
                        }
                        catch (Exception ex)
                        {
                            // MessageBox.Show("Could not open the excel file in this module and we hence have to terminate this report request." & vbCrLf & vbCrLf & ex.Message, "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Stop)      'Warn the User and Exit the sub 
                            // Return False
                            GotDSOFramer = false;
                        }

                    ProgramNameNotExists:
                        ;
                        try
                        {
                            if (_sheetBox.Items.Count >= 1)
                                _sheetBox.Items.Clear(); // Assigning the names of the Report Sheets to the combo box 
                            if (GotDSOFramer)
                            {
                                for (int i = 1, loopTo1 = Conversions.ToInteger(xL.workbook.NumberOfSheets); i <= loopTo1; i++)
                                    _sheetBox.Items.Add(xL.workbook.GetNameAt(i));
                            }
                            else
                            {
                                xL.InitializeExcelApplication(ReportTemplateName, "");
                                xL.Open_File(ReportTemplateName);
                                for (int i = 1, loopTo2 = Conversions.ToInteger(xL.workbook.NumberOfSheets); i <= loopTo2; i++)
                                    _sheetBox.Items.Add(xL.workbook.GetNameAt(i));
                                xL.Close();
                                xL.ExitExcel();
                            }
                        }
                        catch (Exception ex)
                        {
                        }

                        _sheetBox.SelectedItem = _sheetBox.Items[0];
                        if (!NewStyle)
                        {
                            _reportNameBox.ReadOnly = true;
                            _reportNameBox.Text = ReportName.Substring(R_index + 1, ReportName.Length - R_index - 1);
                            // Assigning a name to the report style for the program choosen.
                            _reportStyleNameBox.Enabled = false;
                            _reportStyleNameBox.Text = ReportStyleName;
                        }
                        else
                        {
                            _reportNameBox.ReadOnly = false;
                            int Reportcount = 1;
                            int Program_Id = Conversions.ToInteger(Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName)[0][0]);
                            var Stylerows = Data.Select_Rows("ReportStyleInformation", "Program_Id", Program_Id);
                            _reportStyleNameBox.Text = _programNameBox.Text.Split('.')[0] + "_ReportStyle" + (Stylerows.Length + 1).ToString();
                            for (int i = 1, loopTo3 = Stylerows.Length; i <= loopTo3; i++)
                            {
                                var Reportrows = Data.Select_Rows("ReportInformation", "Report_Style_Id", Stylerows[i - 1][0]);
                                Reportcount += Reportrows.Length;
                            }

                            _reportNameBox.Text = _programNameBox.Text.Split('.')[0] + "_Report" + Reportcount;
                        }

                        SetDefaultValuesForStyles();
                        if (GotDSOFramer)
                        {
                            ToolBar1 = new ToolBar();
                                   // By Default Showing both Menu and Toolbar Into the DSO framer Window 
                        }

                        ShowToolBar.Checked = true;
                        showExcelMenu.Checked = false;
                        IncludeRawData = false;
                        CheckBox1.Checked = false;
                        Rawdataascol.Checked = true;
                        RawDataSheetType = "ColumnWise";
                        // -----------------------------Making the Default Checked state For the Radiobuttons----------------------------------------------------------------------
                        colorPrinter.Checked = true;
                        DetailSelect.CheckState = CheckState.Unchecked;
                        _tabControl.SelectedIndex = 0;
                        _conditionalFormatting1.Enabled = false;                // Making Default tab page for the Tablist Control
                        _conditionalFormatting1.CheckState = CheckState.Unchecked;
                        ConditionalFormattingFlag = false;        // Reset The CheckBoxes
                        _allowedRange.Checked = false;
                        _nomialPlusMinus.Checked = false;
                        _nominalMinMax.Checked = false;    // Resetting the tolerances radio buttons 
                        No_of_New_TitleRows = 0; // Resetting the Tolerance Format aand No_of new title rows created by user 
                        pnlInsertPicture.Enabled = false;
                        txtWidth.Text = "295";
                        txtHeight.Text = "221";
                        txtRow.Text = "10";
                        txtColumn.Text = "B";   // Resetting the picture default values.
                        InsertDoublePicture = false;
                        chkInsertPicture.CheckState = CheckState.Unchecked; // 'ImagePaths = Nothing :
                        if (ToleranceFormat != 0)
                        {
                            _conditionalFormatting1.Enabled = true;
                            _conditionalFormatting1.CheckState = CheckState.Checked;
                            IncludeTolerence = true;
                            if (ToleranceFormat == 1)
                            {
                                _allowedRange.Checked = true;
                            }
                            else if (ToleranceFormat == 2)
                            {
                                _nomialPlusMinus.Checked = true;
                            }
                            else
                            {
                                _nominalMinMax.Checked = true;
                            }
                        }

                        if (NewStyle)
                        {
                            PicturePath = ProgramName.Split('.')[0] + "_ProgramPreviewImage.jpg";
                            _conditionalFormatting1.Enabled = false;
                            _measurementListView.Columns[7].Visible = true;
                            _measurementListView.Columns[7].ReadOnly = true;
                            _measurementListView.Columns[8].Visible = true;
                            _measurementListView.Columns[8].ReadOnly = true;
                            _measurementListView.Columns[9].Visible = true;
                            _measurementListView.Columns[9].ReadOnly = true;
                        }
                    }

                    IncludeTolerence = false;
                    if (UseExistingReport)
                    {
                        if (MultiProgramReport)
                        {
                            if (!AllProgramInSingleReport)
                            {
                                ProgramInNewSheet.CheckState = CheckState.Checked;
                            }

                            ProgramInNewSheet.Enabled = false;
                        }

                        btnUpdateRange.Enabled = false;
                        DetailSelect.Enabled = false;
                        _conditionalFormatting1.Enabled = false;                // Making Default tab page for the Tablist Control
                        _conditionalFormatting1.Enabled = false;
                        ConditionalFormattingFlag = false;        // Reset The CheckBoxes
                        _allowedRange.Enabled = false;
                        _nomialPlusMinus.Enabled = false;
                        _nominalMinMax.Enabled = false;
                    }

                    for (int i = 0, loopTo4 = MeasurementStringList.Count - 1; i <= loopTo4; i++)
                    {
                        string TempStr = MeasurementStringList[i][3].ToString();
                        string TempCmpStr = "0° 00' 00''";
                        if (TempStr.Contains(Conversions.ToString(degree)))
                        {
                            if ((MeasurementStringList[i][3].ToString() ?? "") != (TempCmpStr ?? "") | (MeasurementStringList[i][4].ToString() ?? "") != (TempCmpStr ?? ""))
                            {
                                // ToleranceFormat = 2
                                // _nomialPlusMinus.Checked = CheckState.Checked
                                _conditionalFormatting1.CheckState = CheckState.Checked;
                                IncludeTolerence = true;
                                break;
                            }
                        }
                        else if (Conversions.ToBoolean(Operators.OrObject(Operators.ConditionalCompareObjectNotEqual(MeasurementStringList[i][3], 0, false), Operators.ConditionalCompareObjectNotEqual(MeasurementStringList[i][3], 0, false))))
                        {
                            // ToleranceFormat = 2
                            // _nomialPlusMinus.Checked = CheckState.Checked
                            _conditionalFormatting1.CheckState = CheckState.Checked;
                            IncludeTolerence = true;
                            break;
                        }
                    }

                    Formatdigits = (int)Math.Round(nudFormatdigits.Value);
                    ProgramInNewSheet.CheckState = CheckState.Unchecked;
                    Picture_Width = int.Parse(txtWidth.Text);
                    Picture_Height = int.Parse(txtHeight.Text);
                    Picture_Left = GetColIndex(txtColumn.Text);
                    Picture_Top = int.Parse(txtRow.Text);
                    if (File.Exists(PicturePath))
                    {
                        var imagemap = new Bitmap(PicturePath);
                        asratio1 = imagemap.Width / (double)imagemap.Height;
                    }

                    Picture2_Width = int.Parse(Rcadwidth.Text);
                    Picture2_Height = int.Parse(Rcadheight.Text);
                    Picture2_Left = GetColIndex(RcadCol.Text);
                    Picture2_Top = int.Parse(RcadRow.Text);
                    if (File.Exists(Picture2Path))
                    {
                        var imagemap = new Bitmap(Picture2Path);
                        asratio2 = imagemap.Width / (double)imagemap.Height;
                    }
                }

                if (IsHCM && HCMno > 0)
                {
                    SelectedSheet = "Data";
                    SelectedDataSheet = "Data";
                }

                return true;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E009", ex);
                return false;
            }
        }

        private void NewProgramParam_old()
        {
            try
            {
                SourceFinalRange = TemplateFinalRange;
                SourceInitialRange = TemplateInitialRange;
                if (AllProgramInSingleReport)
                {
                    var ReportR = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName)[0];
                    if (Conversions.ToBoolean(ReportR[32]))
                    {
                        var R_Id = ReportR[0];
                        var tmpDataR = Data.Select_Rows("MultiProgramReport", "Report_Id", R_Id);
                        TemplateFinalRange = Conversions.ToString(tmpDataR[tmpDataR.Length - 1][4]);
                    }

                    int endRow = default, colGap;
                    string EndCol = "";
                    for (int i = 0, loopTo = TemplateFinalRange.Length - 1; i <= loopTo; i++)
                    {
                        if (char.IsNumber(TemplateFinalRange[i]))
                        {
                            endRow = Conversions.ToInteger(TemplateFinalRange.Substring(i, TemplateFinalRange.Length - i));
                            EndCol = GetColumnName(GetColIndex(TemplateFinalRange.Substring(0, i)));
                            break;
                        }
                    }

                    if (ReportType.Contains("Column"))
                    {
                        colGap = GetColIndex(ColumnMax) - GetColIndex(Initial_Column);
                        Initial_Column = GetColumnName(GetColIndex(EndCol) + 1);
                        InitialField_Column = GetColumnName(GetColIndex(InitialField_Column) + colGap + 1);
                    }
                    // TemplateInitialRange = Initial_Column + CStr(Initial_Row)
                    // TemplateFinalRange = GetColumnName(GetColIndex(Initial_Column) + colGap + Sider) + CStr(endRow)
                    // InitialField_Column = GetColumnName(GetColIndex(Initial_Column) + colGap)
                    else
                    {
                        Initial_Row = endRow + 2;
                        InitialField_Row = Initial_Row + Header - 1;
                        endRow = Initial_Row + RowMax + Footer;
                        TemplateInitialRange = Initial_Column + Initial_Row.ToString();
                        TemplateFinalRange = EndCol + endRow.ToString();
                        // RowMax = Initial_Row + RowMax - 1
                    }
                }

                MultiProgramReport = true;
            }
            catch (Exception ex)
            {
            }
        }

        private void NewProgramParam()
        {
            try
            {
                // If MultipleReportsInDifferentPages Then
                // SourceFinalRange = TemplateFinalRange
                // SourceInitialRange = TemplateInitialRange
                // End If

                if (AllProgramInSingleReport)
                {
                    var ReportR = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName);
                    if (ReportR.Length > 0)
                    {
                        int R_Id = Conversions.ToInteger(ReportR[0][0]);
                        if (Conversions.ToBoolean(ReportR[0][32]))
                        {
                            var tmpDataR = Data.Select_Rows("MultiProgramReport", "Report_Id", R_Id);
                            TemplateFinalRange = Conversions.ToString(tmpDataR[tmpDataR.Length - 1][4]);
                        }

                        int endRow, colGap;
                        string EndCol = "";
                        var EndReads = GetRowandColumnfromRange(TemplateFinalRange);
                        endRow = EndReads[0];
                        EndCol = GetColumnName(EndReads[1]);
                        var MaxReads = new int[3]; // '= GetRowandColumnfromRange(TemplateFinalRange)
                        var currProgRecords = Data.DataS.Tables["ProgramInformation"].Select("Program_Name = '" + ProgramName + "'");
                        // If currProgRecords.Length <= 0 Then
                        // MaxReads(0) = ReportR(0)(26)
                        // MaxReads(1) = GetColIndex(ReportR(0)(27))
                        // Else

                        // End If
                        // 'Check how many programs we have connected with this report....
                        var MultiRptInfo = Data.Select_Rows("MultiProgramReport", "Report_Id", R_Id);
                        // 'Go through all the programs and get the last used Column and Template range
                        string maxTemplateRange = TemplateFinalRange;
                        // Dim MaxTemplateColumn As Integer = CInt(maxTemplateRange.Substring)
                        MaxReads[0] = Conversions.ToInteger(ReportR[0][26]);
                        MaxReads[1] = GetColIndex(Conversions.ToString(ReportR[0][27]));
                        for (int j = 0, loopTo = MultiRptInfo.Length - 1; j <= loopTo; j++)
                        {
                            // EndReads = GetRowandColumnfromRange(MultiRptInfo(j)(4))
                            if (currProgRecords.Length > 0) // 'Check if we are writing for a new Report file for preexisting program...
                            {
                                if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(MultiRptInfo[j][1], currProgRecords[0][0], false)))
                                {
                                    MaxReads[0] = Conversions.ToInteger(MultiRptInfo[j][7]);
                                    MaxReads[1] = GetColIndex(Conversions.ToString(MultiRptInfo[j][8]));
                                    break;
                                }
                            }
                            else // 'we are generating the program just now.
                            {
                                if (Conversions.ToBoolean(Operators.ConditionalCompareObjectGreater(MultiRptInfo[j][7], MaxReads[0], false)))
                                    MaxReads[0] = Conversions.ToInteger(MultiRptInfo[j][7]); // ' EndReads(0)
                                if (GetColIndex(Conversions.ToString(MultiRptInfo[j][8])) > MaxReads[1])
                                    MaxReads[1] = GetColIndex(Conversions.ToString(MultiRptInfo[j][8]));
                            } // 'EndReads(1)
                        }
                        // colGap = GetColIndex(ColumnMax) - GetColIndex(Initial_Column)
                        // Initial_Column = GetColumnName(GetColIndex(EndCol) + 2)
                        // InitialField_Column = GetColumnName(GetColIndex(InitialField_Column) + colGap + 2)
                        // TemplateInitialRange = Initial_Column + CStr(Initial_Row)
                        // TemplateFinalRange = GetColumnName(GetColIndex(Initial_Column) + colGap + Sider) + CStr(endRow)
                        if (!MultipleReportsInDifferentPages)
                        {
                            if (ReportType.Contains("Column"))
                            {
                                NoOfPages = (MaxReads[1] + _measurementListView.Rows.Count + MultiRptInfo.Length) / GetColIndex(ColumnMax) + 1;
                                TemplateInitialRange = GetColumnName(GetColIndex(ColumnMax) * (NoOfPages - 1) + 1) + Initial_Row;
                                TemplateFinalRange = GetColumnName(GetColIndex(ColumnMax) * NoOfPages) + Initial_Row;
                                if (MaxReads[1] == EndReads[1] + 1 || MaxReads[1] == GetColIndex(Initial_Column) || MaxReads[0] == Initial_Row + Header - 1)
                                {
                                    InitialField_Column = GetColumnName(MaxReads[1]);
                                }
                                else if (MaxReads[1] == EndReads[1] - 1)
                                {
                                    InitialField_Column = GetColumnName(EndReads[1] + 1);
                                }
                                else
                                {
                                    InitialField_Column = GetColumnName(MaxReads[1] + 1);
                                } // 'GetColIndex(ReportR(0)(27)) + 
                            }

                            // ColumnAfterWhich_PaginationShouldBeDone = GetColumnName(EndReads(1)) 'Making variables to be updated for next comparision.
                            else
                            {
                                NoOfPages = (MaxReads[0] + _measurementListView.Rows.Count) / (RowMax + Footer); // + 1 ''+ MultiRptInfo.Length
                                                                                                                 // TemplateInitialRange = Initial_Column & ((RowMax + Footer) * NoOfPages + 1)
                                                                                                                 // TemplateFinalRange = ColumnMax & ((RowMax + Footer) * NoOfPages)

                                if (MaxReads[0] == EndReads[0] + 1 - Footer || MaxReads[0] == Initial_Row + Header - 1 || MaxReads[0] == EndReads[0] + Header + 1 || MaxReads[1] == GetColIndex(Initial_Column))
                                {
                                    InitialField_Row = MaxReads[0];
                                }
                                else if (MaxReads[0] == EndReads[0] - Footer || MaxReads[0] == EndReads[0] - Footer - 1)
                                {
                                    InitialField_Row = NoOfPages * (RowMax + Footer + Initial_Row - 1) + Header; // + 1
                                }
                                else
                                {
                                    InitialField_Row = MaxReads[0] + 2;
                                } // 'Because we need one more row for writing name of component. 
                            }

                            // SourceInitialRange = "A1" : SourceFinalRange = ReportR(0)(10)
                            if (ReportType.Contains("Column"))
                            {
                            }
                            else
                            {
                            }
                        }

                        // If NoOfPages > 0 Then
                        // End If
                        else
                        {
                            Initial_Column = GetColumnName(GetColIndex(ColumnMax) * (MultiRptInfo.Length + 1));
                            InitialField_Column = Initial_Column;
                            TemplateInitialRange = Initial_Column + Initial_Row;
                            TemplateFinalRange = GetColumnName(GetColIndex(ColumnMax) * (MultiRptInfo.Length + 2)) + Initial_Row;
                        }
                        // InitialField_Column = GetColumnName(GetColIndex(Initial_Column) + colGap)
                        // Else
                        // Initial_Row = endRow + 2
                        // InitialField_Row = Initial_Row + Header - 1
                        // If Not MultipleReportsInDifferentPages Then
                        // Initial_Row = ReportR(0)(26) + 1
                        // InitialField_Row = Initial_Row
                        // End If

                        // endRow = Initial_Row + (RowMax) + Footer
                        // TemplateInitialRange = Initial_Column + CStr(Initial_Row)
                        // TemplateFinalRange = EndCol + CStr(endRow)
                        // 'RowMax = Initial_Row + RowMax - 1
                        // End If
                    }

                    MultiProgramReport = true;
                }
            }
            catch (Exception ex)
            {
            }
        }

        private int[] GetRowandColumnfromRange(string inputRange)
        {
            var Ans = new int[3];
            for (int i = 0, loopTo = inputRange.Length - 1; i <= loopTo; i++)
            {
                if (char.IsNumber(inputRange[i]))
                {
                    Ans[0] = Conversions.ToInteger(inputRange.Substring(i, inputRange.Length - i));
                    Ans[1] = GetColIndex(inputRange.Substring(0, i));
                    break;
                }
            }

            return Ans;
        }


        // ______________________________________________________________Handling the Tolernance Checkbox Check-Uncheck event._________________________________________________________-
        private void ConditionalFormatting_CheckedChanged(object sender, EventArgs e)
        {
            try
            {
                if (_conditionalFormatting1.CheckState == CheckState.Checked)
                {
                    panelToleranceType.Visible = true;
                    _measurementListView.Columns[7].Visible = true;
                    _measurementListView.Columns[8].Visible = true;
                    _measurementListView.Columns[9].Visible = true;
                    ConditionalFormattingFlag = true;
                    if (_allowedRange.Checked)
                        ToleranceFormat = 1;
                    if (_nomialPlusMinus.Checked)
                        ToleranceFormat = 2;
                    if (_nominalMinMax.Checked)
                        ToleranceFormat = 3;
                }
                else
                {
                    panelToleranceType.Visible = false;
                    _measurementListView.Columns[7].Visible = false;
                    _measurementListView.Columns[8].Visible = false;
                    _measurementListView.Columns[9].Visible = false;
                    ConditionalFormattingFlag = false;
                    ToleranceFormat = 0;
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E010", ex);
            }
        }

        // _______________________________Handling the Click Event Of the Add New Title Button--Here Adding new row to the table "TitleInformation" into the dataset.__________________
        private DataRow addNewTitle_Click(object sender, EventArgs e)
        {
            try
            {
                int Id;
                try
                {
                    Id = Data.ReturnsID("Select Max([Report_Style_Id]) FROM ReportStyleInformation");
                }
                catch (Exception ex)
                {
                    Id = 1;
                }

                DataRow New_Row = Data.GetTable("TitleInformation").NewRow();
                New_Row[1] = Id + 1;
                Data.GetTable("TitleInformation").Rows.Add(New_Row);
                No_of_New_TitleRows += 1;                            // This is incremented to track the record of the new titles generated
                var Com = new DataGridViewComboBoxCell();             // Creating the new combobox column for the Title Type 
                var List = new string[] { "Date", "Time", "Predefined", "" };
                for (int i = 0, loopTo = List.Length - 1; i <= loopTo; i++)
                    Com.Items.Add(List[i]);
                Com.Value = "";
                _extraInformationDataView.Rows[No_of_New_TitleRows - 1].Cells[3] = Com;
                return New_Row;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E:0011", ex);
                return null;
            }
        }

        #region Tolerance Panel click event handling 
        private void Btn_TolerancePanelClose_Click(object sender, EventArgs e)
        {
            panelToleranceType.Visible = false;
        }

        private void _allowedRange_CheckedChanged(object sender, EventArgs e)
        {
            ToleranceFormat = 1;
        }

        private void _nomialPlusMinus_CheckedChanged(object sender, EventArgs e)
        {
            ToleranceFormat = 2;
        }

        private void _nominalMinMax_CheckedChanged(object sender, EventArgs e)
        {
            ToleranceFormat = 3;
        }
        #endregion
        // __________________________________________________Handling the Excel menu and toolbar chekbox check change event _________________________________________________________
        private void showExcelMenu_CheckedChanged(object sender, EventArgs e)
        {
            //if (showExcelMenu.CheckState == CheckState.Checked)
            //    _dsoFramer.Menubar = true;
            //else
            //    _dsoFramer.Menubar = false;
            //if (ShowToolBar.CheckState == CheckState.Checked)
            //    _dsoFramer.Toolbars = true;
            //else
            //    _dsoFramer.Toolbars = false;
        }
        // _________________________________________Handling The MoveUp and MoveDown actions of the Measurementdatagridview rows by user.______________________________________________
        private void BtnMoveUp_Click(object sender, EventArgs e)
        {
            int RowIndex = _measurementListView.SelectedRows[0].Index;
            int TotalRowCount = _measurementListView.Rows.Count;
            try
            {
                if (_measurementListView.SelectedRows.Count > 1)
                {
                    Interaction.MsgBox("Cannot Move many Rows At a time :: Please select Only One row to move !!");
                    return;
                }

                if (sender.Equals(BtnMoveUp))
                {
                    if (RowIndex > 0)                                                                       // The UP button is clicked so Move the row Up one position.
                    {
                        var _selectedRow = Data.GetTable("FieldInformation").Rows[RowIndex];
                        var _newRow = Data.GetTable("FieldInformation").NewRow();
                        _newRow.ItemArray = _selectedRow.ItemArray;
                        Data.GetTable("FieldInformation").Rows.Remove(_selectedRow);
                        Data.GetTable("FieldInformation").Rows.InsertAt(_newRow, RowIndex - 1);
                        if (!_measurementListView.Rows[RowIndex - 1].Selected)
                        {
                            _measurementListView.ClearSelection();
                            _measurementListView.Rows[RowIndex - 1].Selected = true;
                        }

                        RefreshFieldNumbers();
                    }
                }
                else if (RowIndex < TotalRowCount - 1)                                                     // The DOWN button is clicked so move the row down one position.
                {
                    var _selectedRow = Data.GetTable("FieldInformation").Rows[RowIndex];
                    var _newRow = Data.GetTable("FieldInformation").NewRow();
                    _newRow.ItemArray = _selectedRow.ItemArray;
                    Data.GetTable("FieldInformation").Rows.Remove(_selectedRow);
                    Data.GetTable("FieldInformation").Rows.InsertAt(_newRow, RowIndex + 1);
                    if (!_measurementListView.Rows[RowIndex + 1].Selected)
                    {
                        _measurementListView.ClearSelection();
                        _measurementListView.Rows[RowIndex + 1].Selected = true;
                    }

                    RefreshFieldNumbers();
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E012", ex);
            }
        }
        // __________________________________________Function to insert a new blank row into the measurementlist gridview._________________________________________________________________________________________________________
        private DataRow _measurementListView_RowHeaderMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            try
            {
                if (_measurementListView.SelectedRows.Count > 1)
                {
                    Interaction.MsgBox("Please select only one Row !!");
                    //return null;
                }

                var _newRow = Data.GetTable("FieldInformation").NewRow();                   // Instantiating the new row 
                _newRow[2] = "";
                _newRow[6] = "";
                _newRow[10] = false;
                _newRow[13] = "";
                Data.GetTable("FieldInformation").Rows.InsertAt(_newRow, _measurementListView.SelectedRows[0].Index + 1);        // Inserting the new row 
                _measurementListView.AllowUserToDeleteRows = true;
                return _newRow;
            }
            catch (Exception ex)
            {
                return null;
            }
        }
        // _____________This sub is to close the DSO active document properly.Due to several bugs in DSO and they manifest with time and Usage so extra precautionary steps are taken to ensure proper and effective closing of the DSO framer Active document.
        private void CloseDSOActiveDocument()
        {
            try
            {
                //_dsoFramer.ActiveDocument.Save();  // Closing the DSO framer window and calling the GC to collect the unwanted excel generated by DSO framer.
                //System.Runtime.InteropServices.Marshal.FinalReleaseComObject(_dsoFramer.ActiveDocument);
            }
            catch (Exception ex)
            {
            }                  // Sometimes the DSO framer catches exception in saving.(due to bug in DSO framer itself.)
            finally
            {
                try                                // Due to that bug, this is a precautionary step to ensure that DSO framer excel is closed properly.
                {
                    System.Runtime.InteropServices.Marshal.FinalReleaseComObject(xL.workbook.ActiveSheetIndex);
                    xL.workbook.Close();
                }
                catch (Exception ex)
                {
                    // This will be caused when the active document property of the DSO framer get corrupted(again a bug in DSO framer).
                    // So here calling the close method on DSO framer itself(bcoz this exception tells us that DSO framer has not associated its activeX host control to any object (when in fact it should have as you will be seeing a active document on screen hosted !!)
                    xL.workbook.Close();
                }
                finally
                {
                    GC.Collect();
                    GC.WaitForPendingFinalizers();
                }
            }
        }
        // ____________________________________________Handling the Cancel Button Click event -- Hiding the Form.___________________________________________________________________
        private void _cancelButton_Click(object sender, EventArgs e)
        {
            if (NewStyle)
            {
                NewStyle = false;
            }

            try
            {
                CloseDSOActiveDocument();
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E013", ex);
            }
            finally
            {
                Hide();
            }
            // Can i here destroy the dataset <OR> call reject changes on dataset ....??
            // Anyhow ,the next time dataset will be loaded with the new values only.
        }
        // vinod to know that the hob report we  have to made
        public bool IsHobCheckerMeasurementNo123(int MeasurementNo, bool IsHobCheckerMsrmnt = false)
        {
            try
            {
                HCMno = MeasurementNo;
                IsHCM = IsHobCheckerMsrmnt;
                _IsDoPagination = false;
                return true;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E015a", ex);
                return false;
            }
        }
        // 'Make a function to write all hob details for entering into report
        public bool EnterHobParameterData()
        {
            // 'Get the data in the following order - 
            // 0 Order No. :
            // 1 Serial No. :
            // 2 Customer:
            // 3 Date.
            // 6 Module 
            // 7 Lead(Angle)
            // 9 No. of Starts :
            // 10 No.of(Gashes)
            // 11 Lead of Gashes :
            // 12 Tooth Depth :
            // 13 Hob(Length) - ActiveLength
            // 14 Outer(Dia) - Calculated value if First Tooth Done
            // 4 Machine No. - Not Present. So ignore for now R A 27 Jun 14
            // 5 Operator - Not Present anymore R A 27 Jun 14
            // 8 Pressure(Angle) - Not Present Yet

            // '''''---------------
            // '''' Rehash to converge on Balzer's report
            // 0 - Module  
            // 1 - No of Gashes
            // 2 - Gash Lead
            // 3 - Hand
            // 4 - No of Starts
            // 5 - Lead Angle
            // 6 - Hob OD
            // 7 - Active Length
            // 8 - Tooth Depth
            // 9 - Class
            // 10 - Pressure Angle
            // 11 - Machine No.
            // 12 - Date
            // 13 - Customer
            // 14 - Order No.
            // 15 - Serial No.
            // 16 - Operator

            // Dim cLL() As String = {"B", "B", "B", "D", "F", "F", "B", "B", "B", "D", "D", "D", "F", "F", "F"}
            // Dim rLL() As Integer = {2, 3, 4, 2, 2, 3, 5, 6, 7, 5, 6, 7, 5, 6, 7}
            try
            {
                var cLL = new string[] { "C", "C", "C", "C", "C", "C", "F", "F", "F", "F", "F", "F", "H", "H", "H", "H", "H" };
                var rLL = new int[] { 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 9 };
                for (int i = 0, loopTo = cLL.Length - 1; i <= loopTo; i++)
                    xL[rLL[i], cLL[i]] = HobPersonalData[i];
                return true;
            }
            catch (Exception ex)
            {
                MessageBox.Show("Could not fill in the Hob Parameter data in Excel file." + Microsoft.VisualBasic.Constants.vbCrLf + Microsoft.VisualBasic.Constants.vbCrLf + ex.Message, "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Stop);      // Warn the User and Exit the sub 
                return false;
            }
        }
        // vinod to know that the delphi_tvs report we  have to made
        public bool IsDelphiTVSMeasurement(bool IsDelphiTVSMsrmnt = false)
        {
            try
            {
                IsDTVSM = IsDelphiTVSMsrmnt;
                return true;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E015a", ex);
                return false;
            }
        }
        // vinod to know that the GridImageCapture report we  have to made
        public bool GridImageProgram(short Grid_Rows = 0, short Grid_Cols = 0, bool IsGridImageCapture = false)
        {
            try
            {
                GridImgCaptureRows = Grid_Rows;
                GridImgCaptureCols = Grid_Cols;
                IsGrigImagecaptureM = IsGridImageCapture;
                return true;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E015b", ex);
                return false;
            }
        }

        // _____________________________________________________________Handling the save button click function.______________________________________________________________________-
        private void _saveButton_Click(object sender, EventArgs e)
        {
            _IsDoPagination = cbPaginateReport.Checked;
            if (NewStyle)
            {
                if (Data.Find_Record("ReportInformation", _reportPathNameBox.Text + @"\" + _reportNameBox.Text, 2))
                {
                    Interaction.MsgBox("This ReportName already exists, please change the Reportname");
                    return;
                }
                else if (Data.Find_Record("ReportStyleInformation", _reportStyleNameBox.Text, 2))
                {
                    Interaction.MsgBox("This ReportStyleName already exists, please change the ReportStyleName");
                    return;
                }

                ReportName = _reportPathNameBox.Text + @"\" + _reportNameBox.Text + ".xls";
                if (CreateCsv)
                {
                    ReportNameCsv = _reportPathNameBox.Text + @"\" + _reportNameBox.Text + ".csv";
                }

                ReportStyleName = _reportStyleNameBox.Text;
            }

            for (int i = 0, loopTo = _extraInformationDataView.Rows.Count - 1; i <= loopTo; i++)            // Checking the information into the Title rows.(It should be complete otherwise exit this sub)
            {

                // -----------------------------------Checking the Information about the titles are given full or not -------------------------------------------------------------

                if (ReferenceEquals(_extraInformationDataView.Rows[i].Cells[2].Value, DBNull.Value) | ReferenceEquals(_extraInformationDataView.Rows[i].Cells[4].Value, DBNull.Value) | ReferenceEquals(_extraInformationDataView.Rows[i].Cells[5].Value, DBNull.Value) | ReferenceEquals(_extraInformationDataView.Rows[i].Cells[6].Value, DBNull.Value))
                {
                    MessageBox.Show("Please Fill the Full Information about the New Title Created !! ", "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                else if (_extraInformationDataView.Rows[i].Cells[5].Value as string is null)
                {
                    MessageBox.Show("The Column specified for the title must be a char or string only.Numbers cannot be accepted.", "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }

            try
            {
                ShwfieldNum = ChkShwFieldNum.Checked;
                InsertImage = chkInsertPicture.Checked;
                IncludeTolerence = _conditionalFormatting1.Checked;
                Formatdigits = (int)Math.Round(nudFormatdigits.Value);
                if (InsertImage)                  // checking whether the picture details are right or not.
                {
                    try
                    {
                        Picture_Width = int.Parse(txtWidth.Text);
                        Picture_Height = int.Parse(txtHeight.Text);
                        Picture_Left = GetColIndex(txtColumn.Text);
                        Picture_Top = int.Parse(txtRow.Text);
                        if (File.Exists(PicturePath))
                        {
                            var imagemap = new Bitmap(PicturePath);
                            asratio1 = imagemap.Width / (double)imagemap.Height;
                        }

                        Picture2_Width = int.Parse(Rcadwidth.Text);
                        Picture2_Height = int.Parse(Rcadheight.Text);
                        Picture2_Left = GetColIndex(RcadCol.Text);
                        Picture2_Top = int.Parse(RcadRow.Text);
                        if (File.Exists(Picture2Path))
                        {
                            var imagemap = new Bitmap(Picture2Path);
                            asratio2 = imagemap.Width / (double)imagemap.Height;
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("The details for the picture are not correct," + Microsoft.VisualBasic.Constants.vbCrLf + ex.Message, "Excel Module ", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                }

                CloseDSOActiveDocument();                                                   // Closing the DSO framer window and calling the GC to collect the unwanted excel generated by DSO framer.
                xL.InitializeExcelApplication(ReportTemplateName, SelectedSheet);                           // Initializing the excel to write the tolerances and title information for templates.
                for (int i = 0, loopTo1 = _extraInformationDataView.Rows.Count - 1; i <= loopTo1; i++)            // Writing the title information into the excel file first. 
                {
                    int Title_Row = Conversions.ToInteger(_extraInformationDataView.Rows[i].Cells[6].Value);                  // The row  information
                    string Title_Col = Conversions.ToString(_extraInformationDataView.Rows[i].Cells[5].Value);                   // The column Information 
                    string Title_Name = Conversions.ToString(_extraInformationDataView.Rows[i].Cells[2].Value);                  // The title's name 
                    xL[Title_Row, Title_Col] = Title_Name;
                    xL.set_Set_Alignment(Title_Row, Title_Col, -4108);                                                  // Left Alignment for the title name 

                    // ----------------------------Calculating the next coloumn for putting the Title's Value -------------------------------------------------------------------

                    int Next_Col_TitleValue = GetColIndex(Strings.StrConv(Title_Col, VbStrConv.Uppercase));
                    string Col_Next = GetColumnName(Next_Col_TitleValue + 1);
                    string Title_Value = Conversions.ToString(_extraInformationDataView.Rows[i].Cells[4].Value);
                    xL[Title_Row, Col_Next] = Title_Value;
                    xL.set_Set_Alignment(Title_Row, Col_Next, -4152);                                                 // Right Alignment for the title value 
                }

                if (chkWriteprogramNameInReport.Checked)                                                       // ==Writing the program Name if the option is checked.
                {
                    string OnlyprogramName = _programNameBox.Text.Split('.')[0];
                    if (ReportType.Contains("Column"))
                    {
                        xL[InitialField_Row - 1, "D"] = OnlyprogramName;
                    }
                    else
                    {
                        xL[InitialField_Row - 1, "C"] = OnlyprogramName;
                    }
                }

                xL.Save();             // Now save the new field values and give it to the main application 
                xL.ExitExcel();
                if (NewStyle)
                {
                    Update_Database();
                    NewStyleCreated?.Invoke();
                    GenerateExcel();
                }

                if (UseExistingReport)
                {
                    if (ProgramInNewSheet.Checked)
                    {
                        AllProgramInSingleReport = false;
                    }
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E014", ex);
            }
            finally
            {
                Hide();
            }
        }

        // ____________________________________________________Function to call to generate the excel file ____________________________________________________________________________
        public bool GenerateExcel(bool AgainWriteToSameFile = false, bool FastTransfer = false)
        {
            // Dim stopWatch As New System.Diagnostics.Stopwatch
            // stopWatch.Start()
            try
            {
                WithinRangeCheckFlag = false;
                if (FastTransfer | !AgainWriteToSameFile)
                {
                    try
                    {
                        //if (_dsoFramer.ActiveDocument is object)                  // Closing the DSO framer before writing
                        //{
                        //    _dsoFramer.ActiveDocument.save();
                        //}

                        xL.workbook.Close();
                        GC.Collect();
                        GC.WaitForPendingFinalizers();
                    }
                    catch (Exception ex)
                    {
                    }                                                // Due to well known bug in DSO framer sometimes the save method catches exception.
                }

                if (AgainWriteToSameFile)
                {
                    xL.InitializeExcelApplication(ReportName, SelectedSheet, Password);
                }

                if (!Writing_to_Report(AgainWriteToSameFile, FastTransfer))
                    throw new Exception("Writing to Excel Failed!!"); // Now Write the values into the excel file.
                                                                      // xL.xlW.Application.CutCopyMode = XlCutCopyMode.False
                if (AgainWriteToSameFile == false & UseExistingReport == false)
                {
                    //if (!string.IsNullOrEmpty(Password) | Password is object)                     // Now save the file with password support.
                    //{
                    //    xL.xlW.SaveAs(ReportName, default, default, Password);
                    //}
                    //else
                    //{
                    //    xL.xlW.SaveAs(ReportName);
                    //}

                    xL.Close();
                    xL.ExitExcel();
                }
                else
                {
                    xL.Save();
                    xL.ExitExcel();
                }

                DeleteDirectory();                                                         // Delete the temporary directory generated 
                                                                                           // stopWatch.Stop()
                                                                                           // MessageBox.Show(stopWatch.Elapsed.ToString)
                UpdateLastReport = false;
                return true;
            }
            catch (Exception ex)
            {
                UpdateLastReport = false;
                OnErrorHappen?.Invoke("E015", ex);
                if (xL is object)
                {
                    xL.Close();
                    xL.ExitExcel(); // Just for precaution exiting the excel instance.
                }

                return false;
            }
        }

        public string GeneratePrintReport(string ReportPath)
        {
            try
            {
                WithinRangeCheckFlag = false;
                // If FastTransfer Or Not (AgainWriteToSameFile) Then
                try
                {
                    //if (_dsoFramer.ActiveDocument is object)                  // Closing the DSO framer before writing
                    //{
                    //    _dsoFramer.ActiveDocument.save();
                    //}

                    xL.workbook.Close();
                    GC.Collect();
                    GC.WaitForPendingFinalizers();
                }
                catch (Exception ex)
                {
                }                                                // Due to well known bug in DSO framer sometimes the save method catches exception.
                                                                 // End Ifxcxcv
                                                                 // If AgainWriteToSameFile Thenxvxcxcv
                                                                 // xL.InitializeExcelApplication(ReportName, SelectedSheet, Password)
                                                                 // End If
                ArrayList Datearray = new ArrayList(), TimeArray = new ArrayList();
                Datearray.Add("Date");
                Datearray.Add("Date:");
                Datearray.Add(string.Format("{0:dd-MM-yyyy}", DateTime.Today));
                Datearray.Add(0);
                Datearray.Add(0);
                Datearray.Add(0);
                Datearray.Add("D1");
                TimeArray.Add("Time");
                TimeArray.Add("Time:");
                TimeArray.Add(DateTime.Now.Hour + ":" + DateTime.Now.Minute + ":" + DateTime.Now.Second);
                TimeArray.Add(0);
                TimeArray.Add(0);
                TimeArray.Add(0);
                TimeArray.Add("T1");
                var qrNames = Directory.GetFileSystemEntries(ReportPath, "*.xlsx");
                int prev = 0;
                int MaxReportCount = 0;
                for (int i = 0, loopTo = qrNames.Length - 1; i <= loopTo; i++)
                {
                    if (qrNames[i].Contains("QuickReport-" + Datearray[2].ToString()))
                    {
                        string _prog = qrNames[i].Substring(qrNames[i].LastIndexOf("-", StringComparison.InvariantCulture) + 1).Split(Conversions.ToCharArrayRankOne("."))[0];
                        if (int.TryParse(_prog, out prev))
                        {
                            if (prev > MaxReportCount)
                            {
                                MaxReportCount = prev;
                            }
                        }
                    }
                }

                MaxReportCount += 1;
                string qreportname = ReportPath + @"\" + "QuickReport-" + Datearray[2].ToString() + "-" + MaxReportCount + ".xlsx";
                File.Copy(DatabaseConstants.TemplatePath + @"\QuickReportTemplate.xlsx", qreportname, true);
                xL.InitializeExcelApplication(qreportname, "Report");
                if (MeasurementStringList.Count > 0)
                {
                    if (MeasurementStringList[0].GetType().ToString().Contains("ArrayList"))
                    {
                        var ii = default(int);
                        while (ii < MeasurementStringList.Count)
                        {
                            var tempAL = MeasurementStringList[0];
                            if (tempAL[0].ToString() == "Date")
                            {
                                MeasurementStringList.Remove(tempAL);
                                continue;
                            }

                            if (tempAL[1].ToString() == "Time")
                            {
                                MeasurementStringList.Remove(tempAL);
                                continue;
                            }

                            ii += 1;
                        }
                    }
                }

                MeasurementStringList.Insert(0, Datearray);
                MeasurementStringList.Insert(1, TimeArray);
                int currentrow = 3;
                // xL.xlS.Cells(2, 2).Value = 5

                for (int i = 0, loopTo1 = MeasurementStringList.Count - 1; i <= loopTo1; i++)
                {
                    // Dim tempAL As ArrayList = MeasurementStringList(i)
                    // xL.Get_Set_Value(currentrow, 1) = MeasurementStringList.Item(i).Item(1)
                    if (MeasurementStringList[i][0].ToString() == "Date")
                    {
                        CompareAndWrite_FieldValues(MeasurementStringList[i][2].ToString(), currentrow, "B", NumberFormat: -2);
                    }
                    else if (MeasurementStringList[i][0].ToString() == "Time")
                    {
                        CompareAndWrite_FieldValues(MeasurementStringList[i][2].ToString(), currentrow, "B", NumberFormat: -3);
                    }
                    else
                    {
                        CompareAndWrite_FieldValues(MeasurementStringList[i][0].ToString(), currentrow, "A", NumberFormat: Formatdigits);
                        CompareAndWrite_FieldValues(MeasurementStringList[i][2].ToString(), currentrow, "B", NumberFormat: Formatdigits);
                    }

                    currentrow += 1;
                }

                //xL.xlW.Save();
                xL.Close();
                xL.ExitExcel();
                return qreportname;
            }
            catch (Exception ex)
            {
                UpdateLastReport = false;
                OnErrorHappen?.Invoke("E015q", ex);
                if (xL is object)
                {
                    xL.Close();
                    xL.ExitExcel(); // Just for precaution exiting the excel instance.
                }

                return "";
            }
        }

        // ---------------------------------------------------------------------------------------------------------------------------------------------

        public void UpdateMultiProgramTable()
        {
            var tmpDataR = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName)[0];
            var Report_Id = tmpDataR[0];
            var Styleid = tmpDataR[1];
            if (Conversions.ToBoolean(!(bool)tmpDataR[32]))
            {
                int OldProgram_Id = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "Report_Style_Id", Styleid)[0][1]);
                var OldRep_Row = Data.GetTable("MultiProgramReport").NewRow();
                var Oldobj = new object[] { Report_Id, OldProgram_Id, SelectedSheet, SelectedDataSheet, tmpDataR[10], tmpDataR[21], tmpDataR[22], tmpDataR[26], tmpDataR[27], tmpDataR[9], tmpDataR[13] };
                for (int i = 0; i <= 10; i++)
                    OldRep_Row[i] = Oldobj[i];
                Data.GetTable("MultiProgramReport").Rows.Add(OldRep_Row);
                Data.Update_Table("MultiProgramReport");
            }

            int Program_Id = Conversions.ToInteger(Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName)[0][0]);
            var Rep_Row = Data.GetTable("MultiProgramReport").NewRow();
            var obj = new object[] { Report_Id, Program_Id, SelectedSheet, SelectedDataSheet, TemplateFinalRange, InitialField_Row, InitialField_Column, LastRowUsed, LastColumnUsed, RowMax, ColumnMax };
            for (int i = 0; i <= 10; i++)
                Rep_Row[i] = obj[i];
            Data.GetTable("MultiProgramReport").Rows.Add(Rep_Row);
            Data.Update_Table("MultiProgramReport");
        }

        public bool DeleteReport_Database(string reportName)
        {
            if (Data is null)
            {
                Data = new xl_DB();
                dataS = Data.FillTableRange(DatabaseConstants.TableNames);
            }

            var drows = Data.Select_Rows("ReportInformation", "R_ReportName", reportName);
            if (drows.Length > 0)
            {
                drows[0].Delete();
                Data.Update_Table("ReportInformation");
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool DeleteProgram_Database(string programName)
        {
            int Program_Id;
            int ReportStyle_Id = 0;
            try
            {
                if (Data is null)
                {
                    Data = new xl_DB();
                    dataS = Data.FillTableRange(DatabaseConstants.TableNames);
                }

                if (Data.Select_Rows("ProgramInformation", "Program_Name", programName).Length > 0)                 // Checking the program name already exists into the database or not 
                {
                    Program_Id = Conversions.ToInteger(Data.Select_Rows("ProgramInformation", "Program_Name", programName)[0][0]);
                    Data.Delete_Row("ProgramInformation", "Program_Name", programName);
                    Data.Update_Table("ProgramInformation");
                    if (Data.Select_RowsInt("ReportStyleInformation", "Program_Id", Program_Id).Length > 0)
                    {
                        ReportStyle_Id = Conversions.ToInteger(Data.Select_RowsInt("ReportStyleInformation", "Program_Id", Program_Id)[0][0]);
                        Data.Delete_Row("ReportStyleInformation", "Program_Id", (object)Program_Id);
                        Data.Update_Table("ReportStyleInformation");
                        if (Data.Select_RowsInt("ReportInformation", "Report_Style_Id", ReportStyle_Id).Length > 0)
                        {
                            Data.Delete_Row("ReportInformation", "Report_Style_Id", (object)ReportStyle_Id);
                            Data.Update_Table("ReportInformation");
                        }

                        if (Data.Select_RowsInt("FieldInformation", "Report_Style_Id", ReportStyle_Id).Length > 0)
                        {
                            Data.Delete_Row("FieldInformation", "Report_Style_Id", (object)ReportStyle_Id);
                            Data.Update_Table("FieldInformation");
                        }
                    }

                    var lf = new LoadFrm();
                    lf.Reset_All();
                    return true;
                }
                else
                {
                    return false;
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E210", ex);
                return false;
            }
        }

        public bool RenameProgram_Database(string programName, string newProgramName)
        {
            if (Data is null)
            {
                Data = new xl_DB();
                dataS = Data.FillTableRange(DatabaseConstants.TableNames);
            }

            if (Data.Select_Rows("ProgramInformation", "Program_Name", programName).Length > 0)                 // Checking the program name already exists into the database or not 
            {
                Data.Select_Rows("ProgramInformation", "Program_Name", programName)[0][1] = newProgramName;
                Data.Update_Table("ProgramInformation");
                return true;
            }
            else
            {
                return false;
            }
        }

        // ________________________________________________Function to update the database __________________________________________________________________________________________
        public bool Update_Database()
        {
            bool ProgramUpdated = default, ReportStyleUpdated = default, ReportUpdated = default, TitleUpdated = default, FieldUpdated = default;
            var Program_Id = default(int);
            var ReportStyle_Id = default(int);
            try
            {
                if (!NewStyle)
                {
                    Data.FillTable("ProgramInformation");
                    if (!Data.Find_Record("ProgramInformation", ProgramName, 1))                 // Checking the program name already exists into the database or not 
                    {
                        var Prog_Row = Data.GetTable("ProgramInformation").NewRow();             // Program name does not exists.Create The entry for it.
                        if (!(Data.DataS.Tables["ProgramInformation"].Rows.Count == 0))
                        {
                            Prog_Row[0] = Data.ReturnsID("select max([Program_Id]) from ProgramInformation") + 1;
                        }
                        else
                        {
                            Prog_Row[0] = 1;
                        }

                        Prog_Row[1] = ProgramName;
                        Prog_Row[2] = UserName;
                        Data.GetTable("ProgramInformation").Rows.Add(Prog_Row);
                        Data.Update_Table("ProgramInformation");
                        ProgramUpdated = true;
                        Data.DataS.Tables.Remove("ProgramInformation");
                        Data.FillTable("ProgramInformation");
                    }
                }

                Program_Id = Conversions.ToInteger(Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName)[0][0]);
                if (!UseExistingReport)
                {
                    // Program Already exists.Pick The Program ID for the given Program name and proceed to the Report Style Information Table 
                    if (ReportStyleName is null | string.IsNullOrEmpty(ReportStyleName))
                        ReportStyleName = ProgramName + "_Default"; // Checking the text value into the report style textbox ,If nothing then taking the default value.
                    bool WarnFlagForReportStyle = Data.Find_Record("ReportStyleInformation", ReportStyleName, 2) & Data.Find_Record("ReportStyleInformation", Program_Id.ToString(), 1);
                    if (!WarnFlagForReportStyle)              // The ReportStyle does not exist 
                    {
                        var RepS_Row = Data.GetTable("ReportStyleInformation").NewRow();
                        if (!(Data.DataS.Tables["ReportStyleInformation"].Rows.Count == 0))
                        {
                            RepS_Row[0] = Data.ReturnsID("select max([Report_Style_Id]) from ReportStyleInformation") + 1;
                        }
                        else
                        {
                            RepS_Row[0] = 1;
                        }

                        RepS_Row[1] = Program_Id;
                        RepS_Row[2] = ReportStyleName;
                        RepS_Row[4] = TemplateFolderName + @"\" + ReportStyleName + "_Template.xls";
                        RepS_Row[5] = ReportType;
                        RepS_Row[6] = Password;
                        Data.GetTable("ReportStyleInformation").Rows.Add(RepS_Row);
                        Data.Update_Table("ReportStyleInformation");
                        ReportStyleUpdated = true;
                        Data.DataS.Tables.Remove("ReportStyleInformation");
                        Data.FillTable("ReportStyleInformation");
                    }

                    ReportStyle_Id = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "ReportStyleName", ReportStyleName)[0][0]);     // ReportStyleName already exists ,Pick its Id and move to Report Information table 
                    var Rep_Row = Data.GetTable("ReportInformation").NewRow();
                    if (Data.DataS.Tables["ReportInformation"].Rows.Count != 0)
                    {
                        Rep_Row[0] = Data.ReturnsID("select max([Report_Id]) from ReportInformation") + 1;
                    }
                    else
                    {
                        Rep_Row[0] = 1;
                    }

                    int tempTolFormat = ToleranceFormat;
                    if (!IncludeTolerence)
                        ToleranceFormat = -1;
                    var obj = new object[] { ReportStyle_Id, ReportName, ConditionalFormattingFlag, ReportType, "Report", ToleranceFormat, Header, Footer, RowMax, TemplateFinalRange, PrinterState, Password, ColumnMax, NoOfPastedTemplates, _IsDoPagination, InsertImage, Picture_Width, Picture_Height, Picture_Top, Picture_Left, InitialField_Row, InitialField_Column, InsertDoublePicture, Formatdigits, Sider, LastRowUsed, LastColumnUsed, RowMax, ColumnMax, ShwfieldNum, ShowDetail, MultiProgramReport, AllProgramInSingleReport, Picture2_Width, Picture2_Height, Picture2_Top, Picture2_Left, CreateCsv, ReportNameCsv, IncludeRawData, RawDataSheetType };
                    for (int i = 1; i <= 41; i++)
                        Rep_Row[i] = obj[i - 1];
                    Data.GetTable("ReportInformation").Rows.Add(Rep_Row);
                    Data.Update_Table("ReportInformation");
                    ReportUpdated = true;
                    ToleranceFormat = tempTolFormat;
                }
                else
                {
                    UpdateMultiProgramTable();
                    Data.DataA = new OleDbDataAdapter("Select * From [ReportInformation] Where [ReportInformation].[R_ReportName] = '" + ReportName + "'", DatabaseConstants.ConnectionString);
                    var ds = new DataSet();
                    Data.DataA.Fill(ds);
                    ds.Tables[0].Rows[0][32] = MultiProgramReport;
                    ds.Tables[0].Rows[0][33] = AllProgramInSingleReport;
                    var cb = new OleDbCommandBuilder(Data.DataA);
                    Data.DataA.Update(ds);
                    var tmprow = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName)[0];
                    var tmpid = tmprow[1];
                    var reportstylerow = Data.Select_Rows("ReportStyleInformation", "Report_Style_Id", tmpid)[0];
                    var RepS_Row = Data.GetTable("ReportStyleInformation").NewRow();
                    if (!(Data.DataS.Tables["ReportStyleInformation"].Rows.Count == 0))
                    {
                        RepS_Row[0] = Data.ReturnsID("select max([Report_Style_Id]) from ReportStyleInformation") + 1;
                    }
                    else
                    {
                        RepS_Row[0] = 1;
                    }

                    RepS_Row[1] = Program_Id;
                    RepS_Row[2] = reportstylerow[2];
                    RepS_Row[4] = reportstylerow[4];
                    RepS_Row[5] = reportstylerow[5];
                    RepS_Row[6] = reportstylerow[6];
                    Data.GetTable("ReportStyleInformation").Rows.Add(RepS_Row);
                    Data.Update_Table("ReportStyleInformation");
                    ReportStyleUpdated = true;
                    ReportUpdated = true;
                    UseExistingReport = false;
                    AllProgramInSingleReport = false;
                    MultiProgramReport = false;
                    ReportStyle_Id = Conversions.ToInteger(Data.Select_Rows("ReportStyleInformation", "Program_Id", Program_Id)[0][0]);
                }
                // ----------------------------------------------Updating the Field information table with the required values-----------------------------------------------------
                for (int i = 0, loopTo = _measurementListView.Rows.Count - 1; i <= loopTo; i++)
                    Fieldrows[i][1] = ReportStyle_Id;
                _measurementListView.DataSource = null;
                Data.Update_Table("FieldInformation");
                FieldUpdated = true;
                Data.Update_Table("TitleInformation");           // Updating the Title Information 
                TitleUpdated = true;
            }
            catch (Exception ex)
            {
                if (ProgramUpdated)     // If any error come just perform the rollback operation.We have to manually implement this because OLEDB Jet 4.0 provider does not support the Transaction Scope class for LTM and DTM (transaction management).
                {
                    Data.DeleteRecords("Delete from [ProgramInformation] where Program_name=" + "'" + ProgramName + "'");
                }
                else if (ReportStyleUpdated)
                {
                    Data.DeleteRecords("Delete from [ReportStyleInformation] where Program_Id=" + "'" + Program_Id + "'");
                }
                else if (ReportUpdated)
                {
                    Data.DeleteRecords("Delete from [ReportInformation] where Report_Style_Id=" + "'" + ReportStyle_Id + "'");
                }
                else if (FieldUpdated)
                {
                    Data.DeleteRecords("Delete from [FieldInformation] where Report_Style_Id=" + "'" + ReportStyle_Id + "'");
                }
                else if (TitleUpdated)
                {
                    Data.DeleteRecords("Delete from [TitleInformation] where Report_Style_Id=" + "'" + ReportStyle_Id + "'");
                }

                OnErrorHappen?.Invoke("E016", ex);
            }                // Raise the error to the using application.

            Data = null;
            NewStyle = false;
            ShowDetail = false;
            CreateCsv = false;
            ReportNameCsv = "";
            IncludeRawData = false;
            RawDataSheetType = "ColumnWise";
            return true;
        }
        // ________________________________________This function is to handle the file password set button click event______________________________________________________________________
        private void filePassword_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(passwordBox.Text))
            {
                Password = passwordBox.Text;
            }

            passwordBox.Text = "";
            MessageBox.Show("Password setted successfully !! ", "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }
        // ____________________________________________This function handles the EditPredefined list button click action._______________________________________________________________
        private void ShowPredefinedList_Click(object sender, EventArgs e)
        {
            pnlTitlePredefinedList.Visible = true;
        }
        // ___________________________________________________This handles the predefined list SET button click._________________________________________________________________________
        private void btnSetList_Click(object sender, EventArgs e)
        {
            try
            {
                PreDefinedList.Clear();                      // Clear the predefined list.
                var Comm = new OleDbCommand("Delete * from PreExistingFieldValue", new OleDbConnection(DatabaseConstants.ConnectionString));   // delete all earlier lists from the table.
                Comm.Connection.Open();
                Comm.ExecuteNonQuery();
                Comm.Connection.Close();
                Comm.Dispose();
                Data.DataA = new OleDbDataAdapter("Select * From PreExistingFieldValue", DatabaseConstants.ConnectionString);    // The table is filled to update the new values.
                Data.DataA.Fill(Data.DataS, "PreExistingFieldValue");
                Data.DataS.Tables["PreExistingFieldValue"].Rows.Clear();
                for (int i = 0, loopTo = txtPredefinedList.Lines.Length - 1; i <= loopTo; i++)
                {
                    PreDefinedList.Add(txtPredefinedList.Lines[i]);
                    PreDefinedList[i].Trim();
                    var D = Data.DataS.Tables["PreExistingFieldValue"].NewRow();
                    D[1] = PreDefinedList[i];
                    Data.DataS.Tables["PreExistingFieldValue"].Rows.Add(D);
                }

                Data.Update_Table("PreExistingFieldValue");
                Data.DataS.Tables.Remove("PreExistingFieldValue");
                pnlTitlePredefinedList.Visible = false;
                MessageBox.Show("List updated successfully !!", "Excel Module ", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E017", ex);
            }
        }
        // _______________________Its The Main Function tht will actually write to the excel file. Taking all conditions internally._____________________________________________________
        private bool Writing_to_Report(bool WriteAgain = false, bool Fasttransfer = false)
        {
            try
            {
                int srNo = 0;
                int templateno = 1;
                // If Fasttransfer And ToleranceFormat <> 0 Then
                // ToleranceFormat = SetToleranceFormat()
                // End If
                bool InsertedNewRowinExcelSheet = default, InsertedNewColumninExcelSheet = default, NewReportFirstTimeForMultiPP = default;
                if (Fasttransfer)
                {
                    MultiProgramReport = false;
                    UseExistingReport = false;
                }

                if (MultiProgramReport)
                {
                    if (ReportType.Contains("Column"))
                    {
                        if (LastRowUsed == Initial_Row + Header - 1)
                        {
                            WriteAgain = false;
                            LastRowUsed += 1;
                            UseExistingReport = true;
                            NewReportFirstTimeForMultiPP = true;
                        }
                    }
                    else if ((LastColumnUsed ?? "") == (Initial_Column ?? ""))
                    {
                        WriteAgain = false;
                        LastColumnUsed = GetColumnName(GetColIndex(LastColumnUsed) + 1);
                        UseExistingReport = true;
                        NewReportFirstTimeForMultiPP = true;
                    }
                }

                if (!WriteAgain)
                {
                    if (IsHCM)
                    {
                        if (HCMno == 0)
                        {
                            xL.InitializeExcelApplication(ReportTemplateName, "Report");
                            MeasurementStringList.Clear();
                            EnterHobParameterData();
                        }
                        else
                        {
                            xL.InitializeExcelApplication(ReportTemplateName, "Data");
                        }
                    }
                    else if (IsDTVSM)
                    {
                        xL.InitializeExcelApplication(ReportTemplateName, "Report");
                    }
                    else if (!UseExistingReport)
                    {
                        xL.InitializeExcelApplication(ReportTemplateName, "Report");
                    }
                    else
                    {
                        NewProgramParam();
                        xL.InitializeExcelApplication(ReportName, SelectedSheet);
                        // If NewReportFirstTimeForMultiPP Then
                        // LastRowUsed -= 1
                        // End If
                    }

                    if (!IsHCM)
                    {
                        if (!Fasttransfer)
                        {
                            if (NewReportFirstTimeForMultiPP)
                            {
                                var MultiRptInfo = Data.Select_Rows("MultiProgramReport", "Report_Id", DataR[0]);
                                int prevpgn = 0;
                                int pgn;
                                var MaxReads = new double[3];
                                var currReads = new int[3];
                                bool AddInitialTemplates = true;
                                for (int kk = 0, loopTo = MultiRptInfo.Length - 1; kk <= loopTo; kk++)
                                {
                                    currReads = GetRowandColumnfromRange(Conversions.ToString(MultiRptInfo[kk][4]));
                                    if (ReportType.Contains("Column"))
                                    {
                                        // pgn = GetColIndex(MultiRptInfo(kk).Item(8))
                                        if (currReads[1] > MaxReads[1])
                                            MaxReads[1] = currReads[1];
                                        if (Conversions.ToBoolean(Operators.ConditionalCompareObjectGreater(MultiRptInfo[kk][7], MultiRptInfo[kk][5], false)))
                                            AddInitialTemplates = false;
                                    }
                                    else if (ReportType.Contains("Row"))
                                    {
                                        if (currReads[0] > MaxReads[0])
                                            MaxReads[0] = currReads[0]; // MultiRptInfo(kk).Item(7)
                                        if (GetColIndex(Conversions.ToString(MultiRptInfo[kk][8])) > GetColIndex(Conversions.ToString(MultiRptInfo[kk][6])))
                                            AddInitialTemplates = false;
                                    }
                                }

                                pgn = 1;
                                if (AddInitialTemplates)
                                {
                                    if (ReportType.Contains("Column"))
                                    {
                                        MaxReads[1] = MaxReads[1] / GetColIndex(ColumnMax);
                                        while (MaxReads[1] > 1d)
                                        {
                                            // pgn = CInt(Int(GetColIndex(MultiRptInfo(kk).Item(6)) / GetColIndex(ColumnMax)))
                                            string tempInit_Col;
                                            tempInit_Col = GetColumnName(pgn * GetColIndex(ColumnMax) + GetColIndex(Initial_Column));
                                            // ColumnAfterWhich_PaginationShouldBeDone = GetColumnName((pgn + 1) * (GetColIndex(ColumnMax)))
                                            // If Not pgn = prevpgn Then
                                            // If PageNotAlreadyPasted(MultiRptInfo(kk).Item(6), MultiRptInfo(kk).Item(5), False) Then
                                            xL.Paste(SelectedDataSheet, SourceFinalRange, SourceInitialRange, Conversions.ToString(Operators.ConcatenateObject(tempInit_Col, Operators.AddObject(Operators.SubtractObject(MultiRptInfo[0][5], Header), 1)))); // Its Time to do the pagination.
                                            xL.ReleaseExtraDatasheet();
                                            // End If
                                            MaxReads[1] -= 1d;
                                            pgn += 1;
                                        }
                                    }
                                    else if (ReportType.Contains("Row"))
                                    {
                                        MaxReads[0] = MaxReads[0] / (RowMax + Footer) - 1d;
                                        // pgn = CInt(Int(MultiRptInfo(kk).Item(5) / (RowMax + Footer)))
                                        while (MaxReads[0] > 1d)
                                        {
                                            RowAfterWhich_PaginationShouldBeDone = pgn * (RowMax + Footer) - Footer;
                                            // If Not pgn = prevpgn Then

                                            // If PageNotAlreadyPasted(MultiRptInfo(kk).Item(6), MultiRptInfo(kk).Item(5), True) Then
                                            xL.Paste(SelectedDataSheet, SourceFinalRange, SourceInitialRange, Conversions.ToString(Operators.ConcatenateObject(MultiRptInfo[0][6], (pgn * (RowMax + Footer) + 1).ToString())));  // 'GetColumnName(GetColIndex(Tole_Column) + 1)   'Its Time to paste the template.
                                            xL.ReleaseExtraDatasheet();
                                            // End If
                                            MaxReads[0] -= 1d;
                                            pgn += 1;
                                        }
                                    }

                                    // prevpgn = pgn
                                    // For kk As Integer = 0 To MultiRptInfo.Length - 1
                                    // WritingTolerances(MultiRptInfo(kk).Item(5), MultiRptInfo(kk).Item(6), ReportType, ToleranceFormat, True)
                                    // Next
                                    // Dim dc As String = "F"
                                    // Dim dr As Integer = 5
                                    // SetPaginationLimits(dc, dr)
                                }
                            }

                            bool PastedSheetjustNow = false;
                            if (ReportType.Contains("Column"))
                            {
                                // 'Check if we already have a page at initial field row and initial field column
                                int pgn = (int)Math.Round(Conversion.Int(GetColIndex(InitialField_Column) / (double)GetColIndex(ColumnMax)));
                                string tempInit_Col;
                                // 'If pgn = 0 Then tempInit_Col = Initial_Column Else
                                tempInit_Col = GetColumnName(pgn * GetColIndex(ColumnMax) + GetColIndex(Initial_Column));
                                int diff = GetColIndex(InitialField_Column) - pgn * GetColIndex(ColumnMax);
                                

                                // 'Now check if left most
                                if (pgn > 0 && diff == 1)
                                {
                                    if (PageNotAlreadyPasted(InitialField_Column, InitialField_Row))
                                    {
                                        xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, "Report", tempInit_Col + Initial_Row);
                                        PastedSheetjustNow = true;
                                    }
                                }
                            }
                            else if (ReportType.Contains("Row"))
                            {
                                // 'Check if we already have a page at initial field row and initial field column
                                int pgSize = RowMax + Initial_Row - 1 + Footer;
                                int pgn1 = (int)Math.Round(Conversion.Int(InitialField_Row / (double)pgSize));
                                int diff = InitialField_Row - pgn1 * pgSize;
                                if (pgn1 > 0 && diff == Header)
                                {
                                    if (PageNotAlreadyPasted(InitialField_Column, InitialField_Row))
                                    {
                                        xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, "Report", InitialField_Column + (pgn1 * pgSize + Initial_Row));
                                        PastedSheetjustNow = true;
                                    }
                                }
                                else if (diff == RowMax || diff == RowMax + 1)
                                {
                                    InitialField_Row = (pgn1 + 1) * pgSize + Header + Initial_Row - 1;
                                    if (PageNotAlreadyPasted(InitialField_Column, InitialField_Row))
                                    {
                                        xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, "Report", InitialField_Column + ((pgn1 + 1) * pgSize + Initial_Row));
                                        PastedSheetjustNow = true;
                                    }
                                }
                            }

                            int P_index = ProgramName.LastIndexOf(@"\");
                            int P_index2 = ProgramName.LastIndexOf(".");
                            string tmpStr = ProgramName.Substring(P_index + 1, P_index2 - P_index - 1);
                            int pN_L_row, pN_L_col = default;

                            // pN_L_row = InitialField_Row - PartNameLabelLocation_Row
                            // pN_L_col = GetColIndex(Initial_Column) - GetColIndex(PartNameLabelLocation_Col)
                            pN_L_row = InitialField_Row + PartNameLabelLocation_Row;
                            pN_L_col = GetColIndex(InitialField_Column) + pN_L_col;

                            //-----------------Changes made------------------------------
                            xL.cell = xL.sheet.CreateRow(pN_L_row).CreateCell(pN_L_col);
                            xL.cell.SetCellValue("Part:");
                            xL.cell.CellStyle.Alignment = NPOI.SS.UserModel.HorizontalAlignment.Center;
                            //----------------Instead of below lines---------------------
                            //xL[pN_L_row, GetColumnName(pN_L_col)] = "Part :";
                            //xL.set_Set_Alignment(pN_L_row, GetColumnName(pN_L_col), (int)XlHAlign.xlHAlignRight);
                            //-----------------------------------------------------------

                            // pN_L_row = InitialField_Row - PartName_Location_Row
                            // pN_L_col = GetColIndex(Initial_Column) + PartName_Location_Col
                            pN_L_row = InitialField_Row + PartName_Location_Row;
                            pN_L_col = GetColIndex(InitialField_Column) + PartName_Location_Col;

                            //-----------------Changes made------------------------------
                            xL.cell = xL.sheet.CreateRow(pN_L_row).CreateCell(pN_L_col);
                            xL.cell.SetCellValue(tmpStr);
                            xL.cell.CellStyle.Alignment = NPOI.SS.UserModel.HorizontalAlignment.Center;
                            //----------------Instead of below lines---------------------
                            //xL[pN_L_row, GetColumnName(pN_L_col)] = tmpStr;
                            //xL.set_Set_Alignment(pN_L_row, GetColumnName(pN_L_col), (int)XlHAlign.xlHAlignLeft);
                            //xL.Get_Range(pN_L_row, GetColumnName(pN_L_col)).Font.Bold = true;
                            //-----------------------------------------------------------

                            // 'Original working code
                            // xL.Get_Set_Value(InitialField_Row - 1, GetColumnName(GetColIndex(InitialField_Column))) = "Part :"
                            // xL.Set_Alignment(InitialField_Row - 1, GetColumnName(GetColIndex(InitialField_Column))) = Excel.XlHAlign.xlHAlignRight
                            // xL.Get_Set_Value(InitialField_Row - 1, GetColumnName(GetColIndex(InitialField_Column) + 1)) = tmpStr
                            // xL.Set_Alignment(InitialField_Row - 1, GetColumnName(GetColIndex(InitialField_Column) + 1)) = Excel.XlHAlign.xlHAlignLeft
                            // xL.Get_Range(InitialField_Row - 1, GetColumnName(GetColIndex(InitialField_Column) + 1)).Font.Bold = True

                            if (UseExistingReport)
                            {
                                if (!AllProgramInSingleReport)
                                {
                                    Worksheet xlsht;
                                    xlsht = (Worksheet)xL.workbook;
                                    //xlsht = (Worksheet)xL.xlW.worksheets.Add;
                                    xlsht.Name = tmpStr + "Report";
                                    xL.workbook.CreateSheet(xlsht.Name);
                                    //xL.xlW.Sheets(xlsht.Name).Select();
                                    //xL.xlW.Sheets(xlsht.Name).Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
                                    xL.xlS = xL.workbook.CreateSheet(xlsht.Name);
                                    SelectedSheet = xlsht.Name;
                                    Worksheet xlDatasht;
                                    xlDatasht = (Worksheet)xL.workbook.CreateSheet();
                                    //xlDatasht = (Worksheet)xL.xlW.worksheets.Add;
                                    xlDatasht.Name = tmpStr + "Data";
                                    SelectedDataSheet = xlDatasht.Name;
                                    //xL.xlW.Sheets(xlsht.Name).Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
                                }

                                if (!MultipleReportsInDifferentPages)
                                {
                                }
                                // xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, SelectedSheet, TemplateInitialRange)
                                // xL.ClearFormatCondition(SelectedSheet, TemplateFinalRange, TemplateInitialRange)
                                // xL.ClearData(SelectedDataSheet, TemplateFinalRange, InitialField_Column & InitialField_Row)
                                // xL.Paste(SelectedDataSheet, TemplateFinalRange, TemplateInitialRange, TemplateInitialRange)          'Its time to paste the template.
                                else
                                {
                                    xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, SelectedDataSheet, TemplateInitialRange);
                                    xL.ClearFormatCondition(SelectedDataSheet, TemplateFinalRange, TemplateInitialRange);
                                    xL.ClearData(SelectedDataSheet, TemplateFinalRange, InitialField_Column + InitialField_Row);
                                    xL.Paste(SelectedDataSheet, TemplateFinalRange, TemplateInitialRange, TemplateInitialRange);
                                }          // Its time to paste the template.
                                           // If ReportType.Contains("Column") Then
                                           // xL.Get_Set_Value(InitialField_Row - 1, GetColumnName(GetColIndex(InitialField_Column) + 1)) = tmpStr
                                           // Else
                                           // End If
                            }

                            xL[InitialField_Row, InitialField_Column] = "S.No.";
                            if (!MultipleReportsInDifferentPages && !PastedSheetjustNow)
                                xL.ReleaseExtraDatasheet();
                        }
                    }

                    // ----------------------------------------------------------------------------------------------------------------------------
                    // ColumnAfterWhich_PaginationShouldBeDone = ColumnMax ' for the columnWise reports 
                    // RowAfterWhich_PaginationShouldBeDone = RowMax + InitialField_Row - Header 'for the rowwise reports
                    // Fieldrows = Data.DataS.Tables("FieldInformation").Select()
                    if (!IsHCM)
                        WritingTolerances(InitialField_Row, InitialField_Column, ReportType, ToleranceFormat, true);
                    if (ShowDetail)
                        CreateDetailSheet();
                    if (IncludeRawData)
                        CreateRawSheet();

                    // --------------------------Fasttransfer is to create just one time measurement report-------------------------------------------------------------
                    if (!Fasttransfer & !IsHCM)
                    {
                        if (!UseExistingReport)
                        {
                            // xL.Copy(TemplateFinalRange, Initial_Column & Initial_Row, "Data", Initial_Column & Initial_Row)
                            xL.ReleaseExtraDatasheet();
                            if (!string.IsNullOrEmpty(Password))
                            {
                                //xL.xlW.SaveAs(TemplateFolderName + @"\" + ReportStyleName + "_Template.xls", default, default, Password);
                            }
                            else
                            {
                                //xL.xlW.SaveAs(TemplateFolderName + @"\" + ReportStyleName + "_Template.xls");
                            }
                        }
                        else
                        {
                        }
                    }
                    // ------------------------------------------Writing the title information into the excel file first. ------------------------------------------------
                    for (int i = 0, loopTo1 = _extraInformationDataView.Rows.Count - 1; i <= loopTo1; i++)
                    {
                        int Title_Row = Conversions.ToInteger(_extraInformationDataView.Rows[i].Cells[6].Value); // First writing the title name,The row  information
                        string Title_Col = Conversions.ToString(_extraInformationDataView.Rows[i].Cells[5].Value); // The column Information 
                        string Title_Name = Conversions.ToString(_extraInformationDataView.Rows[i].Cells[2].Value); // The title's name 
                        xL[Title_Row, Title_Col] = Title_Name;
                        xL.set_Set_Alignment(Title_Row, Title_Col, -4108); // Left Alignment for the title name 
                        int Next_Col_TitleValue = GetColIndex(Strings.StrConv(Title_Col, VbStrConv.Uppercase));      // Calculating the next coloumn for putting the Title's Value 
                        string Col_Next = GetColumnName(Next_Col_TitleValue + 1);
                        string Title_Value = Conversions.ToString(_extraInformationDataView.Rows[i].Cells[4].Value);
                        xL[Title_Row, Col_Next] = Title_Value;
                        xL.set_Set_Alignment(Title_Row, Col_Next, -4152); // Right Alignment for the title value 
                    }
                }
                // _______________________________________________________________________________________________________________________________________________________________
                // --------------------------------------WriteAgain  -----------------------------------------
                // 'Calculate Serial Number, check for pagination and insert new page and fill with headings etc. 
                else if (ReportType.Contains("Column"))
                {
                    if (LastRowUsed == InitialField_Row + 1)
                    {
                        srNo = 0;
                    }
                    else if (LastRowUsed == InitialField_Row)
                    {
                        WritingTolerances(InitialField_Row, LastColumnUsed, ReportType, ToleranceFormat, true);
                        srNo = 0;
                    }
                    else
                    {
                        for (int i = LastRowUsed - 1, loopTo2 = InitialField_Row + 1; i >= loopTo2; i -= 1)  // Bcoz upto 8 the row the serial no (Sr.No.) is written.
                        {
                            if (xL[i, InitialField_Column] is null)
                            {
                                continue;
                            }

                            if (_IsDoPagination)
                            {
                                if (i == RowAfterWhich_PaginationShouldBeDone) // MaximumFilledRowPerTemplate Then
                                {
                                    // Dim frr As Double
                                    int pgn = (int)Math.Round(Conversion.Int(GetColIndex(InitialField_Column) / (double)GetColIndex(ColumnMax)));
                                    string tempInit_Col;
                                    // 'If pgn = 0 Then tempInit_Col = Initial_Column Else
                                    tempInit_Col = GetColumnName(pgn * GetColIndex(ColumnMax) + GetColIndex(Initial_Column));
                                    // 'Now check if left most
                                    if (PageNotAlreadyPasted(InitialField_Column, i + Footer + 1))
                                    {
                                        xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, "Report", tempInit_Col + (i + Footer + 1));
                                    }
                                    // 'Remake the page by calling WRitingTolerances!
                                    WritingTolerances(i + Footer + Header, InitialField_Column, ReportType, ToleranceFormat, true);
                                    // xL.Paste(SelectedDataSheet, SourceFinalRange, SourceInitialRange, GetColumnName(GetColIndex(ColumnAfterWhich_PaginationShouldBeDone) + 1) & i + Footer + 1) 'Its Time to do the pagination.
                                    // xL.Paste("Data", SourceFinalRange, Initial_Column & (Initial_Row), Initial_Column & (i + Footer + 1))          'Its time to paste the template.
                                    // xL.ReleaseExtraDatasheet()
                                    // MaximumFilledRowPerTemplate += RowMax + Footer
                                    LastRowUsed = i + InitialField_Row + Footer + 1; // RowMax + 
                                    if (IncludeTolerence)
                                    {
                                        if (Conversions.ToBoolean(Operators.OrObject(Operators.ConditionalCompareObjectEqual(DataR[6], 1, false), Operators.ConditionalCompareObjectEqual(DataR[6], 2, false))))
                                        {
                                            LastRowUsed = i + InitialField_Row + Footer + 2; // + RowMax
                                        }
                                        else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(DataR[6], 3, false)))
                                        {
                                            LastRowUsed = i + InitialField_Row + Footer + 4; // RowMax 
                                        }
                                    }

                                    if (ShwfieldNum)
                                    {
                                        LastRowUsed += 1;
                                    }

                                    srNo = Conversions.ToInteger(xL[i, InitialField_Column]);
                                    break;
                                }
                                else
                                {
                                CheckValue:
                                    ;
                                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectNotEqual(xL[i, InitialField_Column], null, false)))
                                    {
                                        try
                                        {
                                            LastRowUsed = i + 1;
                                            srNo = Conversions.ToInteger(xL[i, InitialField_Column]);
                                        }
                                        catch (Exception ex)
                                        {
                                            srNo = 0;
                                        }

                                        break;
                                    }
                                }
                            }
                            else
                            {
                                string ss = xL[i, InitialField_Column].ToString();
                                string ssNext;
                                try
                                {
                                    ssNext = xL[i + 1, InitialField_Column].ToString();
                                }
                                catch (Exception exx)
                                {
                                    ssNext = null;
                                }

                                if (ss is object)
                                {
                                    int sno2;
                                    if (ssNext is object)
                                    {
                                        if (int.TryParse(ss.Trim(), out sno2)) // Not IsNumeric(ss.Trim) Then
                                        {
                                            LastRowUsed = i + 1;
                                            //var templastrow = xL.sheet.LastRowNum;
                                            //xL.cell.SetCellValue(XlInsertShiftDirection.xlShiftDown, XlInsertFormatOrigin.xlFormatFromLeftOrAbove);
                                            InsertedNewRowinExcelSheet = true;
                                            srNo = sno2;
                                        }
                                        // LastRowUsed += 1
                                        else
                                        {
                                            int ssNo;
                                            if (int.TryParse(Conversions.ToString(xL[LastRowUsed - 1, InitialField_Column]), out ssNo))
                                            {
                                                srNo = ssNo;
                                            }
                                            else
                                            {
                                                srNo = 0;
                                            }
                                            // ' srNo = CInt(ss.Trim)
                                        }
                                    }
                                    else
                                    {
                                        srNo = Conversions.ToInteger(ss.Trim());
                                    }
                                }
                                else // If xL.Get_Set_Value(i, InitialField_Column) Is Nothing Then
                                {
                                    LastRowUsed = i;
                                    try
                                    {
                                        srNo = Conversions.ToInteger(xL[i - 1, InitialField_Column]);
                                    }
                                    catch (InvalidCastException ex)
                                    {
                                        srNo = 0;
                                    }
                                }

                                break;
                            }
                        }
                    }
                }
                else if (LastColumnUsed == "B")                          // For RowWise Reports
                {
                    srNo = 0;
                }
                else if (LastColumnUsed == "A")
                {
                    WritingTolerances(InitialField_Row, LastColumnUsed, ReportType, ToleranceFormat, true);
                    LastColumnUsed = "B";
                    srNo = 0;
                }
                else
                {
                    for (int i = GetColIndex(LastColumnUsed) - 1; i >= 2; i -= 1) // bcoz upto first column Sr No is written
                    {
                        if (xL[InitialField_Row, GetColumnName(i)] is null)
                        {
                            continue;
                        }

                        if (_IsDoPagination)
                        {
                            if ((GetColumnName(i) ?? "") == (ColumnAfterWhich_PaginationShouldBeDone ?? "")) // ' MaximumFilledColumnPerTemplate Then
                            {
                                int pgSize = RowMax + Initial_Row - 1 + Footer;
                                int pgn = (int)Math.Round(Conversion.Int(InitialField_Row / (double)pgSize));
                                if (PageNotAlreadyPasted(GetColumnName(i + Sider + 1), InitialField_Row))
                                {
                                    xL.CopyBetweenTwoSheets("Data", SourceFinalRange, SourceInitialRange, "Report", GetColumnName(i + Sider + 1) + (pgn * pgSize + Initial_Row));
                                }

                                WritingTolerances(InitialField_Row, GetColumnName(i + Sider + 1), ReportType, ToleranceFormat, true);

                                // MaximumFilledColumnPerTemplate = GetColumnName((GetColIndex(MaximumFilledColumnPerTemplate) + Sider + GetColIndex(ColumnMax)))
                                // xL.Paste("Data",
                                // TemplateFinalRange,
                                // Initial_Column & (InitialField_Row - Header + 1),
                                // GetColumnName(i + Sider + 1) & (InitialField_Row - Header + 1))        'Its time to paste the template.
                                // xL.ReleaseExtraDatasheet()
                                // ---------------------make the variables to be updated.--------------------------
                                srNo = Conversions.ToInteger(xL[InitialField_Row, GetColumnName(i)]);
                                int colGap = GetColIndex(ColumnMax) - GetColIndex(Initial_Column) + 1;
                                LastColumnUsed = GetColumnName(i + GetColIndex(InitialField_Column) + Sider + 1);
                                if (IncludeTolerence)
                                {
                                    if (Conversions.ToBoolean(Operators.OrObject(Operators.ConditionalCompareObjectEqual(DataR[6], 1, false), Operators.ConditionalCompareObjectEqual(DataR[6], 2, false))))
                                    {
                                        LastColumnUsed = GetColumnName(i + GetColIndex(InitialField_Column) + Sider + 2);
                                    }
                                    else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(DataR[6], 3, false)))
                                    {
                                        LastColumnUsed = GetColumnName(i + GetColIndex(InitialField_Column) + Sider + 4);
                                    }
                                }

                                if (ShwfieldNum)
                                {
                                    LastColumnUsed = GetColumnName(i + GetColIndex(InitialField_Column) + 1);
                                }

                                break;
                            }
                            else
                            {
                            CheckValue1:
                                ;
                                try
                                {
                                    LastColumnUsed = GetColumnName(i + 1);
                                    srNo = Conversions.ToInteger(xL[InitialField_Row, GetColumnName(i)]);
                                }
                                catch (Exception ex)
                                {
                                    srNo = 0;
                                }

                                break;
                            }
                        }
                        else
                        {
                            string str = GetColumnName(i);
                            string ss = Conversions.ToString(xL[InitialField_Row, str]);
                            string ssNext;
                            try
                            {
                                ssNext = xL[InitialField_Row, GetColumnName(i + 1)].ToString(); // ', InitialField_Column).ToString
                            }
                            catch (Exception exx)
                            {
                                ssNext = null;
                            }

                            int snolast;
                            if (ss is object)
                            {
                                if (ssNext is object)
                                {
                                    try
                                    {
                                        snolast = Conversions.ToInteger(ss.Trim());
                                        LastColumnUsed = GetColumnName(i + 1); // ' (str)
                                        //xL.xlS.Columns(LastColumnUsed).Insert(XlInsertShiftDirection.xlShiftToRight, XlInsertFormatOrigin.xlFormatFromLeftOrAbove);
                                        InsertedNewColumninExcelSheet = true;
                                        srNo = snolast;
                                    }
                                    catch (Exception ex33)
                                    {
                                        // Dim ssNo As Integer
                                        // If Integer.TryParse(CInt(xL.Get_Set_Value(InitialField_Row, GetColumnName(GetColIndex(str) - 1))), ssNo) Then
                                        // srNo = ssNo
                                        // Else
                                        srNo = 0;
                                        // End If
                                    }
                                    // If Integer.TryParse(ss.Trim, snolast) Then
                                    // Else
                                    // End If
                                    break;
                                }
                                else // If xL.Get_Set_Value(InitialField_Row, str) Is Nothing Then
                                {
                                    // 'srNo = CInt(ss.Trim)
                                    int.TryParse(ss.Trim(), out srNo);
                                    break;
                                }
                            }
                            else
                            {
                                LastColumnUsed = str;
                                try
                                {
                                    srNo = Conversions.ToInteger(xL[InitialField_Row, GetColumnName(i - 1)]); // GetColIndex(str)
                                }
                                catch (InvalidCastException ex)
                                {
                                    srNo = 0;
                                }

                                break;
                            }
                        }
                    }
                }
                // ColumnAfterWhich_PaginationShouldBeDone = ColumnMax                 'for the columnWise reports
                // RowAfterWhich_PaginationShouldBeDone = InitialField_Row - Header + RowMax                       'for the rowwise reports
                string dummyCol = "F";
                int dummyRow = 5;
                if (ReportType.Contains("Column"))
                {
                    dummyCol = Initial_Column;
                    dummyRow = LastRowUsed - 1;
                }
                else
                {
                    dummyCol = GetColumnName(GetColIndex(LastColumnUsed) - 1);
                    dummyRow = Initial_Row;
                }

                SetPaginationLimits(ref dummyCol, ref dummyRow);
                if (UpdateLastReport)
                {
                    srNo -= 1;
                    LastRowUsed -= 1;
                    LastColumnUsed = GetColumnName(GetColIndex(LastColumnUsed) - 1);
                    UpdateLastReport = false;
                }

                ComponentNo = srNo + 1;
                // --------------------------------writing field values for GridImagecapture--------------------------------------------------------------
                // Vinod Jakhar..  07/05/2014 
                if (IsGrigImagecaptureM)
                {
                    int a = GetColIndex(InitialField_Column);
                    if (IsGrigImagecaptureM)
                    {
                        a = 0;         // we need Reference have to put ok not ok from first column
                        CompareAndWrite_FieldValues("", 8, GetColumnName(a + 1), NumberFormat: Formatdigits);       // there is  Sr. no. in template we dont need it for grid program report
                    }

                    a = 1;
                    for (int i = 0, loopTo3 = MeasurementStringList.Count - 1; i <= loopTo3; i++)
                    {
                        if (a < GridImgCaptureCols + 1)
                        {
                            a = a + 1;   // go to next column in a row
                        }
                        else
                        {
                            a = 2;       // for new row start from 1st column
                            LastRowUsed = LastRowUsed + 1;
                        }   // increment row no.

                        int FieldInt;
                        int.TryParse(Conversions.ToString(MeasurementStringList[i][2]), out FieldInt);
                        string temp = "";
                        if (FieldInt == 1)
                        {
                            temp = "Ok";
                        }
                        else if (FieldInt == 0)
                        {
                            temp = "Not Ok";
                        }
                        else
                        {
                            temp = "NA";
                        }

                        CompareAndWrite_FieldValues(temp, LastRowUsed, GetColumnName(a), NumberFormat: Formatdigits);
                        if (FieldInt == 1)
                        {
                            xL.set_Set_Color(LastRowUsed, GetColumnName(a), 4);        // 4 means green colour
                        }
                        else if (FieldInt == 0)
                        {
                            xL.set_Set_Color(LastRowUsed, GetColumnName(a), 3);        // 3 means red colour
                        }
                        else
                        {
                            xL.set_Set_Color(LastRowUsed, GetColumnName(a), 2);
                        }        // 2 means white
                    }

                    IsGrigImagecaptureM = false;
                    if (passStatus == PassFail.pass)
                    {
                        CompareAndWrite_FieldValues("PASS", LastRowUsed, GetColumnName(a), NumberFormat: Formatdigits);
                        xL.set_Set_Color(LastRowUsed, GetColumnName(a), 35);
                    }
                    else if (passStatus == PassFail.fail)
                    {
                        CompareAndWrite_FieldValues("FAIL", LastRowUsed, GetColumnName(a), NumberFormat: Formatdigits);
                        xL.set_Set_Color(LastRowUsed, GetColumnName(a), 46);
                    }
                }

                // ------------Writing Hob Checker Reports to Balzers' style
                else if (IsHCM)
                {
                    // 'Directly and simply write the values to Excel directly. 
                    int Currentrow = InitialField_Row;
                    int numberofPts;
                    Currentrow = 4;
                    var columnTitle = new string[] { "FSE", "TIR", "GLE" };
                    string currentcolumn = "B";
                    if (HCMno > 0)
                    {
                        for (int ii = 0; ii <= 40; ii++)
                        {
                            var vall = xL[2, GetColumnName(ii)];
                            if (vall is object && vall.ToString().Contains(columnTitle[HCMno - 1]))
                            {
                                currentcolumn = GetColumnName(ii);
                                break;
                            }
                        }
                    }

                    switch (HCMno)
                    {
                        case 0:
                            {
                                return true;
                            }

                        default:
                            {
                                numberofPts = MeasurementStringList.Count - 1;
                                for (int i = 0, loopTo6 = numberofPts; i <= loopTo6; i++)
                                {
                                    CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), Currentrow, currentcolumn, NumberFormat: 4);
                                    Currentrow += 1;
                                }

                                break;
                            }
                            // Case 2
                            // numberofPts = MeasurementStringList.Count - 1
                            // For i = 0 To numberofPts
                            // CompareAndWrite_FieldValues(MeasurementStringList.Item(i).Item(2), Currentrow, "B", , , 4)
                            // Currentrow += 1
                            // Next
                            // Case 3
                            // numberofPts = MeasurementStringList.Count - 1
                            // For i = 0 To numberofPts
                            // CompareAndWrite_FieldValues(MeasurementStringList.Item(i).Item(2), Currentrow, "AB", , , 4)
                            // Currentrow += 1
                            // Next
                    }
                }

                // --------------------------------writing field values for columnwise view--------------------------------------------------------------
                else if (ReportType.Contains("Column"))
                {
                    int CurrentUsedColumn = GetColIndex(InitialField_Column);
                    xL.cell = xL.sheet.CreateRow(LastRowUsed).CreateCell(CurrentUsedColumn);
                    xL.cell.SetCellValue((double)srNo + 1);
                    //xL[LastRowUsed, GetColumnName(CurrentUsedColumn)] = srNo + 1;
                    var fit = xL.cell.CellStyle.ShrinkToFit;
                    //xL.set_Set_Alignment(LastRowUsed, GetColumnName(CurrentUsedColumn), -4108);
                    //xL.Get_Range(LastRowUsed, GetColumnName(CurrentUsedColumn)).NumberFormat = "0";
                    //xL.Get_Range(LastRowUsed, GetColumnName(CurrentUsedColumn)).FormatConditions.Delete();
                    //xL.Get_Range(LastRowUsed, GetColumnName(CurrentUsedColumn)).EntireColumn.Autofit();
                    CurrentUsedColumn += 1;
                    for (int j = 0, loopTo7 = Fieldrows.Length - 1; j <= loopTo7; j++)
                    {
                        if ((GetColumnName(CurrentUsedColumn - 1) ?? "") != (ColumnAfterWhich_PaginationShouldBeDone ?? ""))
                        {
                            if (Information.IsDBNull(Fieldrows[j][5]))
                            {
                                CurrentUsedColumn += 1;
                                continue;
                            }

                            if (!string.IsNullOrEmpty(Fieldrows[j][13].ToString()))
                            {
                                for (int i = 0, loopTo8 = MeasurementStringList.Count - 1; i <= loopTo8; i++)
                                {
                                    // 'If MeasurementStringList.Item(i).Item(0).ToString = "Date" Then CurrentUsedColumn = GetColIndex(InitialField_Column)
                                    if ((Fieldrows[j][13].ToString() ?? "") != (MeasurementStringList[i][6].ToString() ?? ""))
                                        continue;

                                    // ' We got the correct column. let us enter the value
                                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(MeasurementStringList[i][0], "Date", false)))
                                    {
                                        CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, GetColumnName(CurrentUsedColumn), NumberFormat: -2);
                                    }
                                    else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(MeasurementStringList[i][0], "Time", false)))
                                    {
                                        CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, GetColumnName(CurrentUsedColumn), NumberFormat: -3);
                                    }
                                    else
                                    {
                                        CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, GetColumnName(CurrentUsedColumn), NumberFormat: Formatdigits);
                                    }

                                    if (Fieldrows[j][5].ToString().Contains(Conversions.ToString(degree)) && ToleranceFormat != 0)
                                    {
                                        bool LsAbsent = default, HsAbsent = default;
                                        string lsstring, hsstring;
                                        lsstring = Fieldrows[j][11].ToString();
                                        hsstring = Fieldrows[j][12].ToString();
                                        if (string.IsNullOrEmpty(lsstring) | lsstring == "0° 00' 00''" | lsstring == "0°00'00''")
                                            LsAbsent = true;
                                        if (string.IsNullOrEmpty(hsstring) | hsstring == "0° 00' 00''" | hsstring == "0°00'00''")
                                            HsAbsent = true;
                                        // If (Fieldrows(j)(11).ToString = "" Or Fieldrows(j)(11).ToString = "0° 00' 00''") Then LsAbsent = True
                                        // If (Fieldrows(j)(12).ToString = "" Or Fieldrows(j)(12).ToString = "0° 00' 00''") Then HsAbsent = True
                                        if (!LsAbsent && !HsAbsent)
                                        {
                                            if (ConvertDegMinSecToDecimal(Conversions.ToString(MeasurementStringList[i][2])) <= ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][11])) & ConvertDegMinSecToDecimal(Conversions.ToString(MeasurementStringList[i][2])) >= ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][12])))
                                            {
                                                xL.set_Set_Color(LastRowUsed, GetColumnName(CurrentUsedColumn), 35);        // 35 means green colour
                                            }
                                            else
                                            {
                                                xL.set_Set_Color(LastRowUsed, GetColumnName(CurrentUsedColumn), 46);
                                            }        // 46 means red colour
                                        }
                                    }

                                    if (ShowAvging && !_IsDoPagination) // 'Update Formulas for statistics if we have inserted a new row for entering values.
                                    {
                                        //if (InsertedNewRowinExcelSheet && !Fieldrows[j][5].ToString().Contains(Conversions.ToString(degree)))
                                        //{
                                        //    string formula_String;
                                        //    string currColumn = GetColumnName(CurrentUsedColumn);
                                        //    for (int ii = 1; ii <= 3; ii++)
                                        //    {
                                        //        formula_String = xL.xlS.Cells(LastRowUsed + ii, currColumn).Formula.ToString();
                                        //        if (!string.IsNullOrEmpty(formula_String))
                                        //        {
                                        //            int khj = formula_String.LastIndexOf(currColumn);
                                        //            formula_String = formula_String.Substring(0, khj + currColumn.Length);
                                        //            formula_String += LastRowUsed.ToString() + ")";
                                        //            xL.xlS.Cells(LastRowUsed + ii, currColumn).Formula = formula_String;
                                        //        }
                                        //    }
                                        //}
                                    }

                                    CurrentUsedColumn += 1;
                                    break;
                                }
                            }
                            else if (!WriteAgain)
                            {
                                CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][5]), LastRowUsed, GetColumnName(CurrentUsedColumn), NumberFormat: Formatdigits);
                            }
                        }
                        else
                        {
                            xL[LastRowUsed, GetColumnName(CurrentUsedColumn)] = srNo + 1;
                            j -= 1;
                            xL.set_Set_Alignment(LastRowUsed, GetColumnName(CurrentUsedColumn), -4108);
                            ColumnAfterWhich_PaginationShouldBeDone = GetColumnName(GetColIndex(ColumnAfterWhich_PaginationShouldBeDone) + GetColIndex(ColumnMax));
                            CurrentUsedColumn += 1;
                        }
                    }

                    if (passStatus == PassFail.pass)
                    {
                        CompareAndWrite_FieldValues("PASS", LastRowUsed, GetColumnName(CurrentUsedColumn), NumberFormat: Formatdigits);
                        xL.set_Set_Color(LastRowUsed, GetColumnName(CurrentUsedColumn), 35);
                    }
                    else if (passStatus == PassFail.fail)
                    {
                        CompareAndWrite_FieldValues("FAIL", LastRowUsed, GetColumnName(CurrentUsedColumn), NumberFormat: Formatdigits);
                        xL.set_Set_Color(LastRowUsed, GetColumnName(CurrentUsedColumn), 46);
                    }
                }
                else if (ReportType.Contains("TVS"))                          // for delphiTVS report measurement filling 16/04/2014
                {
                    CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[0][2]), 2, "E", NumberFormat: Formatdigits);                // print date @ appropriate place
                    CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[1][2]), 3, "E", NumberFormat: Formatdigits);                // print time @ appropriate place
                    int LAst_Row = LastRowUsed;
                    LastRowUsed = 7;
                    int Reminder;
                    for (int i = 2, loopTo9 = MeasurementStringList.Count - 1; i <= loopTo9; i++)
                    {
                        Math.DivRem(i, 4, out Reminder);
                        if (Reminder == 2)                                                    // Item(2) means Content of measurement.
                        {
                            CompareAndWrite_FieldValues(((i - 2) / 4d + 1d).ToString(), LastRowUsed, "A", NumberFormat: Formatdigits);        // print serial no.
                            CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, "B", NumberFormat: Formatdigits);      // 4 columns for each measurement
                        }
                        else if (Reminder == 3)
                        {
                            CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, "C", NumberFormat: Formatdigits);
                        }
                        else if (Reminder == 0)
                        {
                            CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, "D", NumberFormat: Formatdigits);
                        }
                        else if (Reminder == 1)
                        {
                            CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), LastRowUsed, "E", NumberFormat: Formatdigits);
                            LastRowUsed += 1;
                        }
                    }

                    if (passStatus == PassFail.pass)
                    {
                        CompareAndWrite_FieldValues("PASS", LastRowUsed, "E", NumberFormat: Formatdigits);
                        xL.set_Set_Color(LastRowUsed, "E", 35);
                    }
                    else if (passStatus == PassFail.fail)
                    {
                        CompareAndWrite_FieldValues("FAIL", LastRowUsed, "E", NumberFormat: Formatdigits);
                        xL.set_Set_Color(LastRowUsed, "E", 46);
                    }

                    LastRowUsed = LAst_Row;
                }
                else
                {
                    // --------------------------------writing field values for rowwise view--------------------------------------------------------------
                    int Currentrow = InitialField_Row;
                    xL.cell = xL.sheet.CreateRow(Currentrow).CreateCell(GetColIndex(LastColumnUsed));
                    xL.cell.SetCellValue((double)srNo + 1);
                    //xL[Currentrow, LastColumnUsed] = srNo + 1;
                    //xL.set_Set_Alignment(Currentrow, LastColumnUsed, -4108);
                    //xL.Get_Range(Currentrow, LastColumnUsed).NumberFormat = "0";
                    //xL.Get_Range(Currentrow, LastColumnUsed).FormatConditions.Delete();
                    Currentrow += 1;
                    for (int j = 0, loopTo4 = Fieldrows.Length - 1; j <= loopTo4; j++)
                    {
                        if (Currentrow - 1 != RowAfterWhich_PaginationShouldBeDone)
                        {
                            if (Information.IsDBNull(Fieldrows[j][5]))
                            {
                                Currentrow += 1;
                                continue;
                            }

                            if (!string.IsNullOrEmpty(Fieldrows[j][13].ToString()))
                            {
                                for (int i = 0, loopTo5 = MeasurementStringList.Count - 1; i <= loopTo5; i++)
                                {
                                    if ((Fieldrows[j][13].ToString() ?? "") != (MeasurementStringList[i][6].ToString() ?? ""))
                                        continue;
                                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(MeasurementStringList[i][0], "Date", false)))
                                    {
                                        CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), Currentrow, LastColumnUsed, NumberFormat: -2);
                                    }
                                    else if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(MeasurementStringList[i][0], "Time", false)))
                                    {
                                        CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), Currentrow, LastColumnUsed, NumberFormat: -3);
                                    }
                                    else
                                    {
                                        CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), Currentrow, LastColumnUsed, NumberFormat: Formatdigits);
                                    }

                                    // CompareAndWrite_FieldValues(MeasurementStringList.Item(i).Item(2), Currentrow, LastColumnUsed, , , Formatdigits)
                                    if (Fieldrows[j][5].ToString().Contains(Conversions.ToString(degree)) & ToleranceFormat != 0)
                                    {
                                        bool LsAbsent = default, HsAbsent = default;
                                        string lsstring, hsstring;
                                        lsstring = Fieldrows[j][11].ToString();
                                        hsstring = Fieldrows[j][12].ToString();
                                        if (string.IsNullOrEmpty(lsstring) | lsstring == "0° 00' 00''" | lsstring == "0°00'00''")
                                            LsAbsent = true;
                                        if (string.IsNullOrEmpty(hsstring) | hsstring == "0° 00' 00''" | hsstring == "0°00'00''")
                                            HsAbsent = true;
                                        if (!LsAbsent && !HsAbsent)
                                        {
                                            if (ConvertDegMinSecToDecimal(Conversions.ToString(MeasurementStringList[i][2])) <= ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][11])) & ConvertDegMinSecToDecimal(Conversions.ToString(MeasurementStringList[i][2])) >= ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][12])))
                                            {
                                                xL.set_Set_Color(Currentrow, LastColumnUsed, 35);
                                            }
                                            else
                                            {
                                                xL.set_Set_Color(Currentrow, LastColumnUsed, 46);
                                            }
                                        }
                                    }

                                    if (ShowAvging && !_IsDoPagination) // 'Update Formulas for statistics if we have inserted a new row for entering values.
                                    {
                                        //if (InsertedNewColumninExcelSheet && !Fieldrows[j][5].ToString().Contains(Conversions.ToString(degree)))
                                        //{
                                        //    string formula_String, formularCol;
                                        //    for (int ii = 1; ii <= 3; ii++)
                                        //    {
                                        //        formularCol = GetColumnName(GetColIndex(LastColumnUsed) + ii);
                                        //        formula_String = xL.xlS.Cells(Currentrow, formularCol).Formula.ToString();
                                        //        if (!string.IsNullOrEmpty(formula_String))
                                        //        {
                                        //            int khj = formula_String.LastIndexOf(":");
                                        //            formula_String = formula_String.Substring(0, khj) + ":" + LastColumnUsed + Currentrow.ToString() + ")";
                                        //            xL.xlS.Cells(Currentrow, formularCol).Formula = formula_String;
                                        //        }
                                        //    }
                                        //}
                                    }

                                    break;
                                }
                            }
                            else if (!WriteAgain)
                            {
                                //CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][5]), Currentrow, LastColumnUsed, NumberFormat: Formatdigits);
                            }

                            Currentrow += 1;
                        }
                        else
                        {
                            Currentrow += Footer + Header - 1;
                            xL[Currentrow, LastColumnUsed] = srNo + 1;
                            j -= 1;
                            xL.set_Set_Alignment(Currentrow, LastColumnUsed, -4108);
                            RowAfterWhich_PaginationShouldBeDone += Footer + RowMax;
                            Currentrow += 1;
                        }
                    }

                    if (passStatus == PassFail.pass)
                    {
                        CompareAndWrite_FieldValues("PASS", Currentrow, LastColumnUsed, NumberFormat: Formatdigits);
                        xL.set_Set_Color(Currentrow, LastColumnUsed, 35);
                    }
                    else if (passStatus == PassFail.fail)
                    {
                        CompareAndWrite_FieldValues("FAIL", Currentrow, LastColumnUsed, NumberFormat: Formatdigits);
                        xL.set_Set_Color(Currentrow, LastColumnUsed, 46);
                    }
                }

                if (CreateCsv)
                {
                    try
                    {
                        var sWriter = new StreamWriter(ReportNameCsv, true); // 'My.Computer.FileSystem.OpenTextFileWriter(ReportNameCsv, True)
                        if (!WriteAgain)
                        {
                            string headerline = "S.No.";
                            for (int i = 0, loopTo10 = MeasurementStringList.Count - 1; i <= loopTo10; i++)
                                headerline += "," + MeasurementStringList[i][0].ToString();
                            sWriter.WriteLine(headerline);
                        }

                        string Values = (srNo + 1).ToString();
                        for (int i = 0, loopTo11 = MeasurementStringList.Count - 1; i <= loopTo11; i++)
                            Values += "," + MeasurementStringList[i][2].ToString();
                        sWriter.WriteLine(Values);
                        sWriter.Close();
                    }
                    catch (Exception ex)
                    {
                    }
                }

                // -------------------------If pass column is included in measurement----------------------------------------------------------------------------

                if (IsDTVSM)     // for delphi tvs
                {
                    try
                    {
                        IsDTVSM = false;
                        var ARatio = default(double);
                        if (File.Exists(PicturePath))
                        {
                            var TempBM = new Bitmap(PicturePath);      // calculate aspectratio
                            ARatio = TempBM.Width / (double)TempBM.Height;
                        }

                        InsertImageForDataTransfer = false;
                        xL.workbook.GetSheet("Report");
                        //xL.xlW.Sheets("Report").Select();
                        //xL.xlS = xL.xlW.Sheets("Report");        // correction by Vinod Jakhar... here Name is repplaced by "ImageSheet" 
                        if (!WriteAgain)
                        {
                            Picture_Left = 6d;        // We have to start from F column for the position of Rcad Image in Excel format
                            Picture_Top = 1d;        // We have to start from 1 Row
                            CalculateImageSize();
                            CalculateImageSize_Rcad();
                        }

                        if (Picture_Height > Picture_Width) // height is higher , so maintain the aspect ratio of the image now 
                        {
                            Picture_Width = Picture_Height * ARatio;
                        }
                        else
                        {
                            Picture_Height = Picture_Width / ARatio;
                        }        // MaintainAspectRatio here 

                        Picture_Width *= 2d;
                        Picture_Height *= 2d;
                        if (File.Exists(PicturePath))
                        {
                            byte[] data = File.ReadAllBytes(PicturePath);
                            int pictureIndex = xL.workbook.AddPicture(data, PictureType.PNG);

                            //xL.xlS.Shapes.AddPicture(PicturePath, 0, 1, Picture_Left, Picture_Top, Picture_Width, Picture_Height);
                        }

                        if (File.Exists(Picture2Path))
                        {
                            //xL.xlS.Shapes.AddPicture(Picture2Path, 0, 1, Picture2_Left, Picture2_Top, Picture2_Width, Picture2_Height);
                        }
                        xL.workbook.GetSheet("Report");
                        //xL.xlW.Sheets("Report").Select();
                        //xL.xlS = xL.xlW.Sheets("Report");
                    }
                    catch (Exception ex)
                    {
                    }
                }
                else if (Fasttransfer & InsertImageForDataTransfer)            // for remaining all pics
                {
                    try
                    {
                        InsertImageForDataTransfer = false;
                        var ARatio = default(double);
                        if (File.Exists(PicturePath))
                        {
                            var TBM = new Bitmap(PicturePath);      // calculate aspectratio
                            ARatio = TBM.Width / (double)TBM.Height;
                        }
                        // 'xL.xlW.Sheets("ImageSheet").Select()
                        // 'xL.xlS = xL.xlW.Sheets("ImageSheet")        'correction by Vinod Jakhar... here Name is repplaced by "ImageSheet" 
                        xL.workbook.CreateSheet("Report");
                        //xL.xlW.Sheets("Report").Select();
                        //xL.xlS = xL.xlW.Sheets("Report");
                        if (!WriteAgain)
                        {
                            CalculateImageSize();
                            CalculateImageSize_Rcad();
                        }

                        if (Picture_Height > Picture_Width) // height is higher , so maintain the aspect ratio of the image now 
                        {
                            Picture_Width = Picture_Height * ARatio;
                        }
                        else
                        {
                            Picture_Height = Picture_Width / ARatio;
                        }       // MaintainAspectRatio here 

                        if (File.Exists(PicturePath))
                        {
                            //xL.xlS.Shapes.AddPicture(PicturePath, 0, 1, Picture_Left, Picture_Top, Picture_Width, Picture_Height);
                        }

                        if (File.Exists(Picture2Path))
                        {
                            //xL.xlS.Shapes.AddPicture(Picture2Path, 0, 1, Picture2_Left, Picture2_Top, Picture2_Width, Picture2_Height);
                        }

                        xL.sheet = xL.workbook.GetSheet("Report");
                        //xL.xlW.Sheets("Report").Select();
                        //xL.xlS = xL.xlW.Sheets("Report");
                    }
                    catch (Exception ex)
                    {
                    }
                }
                else if (InsertImage)
                {
                    InsertImageForDataTransfer = false;
                    
                    //Worksheet xlsht;
                    //xlsht = (Worksheet)xL.xlW.worksheets.Add;
                    string name = "ImageSheet" + ComponentNo;
                    xL.workbook.GetSheet(name);
                    //xlsht.Name = name;
                    //xL.xlW.Sheets(name).Select();
                    //xL.xlW.Sheets(name).Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
                    //xL.xlS = xL.xlW.Sheets(name);
                    // xL.CopyDetail(TemplateFinalRange)          'Its time to paste the template.
                    xL.CopyImageSheet(TemplateFinalRange);
                    xL.ReleaseExtraDatasheet();
                    if (!WriteAgain)
                    {
                        CalculateImageSize();
                        CalculateImageSize_Rcad();
                    }

                    if (File.Exists(PicturePath))
                    {
                        //xL.xlS.Shapes.AddPicture(PicturePath, 0, 1, Picture_Left, Picture_Top, Picture_Width, Picture_Height);
                    }

                    if (File.Exists(Picture2Path))
                    {
                        //xL.xlS.Shapes.AddPicture(Picture2Path, 0, 1, Picture2_Left, Picture2_Top, Picture2_Width, Picture2_Height);
                    }

                    xL.sheet = xL.workbook.GetSheet("Report");
                    //xL.xlW.Sheets("Report").Select();
                    //xL.xlS = xL.xlW.Sheets("Report");
                }

                // ------------------------Now inserting the images (double for a specific format) for Mumabi & sadashivam only.-----------------------------------------------
                if (!IsHCM && InsertDoublePicture && ImagePaths.Length >= 2)
                {
                    int _width;
                    int _height;
                    int _left;
                    int _top;
                    if (ReportType.Contains("Column"))
                    {
                        _width = 327;
                        _height = 245;
                        _left = 0;
                        _top = 100;
                    }
                    else
                    {
                        _width = 200;
                        _height = 150;
                        _left = 0;
                        _top = 110;
                    }

                    //for (int i = 0, loopTo12 = ImagePaths.Length - 1; i <= loopTo12; i++)
                        //xL.xlS.Shapes.AddPicture(ImagePaths[i], 0, 1, (5 + _width) * i, _top, _width, _height);
                }

                // --------------------------------------If detailsheet is to be included-----------------------------------------------------------------------------

                if (ShowDetail)
                {
                    WriteDetailInfo();
                }

                if (IncludeRawData)
                {
                    WriteRawInfo(WriteAgain);
                }

                if (IsHCM)
                    WriteAgain = false;
                // ------------------------------------update reportinformation table for last row and column used for measurement-------------------------------------
                if (ReportType.Contains("Column"))
                {
                    LastRowUsed = LastRowUsed + 1;
                    // If InsertedNewRowinExcelSheet Then
                    // LastRowUsed += 1
                    InsertedNewRowinExcelSheet = false;
                }
                // End If
                else if (ReportType.Contains("Row"))
                {
                    LastColumnUsed = GetColumnName(GetColIndex(LastColumnUsed) + 1);

                    // If InsertedNewColumninExcelSheet Then
                    // LastColumnUsed = GetColumnName(GetColIndex(LastColumnUsed) + 1)
                    InsertedNewColumninExcelSheet = false;
                }

                // If WriteAgain Then
                if (MultiProgramReport)
                {
                    var ProgramList = Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName);
                    if (ProgramList.Length > 0)
                    {
                        int ProgramId = Conversions.ToInteger(ProgramList[0][0]);
                        Data.DataA = new OleDbDataAdapter("Select * From [MultiProgramReport] Where Program_id = " + ProgramId, DatabaseConstants.ConnectionString);
                        var ds1 = new DataSet();
                        Data.DataA.Fill(ds1);
                        for (int i = 0, loopTo13 = ds1.Tables[0].Rows.Count - 1; i <= loopTo13; i++)
                        {
                            if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(ds1.Tables[0].Rows[i][0], DataR[0], false)))
                            {
                                ds1.Tables[0].Rows[i][7] = LastRowUsed;
                                ds1.Tables[0].Rows[i][8] = LastColumnUsed;
                                var cb = new OleDbCommandBuilder(Data.DataA);
                                Data.DataA.Update(ds1);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    Data.DataA = new OleDbDataAdapter("Select * From [ReportInformation] Where [ReportInformation].[R_ReportName] = '" + ReportName + "'", DatabaseConstants.ConnectionString);
                    var ds = new DataSet();
                    Data.DataA.Fill(ds);
                    if (ds.Tables[0].Rows.Count > 0)
                    {
                        ds.Tables[0].Rows[0][26] = LastRowUsed;
                        ds.Tables[0].Rows[0][27] = LastColumnUsed;
                        // ds.Tables(0).Rows(0).Item(28) = MaximumFilledRowPerTemplate
                        // ds.Tables(0).Rows(0).Item(29) = MaximumFilledColumnPerTemplate
                        var cb = new OleDbCommandBuilder(Data.DataA);
                        Data.DataA.Update(ds);
                    }
                }
                // -------------------Deselect any range
                xL.sheet = xL.workbook.GetSheet("Report");
                //xL.xlW.Sheets("Report").Activate();
                //xL.xlW.Sheets("Report").Range("A1:A1").Select();
                // xL.xlW.Sheets("Report").Selection.Clear()
                // 'xL.xlW.Sheets("Report").Range("G17").Activate() ''InitialField_Row, GetColIndex(InitialField_Column)).Select()

                // ------------------------------------ PDF Report Generation
                try
                {
                    if (MakePDFReport)
                    {
                        // ------------------------------------ PDF Report Generation
                        string pdfReportName = ReportName.Substring(0, ReportName.Length - 4) + ".pdf";
                        if (File.Exists(pdfReportName))
                        {
                            File.Delete(pdfReportName);
                        }
                        // dim activeSheet As Microsoft.Office.Interop.Excel._Worksheet = xL.xlW.Sheets("Report")
                        if (_columnWise.Checked)
                        {
                            //xL.xlW.WorkSheets("Report").Pagesetup.Orientation = XlPageOrientation.xlLandscape;
                        }
                        else
                        {
                            //xL.xlW.WorkSheets("Report").PageSetup.Orientation = XlPageOrientation.xlPortrait;
                        }
                        // With xL.xlW.WorkSheets("Report").Pagesetup
                        // .LeftMargin = xL.xlA.InchesToPoints(0.5)
                        // .RightMargin = xL.xlA.InchesToPoints(0.5)
                        // .TopMargin = xL.xlA.InchesToPoints(1)
                        // .BottomMargin = xL.xlA.InchesToPoints(1)
                        // .HeaderMargin = xL.xlA.InchesToPoints(0.5)
                        // .FooterMargin = xL.xlA.InchesToPoints(0.5)
                        // End With

                        //xL.xlW.WorkSheets("Report").ExportAsFixedFormat(XlFixedFormatType.xlTypePDF, pdfReportName, XlFixedFormatQuality.xlQualityStandard, false, true, default, default, true);
                    }
                }
                catch (Exception exx)
                {
                }

                // -----------------------------------End of WriteExcel Report------------------------------------------------------------------------------------------
                return true;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E031", ex);
                xL.Close();
                xL.ExitExcel();
                return false;
            }
        }

        private void CreateDetailSheet()
        {
            //Worksheet xlsht;
            //xlsht = (Worksheet)xL.xlW.worksheets.Add;
            //xlsht.Name = "DetailSheet";
            //xL.xlW.Sheets("DetailSheet").Select();
            xL.sheet = xL.workbook.CreateSheet("DetailSheet");
            //xL.xlW.Sheets("DetailSheet").Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
            //xL.xlS = xL.xlW.Sheets("DetailSheet");
            xL.CopyDetail(TemplateFinalRange);          // Its time to paste the template.
            xL.ReleaseExtraDatasheet();
            for (int i = 0, loopTo = MeasurementStringList.Count - (RowMax - Header); i <= loopTo; i++)
                xL.sheet.CreateRow(15);
                //xL.xlS.Rows("15").Insert();
            var Tole_Headers = new List<string>();
            Tole_Headers.Add("MSR Name");
            Tole_Headers.Add("MSR Val");
            Tole_Headers.Add("Nominal");
            Tole_Headers.Add("+ Tol.");
            Tole_Headers.Add("- Tol.");
            Tole_Headers.Add("Dev.");
            Tole_Headers.Add("OutOfTol.");
            for (int i = 0, loopTo1 = Tole_Headers.Count - 1; i <= loopTo1; i++)
            {
                string Col = GetColumnName(GetColIndex(InitialField_Column) + i);
                xL[InitialField_Row, Col] = Tole_Headers[i];
                xL.set_Set_Alignment(InitialField_Row, Col, -4108);
                xL.set_Set_Color(InitialField_Row, Col, 35);
            }

            //for (int i = 1; i <= 4; i++)
            //{
            //    xL.xlS.Cells(InitialField_Row + i, InitialField_Column) = null;
            //    xL.xlS.Cells(InitialField_Row + i, InitialField_Column).Interior.ColorIndex = XlColorIndex.xlColorIndexNone;
            //}

            //xL.xlS = xL.xlW.Sheets("Report");
        }

        private void CreateRawSheet()
        {
            //Worksheet xlsht;
            //xlsht = (Worksheet)xL.xlW.worksheets.Add;
            //xlsht.Name = "RawDataSheet";
            //xL.xlW.Sheets("RawDataSheet").Select();
            //xL.xlW.Sheets("RawDataSheet").Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
            //xL.xlS = xL.xlW.Sheets("RawDataSheet");
            xL.sheet = xL.workbook.CreateSheet("RawDataSheet");
            //if (RawDataSheetType.Contains("Column"))
            //{
            //    xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column));
            //    xL.cell.SetCellValue((double)MeasurementStringList[i][0]);
            //    int a = 1;
            //    xL[1, GetColumnName(a)] = "S.No.";
            //    xL.set_Set_Alignment(1, GetColumnName(a), -4108);
            //    a += 1;
            //    for (int j = 0, loopTo = Fieldrows.Length - 1; j <= loopTo; j++)
            //    {
            //        CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][3]), 1, GetColumnName(a), NumberFormat: Formatdigits);
            //        a += 1;
            //    }
            //}
            //else
            //{
            //    int Currentrow = 1;
            //    xL[Currentrow, GetColumnName(1)] = "S.No.";
            //    xL.set_Set_Alignment(Currentrow, GetColumnName(1), -4108);
            //    Currentrow += 1;
            //    for (int j = 0, loopTo1 = Fieldrows.Length - 1; j <= loopTo1; j++)
            //    {
            //        CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][3]), Currentrow, GetColumnName(1), NumberFormat: Formatdigits);
            //        Currentrow += 1;
            //    }
            //}

            //xL.xlW.Sheets("Report").Select();
            //xL.xlS = xL.xlW.Sheets("Report");
        }

        private void CreateReportSheetWithImage(ref int sRno, bool writeagain)
        {
            //try
            //{
            //    int Rno;
            //    if (ShowDetail)
            //    {
            //        Rno = Conversions.ToInteger(Operators.DivideObject(Operators.SubtractObject(xL.xlW.Worksheets.Count, 1), 2));
            //    }
            //    else
            //    {
            //        Rno = Conversions.ToInteger(Operators.SubtractObject(xL.xlW.Worksheets.Count, 1));
            //    }

            //    if (Rno < 1)
            //    {
            //        Rno = 1;
            //    }

            //    Worksheet xlsht;
            //    xlsht = (Worksheet)xL.xlW.worksheets.Add;
            //    xlsht.Name = "Component" + Rno;
            //    xL.xlW.Sheets(xlsht.Name).Select();
            //    xL.xlW.Sheets(xlsht.Name).Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
            //    xL.xlS = xL.xlW.Sheets(xlsht.Name);
            //    xL.Paste("Data", TemplateFinalRange, TemplateInitialRange, TemplateInitialRange);
            //    xL.ReleaseExtraDatasheet();
            //    int endRow = LastRowUsed;
            //    string EndCol = LastColumnUsed;
            //    for (int i = 0, loopTo = TemplateFinalRange.Length - 1; i <= loopTo; i++)
            //    {
            //        if (char.IsNumber(TemplateFinalRange[i]))
            //        {
            //            endRow = (int)Math.Round(Conversions.ToDouble(TemplateFinalRange.Substring(i, TemplateFinalRange.Length - i)) - (Footer + 1));
            //            EndCol = GetColumnName(GetColIndex(TemplateFinalRange.Substring(0, i)) - 1);
            //            break;
            //        }
            //    }

            //    if (ReportType.Contains("Column"))
            //    {
            //        int rowToPaste = InitialField_Row + 1;
            //        if (ToleranceFormat == 3)
            //        {
            //            rowToPaste += 3;
            //        }
            //        else if (ToleranceFormat == 1 | ToleranceFormat == 2)
            //        {
            //            rowToPaste += 1;
            //        }

            //        if (ShwfieldNum)
            //        {
            //            rowToPaste += 1;
            //        }

            //        xL.xlW.Sheets("Report").Range(InitialField_Column + LastRowUsed, EndCol + LastRowUsed).Copy();
            //        xL.xlS.Range(InitialField_Column + rowToPaste).PasteSpecial(13);
            //    }
            //    else
            //    {
            //        string colToPaste = GetColumnName(GetColIndex(InitialField_Column) + 1);
            //        if (ToleranceFormat == 3)
            //        {
            //            colToPaste = GetColumnName(GetColIndex(colToPaste) + 3);
            //        }
            //        else if (ToleranceFormat == 1 | ToleranceFormat == 2)
            //        {
            //            colToPaste = GetColumnName(GetColIndex(colToPaste) + 1);
            //        }

            //        if (ShwfieldNum)
            //        {
            //            colToPaste = GetColumnName(GetColIndex(colToPaste) + 1);
            //        }

            //        xL.xlW.Sheets("Report").Range(LastColumnUsed + InitialField_Row, LastColumnUsed + endRow).Copy();
            //        xL.xlS.Range(colToPaste + InitialField_Row).PasteSpecial(13);
            //    }

            //    if (File.Exists(PicturePath) == true)
            //    {
            //        xL.xlS.Shapes.AddPicture(PicturePath, 0, 1, Picture_Left, Picture_Top, Picture_Width, Picture_Height);
            //    }

            //    sRno = Rno - 1;
            //}
            //catch (Exception ex)
            //{
            //}
        }

        private void WriteDetailInfo()
        {
            //int Dno;
            //if (InsertImage & File.Exists(PicturePath) == true)
            //{
            //    Dno = Conversions.ToInteger(Operators.DivideObject(Operators.SubtractObject(xL.xlW.Worksheets.Count, 2), 2));
            //}
            //else
            //{
            //    Dno = Conversions.ToInteger(Operators.SubtractObject(xL.xlW.Worksheets.Count, 2));
            //}

            //string outoftoldeg;
            //double outoftol;
            //Worksheet xlsht;
            //xlsht = (Worksheet)xL.xlW.worksheets.Add;
            //xlsht.Name = "DetailSheet" + Dno;
            //xL.xlW.Sheets(xlsht.Name).Select();
            //xL.xlW.Sheets(xlsht.Name).Move(After: xL.xlW.Worksheets(xL.xlW.Worksheets.Count));
            //xL.xlS = xL.xlW.Sheets(xlsht.Name);
            //xL.PasteDetail(TemplateFinalRange, Initial_Column + Initial_Row);          // Its time to paste the template.
            //xL.ReleaseExtraDatasheet();
            //xL.xlS.Range(TemplateFinalRange).FormatConditions.Delete();
            //int Currentrow = InitialField_Row + 1;
            //int a = GetColIndex(InitialField_Column);
            //for (int j = 0, loopTo = Fieldrows.Length - 1; j <= loopTo; j++)
            //{
            //    for (int i = 0, loopTo1 = MeasurementStringList.Count - 1; i <= loopTo1; i++)
            //    {
            //        if ((Fieldrows[j][13].ToString() ?? "") != (MeasurementStringList[i][6].ToString() ?? ""))
            //            continue;
            //        if (!string.IsNullOrEmpty(Fieldrows[j][13].ToString()) & Fieldrows[j][3].ToString() != "Date" & Fieldrows[j][3].ToString() != "Time")
            //        {
            //            CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][3]), Currentrow, GetColumnName(a), NumberFormat: Formatdigits);
            //            xL.set_Set_Color(Currentrow, GetColumnName(a), 35);
            //            CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), Currentrow, GetColumnName(a + 1), NumberFormat: Formatdigits);
            //            if (!string.IsNullOrEmpty(Fieldrows[j][7].ToString()))
            //            {
            //                CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][7]), Currentrow, GetColumnName(a + 2), NumberFormat: Formatdigits);
            //                CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][8]), Currentrow, GetColumnName(a + 3), NumberFormat: Formatdigits);
            //                CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][9]), Currentrow, GetColumnName(a + 4), NumberFormat: Formatdigits);
            //                if (!Fieldrows[j][5].ToString().Contains(Conversions.ToString(degree)))
            //                {
            //                    xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value = Conversions.ToDouble(MeasurementStringList[i][2]) - Conversions.ToDouble(Fieldrows[j][7]);
            //                    if (xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value.ToString().Contains("-"))
            //                    {
            //                        outoftol = Conversions.ToDouble(Operators.SubtractObject(Conversions.ToDouble(xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value), Fieldrows[j][9]));
            //                        if (!outoftol.ToString().Contains("-"))
            //                        {
            //                            outoftol = 0.0d;
            //                        }
            //                        else
            //                        {
            //                            xL.set_Set_Color(Currentrow, GetColumnName(a + 6), 46);
            //                        }

            //                        xL.xlS.Cells(Currentrow, GetColumnName(a + 6)).Value = outoftol;
            //                    }
            //                    else
            //                    {
            //                        outoftol = Conversions.ToDouble(Operators.SubtractObject(Conversions.ToDouble(xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value), Fieldrows[j][8]));
            //                        if (outoftol.ToString().Contains("-"))
            //                        {
            //                            outoftol = 0.0d;
            //                        }
            //                        else
            //                        {
            //                            xL.set_Set_Color(Currentrow, GetColumnName(a + 6), 46);
            //                        }

            //                        xL.xlS.Cells(Currentrow, GetColumnName(a + 6)).Value = outoftol;
            //                    }
            //                }
            //                else
            //                {
            //                    xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value = chgeform((ConvertDegMinSecToDecimal(Conversions.ToString(MeasurementStringList[i][2])) - ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][7]))).ToString(), false);
            //                    if (xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value.ToString().Contains("-"))
            //                    {
            //                        outoftoldeg = this.chgeform((ConvertDegMinSecToDecimal(Conversions.ToString(xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value)) - ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][9]))).ToString(), false);
            //                        if (!outoftoldeg.ToString().Contains("-"))
            //                        {
            //                            outoftoldeg = 0.ToString() + degree;
            //                        }
            //                        else
            //                        {
            //                            xL.set_Set_Color(Currentrow, GetColumnName(a + 6), 46);
            //                        }

            //                        xL.xlS.Cells(Currentrow, GetColumnName(a + 6)).Value = outoftoldeg;
            //                    }
            //                    else
            //                    {
            //                        outoftoldeg = this.chgeform((ConvertDegMinSecToDecimal(Conversions.ToString(xL.xlS.Cells(Currentrow, GetColumnName(a + 5)).Value)) - ConvertDegMinSecToDecimal(Conversions.ToString(Fieldrows[j][8]))).ToString(), false);
            //                        if (outoftoldeg.ToString().Contains("-"))
            //                        {
            //                            outoftoldeg = 0.ToString() + degree;
            //                        }
            //                        else
            //                        {
            //                            xL.set_Set_Color(Currentrow, GetColumnName(a + 6), 46);
            //                        }

            //                        xL.xlS.Cells(Currentrow, GetColumnName(a + 6)).Value = outoftoldeg;
            //                    }
            //                }
            //            }

            //            Currentrow += 1;
            //        }

            //        break;
            //    }
            //}

            //xL.xlW.Sheets("Report").Select();
        }

        private void WriteRawInfo(bool writeagain)
        {
            //xL.xlS = xL.xlW.Sheets("RawDataSheet");
            //if (RawDataSheetType.Contains("Column"))
            //{
            //    int a = 1;
            //    xL[ComponentNo + 1, GetColumnName(a)] = ComponentNo;
            //    xL.set_Set_Alignment(ComponentNo + 1, GetColumnName(a), -4108);
            //    a += 1;
            //    for (int j = 0, loopTo = Fieldrows.Length - 1; j <= loopTo; j++)
            //    {
            //        if (!string.IsNullOrEmpty(Fieldrows[j][13].ToString()))
            //        {
            //            for (int i = 0, loopTo1 = MeasurementStringList.Count - 1; i <= loopTo1; i++)
            //            {
            //                if ((Fieldrows[j][13].ToString() ?? "") != (MeasurementStringList[i][6].ToString() ?? ""))
            //                    continue;
            //                CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), ComponentNo + 1, GetColumnName(a), NumberFormat: Formatdigits);
            //            }
            //        }
            //        else if (!writeagain)
            //        {
            //            CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][5]), ComponentNo + 1, GetColumnName(a), NumberFormat: Formatdigits);
            //        }

            //        a += 1;
            //    }
            //}
            //else
            //{
            //    int Currentrow = 1;
            //    xL[Currentrow, GetColumnName(ComponentNo + 1)] = ComponentNo;
            //    xL.set_Set_Alignment(Currentrow, GetColumnName(ComponentNo + 1), -4108);
            //    Currentrow += 1;
            //    for (int j = 0, loopTo2 = Fieldrows.Length - 1; j <= loopTo2; j++)
            //    {
            //        if (!string.IsNullOrEmpty(Fieldrows[j][13].ToString()))
            //        {
            //            for (int i = 0, loopTo3 = MeasurementStringList.Count - 1; i <= loopTo3; i++)
            //            {
            //                if ((Fieldrows[j][13].ToString() ?? "") != (MeasurementStringList[i][6].ToString() ?? ""))
            //                    continue;
            //                CompareAndWrite_FieldValues(Conversions.ToString(MeasurementStringList[i][2]), Currentrow, GetColumnName(ComponentNo + 1), NumberFormat: Formatdigits);
            //            }
            //        }
            //        else if (!writeagain)
            //        {
            //            CompareAndWrite_FieldValues(Conversions.ToString(Fieldrows[j][5]), Currentrow, GetColumnName(ComponentNo + 1), NumberFormat: Formatdigits);
            //        }

            //        Currentrow += 1;
            //    }
            //}

            //xL.xlW.Sheets("Report").Select();
            //xL.xlS = xL.xlW.Sheets("Report");
        }

        private void CalculateImageSize()
        {
            // ---------------------------------------------------CALCULATING IMAGE SIZE------------------------------------------------------------------------------------
            //try
            //{
            //    int ColumnNumber = (int)Math.Round(Picture_Left);
            //    int RowNumber = (int)Math.Round(Picture_Top);
            //    Picture_Top = Conversions.ToDouble(xL.xlS.range(GetColumnName(ColumnNumber) + RowNumber).top);
            //    Picture_Left = Conversions.ToDouble(xL.xlS.range(GetColumnName(ColumnNumber) + RowNumber).left);
            //    Data.DataA = new OleDbDataAdapter("Select * From [Settings]", DatabaseConstants.ConnectionString);
            //    var ds1 = new DataSet();
            //    Data.DataA.Fill(ds1);
            //    if (ds1.Tables[0].Rows.Count > 0)
            //    {
            //        if (Conversions.ToBoolean(Operators.OrObject(ds1.Tables[0].Rows[0]["Image1_width"] is object, Operators.ConditionalCompareObjectNotEqual(ds1.Tables[0].Rows[0]["Image1_width"], 0, false))))
            //        {
            //            Picture_Width = Conversions.ToDouble(ds1.Tables[0].Rows[0]["Image1_width"]);
            //            Picture_Height = Conversions.ToDouble(ds1.Tables[0].Rows[0]["Image1_Height"]);
            //            return;
            //        }
            //    }
            //}
            //catch (Exception ex)
            //{
            //    OnErrorHappen?.Invoke("E070", ex);
            //}
        }

        private void CalculateImageSize_Rcad()
        {
            //try
            //{
            //    // ---------------------------------------------------CALCULATING IMAGE SIZE-----------------------------------------------------------------------------------

            //    int ColumnNumber = (int)Math.Round(Picture2_Left);
            //    int RowNumber = (int)Math.Round(Picture2_Top);
            //    Picture2_Top = Conversions.ToDouble(xL.xlS.range(GetColumnName(ColumnNumber) + RowNumber).top);
            //    Picture2_Left = Conversions.ToDouble(xL.xlS.range(GetColumnName(ColumnNumber) + RowNumber).left);
            //    Data.DataA = new OleDbDataAdapter("Select * From [Settings]", DatabaseConstants.ConnectionString);
            //    var ds1 = new DataSet();
            //    Data.DataA.Fill(ds1);
            //    if (ds1.Tables[0].Rows.Count > 0)
            //    {
            //        if (Conversions.ToBoolean(Operators.OrObject(ds1.Tables[0].Rows[0]["Image2_width"] is object, Operators.ConditionalCompareObjectNotEqual(ds1.Tables[0].Rows[0]["Image2_width"], 0, false))))
            //        {
            //            Picture_Width = Conversions.ToDouble(ds1.Tables[0].Rows[0]["Image2_width"]);
            //            Picture_Height = Conversions.ToDouble(ds1.Tables[0].Rows[0]["Image2_Height"]);
            //            return;
            //        }
            //    }
            //}
            //catch (Exception ex)
            //{
            //    OnErrorHappen?.Invoke("E071", ex);
            //}
        }

        // __________________Function to Write the fieldValues and also doing comparisions with tolerance ranges and also with the pass/fail types._________________________________
        public void CompareAndWrite_FieldValues(string FieldValue, int FieldValue_Row, string FieldValue_Column, bool WritePassFailFirstTime = false, int CurrentIndex = 0, int NumberFormat = -1)
        {
            try
            {
                double FieldDbl;
                string s = "0";
                if (NumberFormat < 0)
                {
                    switch (NumberFormat)
                    {
                        case -1:
                            {
                                s = "0";
                                break;
                            }

                        case -2:
                            {
                                s = "dd/mm/yyyy";
                                break;
                            }

                        case -3:
                            {
                                s = "hh:mm:ss";
                                break;
                            }
                    }
                }
                else
                {
                    s = "##0.";
                    if (Formatdigits <= 1)
                        Formatdigits = 4;
                    for (int i = 1, loopTo = Formatdigits; i <= loopTo; i++)
                        s += "0";
                }

                if (double.TryParse(FieldValue, out FieldDbl))
                {
                    xL.sheet.CreateRow(FieldValue_Row).CreateCell(GetColIndex(FieldValue_Column)).SetCellValue(FieldDbl);
                    xL.set_Set_Alignment(FieldValue_Row, FieldValue_Column, -4152);
                }
                else
                {
                    xL.sheet.CreateRow(FieldValue_Row).CreateCell(GetColIndex(FieldValue_Column)).SetCellValue(FieldDbl);
                    //xL.xlS.Cells(FieldValue_Row, FieldValue_Column).Value = FieldValue;
                    // xL.xlS.Range(FieldValue_Column & FieldValue_Row).NumberFormat = "0"
                }

                //xL.xlS.Range(FieldValue_Column + FieldValue_Row).NumberFormat = s;
                // xL.xlS.Range(FieldValue_Column & FieldValue_Row).EntireColumn.AutoFit()
                //xL.xlS.Cells(FieldValue_Row, FieldValue_Column).Font.ColorIndex = 1;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E030", ex);
            }
        }

        // __________________________________________________________Function to write the tolerances values into the report._____________________________________________________________
        private void WritingTolerances(int Tole_Row, string Tole_Column, string ReportT, int ToleranceF, bool WriteHeaders = false)
        {
            try
            {
                string Nomi;
                string USL;
                string LSL;
                if (WriteHeaders)
                {
                    int templateno = 1;
                    SetPaginationLimits(ref Tole_Column, ref Tole_Row);
                    if (ShwfieldNum)
                    {
                        if (ReportType.Contains("Column"))
                        {
                            xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column));
                            xL.cell.SetCellValue("M.No.");
                            //xL[Tole_Row, Tole_Column] = "M.No.";
                            xL.cell = xL.sheet.CreateRow(Tole_Row + 1).CreateCell(GetColIndex(Tole_Column));
                            xL.cell.SetCellValue("S.No.");
                            //xL[Tole_Row + 1, Tole_Column] = "S.No.";
                            //xL.set_Set_Color(Tole_Row + 1, Tole_Column, 35);
                            Tole_Row += 1;
                        }
                        else
                        {
                            xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column));
                            xL.cell.SetCellValue("M.No.");
                            //xL[Tole_Row, Tole_Column] = "M.No.";
                            xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column + 1));
                            xL.cell.SetCellValue("S.No.");
                            //xL[Tole_Row, GetColumnName(GetColIndex(Tole_Column) + 1)] = "S.No.";
                            xL.set_Set_Color(Tole_Row + 1, Tole_Column, 35);
                            Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1);
                        }
                    }

                    if (IncludeTolerence & ToleranceFormat != 0)
                    {
                        // ------------------------------------writing tolerance header here----------------------------------------------------------------
                        var Tole_Headers = new List<string>();
                        switch (ToleranceF)
                        {
                            case 1:
                                {
                                    Tole_Headers.Add("Allowed Range");
                                    break;
                                }

                            case 2:
                                {
                                    Tole_Headers.Add("± Tol.");
                                    break;
                                }

                            case 3:
                                {
                                    Tole_Headers.Add("Nominal");
                                    Tole_Headers.Add("LSL");
                                    Tole_Headers.Add("USL");
                                    break;
                                }
                        }

                        if (ReportType.Contains("Column"))
                        {
                            for (int i = 1, loopTo = Tole_Headers.Count; i <= loopTo; i++)
                            {
                                xL[Tole_Row + i, Tole_Column] = Tole_Headers[i - 1];
                                xL.set_Set_Alignment(Tole_Row + i, Tole_Column, -4108);
                                xL.set_Set_Color(Tole_Row + i, Tole_Column, 35);
                            }
                        }
                        else
                        {
                            for (int i = 1, loopTo1 = Tole_Headers.Count; i <= loopTo1; i++)
                            {
                                string Col = GetColumnName(GetColIndex(Tole_Column) + i);
                                xL[Tole_Row, Col] = Tole_Headers[i - 1];
                                xL.set_Set_Alignment(Tole_Row, Col, -4108);
                                xL.set_Set_Color(Tole_Row, Col, 35);
                            }
                        }
                    }
                    // If Not MultiProgramReport Then
                    // xL.Copy(TemplateFinalRange, TemplateInitialRange, SelectedDataSheet, TemplateInitialRange, False)
                    // 'xL.ClearData(SelectedDataSheet, ColumnAfterWhich_PaginationShouldBeDone & RowAfterWhich_PaginationShouldBeDone, Initial_Column & Initial_Row + 1)
                    // 'For ii As Integer = InitialField_Row + 1 To RowMax + InitialField_Row - Header
                    // '    For jj = GetColIndex(Initial_Column) To GetColIndex(ColumnMax)

                    // '    Next
                    // 'Next

                    // End If
                    // xL.ReleaseExtraDatasheet()
                    // -------------------move to next row or column after writing header-------------------------------------------------------------------------
                    xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column));
                    xL.cell.SetCellValue("S.No.");
                    //xL[Tole_Row, Tole_Column] = "S.No.";
                    if (ReportT.Contains("Column"))
                    {
                        Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1); // 'If InitialField_Column = Initial_Column Then
                    }
                    else
                    {
                        Tole_Row += 1;
                    } // 'If InitialField_Row = Initial_Row Then

                    string s = "##0.";
                    if (Formatdigits <= 1)
                        Formatdigits = 4;
                    for (int i = 1, loopTo2 = Formatdigits; i <= loopTo2; i++)
                        s += "0";
                    string HigherT, LowerT;
                    object max, min;
                    bool IsDegree = false;
                    bool WriteToleranceEntries = false;
                    for (int i = 0, loopTo3 = MeasurementStringList.Count - 1; i <= loopTo3; i++) // ' _measurementListView.Rows.Count - 1
                    {
                        IsDegree = false;
                        WriteToleranceEntries = false;
                        // -----------------------checking for pagination--------------------------------------------------------------------------------------
                        if (ReportType.Contains("Column"))
                        {
                            if (GetColIndex(Tole_Column) - 1 == GetColIndex(ColumnAfterWhich_PaginationShouldBeDone))
                            {
                                if (PageNotAlreadyPasted(Tole_Column, Tole_Row))
                                {
                                    xL.Paste(SelectedDataSheet, SourceFinalRange, SourceInitialRange, GetColumnName(GetColIndex(ColumnAfterWhich_PaginationShouldBeDone) + 1) + (Tole_Row - Header + 1)); // Its Time to do the pagination.
                                                                                                                                                                                                          // xL.Paste(SelectedDataSheet, TemplateFinalRange, TemplateInitialRange, GetColumnName(GetColIndex(ColumnAfterWhich_PaginationShouldBeDone) + 1) & (InitialField_Row - Header + 1)) 'Its Time to do the pagination.
                                    xL.ReleaseExtraDatasheet();
                                }
                                // With xL.xlS.Range(Tole_Column & (Tole_Row + 1) & ":" & Tole_Column & (Tole_Row + RowMax - Header - 1 + 4))
                                // .NumberFormat = "0"
                                // .HorizontalAlignment = -4108
                                // .FormatConditions.Delete()
                                // End With
                                NoOfPastedTemplates += 1;
                                ColumnAfterWhich_PaginationShouldBeDone = GetColumnName(GetColIndex(ColumnAfterWhich_PaginationShouldBeDone) + GetColIndex(ColumnMax)); // Making variables to be updated for next comparision.
                                Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1);
                                templateno += 1;
                            }
                        }
                        else if (ReportType.Contains("Row"))
                        {
                            if (Tole_Row - 1 == RowAfterWhich_PaginationShouldBeDone)
                            {
                                if (PageNotAlreadyPasted(Tole_Column, Tole_Row + Footer + Header))
                                {
                                    xL.Paste(SelectedDataSheet, SourceFinalRange, SourceInitialRange, Tole_Column + (RowAfterWhich_PaginationShouldBeDone + Footer + 1).ToString());  // 'GetColumnName(GetColIndex(Tole_Column) + 1)   'Its Time to paste the template.
                                    xL.ReleaseExtraDatasheet();
                                }
                                // With xL.xlS.Range(Tole_Column & Tole_Row & ":" & GetColumnName(GetColIndex(Tole_Column) + GetColIndex(ColumnMax) - GetColIndex(Initial_Column) - 2 + 4) & Tole_Row)
                                // .NumberFormat = "0"
                                // .HorizontalAlignment = -4108
                                // .FormatConditions.Delete()
                                // End With
                                NoOfPastedTemplates += 1;
                                RowAfterWhich_PaginationShouldBeDone += RowMax + Footer;
                                Tole_Row += Footer + Header;
                                templateno += 1;
                            }
                        }

                        if (ReportT.Contains("Column"))
                        {
                            if (ShwfieldNum)
                            {
                                xL.cell = xL.sheet.CreateRow(Tole_Row - 1).CreateCell(GetColIndex(Tole_Column));
                                xL.cell.SetCellValue((double)MeasurementStringList[i][6]);
                                //xL[Tole_Row - 1, Tole_Column] = MeasurementStringList[i][6]; // ' _measurementListView.Rows(i).Cells(2).Value
                            }
                        }
                        // xL.Get_Set_Value(Tole_Row, Tole_Column) = MeasurementStringList(i)(0) '' _measurementListView.Rows(i).Cells(3).Value
                        else if (ShwfieldNum)
                        {
                            xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column)-1);
                            xL.cell.SetCellValue((double)MeasurementStringList[i][6]);
                            //xL[Tole_Row, GetColumnName(GetColIndex(Tole_Column) - 1)] = MeasurementStringList[i][6]; // ' _measurementListView.Rows(i).Cells(2).Value
                        }
                        xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column));
                        xL.cell.SetCellValue((double)MeasurementStringList[i][0]);
                        xL.cell.CellStyle.Alignment = NPOI.SS.UserModel.HorizontalAlignment.Center;
                        //xL[Tole_Row, Tole_Column] = MeasurementStringList[i][0]; // '_measurementListView.Rows(i).Cells(3).Value
                        //xL.set_Set_Alignment(Tole_Row, Tole_Column, (int)XlHAlign.xlHAlignCenter);
                        if (MeasurementStringList[i][2].ToString().Contains(Conversions.ToString(degree))) // ' _measurementListView.Rows(i).Cells(5).Value.ToString.Contains(degree) Then
                        {
                            IsDegree = true;
                        }

                        // --------------------------------------writing tolerance and format condtion---------------------------------------------------------------------------
                        if (IncludeTolerence & ToleranceFormat != 0) // ' And
                        {
                            // (MeasurementStringList(i)(3).ToString IsNot Nothing) And
                            // (MeasurementStringList(i)(4).ToString IsNot Nothing) And     '' _measurementListView.Rows(i).Cells(6).Value IsNot DBNull.Value) And (_measurementListView.Rows(i).Cells(7).Value IsNot DBNull.Value) _
                            // (MeasurementStringList(i)(5).ToString IsNot Nothing) Then  '' (_measurementListView.Rows(i).Cells(8).Value IsNot DBNull.Value) Then

                            if (i > 1)
                                WriteToleranceEntries = true;
                            Nomi = MeasurementStringList[i][3].ToString(); // ' _measurementListView.Rows(i).Cells(7).Value
                            USL = MeasurementStringList[i][4].ToString(); // '_measurementListView.Rows(i).Cells(8).Value
                            LSL = MeasurementStringList[i][5].ToString(); // '_measurementListView.Rows(i).Cells(9).Value
                            char degree;
                            degree = '°';
                            // --------------------calculating higer and lower tolerance for different datatypes-----------------------------------------
                            double value = 0.0d;
                            double value2 = 0.0d;
                            if (IsDegree)
                            {
                                max = chgeform((ConvertDegMinSecToDecimal(Nomi) + ConvertDegMinSecToDecimal(USL)).ToString(), false);
                                min = chgeform((ConvertDegMinSecToDecimal(Nomi) + ConvertDegMinSecToDecimal(LSL)).ToString(), false);
                                HigherT = USL;
                                LowerT = LSL;
                            }
                            else
                            {
                                HigherT = USL;
                                if (HigherT.Length > s.Length)
                                    HigherT = Conversions.ToDouble(HigherT).ToString(s);
                                LowerT = LSL;
                                if (LowerT.Length > s.Length)
                                    LowerT = Conversions.ToDouble(LowerT).ToString(s);
                                max = Conversions.ToDouble(Nomi) + Conversions.ToDouble(USL);
                                min = Conversions.ToDouble(Nomi) + Conversions.ToDouble(LSL);
                                Nomi = Conversions.ToDouble(Nomi).ToString(s);
                            }
                            // If (Double.TryParse(HigherT, value)) And (Double.TryParse(LowerT, value2)) Then
                            // HigherT = CDbl(HigherT).ToString(s)
                            // LowerT = CDbl(LowerT).ToString(s)
                            // End If
                            // -----------------------updating higher and lower tolerance value in fieldinformation table----------------------------
                            Fieldrows[i][7] = Nomi;
                            Fieldrows[i][8] = HigherT;
                            Fieldrows[i][9] = LowerT;
                            Fieldrows[i][11] = max;
                            Fieldrows[i][12] = min;
                            string lsstring = MeasurementStringList[i][3].ToString();
                            string hsstring = MeasurementStringList[i][5].ToString();
                            string nomstring = MeasurementStringList[i][4].ToString();
                            int ToleranceisPresent = 0;
                            if (IsDegree)
                            {
                                if (i > 1)
                                {
                                    if (string.IsNullOrEmpty(lsstring) | lsstring == "0° 00' 00''" | lsstring == "0°00'00''")
                                        ToleranceisPresent += 1;
                                    if (string.IsNullOrEmpty(hsstring) | hsstring == "0° 00' 00''" | hsstring == "0°00'00''")
                                        ToleranceisPresent += 1;
                                    if (string.IsNullOrEmpty(nomstring) | nomstring == "0° 00' 00''" | nomstring == "0°00'00''")
                                        ToleranceisPresent += 1;
                                    if (ToleranceisPresent == 3)
                                        WriteToleranceEntries = false;
                                    // AndAlso nomstring = "0° 00' 00''" AndAlso MeasurementStringList(i)(4).ToString = "0° 00' 00''" AndAlso MeasurementStringList(i)(5).ToString = "0° 00' 00''" Then WriteToleranceEntries = False
                                }
                            }
                            else if (i > 1 && Conversions.ToDouble(MeasurementStringList[i][3]) == 0d && Conversions.ToDouble(MeasurementStringList[i][4]) == 0d && Conversions.ToDouble(MeasurementStringList[i][5]) == 0d)
                                WriteToleranceEntries = false; // ' Continue For

                            // ------------------------  checking selected tolerance option and giveing formatcondition according to that--------------------------------------------------

                            if (ToleranceF == 1 | ToleranceF == 2)
                            {
                                if (ReportT.Contains("Column"))    // ------------------------- for columnwise reporting--------------------
                                {
                                    if (i > 1)
                                    {
                                        Tole_Row += 1;
                                        int RowToInsert = Tole_Row + RowMax - Header - 1;
                                        if (WriteToleranceEntries)
                                        {
                                            if (ToleranceF == 1)
                                            {
                                                xL.WriteValuesInExcel(Conversions.ToString(min) + " -- " + Conversions.ToString(max), Tole_Column, Tole_Row);
                                            }
                                            else
                                            {
                                                if (!HigherT.Contains("-"))
                                                    HigherT = "+" + HigherT;
                                                if (!LowerT.Contains("-"))
                                                    LowerT = "+" + LowerT;
                                                xL.WriteValuesInExcel(Nomi + "@" + HigherT + ";" + LowerT, Tole_Column, Tole_Row, false, "SuperSub");
                                            }
                                        }

                                        //xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Delete();
                                        if (!IsDegree)
                                        {
                                            if (WriteToleranceEntries)
                                            {
                                                //{
                                                //    var withBlock = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Add(10);
                                                //    withBlock.Interior.ColorIndex = XlColorIndex.xlColorIndexNone;
                                                //}

                                                //{
                                                //    var withBlock1 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Add(XlFormatConditionType.xlCellValue, XlFormatConditionOperator.xlBetween, min, max);
                                                //    withBlock1.Interior.ColorIndex = 35;
                                                //}

                                                //{
                                                //    var withBlock2 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Add(1, 2, min, max);
                                                //    withBlock2.Interior.ColorIndex = 46;
                                                //}
                                            }

                                            if (ShowAvging)
                                            {
                                                {
                                                    //xL.sheet.SetActiveCellRange(RowToInsert + 1, RowToInsert + 1, GetColIndex(Tole_Column), GetColIndex(Tole_Column));
                                                    xL.cell.CellFormula = "=AVERAGE(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                    //var withBlock3 = xL.xlS.Range(Tole_Column + (RowToInsert + 1) + ":" + Tole_Column + (RowToInsert + 1));
                                                    //withBlock3.Formula = "=AVERAGE(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MIN(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                    //var withBlock4 = xL.xlS.Range(Tole_Column + (RowToInsert + 2) + ":" + Tole_Column + (RowToInsert + 2));
                                                    //withBlock4.Formula = "=MIN(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MAX(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                    //var withBlock5 = xL.xlS.Range(Tole_Column + (RowToInsert + 3) + ":" + Tole_Column + (RowToInsert + 3));
                                                    //withBlock5.Formula = "=MAX(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=" + Tole_Column + (RowToInsert + 3) + " - " + Tole_Column + (RowToInsert + 2);
                                                    //var withBlock6 = xL.xlS.Range(Tole_Column + (RowToInsert + 4) + ":" + Tole_Column + (RowToInsert + 4));
                                                    //withBlock6.Formula = "=" + Tole_Column + (RowToInsert + 3) + " - " + Tole_Column + (RowToInsert + 2);
                                                }
                                            }

                                            {
                                                //var withBlock7 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + (RowToInsert + 4));
                                                //withBlock7.NumberFormat = s;
                                                //withBlock7.HorizontalAlignment = -4152;
                                            }
                                        }

                                        Tole_Row -= 1;
                                    }

                                    Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1);
                                }
                                else    // ---------------------------------- for row wise reporting------------------------------------------------------------
                                {
                                    if (i > 1)
                                    {
                                        int GetIndex = GetColIndex(Tole_Column);
                                        if (WriteToleranceEntries)
                                        {
                                            if (ToleranceF == 1)
                                            {
                                                xL.WriteValuesInExcel(Conversions.ToString(min) + " -- " + Conversions.ToString(max), GetColumnName(GetIndex + 1), Tole_Row);
                                            }
                                            else
                                            {
                                                if (!HigherT.Contains("-"))
                                                    HigherT = "+" + HigherT;
                                                if (!LowerT.Contains("-"))
                                                    LowerT = "+" + LowerT;
                                                xL.WriteValuesInExcel(Nomi + "@" + HigherT + ";" + LowerT, GetColumnName(GetIndex + 1), Tole_Row, false, "SuperSub");
                                            }
                                        }

                                        Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 2);
                                        int ColToInsert = GetColIndex(Tole_Column) + GetColIndex(ColumnMax) - GetColIndex(Initial_Column) - 2; // + 1
                                        //xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Delete();
                                        if (!IsDegree)
                                        {
                                            if (WriteToleranceEntries)
                                            {
                                                //{
                                                //    var withBlock8 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Add(10);
                                                //    withBlock8.Interior.ColorIndex = XlColorIndex.xlColorIndexNone;
                                                //}

                                                //{
                                                //    var withBlock9 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Add(XlFormatConditionType.xlCellValue, XlFormatConditionOperator.xlBetween, min, max);
                                                //    withBlock9.Interior.ColorIndex = 35;
                                                //}

                                                //{
                                                //    var withBlock10 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Add(1, 2, min, max);
                                                //    withBlock10.Interior.ColorIndex = 46;
                                                //}
                                            }

                                            if (ShowAvging)
                                            {
                                                {
                                                    xL.cell.CellFormula = "=AVERAGE(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                    //var withBlock11 = xL.xlS.Range(GetColumnName(ColToInsert + 1) + Tole_Row);
                                                    //withBlock11.Formula = "=AVERAGE(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MAX(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                    //var withBlock12 = xL.xlS.Range(GetColumnName(ColToInsert + 2) + Tole_Row);
                                                    //withBlock12.Formula = "=MAX(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MIN(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                    //var withBlock13 = xL.xlS.Range(GetColumnName(ColToInsert + 3) + Tole_Row);
                                                    //withBlock13.Formula = "=MIN(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=" + GetColumnName(ColToInsert + 2) + Tole_Row + " - " + GetColumnName(ColToInsert + 3) + Tole_Row;
                                                    //var withBlock14 = xL.xlS.Range(GetColumnName(ColToInsert + 4) + Tole_Row);
                                                    //withBlock14.Formula = "=" + GetColumnName(ColToInsert + 2) + Tole_Row + " - " + GetColumnName(ColToInsert + 3) + Tole_Row;
                                                }
                                            }

                                            {
                                                //var withBlock15 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert + 4) + Tole_Row);
                                                //withBlock15.NumberFormat = s;
                                                //withBlock15.HorizontalAlignment = -4152;
                                            }
                                        }

                                        Tole_Column = GetColumnName(GetColIndex(Tole_Column) - 2);
                                    }

                                    Tole_Row += 1;
                                }
                            }
                            else if (ToleranceF == 3)                 // If LSL,USL format is selected.
                            {
                                if (ReportT.Contains("Column")) // ------------------------- for columnwise reporting--------------------
                                {
                                    if (i > 1)
                                    {
                                        if (WriteToleranceEntries)
                                        {
                                            xL.WriteValuesInExcel(Nomi, Tole_Column, Tole_Row + 1);
                                            xL.WriteValuesInExcel(Conversions.ToString(min), Tole_Column, Tole_Row + 2);
                                            xL.WriteValuesInExcel(Conversions.ToString(max), Tole_Column, Tole_Row + 3);
                                            {
                                                //var withBlock16 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + (Tole_Row + 3));
                                                //withBlock16.NumberFormat = s;
                                                //withBlock16.HorizontalAlignment = -4152;
                                            }
                                        }

                                        Tole_Row += 3;
                                        int RowToInsert = Tole_Row + RowMax - Header;
                                        //xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Delete();
                                        if (!IsDegree)
                                        {
                                            if (WriteToleranceEntries)
                                            {
                                                //{
                                                //    var withBlock17 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Add(10);
                                                //    withBlock17.Interior.ColorIndex = XlColorIndex.xlColorIndexNone;
                                                //}

                                                //{
                                                //    var withBlock18 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Add(XlFormatConditionType.xlCellValue, XlFormatConditionOperator.xlBetween, min, max);
                                                //    withBlock18.Interior.ColorIndex = 35;
                                                //}

                                                //{
                                                //    var withBlock19 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert).FormatConditions.Add(1, 2, min, max);
                                                //    withBlock19.Interior.ColorIndex = 46;
                                                //}
                                            }

                                            if (ShowAvging)
                                            {
                                                {
                                                    xL.cell.CellFormula = "=AVERAGE(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                    //var withBlock20 = xL.xlS.Range(Tole_Column + (RowToInsert + 1) + ":" + Tole_Column + (RowToInsert + 1));
                                                    //withBlock20.Formula = "=AVERAGE(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MIN(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                    //var withBlock21 = xL.xlS.Range(Tole_Column + (RowToInsert + 2) + ":" + Tole_Column + (RowToInsert + 2));
                                                    //withBlock21.Formula = "=MIN(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MAX(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                    //var withBlock22 = xL.xlS.Range(Tole_Column + (RowToInsert + 3) + ":" + Tole_Column + (RowToInsert + 3));
                                                    //withBlock22.Formula = "=MAX(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=" + Tole_Column + (RowToInsert + 3) + " - " + Tole_Column + (RowToInsert + 2);
                                                    //var withBlock23 = xL.xlS.Range(Tole_Column + (RowToInsert + 4) + ":" + Tole_Column + (RowToInsert + 4));
                                                    //withBlock23.Formula = "=" + Tole_Column + (RowToInsert + 3) + " - " + Tole_Column + (RowToInsert + 2);
                                                }
                                            }

                                            {
                                                //var withBlock24 = xL.xlS.Range(Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + (RowToInsert + 4));
                                                //withBlock24.NumberFormat = s;
                                                //withBlock24.HorizontalAlignment = -4152;
                                            }
                                        }

                                        Tole_Row -= 3;
                                    }

                                    Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1);
                                }
                                else  // ---------------------------------- for row wise reporting------------------------------------------------------------
                                {
                                    if (i > 1)
                                    {
                                        int GetIndex = GetColIndex(Tole_Column);
                                        if (WriteToleranceEntries)
                                        {
                                            xL.WriteValuesInExcel(Nomi, GetColumnName(GetIndex + 1), Tole_Row);
                                            xL.WriteValuesInExcel(Conversions.ToString(min), GetColumnName(GetIndex + 2), Tole_Row);
                                            xL.WriteValuesInExcel(Conversions.ToString(max), GetColumnName(GetIndex + 3), Tole_Row);
                                        }

                                        Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 4);
                                        int ColToInsert = GetColIndex(Tole_Column) + GetColIndex(ColumnMax) - GetColIndex(Initial_Column) - 4; // + 1
                                        //xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Delete();
                                        if (!IsDegree)
                                        {
                                            if (WriteToleranceEntries)
                                            {
                                                //{
                                                //    var withBlock25 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Add(10);
                                                //    withBlock25.Interior.ColorIndex = XlColorIndex.xlColorIndexNone;
                                                //}

                                                //{
                                                //    var withBlock26 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Add(XlFormatConditionType.xlCellValue, XlFormatConditionOperator.xlBetween, min, max);
                                                //    withBlock26.Interior.ColorIndex = 35;
                                                //}

                                                //{
                                                //    var withBlock27 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row).FormatConditions.Add(1, 2, min, max);
                                                //    withBlock27.Interior.ColorIndex = 46;
                                                //}
                                            }

                                            if (ShowAvging)
                                            {
                                                {
                                                    xL.cell.CellFormula = "=AVERAGE(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                    //var withBlock28 = xL.xlS.Range(GetColumnName(ColToInsert + 1) + Tole_Row);
                                                    //withBlock28.Formula = "=AVERAGE(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MIN(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                    //var withBlock29 = xL.xlS.Range(GetColumnName(ColToInsert + 2) + Tole_Row);
                                                    //withBlock29.Formula = "=MIN(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=MAX(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                    //var withBlock30 = xL.xlS.Range(GetColumnName(ColToInsert + 3) + Tole_Row);
                                                    //withBlock30.Formula = "=MAX(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                                }

                                                {
                                                    xL.cell.CellFormula = "=" + GetColumnName(ColToInsert + 3) + Tole_Row + " - " + GetColumnName(ColToInsert + 2) + Tole_Row;
                                                    //var withBlock31 = xL.xlS.Range(GetColumnName(ColToInsert + 4) + Tole_Row);
                                                    //withBlock31.Formula = "=" + GetColumnName(ColToInsert + 3) + Tole_Row + " - " + GetColumnName(ColToInsert + 2) + Tole_Row;
                                                }
                                            }

                                            {
                                                //var withBlock32 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert + 4) + Tole_Row);
                                                //withBlock32.NumberFormat = s;
                                                //withBlock32.HorizontalAlignment = -4152;
                                            }
                                        }

                                        Tole_Column = GetColumnName(GetColIndex(Tole_Column) - 4);
                                    }

                                    Tole_Row += 1;
                                }
                            }

                            IsDegree = false;
                        }
                        // ------------------------if tolerance field is empty or not selected------------------------------------------------------------
                        else if (ReportT.Contains("Column"))    // ------------------------- for columnwise reporting--------------------
                        {
                            int RowToInsert = Tole_Row + RowMax - Header;
                            if (i > 1)
                            {
                                if (ShowAvging & !IsDegree)
                                {
                                    {
                                        xL.cell.CellFormula = "=AVERAGE(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                        //var withBlock33 = xL.xlS.Range(Tole_Column + (RowToInsert + 1) + ":" + Tole_Column + (RowToInsert + 1)); // ' (RowMax + 1))
                                        //withBlock33.Formula = "=AVERAGE(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                    }

                                    {
                                        xL.cell.CellFormula = "=MIN(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                        //var withBlock34 = xL.xlS.Range(Tole_Column + (RowToInsert + 2) + ":" + Tole_Column + (RowToInsert + 2));
                                        //withBlock34.Formula = "=MIN(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                    }

                                    {
                                        xL.cell.CellFormula = "=MAX(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                        //var withBlock35 = xL.xlS.Range(Tole_Column + (RowToInsert + 3) + ":" + Tole_Column + (RowToInsert + 3));
                                        //withBlock35.Formula = "=MAX(" + Tole_Column + (Tole_Row + 1) + ":" + Tole_Column + RowToInsert + ")";
                                    }

                                    {
                                        xL.cell.CellFormula = "=" + Tole_Column + (RowToInsert + 3) + " - " + Tole_Column + (RowToInsert + 2);
                                        //var withBlock36 = xL.xlS.Range(Tole_Column + (RowToInsert + 4) + ":" + Tole_Column + (RowToInsert + 4));
                                        //withBlock36.Formula = "=" + Tole_Column + (RowToInsert + 3) + " - " + Tole_Column + (RowToInsert + 2);
                                    }
                                }

                                {
                                    //var withBlock37 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + Tole_Column + (RowToInsert + 4));
                                    //withBlock37.NumberFormat = s;
                                    //withBlock37.HorizontalAlignment = -4152;
                                    // .EntireColumn.AutoFit()
                                }
                                // Else
                                // If MeasurementStringList(i)(0).ToString = "Date" Then
                                // With xL.xlS.Range(Tole_Column & Tole_Row & ":" & Tole_Column & RowToInsert + 4)
                                // .NumberFormat = "dd/mm/yyyy"
                                // .EntireColumn.AutoFit()
                                // End With
                                // ElseIf MeasurementStringList(i)(0).ToString = "Time" Then
                                // With xL.xlS.Range(Tole_Column & Tole_Row & ":" & Tole_Column & RowToInsert + 4)
                                // .NumberFormat = "hh:mm:ss"
                                // .EntireColumn.AutoFit()
                                // End With
                                // ElseIf MeasurementStringList(i)(0).ToString.Contains("No.") Then
                                // With xL.xlS.Range(Tole_Column & Tole_Row & ":" & Tole_Column & RowToInsert + 4)
                                // .NumberFormat = "0"
                                // .EntireColumn.AutoFit()
                                // End With
                                // End If
                            }

                            Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1);
                        }
                        else   // ---------------------------------- for row wise reporting------------------------------------------------------------
                        {
                            if (i > 1)
                            {
                                int ColToInsert = GetColIndex(Tole_Column) + GetColIndex(ColumnMax) - GetColIndex(Initial_Column); // + 1
                                if (ShowAvging & !IsDegree)
                                {
                                    {
                                        xL.cell.CellFormula = "=AVERAGE(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                        //var withBlock38 = xL.xlS.Range(GetColumnName(ColToInsert + 1) + Tole_Row);
                                        //withBlock38.Formula = "=AVERAGE(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                    }

                                    {
                                        xL.cell.CellFormula = "=MAX(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                        //var withBlock39 = xL.xlS.Range(GetColumnName(ColToInsert + 2) + Tole_Row);
                                        //withBlock39.Formula = "=MAX(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                    }

                                    {
                                        xL.cell.CellFormula = "=MIN(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                        //var withBlock40 = xL.xlS.Range(GetColumnName(ColToInsert + 3) + Tole_Row);
                                        //withBlock40.Formula = "=MIN(" + Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert) + Tole_Row + ")";
                                    }

                                    {
                                        xL.cell.CellFormula = "=" + GetColumnName(ColToInsert + 2) + Tole_Row + " - " + GetColumnName(ColToInsert + 3) + Tole_Row;
                                        //var withBlock41 = xL.xlS.Range(GetColumnName(ColToInsert + 4) + Tole_Row);
                                        //withBlock41.Formula = "=" + GetColumnName(ColToInsert + 2) + Tole_Row + " - " + GetColumnName(ColToInsert + 3) + Tole_Row;
                                    }
                                }

                                {
                                    //var withBlock42 = xL.xlS.Range(Tole_Column + Tole_Row + ":" + GetColumnName(ColToInsert + 4) + Tole_Row);
                                    //withBlock42.NumberFormat = s;
                                    //withBlock42.HorizontalAlignment = -4152;
                                }
                            }

                            Tole_Row += 1;
                        }
                    }

                    if (passStatus != PassFail.none)
                    {
                        if (ReportT.Contains("Column"))
                        {
                            xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column + 1));
                            xL.cell.SetCellValue("PASS/FAIL");
                            //xL[Tole_Row, Tole_Column] = "PASS/FAIL";
                            Tole_Column = GetColumnName(GetColIndex(Tole_Column) + 1);
                        }
                        else
                        {
                            xL.cell = xL.sheet.CreateRow(Tole_Row).CreateCell(GetColIndex(Tole_Column + 1));
                            xL.cell.SetCellValue("PASS/FAIL");
                            //xL[Tole_Row, Tole_Column] = "PASS/FAIL";
                            Tole_Row += 1;
                        }
                    }

                    // -------------------calculate lastcolumn and lastrow unfilled---------------------------------------------------------------------
                    if (ReportType.Contains("Column"))
                    {
                        if (IncludeTolerence & ToleranceFormat != 0)
                        {
                            if (IncludeTolerence & ToleranceFormat == 3)
                            {
                                LastRowUsed = Tole_Row + 4;
                            }
                            else
                            {
                                LastRowUsed = Tole_Row + 2;
                            }
                        }
                        else
                        {
                            LastRowUsed = Tole_Row + 1;
                        }

                        LastColumnUsed = GetColumnName(GetColIndex(Tole_Column) - 1);
                        double templateNumbersD = GetColIndex(LastColumnUsed) / (double)(GetColIndex(ColumnMax) - GetColIndex(Initial_Column) + 1);
                        int templateNumbers = 1;
                        if (Conversion.Int(templateNumbersD) < templateNumbersD)
                        {
                            templateNumbers = (int)Math.Round(Conversion.Int(templateNumbersD)) + 1;
                        }
                        else
                        {
                            templateNumbers = (int)Math.Round(Conversion.Int(templateNumbersD));
                        }

                        TemplateFinalRange = GetColumnName(GetColIndex(Initial_Column) + templateNumbers * (GetColIndex(ColumnMax) - GetColIndex(Initial_Column) + 1) - 1) + (RowMax + Footer);
                    }
                    // TemplateFinalRange = GetColumnName(GetColIndex(Initial_Column) + (GetColIndex(ColumnMax) - GetColIndex(Initial_Column) + 1) * templateno - 1) & (RowMax + Footer)
                    else
                    {
                        if (IncludeTolerence & ToleranceFormat != 0)
                        {
                            if (IncludeTolerence & ToleranceFormat == 3)
                            {
                                LastColumnUsed = GetColumnName(GetColIndex(Tole_Column) + 4);
                            }
                            else
                            {
                                LastColumnUsed = GetColumnName(GetColIndex(Tole_Column) + 2);
                            }
                        }
                        else
                        {
                            LastColumnUsed = GetColumnName(GetColIndex(Tole_Column) + 1);
                        }

                        LastRowUsed = Tole_Row - 1;
                        double templateNumbersD = LastRowUsed / (double)(RowMax + Footer - Initial_Row + 1);
                        int templateNumbers = 1;
                        if (Conversion.Int(templateNumbersD) < templateNumbersD)
                        {
                            templateNumbers = (int)Math.Round(Conversion.Int(templateNumbersD)) + 1;
                        }
                        else
                        {
                            templateNumbers = (int)Math.Round(Conversion.Int(templateNumbersD));
                        }

                        // If LastRowUsed = RowAfterWhich_PaginationShouldBeDone Then
                        // LastRowUsed += Footer + Header + 1
                        // End If

                        // TemplateFinalRange = GetColumnName(GetColIndex(ColumnMax) + Sider) & (InitialField_Row - Header + (RowMax + Footer) * templateno)
                        TemplateFinalRange = GetColumnName(GetColIndex(ColumnMax) + Sider) + (Initial_Row + templateNumbers * (RowMax + Footer - Initial_Row + 1) - 1);
                    }
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E029", ex);
            }
        }

        private void SetPaginationLimits(ref string HeaderColtoWrite, ref int HeaderRowtoWrite)
        {
            ColumnAfterWhich_PaginationShouldBeDone = GetColumnName(GetColIndex(HeaderColtoWrite) + GetColIndex(ColumnMax) - 1); // for the columnWise reports
            RowAfterWhich_PaginationShouldBeDone = RowMax + Initial_Row - 1;      // for the rowwise reports
                                                                                  // If MultiProgramReport AndAlso Not MultipleReportsInDifferentPages Then
            int pgSize;
            double mult;
            int finalmult = 1;
            int diff = 0;
            if (ReportType.Contains("Column"))
            {
                pgSize = GetColIndex(Initial_Column) + GetColIndex(ColumnMax) - 1;
                mult = GetColIndex(InitialField_Column) / (double)pgSize;
                // 'Get the column page number
                if (Conversion.Int(mult) < mult)
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult)) + 1;
                }
                else
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult));
                }

                if (finalmult == 0)
                    finalmult = 1;
                ColumnAfterWhich_PaginationShouldBeDone = GetColumnName(finalmult * pgSize);
                // If finalmult > 1 Then
                // diff = GetColIndex(InitialField_Column) - (finalmult - 1) * pgSize
                // If diff >= 0 AndAlso diff < 2 Then
                // ColumnAfterWhich_PaginationShouldBeDone = GetColumnName((finalmult - 1) * pgSize)
                // HeaderColtoWrite = GetColumnName(GetColIndex(HeaderColtoWrite) - 1)
                // End If
                // End If
                // 'get row page number
                pgSize = RowMax + Initial_Row - 1 + Footer;
                mult = HeaderRowtoWrite / (double)pgSize;
                finalmult = 1;
                if (Conversion.Int(mult) < mult)
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult)) + 1;
                }
                else
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult));
                }

                if (finalmult == 0)
                    finalmult = 1;
                // RowAfterWhich_PaginationShouldBeDone = finalmult * pgSize
                RowAfterWhich_PaginationShouldBeDone = finalmult * pgSize - Footer;
                if (finalmult > 1)
                {
                    diff = InitialField_Row - (finalmult - 1) * pgSize;
                    if (diff >= 0 && diff < 2)
                    {
                        RowAfterWhich_PaginationShouldBeDone = (finalmult - 1) * pgSize - Footer;
                        HeaderRowtoWrite -= 1;
                    }
                    // ElseIf finalmult = 1 Then
                    // RowAfterWhich_PaginationShouldBeDone = pgSize - Footer
                }
            }
            else if (ReportType.Contains("Row"))
            {
                // 'get column page number
                pgSize = GetColIndex(Initial_Column) + GetColIndex(ColumnMax) - 1 + Sider;
                mult = GetColIndex(HeaderColtoWrite) / (double)pgSize;
                if (Conversion.Int(mult) < mult)
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult)) + 1;
                }
                else
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult));
                }

                if (finalmult == 0)
                    finalmult = 1;
                ColumnAfterWhich_PaginationShouldBeDone = GetColumnName(finalmult * pgSize - Sider);
                if (finalmult > 1)
                {
                    diff = GetColIndex(InitialField_Column) - (finalmult - 1) * pgSize;
                    if (diff >= 0 && diff < 2)
                    {
                        ColumnAfterWhich_PaginationShouldBeDone = GetColumnName((finalmult - 1) * pgSize - Footer);
                        HeaderColtoWrite = GetColumnName(GetColIndex(HeaderColtoWrite) - 1);
                    }
                }

                // 'get row page number
                pgSize = RowMax + Initial_Row - 1 + Footer;
                mult = InitialField_Row / (double)pgSize;
                finalmult = 1;
                if (Conversion.Int(mult) < mult)
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult)) + 1;
                }
                else
                {
                    finalmult = (int)Math.Round(Conversion.Int(mult));
                }

                if (finalmult == 0)
                    finalmult = 1;
                RowAfterWhich_PaginationShouldBeDone = finalmult * pgSize - Footer;
                // If finalmult > 1 Then
                // diff = InitialField_Row - (finalmult - 1) * pgSize
                // If diff >= 0 AndAlso diff < 2 Then
                // RowAfterWhich_PaginationShouldBeDone = (finalmult - 1) * pgSize - Footer
                // HeaderRowtoWrite -= 1
                // End If
                // End If
            }
        }

        /// <summary>
    /// Checks if the cell where template has to be pasted belongs to a location where template has already been pasted
    /// </summary>
    /// <param name="CurrColumn"></param>
    /// <param name="CurrRow"></param>
    /// <returns>True if its pasted, False if background is blank and we need to paste template</returns>
        private bool PageNotAlreadyPasted(string CurrColumn, int CurrRow, bool insertBelow)
        {
            // 'Now get all the programs that are linked to this program and check if next page is already ready in report...

            // 'The logic we are going to use is find the last filled page for each of the neighboring programs to the given program
            // 'Then see if current program's addable page is already there in this range. If yes, return false, if no, return true
            // 'We will have a page column number and a page row number. 
            bool CopyTemplate = true;
            try
            {
                int PgColumn_CurrProgram, PgRow_CurrProgram;
                int PgColumn_PrevProgram, PgRow_PrevProgram;
                int PgColumn_NextProgram, PgRow_NextProgram;
                double pgCC, pgRC, pgCm, pgRm, pgCn, pgRn;
                pgCC = GetColIndex(CurrColumn) / (double)GetColIndex(ColumnMax);
                pgRC = CurrRow / (double)(RowMax + Footer);
                PgColumn_CurrProgram = (int)Math.Round(Conversion.Int(GetColIndex(CurrColumn) / (double)GetColIndex(ColumnMax)));
                PgRow_CurrProgram = (int)Math.Round(Conversion.Int(pgRC)); // 'CurrRow / (RowMax))) '' + Footer)))
                if (PgColumn_CurrProgram < pgCC)
                    PgColumn_CurrProgram += 1;
                if (PgRow_CurrProgram < pgRC)
                    PgRow_CurrProgram += 1;
                var tmprow = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName)[0];
                var tmpDataR = Data.Select_Rows("MultiProgramReport", "Report_Id", tmprow[0]);
                if (tmpDataR.Length == 0)
                    return true;
                var tmpProgR = Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName);
                // 'Find out the index of current program. 
                int ProgramIndex = 0;
                for (int jj = 0, loopTo = tmpDataR.Length - 1; jj <= loopTo; jj++)
                {
                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(tmpDataR[jj][1], tmpProgR[0][0], false)))
                    {
                        ProgramIndex = jj;
                        break;
                    } // 'if its this program do not do anything!!
                }

                PgColumn_PrevProgram = -1;
                PgRow_PrevProgram = -1;
                PgColumn_NextProgram = -1;
                PgRow_NextProgram = -1;
                // 'Now check the preceding and succeeding programs situations only
                if (insertBelow)
                {
                    if (DataR[4].ToString().Contains("Column"))
                    {
                        // 'Get the first column and last row of succeeding program
                        // 'Get Last column and last row of preceding program
                        if (ProgramIndex == 0)
                        {
                            // 'Check only the first page of succeeding program
                            pgCn = GetColIndex(tmpDataR[ProgramIndex + 1][6].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRn = (Conversions.ToInteger(tmpDataR[ProgramIndex + 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                            PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                            if (PgColumn_NextProgram < pgCn)
                                PgColumn_NextProgram += 1;
                            if (PgRow_NextProgram < pgRn)
                                PgRow_NextProgram += 1;
                        }
                        else if (ProgramIndex == tmpDataR.Length - 1)
                        {
                            pgCm = GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRm = (Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                            PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                            if (PgColumn_PrevProgram < pgCm)
                                PgColumn_PrevProgram += 1;
                            if (PgRow_PrevProgram < pgRm)
                                PgRow_PrevProgram += 1;
                        }
                        // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                        else
                        {
                            pgCn = GetColIndex(tmpDataR[ProgramIndex + 1][6].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRn = (Conversions.ToInteger(tmpDataR[ProgramIndex + 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                            PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                            if (PgColumn_NextProgram < pgCn)
                                PgColumn_NextProgram += 1;
                            if (PgRow_NextProgram < pgRn)
                                PgRow_NextProgram += 1;
                            pgCm = GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRm = (Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                            PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                            if (PgColumn_PrevProgram < pgCm)
                                PgColumn_PrevProgram += 1;
                            if (PgRow_PrevProgram < pgRm)
                                PgRow_PrevProgram += 1;

                            // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                            // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                            // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(6).ToString) / GetColIndex(ColumnMax)))
                            // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(7)) - 1) / (RowMax + Footer)))
                        }
                        // 'Check if the columns of the current cell is within one of the two pages...
                        if (PgColumn_CurrProgram == PgColumn_PrevProgram)
                        {
                            if (PgRow_PrevProgram >= PgRow_CurrProgram)
                            {
                                CopyTemplate = false;
                            }
                        }

                        if (PgColumn_CurrProgram == PgColumn_NextProgram)
                        {
                            if (PgRow_NextProgram >= PgRow_CurrProgram)
                            {
                                CopyTemplate = false;
                            }
                        }
                    }
                    else
                    {
                        // 'Row-wise and extending for extra measurements or additional programs
                        // If ProgramIndex = 0 Then
                        // ''Check only the first page of succeeding program
                        // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(5)) - 1) / (RowMax + Footer)))
                        // ElseIf ProgramIndex = tmpDataR.Length - 1 Then
                        // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                        // Else
                        // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                        // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(5)) - 1) / (RowMax + Footer)))
                        // End If

                        if (ProgramIndex == 0)
                        {
                            // 'Check only the first page of succeeding program
                            pgCn = GetColIndex(tmpDataR[ProgramIndex + 1][6].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRn = (Conversions.ToInteger(tmpDataR[ProgramIndex + 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                            PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                            if (PgColumn_NextProgram < pgCn)
                                PgColumn_NextProgram += 1;
                            if (PgRow_NextProgram < pgRn)
                                PgRow_NextProgram += 1;
                        }
                        else if (ProgramIndex == tmpDataR.Length - 1)
                        {
                            pgCm = GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRm = (Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                            PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                            if (PgColumn_PrevProgram < pgCm)
                                PgColumn_PrevProgram += 1;
                            if (PgRow_PrevProgram < pgRm)
                                PgRow_PrevProgram += 1;
                        }
                        // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                        else
                        {
                            pgCn = GetColIndex(tmpDataR[ProgramIndex + 1][6].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRn = (Conversions.ToInteger(tmpDataR[ProgramIndex + 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                            PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                            if (PgColumn_NextProgram < pgCn)
                                PgColumn_NextProgram += 1;
                            if (PgRow_NextProgram < pgRn)
                                PgRow_NextProgram += 1;
                            pgCm = GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRm = (Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                            PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                            if (PgColumn_PrevProgram < pgCm)
                                PgColumn_PrevProgram += 1;
                            if (PgRow_PrevProgram < pgRm)
                                PgRow_PrevProgram += 1;

                            // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                            // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                            // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(6).ToString) / GetColIndex(ColumnMax)))
                            // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(7)) - 1) / (RowMax + Footer)))
                        }

                        // 'Check if the columns of the current cell is within one of the two pages...
                        if (PgRow_CurrProgram == PgRow_PrevProgram)
                        {
                            if (PgColumn_PrevProgram >= PgColumn_CurrProgram)
                            {
                                CopyTemplate = false;
                            }
                        }

                        if (PgRow_CurrProgram == PgRow_NextProgram)
                        {
                            if (PgColumn_NextProgram >= PgColumn_CurrProgram)
                            {
                                CopyTemplate = false;
                            }
                        }
                    }
                }
                else if (DataR[4].ToString().Contains("Column"))
                {
                    // 'Get the first column and last row of succeeding program
                    // 'Get Last column and last row of preceding program
                    // If ProgramIndex = 0 Then
                    // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(6).ToString) / GetColIndex(ColumnMax)))
                    // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(7)) - 1) / (RowMax + Footer)))
                    // ElseIf ProgramIndex = tmpDataR.Length - 1 Then
                    // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                    // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                    // Else
                    // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                    // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                    // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(6).ToString) / GetColIndex(ColumnMax)))
                    // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(7)) - 1) / (RowMax + Footer)))
                    // End If

                    if (ProgramIndex == 0)
                    {
                        // 'Check only the first page of succeeding program
                        pgCn = GetColIndex(tmpDataR[ProgramIndex + 1][6].ToString()) / (double)GetColIndex(ColumnMax);
                        pgRn = (Conversions.ToInteger(tmpDataR[ProgramIndex + 1][7]) - 1) / (double)(RowMax + Footer);
                        PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                        PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                        if (PgColumn_NextProgram < pgCn)
                            PgColumn_NextProgram += 1;
                        if (PgRow_NextProgram < pgRn)
                            PgRow_NextProgram += 1;
                    }
                    else if (ProgramIndex == tmpDataR.Length - 1)
                    {
                        pgCm = GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax);
                        pgRm = (Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer);
                        PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                        PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                        if (PgColumn_PrevProgram < pgCm)
                            PgColumn_PrevProgram += 1;
                        if (PgRow_PrevProgram < pgRm)
                            PgRow_PrevProgram += 1;
                    }
                    // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                    // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                    else
                    {
                        pgCn = GetColIndex(tmpDataR[ProgramIndex + 1][6].ToString()) / (double)GetColIndex(ColumnMax);
                        pgRn = (Conversions.ToInteger(tmpDataR[ProgramIndex + 1][7]) - 1) / (double)(RowMax + Footer);
                        PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                        PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                        if (PgColumn_NextProgram < pgCn)
                            PgColumn_NextProgram += 1;
                        if (PgRow_NextProgram < pgRn)
                            PgRow_NextProgram += 1;
                        pgCm = GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax);
                        pgRm = (Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer);
                        PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                        PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                        if (PgColumn_PrevProgram < pgCm)
                            PgColumn_PrevProgram += 1;
                        if (PgRow_PrevProgram < pgRm)
                            PgRow_PrevProgram += 1;

                        // PgColumn_PrevProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex - 1)(8).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_PrevProgram = CInt(Int((CInt(tmpDataR(ProgramIndex - 1)(7)) - 1) / (RowMax + Footer)))
                        // PgColumn_NextProgram = CInt(Int(GetColIndex(tmpDataR(ProgramIndex + 1)(6).ToString) / GetColIndex(ColumnMax)))
                        // PgRow_NextProgram = CInt(Int((CInt(tmpDataR(ProgramIndex + 1)(7)) - 1) / (RowMax + Footer)))
                    }

                    // 'Check if the columns of the current cell is within one of the two pages...

                    if (PgColumn_CurrProgram == PgColumn_PrevProgram)
                    {
                        if (PgRow_PrevProgram >= PgRow_CurrProgram)
                        {
                            CopyTemplate = false;
                        }
                    }

                    if (PgColumn_CurrProgram == PgColumn_NextProgram)
                    {
                        if (PgRow_NextProgram >= PgRow_CurrProgram)
                        {
                            CopyTemplate = false;
                        }
                    }
                }
                // If PgColumn_CurrProgram = 0 AndAlso PgRow_CurrProgram = 0 Then CopyTemplate = False
                else
                {
                    // 'Row Wise and we are extending more runs in next page...
                    if (ProgramIndex == 0)
                    {
                        // 'Check only the first page of succeeding program
                        PgColumn_NextProgram = (int)Math.Round(Conversion.Int(GetColIndex(tmpDataR[ProgramIndex + 1][8].ToString()) / (double)GetColIndex(ColumnMax)));
                        PgRow_NextProgram = (int)Math.Round(Conversion.Int((Conversions.ToInteger(tmpDataR[ProgramIndex + 1][5]) - 1) / (double)(RowMax + Footer)));
                    }
                    else if (ProgramIndex == tmpDataR.Length - 1)
                    {
                        PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax)));
                        PgRow_PrevProgram = (int)Math.Round(Conversion.Int((Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer)));
                    }
                    else
                    {
                        PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(GetColIndex(tmpDataR[ProgramIndex - 1][8].ToString()) / (double)GetColIndex(ColumnMax)));
                        PgRow_PrevProgram = (int)Math.Round(Conversion.Int((Conversions.ToInteger(tmpDataR[ProgramIndex - 1][7]) - 1) / (double)(RowMax + Footer)));
                        PgColumn_NextProgram = (int)Math.Round(Conversion.Int(GetColIndex(tmpDataR[ProgramIndex + 1][8].ToString()) / (double)GetColIndex(ColumnMax)));
                        PgRow_NextProgram = (int)Math.Round(Conversion.Int((Conversions.ToInteger(tmpDataR[ProgramIndex + 1][5]) - 1) / (double)(RowMax + Footer)));
                    }
                    // 'Check if the columns of the current cell is within one of the two pages...
                    if (PgRow_CurrProgram == PgRow_PrevProgram)
                    {
                        if (PgColumn_PrevProgram >= PgColumn_CurrProgram)
                        {
                            CopyTemplate = false;
                        }
                    }

                    if (PgRow_CurrProgram == PgRow_NextProgram)
                    {
                        if (PgColumn_NextProgram >= PgColumn_CurrProgram)
                        {
                            CopyTemplate = false;
                        }
                    }
                }
            }


            // Dim frr As Double = GetColIndex(CurrColumn) / GetColIndex(ColumnMax)
            // Dim pgn As Integer = CInt(Int(frr))
            // Dim tmpDataR As System.Data.DataRow() = Data.Select_Rows("MultiProgramReport", "Report_Id", DataR.Item(0))
            // Dim stCol, enCol As Integer
            // Dim CopyTemplate As Boolean = True
            // Dim tmpProgR As System.Data.DataRow() = Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName)
            // For jj As Integer = 0 To tmpDataR.Length - 1
            // If tmpDataR(jj)(1) = tmpProgR(0)(0) Then Continue For ''if its this program do not do anything!!
            // stCol = GetColIndex(tmpDataR(jj)(6)) ''Get the First used column for this program
            // enCol = GetColIndex(tmpDataR(jj)(8)) ''Get the First used column for this program
            // ''Check if this has already been pasted here because this program may share the last page of the previous program
            // 'frr = thisCol / GetColIndex(ColumnMax)
            // Dim stpgn As Integer = CInt(Int(stCol / GetColIndex(ColumnMax)))
            // Dim enpgn As Integer = CInt(Int(enCol / GetColIndex(ColumnMax)))
            // If stpgn = pgn OrElse enpgn >= pgn Then
            // ''Check if rows have been filled up till the current row. 
            // If tmpDataR(jj)(7) - 1 > CurrRow Then '' check if last used row is in this page....
            // CopyTemplate = False
            // Exit For
            // End If
            // End If
            // Next
            catch (Exception ex)
            {
            }

            return CopyTemplate;
        }

        private bool PageNotAlreadyPasted(string CurrColumn, int CurrRow)
        {
            // 'Now get all the programs that are linked to this program and check if next page is already ready in report...

            // 'The logic we are going to use is find the last filled page for each of the neighboring programs to the given program
            // 'Then see if current program's addable page is already there in this range. If yes, return false, if no, return true
            // 'We will have a page column number and a page row number. 
            bool CopyTemplate = true;
            try
            {
                int PgColumn_CurrProgram, PgRow_CurrProgram;
                int PgColumn_PrevProgram, PgRow_PrevProgram;
                int PgColumn_NextProgram, PgRow_NextProgram;
                double pgCC, pgRC, pgCm, pgRm, pgCn, pgRn;
                var tmprow = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName)[0];
                var tmpDataR = Data.Select_Rows("MultiProgramReport", "Report_Id", tmprow[0]);
                if (tmpDataR.Length == 0)
                    return true;
                var tmpProgR = Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName);
                if (tmpProgR.Length == 0)
                    return true;

                // 'Find out the index of current program. 
                int ProgramIndex = 0;
                for (int jj = 0, loopTo = tmpDataR.Length - 1; jj <= loopTo; jj++)
                {
                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(tmpDataR[jj][1], tmpProgR[0][0], false)))
                    {
                        ProgramIndex = jj;
                        break;
                    }
                }

                PgColumn_PrevProgram = -1;
                PgRow_PrevProgram = -1;
                PgColumn_NextProgram = -1;
                PgRow_NextProgram = -1;
                int checkColumnIndex = -1;
                for (int jj = 0, loopTo1 = tmpDataR.Length - 1; jj <= loopTo1; jj++)
                {
                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectNotEqual(tmpDataR[jj][1], tmpProgR[0][0], false)))
                    {
                        if (DataR[4].ToString().Contains("Column"))
                        {
                            pgCC = GetColIndex(CurrColumn) / (double)GetColIndex(ColumnMax);
                            pgRC = CurrRow / (double)(RowMax + Footer);
                            PgColumn_CurrProgram = (int)Math.Round(pgCC); // ' Int(GetColIndex(CurrColumn) / GetColIndex(ColumnMax)))
                            PgRow_CurrProgram = (int)Math.Round(Conversion.Int(pgRC)); // 'CurrRow / (RowMax))) '' + Footer)))
                            if (PgColumn_CurrProgram < pgCC)
                                PgColumn_CurrProgram += 1;
                            if (PgRow_CurrProgram < pgRC)
                                PgRow_CurrProgram += 1;

                            // 'Get the first and last columns for the last written row for each program of this report. 
                            pgCm = GetColIndex(tmpDataR[jj][6].ToString()) / (double)GetColIndex(ColumnMax);
                            pgCn = GetColIndex(tmpDataR[jj][8].ToString()) / (double)GetColIndex(ColumnMax);
                            pgRn = (Conversions.ToInteger(tmpDataR[jj][7]) - 1) / (double)(RowMax + Footer);
                            PgColumn_NextProgram = (int)Math.Round(Conversion.Int(pgCn));
                            PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                            PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                            if (PgColumn_NextProgram < pgCn)
                                PgColumn_NextProgram += 1;
                            if (PgRow_NextProgram < pgRn)
                                PgRow_NextProgram += 1;
                            if (PgColumn_PrevProgram < pgCm)
                                PgColumn_PrevProgram += 1;
                            if (jj > ProgramIndex)
                                checkColumnIndex = PgColumn_PrevProgram;
                            else
                                checkColumnIndex = PgColumn_NextProgram;
                            if (PgColumn_CurrProgram == checkColumnIndex)
                            {
                                if (PgRow_NextProgram >= PgRow_CurrProgram)
                                {
                                    CopyTemplate = false;
                                    break; // ' if we have common page for even one program, we dont need to write. so we can quit!!
                                }
                            }
                        }
                        else
                        {
                            pgCC = GetColIndex(CurrColumn) / (double)(GetColIndex(ColumnMax) + Sider);
                            pgRC = CurrRow / (double)(RowMax + Footer);
                            PgColumn_CurrProgram = (int)Math.Round(pgCC); // ' Int(GetColIndex(CurrColumn) / GetColIndex(ColumnMax)))
                            PgRow_CurrProgram = (int)Math.Round(Conversion.Int(pgRC)); // 'CurrRow / (RowMax))) '' + Footer)))
                            if (PgColumn_CurrProgram < pgCC)
                                PgColumn_CurrProgram += 1;
                            if (PgRow_CurrProgram < pgRC)
                                PgRow_CurrProgram += 1;
                            // If CurrRow = RowMax * PgRow_CurrProgram + Footer + 1 Then PgRow_CurrProgram += 1

                            // 'Get the first and last rows for the last written column for each program of this report. 
                            pgRm = Conversions.ToInteger(tmpDataR[jj][5]) / (double)(RowMax + Footer);
                            pgRn = Conversions.ToInteger(tmpDataR[jj][7]) / (double)(RowMax + Footer);
                            pgCm = GetColIndex(Conversions.ToString(tmpDataR[jj][8])) / (double)(GetColIndex(ColumnMax) + Sider + GetColIndex(Initial_Column) - 1);
                            PgRow_PrevProgram = (int)Math.Round(Conversion.Int(pgRm));
                            PgRow_NextProgram = (int)Math.Round(Conversion.Int(pgRn));
                            PgColumn_PrevProgram = (int)Math.Round(Conversion.Int(pgCm));
                            if (PgRow_PrevProgram < pgRm)
                                PgRow_PrevProgram += 1;
                            if (PgRow_NextProgram < pgRn)
                                PgRow_NextProgram += 1;
                            if (PgColumn_PrevProgram < pgCm)
                                PgColumn_PrevProgram += 1;
                            if (jj < ProgramIndex)
                                checkColumnIndex = PgRow_NextProgram;
                            else
                                checkColumnIndex = PgRow_PrevProgram;
                            if (PgRow_CurrProgram == checkColumnIndex)
                            {
                                if (PgColumn_PrevProgram >= PgColumn_CurrProgram)
                                {
                                    CopyTemplate = false;
                                    break; // ' if we have common page for even one program, we dont need to write. so we can quit!!
                                }
                            }
                        }
                    } // 'if its this program do not do anything!!
                }
            }
            catch (Exception ex)
            {
            }

            return CopyTemplate;
        }



        // __________________________Function to calculate the current template ranges.The all calculations of Header,Footer,Max and Min Range as according to the ReportType.____________________
        private void CalculateMargins(string ReportType)
        {
            try
            {
                if (Data is null)
                {
                    Data = new xl_DB();
                    dataS = Data.FillTableRange(DatabaseConstants.TableNames);
                }

                DataRow DataR;
                if (IsHCM)
                {
                    // DataR = Data.DataS.Tables("MasterTemplateSettings").Select("ReportType=" & "'" & "ColumnWiseLandscape" & "'")(0)
                    Header = 0;
                    Footer = 0;
                    RowMax = 50;
                    ColumnMax = "B";
                    TemplateFinalRange = "B50";       // set header footer initial, max row and alll data here for HobChecker
                    InitialField_Row = 1;
                    InitialField_Column = "A";
                    Initial_Row = 1;
                    Initial_Column = "A";
                    TemplateInitialRange = Initial_Column + Initial_Row.ToString();
                    nudFormatdigits.Value = Conversions.ToDecimal("4");
                }
                else if (IsDTVSM)              // set header footer initial, max row and alll data here for DelphiTVS
                {
                    Header = 0;
                    Footer = 0;
                    RowMax = 5000;
                    ColumnMax = "F";
                    TemplateFinalRange = "F5000";
                    InitialField_Row = 7;
                    InitialField_Column = "A";
                    Initial_Row = 1;
                    Initial_Column = "A";
                    TemplateInitialRange = Initial_Column + Initial_Row.ToString();
                    nudFormatdigits.Value = Conversions.ToDecimal("4");
                }
                else
                {
                    DataR = Data.DataS.Tables["MasterTemplateSettings"].Select("ReportType=" + "'" + ReportType + "'")[0];
                    Header = Conversions.ToInteger(DataR[2]);
                    Footer = Conversions.ToInteger(DataR[3]);
                    RowMax = Conversions.ToInteger(DataR[4]);
                    ColumnMax = Conversions.ToString(DataR[5]);
                    TemplateFinalRange = Conversions.ToString(DataR[6]);   // set header footer initial, max row and alll data here for all other temmplates
                    InitialField_Row = Conversions.ToInteger(DataR[7]);
                    InitialField_Column = Conversions.ToString(DataR[8]);
                    Initial_Row = 1;
                    Initial_Column = "A";
                    TemplateInitialRange = Initial_Column + Initial_Row.ToString();
                    if (DataR.IsNull(9))
                        DataR[9] = "4";
                    nudFormatdigits.Value = Conversions.ToInteger(DataR[9]);
                    MasterTemplateID = Conversions.ToInteger(DataR["MasterReport_Id"]);
                    try
                    {
                        if (DataR.IsNull(10))
                            DataR[10] = -2;
                        PartNameLabelLocation_Row = Conversions.ToInteger(DataR[10]);
                        if (DataR.IsNull(11))
                            DataR[11] = 0;
                        PartNameLabelLocation_Col = Conversions.ToInteger(DataR[11]);
                        if (DataR.IsNull(12))
                            DataR[12] = -2;
                        PartName_Location_Row = Conversions.ToInteger(DataR[12]);
                        if (DataR.IsNull(13))
                            DataR[13] = 1;
                        PartName_Location_Col = Conversions.ToInteger(DataR[13]);
                    }
                    catch (Exception ex)
                    {
                        // 'Do nothing. We did not get anything. So lets just move on as if nothing happened!!
                    }
                }

                SourceInitialRange = TemplateInitialRange;
                SourceFinalRange = TemplateFinalRange;
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E018", ex);
            }
        }

        private void SetDefaultValuesForStyles()
        {
            try
            {
                txtMaxRow.Text = RowMax.ToString();
                // MessageBox.Show(RowMax)
                txtMaxColumn.Text = GetColumnName(GetColIndex(ColumnMax)); // + Sider)
                                                                           // MessageBox.Show(ColumnMax)
                txtInitialRow.Text = InitialField_Row.ToString();
                txtInitialColumn.Text = InitialField_Column;
                // 'Added 14 Jun 2019. R A N. To accomodate user-defined locations for inserting part names (program name itself). 
                // If PartName_Location_Col = "" Then
                // PartNameLabelLocation_Col = 0
                // PartNameLabelLocation_Row = -2
                // PartName_Location_Col = 1
                // PartName_Location_Row = -2
                // 'End If 

                txtPartName_Label_Col.Text = GetColumnName(GetColIndex(InitialField_Column) + PartNameLabelLocation_Col);
                txtPartName_Label_Row.Text = (InitialField_Row + PartNameLabelLocation_Row).ToString();
                txtPartName_Col.Text = GetColumnName(GetColIndex(InitialField_Column) + PartName_Location_Col);
                txtPartName_Row.Text = (InitialField_Row + PartName_Location_Row).ToString();
                for (int i = 0, loopTo = TemplateFinalRange.Length - 1; i <= loopTo; i++)
                {
                    if (char.IsNumber(TemplateFinalRange[i]))
                    {
                        txtendRow.Text = TemplateFinalRange.Substring(i, TemplateFinalRange.Length - i);
                        txtEndCol.Text = TemplateFinalRange.Substring(0, i);
                        break;
                    }
                }

                Sider = GetColIndex(txtEndCol.Text) - GetColIndex(ColumnMax);
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message + '\r' + ex.StackTrace);
            }
        }

        // __________________________________________________Function to handle the Page range update click________________________________________________________________________________
        private void btnUpdateRange_Click(object sender, EventArgs e)
        {
            try
            {
                int _endRow;
                if (int.TryParse(txtMaxRow.Text, out RowMax) && !string.IsNullOrEmpty(txtMaxColumn.Text) && !string.IsNullOrEmpty(txtInitialColumn.Text) && int.TryParse(txtInitialRow.Text, out InitialField_Row) && int.TryParse(txtendRow.Text, out _endRow) && !string.IsNullOrEmpty(txtEndCol.Text))
                {
                    InitialField_Column = txtInitialColumn.Text.ToUpper();
                    RowMax = Conversions.ToInteger(txtMaxRow.Text); // 'Added by RAN 30 Jan 2015...
                    ColumnMax = txtMaxColumn.Text.ToUpper();
                    EndColumn = txtEndCol.Text.ToUpper();
                    PartNameLabelLocation_Col = GetColIndex(txtPartName_Label_Col.Text.ToUpper()) - GetColIndex(InitialField_Column);
                    PartNameLabelLocation_Row = Conversions.ToInteger(txtPartName_Label_Row.Text) - InitialField_Row;
                    PartName_Location_Col = GetColIndex(txtPartName_Col.Text.ToUpper()) - GetColIndex(InitialField_Column);
                    PartName_Location_Row = Conversions.ToInteger(txtPartName_Row.Text) - InitialField_Row;
                    Sider = GetColIndex(EndColumn) - GetColIndex(ColumnMax);
                    Header = InitialField_Row - Initial_Row + 1;
                    Footer = _endRow - RowMax;
                    if (chkMakeSettingsDefault.Checked)
                    {
                        var DataR = Data.DataS.Tables["MasterTemplateSettings"].Select("ReportType = " + "'" + ReportType + "'")[0];
                        DataR[2] = Header;
                        DataR[3] = Footer;
                        DataR[4] = RowMax; // 'Added by RAN 30 Jan 2015 not present before. Master Template settings for RowWise not updating
                        DataR[5] = ColumnMax; // ' Added by RAN 30 Jan 15
                        DataR[6] = txtEndCol.Text.ToUpper() + Conversions.ToInteger(txtendRow.Text);
                        DataR[7] = InitialField_Row;
                        DataR[8] = InitialField_Column;
                        DataR[9] = nudFormatdigits.Value;
                        DataR[10] = PartNameLabelLocation_Row;
                        DataR[11] = PartNameLabelLocation_Col;
                        DataR[12] = PartName_Location_Row;
                        DataR[13] = PartName_Location_Col;
                        Data.Update_Table("MasterTemplateSettings");
                        TemplateFinalRange = txtEndCol.Text.ToUpper() + Conversions.ToInteger(txtendRow.Text);
                    }
                    else
                    {
                        Report_temp_update();
                    }

                    if (chkEarlierMadeReportsAlso.Checked)
                    {
                        for (int i = 0, loopTo = Data.GetTable("ReportInformation").Rows.Count - 1; i <= loopTo; i++)
                            Data.GetTable("ReportInformation").Rows[i][24] = nudFormatdigits.Value;
                        Data.Update_Table("ReportInformation");
                    }

                    MessageBox.Show("Report Range Updated Successfully !!", "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("Invalid numbers are given as row and coloumns." + Microsoft.VisualBasic.Constants.vbCrLf + "Please correct the values and try again.", "Excel Module ", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            catch (InvalidCastException ex)
            {
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E019", ex);
            }
        }
        // ________________________________________________Function to handle the insertion of the picture.______________________________________________________________________________
        private void chkInsertPicture_CheckedChanged(object sender, EventArgs e)
        {
            pnlInsertPicture.Enabled = Conversions.ToBoolean(Interaction.IIf(chkInsertPicture.Checked, true, false));
            PanelRcadPic.Enabled = Conversions.ToBoolean(Interaction.IIf(chkInsertPicture.Checked, true, false));
        }
        // _____________________________________________Property to tell whether picture is inserted or not._____________________________________________________________________________
        public bool IsPictureToBeInserted
        {
            get
            {
                return chkInsertPicture.Checked;
            }
        }
        // ____________________________________________________property to tell what is size of the picture._____________________________________________________________________________
        public Size SizeOfPicture
        {
            get
            {
                try
                {
                    return new Size(Conversions.ToInteger(txtWidth.Text), Conversions.ToInteger(txtHeight.Text));
                }
                catch (Exception ex)
                {
                    return new Size(0, 0);
                }
            }
        }
        // ___________________________________________Function to calculate the picture sizes for quick data transfer._____________________________________________________________________
        private void AssignDefaultPictureValues(string ReportType)
        {
            try
            {
                Data.DataA = new OleDbDataAdapter("Select * From [Settings]", DatabaseConstants.ConnectionString);
                var ds1 = new DataSet();
                Data.DataA.Fill(ds1);
                DataRow dR;
                if (ds1.Tables[0].Rows.Count > 1)
                {
                    for (int i = 0, loopTo = ds1.Tables[0].Rows.Count - 1; i <= loopTo; i++)
                    {
                        dR = ds1.Tables[0].Rows[i];
                        if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(dR["MasterReport_Id"], MasterTemplateID, false)))
                        {
                            txtWidth.Text = Conversions.ToString(dR["Image1_Width"]);
                            txtHeight.Text = Conversions.ToString(dR["Image1_Height"]);
                            txtRow.Text = Conversions.ToString(dR["Image1_Row"]);
                            txtColumn.Text = Conversions.ToString(dR["Image1_Column"]);
                            Rcadwidth.Text = Conversions.ToString(dR["Image2_Width"]);
                            Rcadheight.Text = Conversions.ToString(dR["Image2_Height"]);
                            RcadRow.Text = Conversions.ToString(dR["Image2_Row"]);
                            RcadCol.Text = Conversions.ToString(dR["Image2_Column"]);
                            return;
                        }
                    }
                }

                if (ReportType == "ColumnWiseLandscape")
                {
                    txtWidth.Text = "295";
                    txtHeight.Text = "221";
                    txtRow.Text = "10";
                    txtColumn.Text = "B";
                    Rcadwidth.Text = "295";
                    Rcadheight.Text = "221";
                    RcadRow.Text = "14";
                    RcadCol.Text = "B";
                }
                else if (ReportType == "DelphiTVSTemplate")
                {
                    txtWidth.Text = "693";
                    txtHeight.Text = "520";
                    txtRow.Text = "10";
                    txtColumn.Text = "B";
                    Rcadwidth.Text = "693";
                    Rcadheight.Text = "520";
                    RcadRow.Text = "14";
                    RcadCol.Text = "B";
                }
                else if (ReportType == "HobCheckerTemplate")
                {
                    txtWidth.Text = "693";
                    txtHeight.Text = "520";
                    txtRow.Text = "10";
                    txtColumn.Text = "B";
                    Rcadwidth.Text = "693";
                    Rcadheight.Text = "520";
                    RcadRow.Text = "14";
                    RcadCol.Text = "B";
                }
                else if (ReportType == "ColumnWisePortrait")
                {
                    txtWidth.Text = "693";
                    txtHeight.Text = "520";
                    txtRow.Text = "10";
                    txtColumn.Text = "B";
                    Rcadwidth.Text = "693";
                    Rcadheight.Text = "520";
                    RcadRow.Text = "14";
                    RcadCol.Text = "B";
                }
                else if (ReportType == "RowWiseLandscape")
                {
                    txtWidth.Text = "373";
                    txtHeight.Text = "280";
                    txtRow.Text = "9";
                    txtColumn.Text = "C";
                    Rcadwidth.Text = "373";
                    Rcadheight.Text = "280";
                    RcadRow.Text = "9";
                    RcadCol.Text = "F";
                }
                else if (ReportType == "RowWisePortrait")
                {
                    txtWidth.Text = "335";
                    txtHeight.Text = "252";
                    txtRow.Text = "9";
                    txtColumn.Text = "C";
                    Rcadwidth.Text = "335";
                    Rcadheight.Text = "252";
                    RcadRow.Text = "9";
                    RcadCol.Text = "F";
                }
            }
            catch (Exception ex)
            {
            }
        }
        // __________________________________________________________Handling manual update about the pictute details.______________________________________________________________________
        private void btnUpdatePicDetails_Click(object sender, EventArgs e)
        {
            try
            {
                //var xls = _dsoFramer.ActiveDocument.Application.Workbooks(1).Worksheets(1);
                //var R = _dsoFramer.ActiveDocument.Application.Selection;
                //txtRow.Text = Conversions.ToString(R.Row);
                //txtColumn.Text = GetColumnName(Conversions.ToInteger(R.Column));
                //int RowCount, ColCount, tmpleft, tmptop;         // Now calculate the height and width of the picture
                //RowCount = Conversions.ToInteger(R.Rows.Count);
                //ColCount = Conversions.ToInteger(R.Columns.Count);
                //tmptop = Conversions.ToInteger(Operators.SubtractObject(xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(Operators.AddObject(R.Column, ColCount))), Operators.AddObject(R.Row, RowCount))).top, xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(R.Column)), R.Row)).top));
                //tmpleft = Conversions.ToInteger(Operators.SubtractObject(xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(Operators.AddObject(R.Column, ColCount))), Operators.AddObject(R.Row, RowCount))).left, xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(R.Column)), R.Row)).left));
                //txtWidth.Text = tmpleft.ToString();
                //txtHeight.Text = tmptop.ToString();
                //// update database..
                //Update_InsertImageValue(tmpleft, tmptop, true);
            }
            catch (Exception ex)
            {
                // Exception can occur if not any selection is made.In that case inform user about that.
                MessageBox.Show("Pleae select the area to be filled with picture on report and then click on Update button.", "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
        }

        private void Update_InsertImageValue(double width, double height, bool Image1)
        {
            try
            {
                Data.DataA = new OleDbDataAdapter("Select * From [Settings]", DatabaseConstants.ConnectionString);
                var ds1 = new DataSet();
                Data.DataA.Fill(ds1);
                if (ds1.Tables[0].Rows.Count < 1)
                {
                    ds1.Tables[0].Rows.Add((object)1);
                }

                if (Image1)
                {
                    ds1.Tables[0].Rows[0]["Image1_width"] = width;
                    ds1.Tables[0].Rows[0]["Image1_Height"] = height;
                    ds1.Tables[0].Rows[0]["Image1_Column"] = txtColumn.Text;
                    ds1.Tables[0].Rows[0]["Image1_Row"] = txtRow.Text;
                }
                else
                {
                    ds1.Tables[0].Rows[0]["Image2_width"] = width;
                    ds1.Tables[0].Rows[0]["Image2_Height"] = height;
                    ds1.Tables[0].Rows[0]["Image2_Column"] = RcadCol.Text;
                    ds1.Tables[0].Rows[0]["Image2_Row"] = RcadRow.Text;
                }

                var cb = new OleDbCommandBuilder(Data.DataA);
                Data.DataA.Update(ds1);
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error in updating Image position Values", "Excel Module", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
        }

        // __________________________________________________________Function to calculate the aspect ratio of 4:3_____________________________________________________________________________
        private System.Drawing.Point MaintainAspectRatio(double width, double height, double asratio)
        {
            if (height > width) // height is higher , so maintain the aspect ratio of the image now 
            {
                width = height * asratio;
            }
            else
            {
                height = width / asratio;
            }

            return new System.Drawing.Point((int)Math.Round(width), (int)Math.Round(height));
        }
        // _________________________________________________Functions to return the column name. ____________________________________________________________________________________
        public string GetColumnName(int colNo)
        {
            int temp = (int)Math.Round(Conversion.Int((colNo - 1) / 26d));
            int Ascii = colNo - 26 * temp + 64;
            string GG;
            if (temp > 0)
            {
                GG = Conversions.ToString((char)(temp + 64)) + (char)Ascii;
            }
            else
            {
                GG = Conversions.ToString((char)Ascii);
            }

            return GG;
        }
        // ______________________________________________Function to ge column index_____________________________________________________________________________
        public int GetColIndex(string ColName)
        {
            var colIndex = default(int);
            if (Strings.Len(ColName) == 1)
            {
                colIndex = Strings.Asc(ColName) - 64;
            }
            else if (Strings.Len(ColName) == 2)
            {
                colIndex = (Strings.Asc(Strings.Mid(ColName, 1, 1)) - 64) * 26 + Strings.Asc(Strings.Mid(ColName, 2, 1)) - 64;
            }

            return colIndex;
        }

        private void ProgramInNewSheet_CheckedChanged(object sender, EventArgs e)
        {
            if (ProgramInNewSheet.Checked)
            {
                AllProgramInSingleReport = false;
            }
            else
            {
                AllProgramInSingleReport = true;
            }
        }

        public void DeviationReporting(string fileName, List<List<double>> devList, ArrayList tolList)
        {
            //try
            //{
            //    var xla = new Microsoft.Office.Interop.Excel.Application();
            //    var xlw = xla.Workbooks.Add();
            //    Worksheet xls = (Worksheet)xlw.ActiveSheet;
            //    if (devList.Count > 0)
            //    {
            //        double min = Conversions.ToDouble(Operators.SubtractObject(0, tolList[1]));
            //        double max = Conversions.ToDouble(Operators.AddObject(0, tolList[1]));
            //        xls.Cells[(object)1, (object)1].Value = "Radius";
            //        xls.Cells[(object)1, (object)2].Value = "Tolerance";
            //        xls.Cells[(object)1, (object)3].Value = "CompanyName";
            //        xls.Cells[(object)1, (object)4].Value = "Component Drawing No.";
            //        xls.Cells[(object)1, (object)5].Value = "Component Serial No.";
            //        xls.Cells[(object)1, (object)6].Value = "Component Revision No.";
            //        for (int i = 1, loopTo = tolList.Count; i <= loopTo; i++)
            //            xls.Cells[(object)2, (object)i].Value = tolList[i - 1];
            //        xls.Cells[(object)4, (object)1].Value = "Count";
            //        xls.Cells[(object)5, (object)1].Value = "ZLevel";
            //        for (int i = 1, loopTo1 = devList.Count; i <= loopTo1; i++)
            //            xls.Cells[(object)(5 + i), (object)1].Value = (object)i;
            //        int maxcol = 0;
            //        for (int i = 0, loopTo2 = devList.Count - 1; i <= loopTo2; i++)
            //        {
            //            if (devList[i].Count > maxcol)
            //            {
            //                maxcol = devList[i].Count;
            //            }
            //        }

            //        xls.get_Range("B$1:F$1").ColumnWidth = (object)20;
            //        for (int i = 1, loopTo3 = maxcol; i <= loopTo3; i++)
            //            xls.Cells[(object)4, (object)(1 + i)].Value = (object)i;
            //        string Lastcol = GetColumnName(maxcol + 1);
            //        int lastrow = devList.Count + 5;
            //        for (int j = 6, loopTo4 = devList.Count + 5; j <= loopTo4; j++)
            //        {
            //            for (int k = 2, loopTo5 = devList[j - 6].Count + 1; k <= loopTo5; k++)
            //                xls.Cells[(object)j, GetColumnName(k)].Value = (object)devList[j - 6][k - 2];
            //        }
            //        // xls.Range("B4:" & Lastcol & lastrow).Value = devList
            //        {
            //            var withBlock = xls.get_Range("B6:" + Lastcol + lastrow).FormatConditions.Add((XlFormatConditionType)10);
            //            withBlock.Interior.ColorIndex = XlColorIndex.xlColorIndexNone;
            //        }

            //        {
            //            var withBlock1 = xls.get_Range("B6:" + Lastcol + lastrow).FormatConditions.Add(XlFormatConditionType.xlCellValue, XlFormatConditionOperator.xlBetween, (object)min, (object)max);
            //            withBlock1.Interior.ColorIndex = (object)35;
            //        }

            //        {
            //            var withBlock2 = xls.get_Range("B6:" + Lastcol + lastrow).FormatConditions.Add((XlFormatConditionType)1, (object)2, (object)min, (object)max);
            //            withBlock2.Interior.ColorIndex = (object)46;
            //        }
            //    }

            //    xlw.SaveAs(fileName);
            //    xlw.Close();
            //    xla.Quit();
            //    if (xls is object)
            //        xls = null;
            //    if (xlw is object)
            //        xlw = null;
            //    if (xla is object)
            //        xla = null;
            //    // Calling Garbage Collector to claim these Unrefrenced Objects from Memory
            //    GC.Collect();
            //    GC.WaitForPendingFinalizers();
            //}
            //catch (Exception ex)
            //{
            //}
        }

        private void UpdateRcadPic_Click(object sender, EventArgs e)
        {
            //var xls = _dsoFramer.ActiveDocument.Application.Workbooks(1).Worksheets(1);
            //var R = _dsoFramer.ActiveDocument.Application.Selection;
            //RcadRow.Text = Conversions.ToString(R.Row);
            //RcadCol.Text = GetColumnName(Conversions.ToInteger(R.Column));
            //int RowCount, ColCount, tmpleft, tmptop;         // Now calculate the height and width of the picture
            //RowCount = Conversions.ToInteger(R.Rows.Count);
            //ColCount = Conversions.ToInteger(R.Columns.Count);
            //tmptop = Conversions.ToInteger(Operators.SubtractObject(xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(Operators.AddObject(R.Column, ColCount))), Operators.AddObject(R.Row, RowCount))).top, xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(R.Column)), R.Row)).top));
            //tmpleft = Conversions.ToInteger(Operators.SubtractObject(xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(Operators.AddObject(R.Column, ColCount))), Operators.AddObject(R.Row, RowCount))).left, xls.range(Operators.ConcatenateObject(GetColumnName(Conversions.ToInteger(R.Column)), R.Row)).left));
            //Rcadwidth.Text = tmpleft.ToString();
            //Rcadheight.Text = tmptop.ToString();
            //// update database..
            //Update_InsertImageValue(tmpleft, tmptop, false);
        }

        // ____________To update the no of rows and columns in template through setting tab end column and end row value, if user clicks on build________________________
        private void Report_temp_update()
        {
            try
            {
                var _xls = default(object);
                int i, tempr;
                string tempc;
                tempr = Conversions.ToInteger(txtendRow.Text);
                tempc = GetColumnName(GetColIndex(txtEndCol.Text));
                try
                {
                    //_xls = _dsoFramer.ActiveDocument.Sheets(1);       // Get the current opened excel sheet in DSO Framer.
                    //_xls.Activate();
                }
                catch (Exception ex)
                {
                    // Return
                }

                if (Conversions.ToInteger(txtendRow.Text) >= 30)               // to update end row value
                {
                    i = Math.Abs(tempr - RowMax);
                    if (_xls is object)
                    {
                        if (Math.Sign(tempr - RowMax) == 1)
                        {
                            for (int a = 1, loopTo = i; a <= loopTo; a++)
                                xL.sheet.CreateRow(15);
                                //_xls.Rows("15").Insert();    // inserting new row after 15th row, this row no. should be atleast less than (rowmax- footer)
                        }
                        else
                        {
                            for (int a = 1, loopTo1 = i; a <= loopTo1; a++)
                                xL.row = xL.sheet.CreateRow(15);
                                xL.sheet.RemoveRow(xL.row);
                                //_xls.Rows("15").Delete();
                        }
                    }

                    RowMax = tempr - Footer;                 // new maximum row no.
                    TemplateFinalRange = GetColumnName(GetColIndex(ColumnMax) + Sider) + txtendRow.Text;
                }

                if (GetColIndex(txtEndCol.Text) > GetColIndex("I"))      // to update end column value, min value for column should be 'I' i.e.>'H'
                {
                    i = Math.Abs(GetColIndex(tempc) - GetColIndex(ColumnMax));
                    if (_xls is object)
                    {
                        //if (Math.Sign(GetColIndex(tempc) - GetColIndex(ColumnMax)) == 1)
                        //{
                        //    for (int a = 1, loopTo2 = i; a <= loopTo2; a++)
                        //        _xls.Columns("E").Insert();      // min value of last columns "I" so inserting after column "E"
                        //}
                        //else
                        //{
                        //    for (int a = 1, loopTo3 = i; a <= loopTo3; a++)
                        //        _xls.Columns("E").Delete();
                        //}
                    }

                    ColumnMax = GetColumnName(GetColIndex(tempc) - Sider);         // new maximum column
                    txtMaxColumn.Text = txtEndCol.Text;
                    TemplateFinalRange = txtEndCol.Text + (RowMax + Footer);
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E028", ex);
            }
        }
        // _____________________________________________________Update textbox of final range if user clicks on update button_____________________________________________________
        private void Updatetextbox()
        {
            txtendRow.Text = (RowMax + Footer).ToString();
            txtEndCol.Text = (Conversions.ToDouble(ColumnMax) + Sider).ToString();
            TemplateFinalRange = txtendRow.Text + txtEndCol.Text;
        }

        // _______________________________To change decimal degree to deg, min, sec______________________________________________________________________________________________
        private string chgeform(string str, bool Flag = true)
        {
            try
            {
                bool neg = false;
                double deg1, min1, sec1;
                if (str.Contains("-"))
                {
                    neg = true;
                    str = str.Replace("-", "");
                }

                deg1 = Math.Floor(Conversions.ToDouble(str));
                min1 = Math.Floor((Conversions.ToDouble(str) - deg1) * 60d);
                sec1 = (int)Math.Round(((Conversions.ToDouble(str) - deg1) * 60d - min1) * 60d);
                if (Flag)
                {
                    return Strings.Format(deg1, "000") + '°' + Strings.Format(min1, "00") + "'" + Strings.Format(sec1, "00") + "''";
                }
                else if (neg)
                {
                    return "-" + deg1.ToString() + '°' + Strings.Format(min1, "00") + "'" + Strings.Format(sec1, "00") + "''";
                }
                else
                {
                    return deg1.ToString() + '°' + Strings.Format(min1, "00") + "'" + Strings.Format(sec1, "00") + "''";
                }
            }
            catch (Exception ex)
            {
                return null;
                OnErrorHappen?.Invoke("E026", ex);
            }
        }

        public bool IsFileOpen()
        {
            return true;
            // Try
            // Dim FS As IO.FileStream = IO.File.Open(ReportName, IO.FileMode.Open, IO.FileAccess.Read, IO.FileShare.None)    'CREATE A FILE STREAM FROM THE FILE, OPENING IT FOR READ ONLY EXCLUSIVE ACCESS
            // FS.Close()          'CLOSE AND CLEAN UP RIGHT AWAY, IF THE OPEN SUCCEEDED, WE HAVE OUR ANSWER ALREADY
            // FS.Dispose()
            // FS = Nothing
            // Return False
            // Catch ex As IO.IOException
            // Return True
            // RaiseEvent OnErrorHappen("E032", ex)
            // Catch ex As Exception
            // Return True
            // RaiseEvent OnErrorHappen("E032", ex)
            // End Try
        }
        // _________________________________________________OPEN excel file to write when user click on run button on front end __________________________________________
        public void OpenExcel()
        {
            // Try
            // Isopeningexcel = True
            // xL.InitializeExcelApplication(ReportName, SelectedSheet)
            // Isopeningexcel = False
            // Catch ex As Exception
            // RaiseEvent OnErrorHappen("E025", ex)
            // End Try
        }
        // _____________________________________________________Collect all information for a particular report when user selects a report____________________________________________
        public bool ReportData(int StyleIndex) // Optional ByVal partprogram As Boolean = True
        {
            try
            {
                Data = new xl_DB();
                dataS = Data.FillTableRange(DatabaseConstants.TableNames);
                int ReportId, ProgramId, Styleid;
                // --------------------------------------------------------------------------Report information table query-------------------------------------------------------------
                var drows = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName);
                if (drows.Length == 0)
                {
                    return false;
                }

                DataR = drows[0];
                ReportId = Conversions.ToInteger(DataR[0]);
                if (!ReferenceEquals(DataR[4], DBNull.Value))
                {
                    ReportType = Conversions.ToString(DataR[4]);
                    CalculateMargins(ReportType);
                    if (!ReferenceEquals(DataR[12], DBNull.Value))
                        Password = Conversions.ToString(DataR[12]);
                    _IsDoPagination = Conversions.ToBoolean(DataR[15]);
                    if (DataR.IsNull(7) | DataR.IsNull(8) | DataR.IsNull(9) | DataR.IsNull(13) | DataR.IsNull(10) | DataR.IsNull(21) | DataR.IsNull(22))
                    {
                        CalculateMargins(ReportType);
                    }
                    else
                    {
                        ToleranceFormat = Conversions.ToInteger(DataR[6]);
                        if (ToleranceFormat < 0)
                        {
                            IncludeTolerence = false;
                            ToleranceFormat = 0;
                        }
                        else
                        {
                            IncludeTolerence = true;
                        }
                        // If Not IncludeTolerence Then ToleranceFormat = 0
                        Header = Conversions.ToInteger(DataR[7]);                   // no. of Headerrow
                        Footer = Conversions.ToInteger(DataR[8]);                   // no. of footer row
                        RowMax = Conversions.ToInteger(DataR[9]);
                        // SourceFinalRange = DataR.Item(10)
                        TemplateFinalRange = Conversions.ToString(DataR[10]);
                        ColumnMax = Conversions.ToString(DataR[13]);
                        InsertImage = Conversions.ToBoolean(DataR[16]);
                        Formatdigits = Conversions.ToInteger(DataR[24]);                        // no. of digit
                        ShwfieldNum = Conversions.ToBoolean(DataR[30]);
                        InsertDoublePicture = Conversions.ToBoolean(DataR[23]);
                        Picture_Width = Conversions.ToDouble(DataR[17]);
                        Picture_Height = Conversions.ToDouble(DataR[18]);
                        Picture_Top = Conversions.ToDouble(DataR[19]);
                        Picture_Left = Conversions.ToDouble(DataR[20]);
                        InitialField_Row = Conversions.ToInteger(DataR[21]);
                        Initial_Row = InitialField_Row - Header + 1; // ' Uncommented by R A N 17 APr 2015 because of wrong settings first time program is made and run
                        InitialField_Column = Conversions.ToString(DataR[22]);
                        Initial_Column = InitialField_Column; // 'Uncommented by R A N 17 Apr 2015
                        Sider = Conversions.ToInteger(DataR[25]);
                        LastRowUsed = Conversions.ToInteger(DataR[26]);
                        LastColumnUsed = Conversions.ToString(DataR[27]);                  // Lastcolumn 
                                                                                           // MaximumFilledRowPerTemplate = DataR.Item(28)
                                                                                           // MaximumFilledColumnPerTemplate = DataR.Item(29)
                        ShowDetail = Conversions.ToBoolean(DataR[31]);
                        MultiProgramReport = Conversions.ToBoolean(DataR[32]);
                        AllProgramInSingleReport = Conversions.ToBoolean(DataR[33]);
                        Picture2_Width = Conversions.ToDouble(DataR[34]);
                        Picture2_Height = Conversions.ToDouble(DataR[35]);
                        Picture2_Top = Conversions.ToDouble(DataR[36]);
                        Picture2_Left = Conversions.ToDouble(DataR[37]);
                        CreateCsv = Conversions.ToBoolean(DataR[38]);
                        if (CreateCsv & DataR[39] is object)
                        {
                            ReportNameCsv = Conversions.ToString(DataR[39]);
                        }
                        else
                        {
                            ReportNameCsv = "Nothing";
                        }

                        IncludeRawData = Conversions.ToBoolean(DataR[40]);
                        if (IncludeRawData & DataR[41] is object)
                        {
                            RawDataSheetType = Conversions.ToString(DataR[41]);
                        }
                        else
                        {
                            RawDataSheetType = "ColumnWise";
                        }

                        if (MultiProgramReport)
                        {
                            DataRow OtherdataR;
                            if (partprogram)
                            {
                                ProgramId = Conversions.ToInteger(Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName)[0][0]);
                                OtherdataR = Data.GetTable("MultiProgramReport").Select("Program_id = " + ProgramId + "AND Report_Id = " + Conversions.ToInteger(DataR[0]))[0];
                            }
                            // OtherdataR = Data.Select_Rows("MultiProgramReport", "Program_id", ProgramId)(0)
                            else
                            {
                                var dataRows = Data.Select_Rows("MultiProgramReport", "Report_Id", ReportId);
                                OtherdataR = dataRows[dataRows.Length - 1];
                                ProgramId = Conversions.ToInteger(OtherdataR[1]);
                            }

                            partprogram = true;
                            SelectedSheet = Conversions.ToString(OtherdataR[2]);
                            SelectedDataSheet = Conversions.ToString(OtherdataR[3]);
                            TemplateFinalRange = Conversions.ToString(OtherdataR[4]);
                            InitialField_Row = Conversions.ToInteger(OtherdataR[5]);
                            InitialField_Column = Conversions.ToString(OtherdataR[6]);
                            LastRowUsed = Conversions.ToInteger(OtherdataR[7]);
                            LastColumnUsed = Conversions.ToString(OtherdataR[8]);                  // Lastcolumn 
                            RowMax = Conversions.ToInteger(OtherdataR[9]);
                            ColumnMax = Conversions.ToString(OtherdataR[10]);
                        }
                        else
                        {
                            ProgramId = Conversions.ToInteger(Data.Select_Rows("ProgramInformation", "Program_Name", ProgramName)[0][0]);
                        }

                        TemplateInitialRange = Initial_Column + Initial_Row.ToString();
                        // SourceInitialRange = TemplateInitialRange
                        // SourceFinalRange = TemplateFinalRange
                        string dummyCol;
                        int dummyRow = 5;
                        if (ReportType.Contains("Column"))
                        {
                            dummyCol = Initial_Column;
                            dummyRow = LastRowUsed - 1;
                        }
                        else
                        {
                            dummyCol = GetColumnName(GetColIndex(LastColumnUsed) - 1);
                            dummyRow = Initial_Row;
                        }

                        SetPaginationLimits(ref dummyCol, ref dummyRow);
                        Data.DataA = new OleDbDataAdapter("Select * From FieldInformation Order By Field_ID", DatabaseConstants.ConnectionString);
                        Data.DataA.Fill(Data.DataS, "NewFieldInformation");
                        var styles = Data.DataS.Tables["ReportStyleInformation"].Select("Program_Id=" + "'" + ProgramId + "'");
                        if (StyleIndex < styles.Length)
                        {
                            Styleid = Conversions.ToInteger(styles[StyleIndex][0]);
                        }
                        else
                        {
                            Styleid = Conversions.ToInteger(styles[0][0]);
                        }

                        Fieldrows = Data.DataS.Tables["NewFieldInformation"].Select("Report_Style_Id=" + "'" + Styleid + "'");
                        if (DataR.IsNull(24))
                        {
                            DataR[24] = 4;
                        }
                    }
                }

                return true;
            }
            catch (Exception ex)
            {
                // RaiseEvent OnErrorHappen("E024", ex)
                Interaction.MsgBox(ex.Message + '\r' + ex.StackTrace);
                return false;
            }
            // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
        }
        // ________________________________________Close excel whenever user click on stop button on frontend_____________________________________________________________________
        public void CloseExcel()
        {
            try
            {
                if (UpdateLastReport)
                {
                    UpdateLastReport = false;
                }

                xL.Save();
                xL.ExitExcel();
            }
            catch (Exception ex)
            {
            }
        }

        public void QuitExcel()
        {
            try
            {
                xL.workbook.Close();
                xL.ExitExcel();
            }
            catch (Exception ex)
            {
            }
        }

        public void DefaultReportSelection(bool IsColumn, bool IsLandscape)
        {
            ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\Column_WiseWithLandscape.xls";
            ReportType = "ColumnWiseLandscape";
            IsColumnSelected = IsColumn;
            _columnWise.Checked = IsColumn;
            _rowWise.Checked = !IsColumn;
            IsLandScapeSelected = IsLandscape;
            _landScapeRadioButton.Checked = IsLandscape;
            _portraitRadioButton.Checked = !IsLandscape;
            if (IsHCM)
            {
                ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\HobCheckerTemplate.xls";
                ReportType = "HobCheckerTemplate";
            }
            else if (IsDTVSM)
            {
                ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\DelphiTVSTemplate.xls";
                ReportType = "DelphiTVSTemplate";
            }
            else if (_columnWise.Checked)
            {
                if (_landScapeRadioButton.Checked)
                {
                    ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\Column_WiseWithLandscape.xls";
                    ReportType = "ColumnWiseLandscape";
                }
                else
                {
                    ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\Column_WiseWithPortrait.xls";
                    ReportType = "ColumnWisePortrait";
                }
            }
            else if (_landScapeRadioButton.Checked)
            {
                ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\Row_WiseWithLandscape.xls";
                ReportType = "RowWiseLandscape";
            }
            else
            {
                ReportTemplateName = DatabaseConstants.TemporaryFilePath + @"\Row_WiseWithPortrait.xls";
                ReportType = "RowWisePortrait";
            }

            CalculateMargins(ReportType);        // Calculate the margins for the type of the report choosen.
            SetDefaultValuesForStyles();
            AssignDefaultPictureValues(ReportType);

            // If _columnWise.Checked = True AndAlso _landScapeRadioButton.Checked = True Then
            // ReportTemplateName = TemporaryFilePath & "\Column_WiseWithLandscape.xls"
            // ReportType = "ColumnWiseLandscape"
            // ElseIf _columnWise.Checked = True AndAlso _portraitRadioButton.Checked = True Then
            // ReportTemplateName = TemporaryFilePath & "\Column_WiseWithPortrait.xls"
            // ReportType = "ColumnWisePortrait"
            // ElseIf _rowWise.Checked = True AndAlso _landScapeRadioButton.Checked = True Then
            // ReportTemplateName = TemporaryFilePath & "\Row_WiseWithLandscape.xls"
            // ReportType = "RowWiseLandscape"
            // ElseIf _rowWise.Checked = True AndAlso _portraitRadioButton.Checked = True Then
            // ReportTemplateName = TemporaryFilePath & "\Row_WiseWithPortrait.xls"
            // ReportType = "RowWisePortrait"
            // End If
            // If ReportType <> Nothing And ReportType <> "" Then
            // 'Try
            // '    _dsoFramer.Open(ReportTemplateName)                'Opening the new document.
            // 'Catch ex As Exception       'This catch is for that times when DSO framer opens the document but after that throws an HR Result Exception(A bug in DSO framer) !!
            // 'End Try
            // CalculateMargins(ReportType)        'Calculate the margins for the type of the report choosen.
            // SetDefaultValuesForStyles()
            // AssignDefaultPictureValues(ReportType)
            // 'Try
            // '    _dsoFramer.Close()
            // 'Catch ex As Exception
            // 'End Try
            // End If

        }

        // Private Sub DefaultOptionForReportType(ByVal IsColumn As Boolean, ByVal IsLandscape As Boolean)
        // Try
        // If IsColumn And IsLandscape Then
        // _columnWise.Checked = True : _landScapeRadioButton.Checked = True
        // _rowWise.Checked = False : _portraitRadioButton.Checked = False
        // ElseIf IsColumn And Not IsLandscape Then
        // _columnWise.Checked = True : _landScapeRadioButton.Checked = False
        // _rowWise.Checked = False : _portraitRadioButton.Checked = True
        // ElseIf Not IsColumn And IsLandscape Then
        // _columnWise.Checked = False : _landScapeRadioButton.Checked = True
        // _rowWise.Checked = True : _portraitRadioButton.Checked = False
        // ElseIf Not IsColumn And Not IsLandscape Then
        // _columnWise.Checked = False : _landScapeRadioButton.Checked = False
        // _rowWise.Checked = True : _portraitRadioButton.Checked = True
        // End If
        // Catch ex As Exception
        // RaiseEvent OnErrorHappen("E022c", ex)
        // End Try
        // End Sub
        // ________________________________________________________To check tolerance condtion to enable pass/fail check ________________________________________________

        private void _measurementListView_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            if (string.IsNullOrEmpty(_measurementListView.SelectedRows[0].Cells[7].Value.ToString()) & string.IsNullOrEmpty(_measurementListView.SelectedRows[0].Cells[8].Value.ToString()) & string.IsNullOrEmpty(_measurementListView.SelectedRows[0].Cells[9].Value.ToString()))
            {
                _measurementListView.SelectedRows[0].Cells[10].ReadOnly = true;
            }
            else
            {
                _measurementListView.SelectedRows[0].Cells[10].ReadOnly = false;
            }
        }
        // _________________________________function to be called on newstyle click________________________________________________________________________________
        public void CreateNewStyle()
        {
            try
            {
                var tmpDataR = Data.Select_Rows("ReportInformation", "R_ReportName", ReportName)[0];
                if (Conversions.ToBoolean(tmpDataR[32]))
                {
                    return;
                }

                NewStyle = true;
                MeasurementStringList.Clear();
                for (int i = 0, loopTo = Fieldrows.Length - 1; i <= loopTo; i++)
                {
                    if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(Fieldrows[i][13], "", false)))
                    {
                        continue;
                    }

                    var FieldArray = new ArrayList();
                    FieldArray.Add(Fieldrows[i][3]);
                    FieldArray.Add(Fieldrows[i][4]);
                    FieldArray.Add(Fieldrows[i][5]);
                    if (string.IsNullOrEmpty(Fieldrows[i][7].ToString()) & string.IsNullOrEmpty(Fieldrows[i][7].ToString()))
                    {
                        FieldArray.Add(0);
                        FieldArray.Add(0);
                        FieldArray.Add(0);
                    }
                    else
                    {
                        FieldArray.Add(Fieldrows[i][7]);
                        FieldArray.Add(Fieldrows[i][8]);
                        FieldArray.Add(Fieldrows[i][9]);
                    }

                    FieldArray.Add(Fieldrows[i][13]);
                    MeasurementStringList.Add(FieldArray);
                }

                Reset_All(false, true);
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E023", ex);
            }
        }

        private void ChkShwFieldNum_CheckedChanged(object sender, EventArgs e)
        {
            if (_measurementListView.Columns.Count > 2)
            {
                if (ChkShwFieldNum.Checked)
                {
                    _measurementListView.Columns[2].Visible = true;
                    RefreshFieldNumbers();
                }
                else
                {
                    _measurementListView.Columns[2].Visible = false;
                }
            }
        }

        private void RefreshFieldNumbers()
        {
            int FieldCount = 1;
            for (int i = 0, loopTo = _measurementListView.Rows.Count - 1; i <= loopTo; i++)
            {
                //if (Conversions.ToBoolean((object)!ReferenceEquals(_measurementListView.Rows[i].Cells[3].Value, DBNull.Value) && !Operators.OrObject(Operators.ConditionalCompareObjectEqual(_measurementListView.Rows[i].Cells[3].Value, "Date", false), Operators.ConditionalCompareObjectEqual(_measurementListView.Rows[i].Cells[3].Value, "Time", false))))
                //{
                //    _measurementListView.Rows[i].Cells[2].Value = FieldCount;
                //    FieldCount += 1;
                //}
                //else
                //{
                //    continue;
                //}
            }
        }

        public void NewTolerance()
        {
            try
            {
                MeasurementStringList.RemoveAt(1);
                MeasurementStringList.RemoveAt(0);
                for (int i = 0, loopTo = MeasurementStringList.Count - 1; i <= loopTo; i++)
                {
                    for (int j = 0, loopTo1 = _measurementListView.Rows.Count - 1; j <= loopTo1; j++)
                    {
                        if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(_measurementListView.Rows[j].Cells[13].Value, MeasurementStringList[i][6], false)))
                        {
                            MeasurementStringList[i][0] = _measurementListView.Rows[j].Cells[3].Value;
                            MeasurementStringList[i][3] = _measurementListView.Rows[j].Cells[7].Value;
                            MeasurementStringList[i][4] = _measurementListView.Rows[j].Cells[8].Value;
                            MeasurementStringList[i][5] = _measurementListView.Rows[j].Cells[9].Value;
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E021", ex);
            }
        }

        private void DetailSelect_CheckedChanged(object sender, EventArgs e)
        {
            if (DetailSelect.Checked)
            {
                ShowDetail = true;
            }
            else
            {
                ShowDetail = false;
            }
        }

        private double ConvertDegMinSecToDecimal(string DegMinSecStr)
        {
            // Dim DegSignIndx As Integer = DegMinSecStr.IndexOf(Chr(176))
            // Dim MinSignIndx As Integer = DegMinSecStr.IndexOf("'")
            // 'If we did not get a proper degree min sec string we return!
            var markers = new char[] { '°', '\'' };
            var sss = DegMinSecStr.Split(markers);
            if (sss.Length < 3)
                return 0d;
            var angleisnegative = default(bool);
            if (sss[0].Contains("-"))
                angleisnegative = true;
            double Ans = Math.Abs(double.Parse(sss[0]));
            Ans += double.Parse(sss[1]) / 60d;
            Ans += double.Parse(sss[2]) / 3600d;

            // If DegSignIndx = -1 Or MinSignIndx = -1 Then
            // Return 0
            // End If
            // DegSignIndx += 1
            // 'If Ans < 0 Then angleisnegative = True : Ans = Math.Abs(Ans)
            // Ans += Double.Parse(DegMinSecStr.Substring(DegSignIndx, MinSignIndx - DegSignIndx)) / 60
            // MinSignIndx += 1 'point to start of second part
            // Ans += Double.Parse(DegMinSecStr.Substring(MinSignIndx, DegMinSecStr.Length - 2 - MinSignIndx)) / 3600
            if (angleisnegative)
                Ans = -1 * Ans;
            return Ans;
        }

        public bool IsReportNameProper()
        {
            if (Data.Find_Record("ReportInformation", ReportName, 2))
            {
                partprogram = false;
                ReportData(0);
                // UseExistingReport = True
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool CheckNames(string Name, int RfNo)
        {
            try
            {
                Data = null;
                Data = new xl_DB();
                dataS = Data.FillTableRange(DatabaseConstants.TableNames);
                switch (RfNo)
                {
                    case 1:
                        {
                            if (Data.Find_Record("ProgramInformation", Name, 1))
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }

                            break;
                        }

                    case 2:
                        {
                            if (Data.Find_Record("ReportStyleInformation", Name, 2))
                            {
                                return true;
                            }
                            else
                            {
                                return false;
                            }

                            break;
                        }

                    case 3:
                        {
                            ReportName = Name;
                            return IsReportNameProper();
                        }

                        // If Data.Find_Record("ReportInformation", Name, 2) Then
                        // Return True
                        // Else
                        // Return False
                        // End If
                }
            }
            catch (Exception ex)
            {
                OnErrorHappen?.Invoke("E022", ex);
                return false;
            }

            return false;
        }

        public List<string> GetAllEntries(string TableName, string ColumName, string OrderByColName = "")
        {
            try
            {
                Data = null;
                Data = new xl_DB();
                dataS = Data.FillTable(TableName);
                var listofString = new List<string>(); // ' (Data.GetTable(TableName).Rows.Count - 1) As String
                                                       // Dim primKey As DataColumn() = Data.GetTable(TableName).PrimaryKey
                if (!string.IsNullOrEmpty(OrderByColName)) // primKey.Length > 0 Then
                {
                    var tablerows = Data.GetTable(TableName).Select("", OrderByColName + " ASC");
                    for (int i = 0, loopTo = tablerows.Length - 1; i <= loopTo; i++)
                        listofString.Add(tablerows[i][ColumName].ToString());
                }
                else
                {
                    for (int i = 0, loopTo1 = Data.GetTable(TableName).Rows.Count - 1; i <= loopTo1; i++)
                        listofString.Add(Data.GetTable(TableName).Rows[i][ColumName].ToString());
                }

                return listofString;
            }
            catch (Exception ex)
            {
                return null;
            }
        }

        //public void AutoMeasurmentFileRead(string FileName, ref List<double> List)
        //{
        //    var xL1 = new xl_Class();
        //    try
        //    {
        //        xL1.InitializeExcelApplication(FileName, "");
        //        int Cno = 1;
        //        int Rno = 1;
        //        double temp;
        //        while (xL1.xlS.Cells(Rno, GetColumnName(Cno)).Value is object)
        //        {
        //            for (int i = 1; i <= 3; i++)
        //            {
        //                if (xL1.xlS.Cells(Rno, GetColumnName(i)).Value is object)
        //                {
        //                    if (!double.TryParse(Conversions.ToString(xL1.xlS.Cells(Rno, GetColumnName(i)).Value), out temp))
        //                    {
        //                        temp = 0d;
        //                    }
        //                }
        //                else
        //                {
        //                    temp = 0d;
        //                }

        //                List.Add(temp);
        //            }

        //            Rno += 1;
        //        }

        //        xL1.Save();
        //        xL1.ExitExcel();
        //    }
        //    catch (Exception ex)
        //    {
        //        xL1.Save();
        //        xL1.ExitExcel();
        //    }
        //}

        //public void ProbePathFileRead(string FileName, ref List<string> List1, ref List<double> List2)
        //{
        //    var xL1 = new xl_Class();
        //    try
        //    {
        //        xL1.InitializeExcelApplication(FileName, "");
        //        double temp;
        //        int i = 1;
        //        while (xL1.xlS.Cells(i, "A").Value is object)
        //        {
        //            List1.Insert(i - 1, Conversions.ToString(xL1.xlS.Cells(i, "A").Value));
        //            for (int j = 2; j <= 4; j++)
        //            {
        //                if (xL1.xlS.Cells(i, GetColumnName(j)).Value is object)
        //                {
        //                    if (!double.TryParse(Conversions.ToString(xL1.xlS.Cells(i, GetColumnName(j)).Value), out temp))
        //                    {
        //                        break;
        //                    }
        //                }
        //                else
        //                {
        //                    temp = 0d;
        //                }

        //                List2.Insert((i - 1) * 3 + j - 2, temp);
        //            }

        //            i = i + 1;
        //        }

        //        xL1.Save();
        //        xL1.ExitExcel();
        //    }
        //    catch (Exception ex)
        //    {
        //        xL1.Save();
        //        xL1.ExitExcel();
        //    }
        //}

        public bool GeneratePDFReport()
        {
            try
            {
                return true;
            }
            catch (Exception ex)
            {
                return false;
            }
        }

        #region Events and its Handler declaration 
        // Error reporting to the using application 
        // The Handler for the Error Event raised 
        public delegate void ErrorEventHandler_ExcelModule(string str, Exception ex);
        // shared Event for raising the error
        public static event ErrorEventHandler_ExcelModule OnErrorHappen;

        public delegate void Invokegridview();                                    // The Invoking Functions for filling the measurementgridview .To maintain the thread safety.

        public event NewStyleCreatedEventHandler NewStyleCreated;

        public delegate void NewStyleCreatedEventHandler();
        #endregion

        #region Required Class Instances 
        private xl_DB Data;
        private xl_Class xL = new xl_Class();
        #endregion

        // _______________________________________________Property being used in to check pass or fail condition___________________________________________________________________

        public bool ComponentIsProper
        {
            get
            {
                return WithinRangeCheckFlag;
            }

            set
            {
                WithinRangeCheckFlag = value;
            }
        }

        public bool IsOpening
        {
            get
            {
                return Isopeningexcel;
            }
        }

        public string UserName
        {
            get
            {
                return _username;
            }

            set
            {
                _username = value;
            }
        }

        public enum PassFail
        {
            pass,
            fail,
            none
        }

        private void RadioButton3_CheckedChanged(object sender, EventArgs e)
        {
            if (Rawdataasrow.Checked)
            {
                RawDataSheetType = "RowWise";
            }
            else if (Rawdataascol.Checked)
            {
                RawDataSheetType = "Columnwise";
            }
        }

        private void CheckBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (CheckBox1.Checked)
            {
                IncludeRawData = true;
            }
            else
            {
                IncludeRawData = true;
            }
        }

        private double Picture_Left = 1d;
        private double Picture_Top = 1d;
        private double Picture_Width = 1d;
        private double Picture_Height = 1d;
        private double Picture2_Left = 1d;
        private double Picture2_Top = 1d;
        private double Picture2_Width = 1d;
        private double Picture2_Height = 1d;
        private char degree = '°';
        private string ColumnAfterWhich_PaginationShouldBeDone; // for the columnWise reports
        private int RowAfterWhich_PaginationShouldBeDone;
        private int LastRowUsed = 1;
        private string LastColumnUsed = "";
        private string ReportTemplateName = "";                                // ReportTemplateName denotes the full path of the file currently opened into the DSO framer.
        private string ReportType = "";
        private int MasterTemplateID;
        private string RawDataSheetType = "ColumnWise";
        // ReportType Denotes the style of report.Default Style is 'Coloumn Wise Landscape'.
        private string PrinterState = "Color";                             // The Printer State currently .Default value is 'Color' printer.
        private Invokegridview Invokingfunction;
        private bool ProgramName_Exists = false;
        private bool ConditionalFormattingFlag = false;    // Boolean variables for various purposes of checking the conditions 
        private int No_of_New_TitleRows = 0;             // Integer variables to keep the track of the records
        public string Password = "";                                           // Variable to store the password of the given file.
        private ArrayList RowList = new ArrayList(), ColumnList = new ArrayList();                          // Arraylists to contain the values of the Rows and column informations of the fields values to be written into the file. 
        private List<string> PreDefinedList = new List<string>();                         // This Array of string will contain all the predefined items for a Title.
        private int Header = 0;
        private int Footer = 0;
        private int Sider = 0;
        private int RowMax = 0;
        private string ColumnMax = "";
        private int InitialField_Row = 8;
        private string InitialField_Column = "A";
        private int Initial_Row = 1;
        private string Initial_Column = "A"; // Variables to contain the values of the various parts of reports 
        private string TemplateInitialRange = "A1";
        private string TemplateFinalRange;
        private string SourceFinalRange = "";
        private string SourceInitialRange = "A1";
        private int NoOfPastedTemplates = 0;
        private bool WithinRangeCheckFlag = false;                        // To check whether the component is proper or not
        private string _username = "";
        private string SelectedDataSheet = "Data";
        private bool ShwfieldNum = false;
        private bool Isopeningexcel = false;
        private bool ShowDetail = false;
        public bool InsertImage = false;
        private bool IncludeRawData = false;
        private bool IncludeTolerence = false;
        private bool IsColumnSelected = true;
        private bool IsLandScapeSelected = true;
        public bool AllProgramInSingleReport = true;
        private bool MultiProgramReport = false;
        private int Formatdigits = 4;
        private DataRow DataR;
        private DataRow[] Fieldrows;
        private DataRow[] TitleRows;
        private string EndColumn = "";
        // Dim MaximumFilledRowPerTemplate As Integer = 0
        // Dim MaximumFilledColumnPerTemplate As String = ""
        private DataSet dataS;
        private bool AddPass = true;
        private double asratio1 = 4d / 3d;
        private double asratio2 = 4d / 3d;
        private bool partprogram = true;
        private int NoOfPages;
        private int PartNameLabelLocation_Row, PartName_Location_Row;
        private int PartNameLabelLocation_Col, PartName_Location_Col;
        // ____________________________________________________________________________________Global variables declarations____________________________________________________________________

        public string ProgramName, ReportName;                         // Program Name and the Report Name to be supplied by the Using application and 
        public string TemplateFolderName;                              // the TemplateFolderPath denotes the path of the folder where the templates for the ReportStyles have to be stored.
        public string PicturePath = "";
        public string Picture2Path = "";                                   // Picture Path is the full path of the picture to be stored.
        public List<ArrayList> MeasurementStringList = new List<ArrayList>();
        private bool _IsDoPagination;
        // Boolean variable to signify that whether pagination should be done or rows/columns have to be inserted.

        public bool IsDoPagination
        {
            get
            {
                return _IsDoPagination;
            }

            set
            {
                _IsDoPagination = value;
                cbPaginateReport.Checked = value;
            }
        }

        // Boolean variable to signify that whether pagination should be done or rows/columns have to be inserted.
        public bool InsertDoublePicture = false;
        public string[] ImagePaths = new string[2];
        public ArrayList HobPersonalData;
        public bool NewStyle = false;
        public string ReportStyleName = "";
        public string SelectedSheet = "Report";
        // Public Property SetToleranceFormat() As Integer
        public bool CreateCsv = false;
        public string ReportNameCsv = "";
        public int ComponentNo = 0;
        public bool ShowPassFail = false;
        public PassFail passStatus = PassFail.none;

        public bool MakePDFReport { get; set; } = true;
        public bool UseExistingReport { get; set; } = false;
        public bool UpdateLastReport { get; set; } = false;
        public bool InsertImageForDataTransfer { get; set; } = true;
        public bool ShowAvging { get; set; } = true;

        public int ToleranceFormat = 0;
        public bool MultipleReportsInDifferentPages = true;
    }
}