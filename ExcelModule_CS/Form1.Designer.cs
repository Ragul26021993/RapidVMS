using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.CompilerServices;
using System.Windows.Forms;
using Microsoft.VisualBasic.CompilerServices;

namespace rrm
{
    [DesignerGenerated()]
    public partial class Form1 : Form
    {

        // Form overrides dispose to clean up the component list.
        [DebuggerNonUserCode()]
        protected override void Dispose(bool disposing)
        {
            try
            {
                if (disposing && components is object)
                {
                    components.Dispose();
                    xL.ExitExcel();
                    try
                    {
                        //_dsoFramer.Close();
                    }
                    catch (Exception ex)
                    {
                    }

                    GC.Collect();
                    GC.WaitForPendingFinalizers();
                }
            }
            finally
            {
                base.Dispose(disposing);
            }
        }
        // Required by the Windows Form Designer
        private System.ComponentModel.IContainer components;

        // NOTE: The following procedure is required by the Windows Form Designer
        // It can be modified using the Windows Form Designer.  
        // Do not modify it using the code editor.
        [DebuggerStepThrough()]
        private void InitializeComponent()
        {
            components = new System.ComponentModel.Container();
            var resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            var DataGridViewCellStyle1 = new DataGridViewCellStyle();
            var DataGridViewCellStyle2 = new DataGridViewCellStyle();
            var DataGridViewCellStyle3 = new DataGridViewCellStyle();
            var DataGridViewCellStyle4 = new DataGridViewCellStyle();
            var DataGridViewCellStyle5 = new DataGridViewCellStyle();
            var DataGridViewCellStyle6 = new DataGridViewCellStyle();
            var DataGridViewCellStyle7 = new DataGridViewCellStyle();
            _reportStyleBox = new GroupBox();
            _moreSettingLink = new LinkLabel();
            __rowWise = new RadioButton();
            __rowWise.CheckedChanged += new EventHandler(HandlingReportStylesForExcel);
            __columnWise = new RadioButton();
            __columnWise.CheckedChanged += new EventHandler(HandlingReportStylesForExcel);
            __landScapeRadioButton = new RadioButton();
            __landScapeRadioButton.CheckedChanged += new EventHandler(HandlingReportStylesForExcel);
            __portraitRadioButton = new RadioButton();
            __portraitRadioButton.CheckedChanged += new EventHandler(HandlingReportStylesForExcel);
            _tabControl = new TabControl();
            _fieldSettings = new TabPage();
            _ChkShwFieldNum = new CheckBox();
            _ChkShwFieldNum.CheckedChanged += new EventHandler(ChkShwFieldNum_CheckedChanged);
            panelToleranceType = new Panel();
            _Btn_TolerancePanelClose = new Button();
            _Btn_TolerancePanelClose.Click += new EventHandler(Btn_TolerancePanelClose_Click);
            __allowedRange = new RadioButton();
            __allowedRange.CheckedChanged += new EventHandler(_allowedRange_CheckedChanged);
            __nominalMinMax = new RadioButton();
            __nominalMinMax.CheckedChanged += new EventHandler(_nominalMinMax_CheckedChanged);
            __nomialPlusMinus = new RadioButton();
            __nomialPlusMinus.CheckedChanged += new EventHandler(_nomialPlusMinus_CheckedChanged);
            __conditionalFormatting1 = new CheckBox();
            __conditionalFormatting1.CheckedChanged += new EventHandler(ConditionalFormatting_CheckedChanged);
            __measurementListView = new DataGridView();
            //__measurementListView.RowHeaderMouseDoubleClick += new DataGridViewCellMouseEventHandler(_measurementListView_RowHeaderMouseDoubleClick);
            __measurementListView.CellContentClick += new DataGridViewCellEventHandler(_measurementListView_CellContentClick);
            _BtnMoveUp = new Button();
            _BtnMoveUp.Click += new EventHandler(BtnMoveUp_Click);
            _BtnMoveDown = new Button();
            _BtnMoveDown.Click += new EventHandler(BtnMoveUp_Click);
            _programSettings = new TabPage();
            _Button1 = new Button();
            _Button1.Click += new EventHandler(ShowPredefinedList_Click);
            _addNewTitle = new Button();
            //_addNewTitle.Click += new EventHandler(addNewTitle_Click);
            pnlTitlePredefinedList = new Panel();
            txtPredefinedList = new RichTextBox();
            Label7 = new Label();
            Label8 = new Label();
            _btnSetList = new Button();
            _btnSetList.Click += new EventHandler(btnSetList_Click);
            __extraInformationDataView = new DataGridView();
            __extraInformationDataView.DoubleClick += new EventHandler(GetSelectedCellInformation);
            __extraInformationDataView.CellEnter += new DataGridViewCellEventHandler(Handle_PredefinedList);
            _settings = new TabPage();
            GroupBox1 = new GroupBox();
            txtPartName_Row = new TextBox();
            txtPartName_Col = new TextBox();
            Label12 = new Label();
            Label27 = new Label();
            Label28 = new Label();
            txtPartName_Label_Row = new TextBox();
            txtPartName_Label_Col = new TextBox();
            Label26 = new Label();
            Label25 = new Label();
            Label11 = new Label();
            grpPasswordProtection = new GroupBox();
            Label10 = new Label();
            _filePassword = new Button();
            _filePassword.Click += new EventHandler(filePassword_Click);
            filepasslabel = new Label();
            passwordBox = new TextBox();
            cbPaginateReport = new CheckBox();
            _CheckBox1 = new CheckBox();
            _CheckBox1.CheckedChanged += new EventHandler(CheckBox1_CheckedChanged);
            Label24 = new Label();
            Label23 = new Label();
            PanelRcadPic = new Panel();
            _UpdateRcadPic = new Button();
            _UpdateRcadPic.Click += new EventHandler(UpdateRcadPic_Click);
            RcadCol = new TextBox();
            Rcadwidth = new TextBox();
            Label19 = new Label();
            Label20 = new Label();
            Label21 = new Label();
            RcadRow = new TextBox();
            Rcadheight = new TextBox();
            Label22 = new Label();
            _ProgramInNewSheet = new CheckBox();
            _ProgramInNewSheet.CheckedChanged += new EventHandler(ProgramInNewSheet_CheckedChanged);
            _DetailSelect = new CheckBox();
            _DetailSelect.CheckedChanged += new EventHandler(DetailSelect_CheckedChanged);
            Label17 = new Label();
            chkEarlierMadeReportsAlso = new CheckBox();
            nudFormatdigits = new NumericUpDown();
            Label16 = new Label();
            Label15 = new Label();
            Label14 = new Label();
            txtMaxColumn = new TextBox();
            chkMakeSettingsDefault = new CheckBox();
            txtMaxRow = new TextBox();
            txtInitialColumn = new TextBox();
            Label1 = new Label();
            txtInitialRow = new TextBox();
            Label3 = new Label();
            pnlInsertPicture = new Panel();
            _btnUpdatePicDetails = new Button();
            _btnUpdatePicDetails.Click += new EventHandler(btnUpdatePicDetails_Click);
            txtColumn = new TextBox();
            txtWidth = new TextBox();
            lblWidth = new Label();
            lblHeight = new Label();
            lblTop = new Label();
            txtRow = new TextBox();
            txtHeight = new TextBox();
            lblLeft = new Label();
            _chkInsertPicture = new CheckBox();
            _chkInsertPicture.CheckedChanged += new EventHandler(chkInsertPicture_CheckedChanged);
            chkWriteprogramNameInReport = new CheckBox();
            Label9 = new Label();
            columnLabel = new Label();
            txtendRow = new TextBox();
            txtEndCol = new TextBox();
            Label6 = new Label();
            _btnUpdateRange = new Button();
            _btnUpdateRange.Click += new EventHandler(btnUpdateRange_Click);
            _templateRangeLabel = new Label();
            Label18 = new Label();
            _ShowToolBar = new CheckBox();
            _ShowToolBar.CheckedChanged += new EventHandler(showExcelMenu_CheckedChanged);
            _showExcelMenu = new CheckBox();
            _showExcelMenu.CheckedChanged += new EventHandler(showExcelMenu_CheckedChanged);
            _sheetBox = new ComboBox();
            _reportSheets = new Label();
            __saveButton = new Button();
            __saveButton.Click += new EventHandler(_saveButton_Click);
            _programName = new Label();
            _programNameBox = new TextBox();
            _programNamePathBox = new TextBox();
            Label2 = new Label();
            _reportName = new Label();
            _reportNameBox = new TextBox();
            _reportPathNameBox = new TextBox();
            Label5 = new Label();
            _pageStyleBox = new GroupBox();
            LinkLabel1 = new LinkLabel();
            GroupBox2 = new GroupBox();
            _reportStyleNameBox = new ComboBox();
            Label4 = new Label();
            LinkLabel2 = new LinkLabel();
            panelButtonEnclosure = new Panel();
            btnPreview = new Button();
            fieldContextMenuStrip = new ContextMenuStrip(components);
            DateToolStripMenuItem = new ToolStripMenuItem();
            TimeToolStripMenuItem = new ToolStripMenuItem();
            PredefinedListToolStripMenuItem = new ToolStripMenuItem();
            PassFailToolStripMenuItem = new ToolStripMenuItem();
            TextToolStripMenuItem = new ToolStripMenuItem();
            titleDataTypeContextMenuList = new ContextMenuStrip(components);
            PredefinedListToolStripMenuItem1 = new ToolStripMenuItem();
            TextToolStripMenuItem1 = new ToolStripMenuItem();
            DateToolStripMenuItem1 = new ToolStripMenuItem();
            TimeToolStripMenuItem1 = new ToolStripMenuItem();
            PassFailToolStripMenuItem1 = new ToolStripMenuItem();
            convertFieldToTitleList = new ContextMenuStrip(components);
            ConvertFieldAsTitleToolStripMenuItem = new ToolStripMenuItem();
            DeleteToolStripMenuItem = new ToolStripMenuItem();
            SetAsPassFailToolStripMenuItem = new ToolStripMenuItem();
            FieldDeleteMenu = new ContextMenuStrip(components);
            DeleteToolStripMenuItem1 = new ToolStripMenuItem();
            GroupBox6 = new GroupBox();
            _blackWhite = new RadioButton();
            _blackWhite.CheckedChanged += new EventHandler(PrinterHandling);
            _colorPrinter = new RadioButton();
            _colorPrinter.CheckedChanged += new EventHandler(PrinterHandling);
            __cancelButton = new Button();
            __cancelButton.Click += new EventHandler(_cancelButton_Click);
            GroupBox3 = new GroupBox();
            Rawdataasrow = new RadioButton();
            _Rawdataascol = new RadioButton();
            _Rawdataascol.CheckedChanged += new EventHandler(RadioButton3_CheckedChanged);
            LinkLabel3 = new LinkLabel();
            lblNoDSOFramer = new Label();
            _reportStyleBox.SuspendLayout();
            _tabControl.SuspendLayout();
            _fieldSettings.SuspendLayout();
            panelToleranceType.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)__measurementListView).BeginInit();
            _programSettings.SuspendLayout();
            pnlTitlePredefinedList.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)__extraInformationDataView).BeginInit();
            _settings.SuspendLayout();
            GroupBox1.SuspendLayout();
            grpPasswordProtection.SuspendLayout();
            PanelRcadPic.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)nudFormatdigits).BeginInit();
            pnlInsertPicture.SuspendLayout();
            _pageStyleBox.SuspendLayout();
            GroupBox2.SuspendLayout();
            panelButtonEnclosure.SuspendLayout();
            fieldContextMenuStrip.SuspendLayout();
            titleDataTypeContextMenuList.SuspendLayout();
            convertFieldToTitleList.SuspendLayout();
            FieldDeleteMenu.SuspendLayout();
            GroupBox6.SuspendLayout();
            GroupBox3.SuspendLayout();
            SuspendLayout();
            // 
            // _reportStyleBox
            // 
            _reportStyleBox.Controls.Add(_moreSettingLink);
            _reportStyleBox.Controls.Add(__rowWise);
            _reportStyleBox.Controls.Add(__columnWise);
            _reportStyleBox.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _reportStyleBox.Location = new Point(7, 50);
            _reportStyleBox.Name = "_reportStyleBox";
            _reportStyleBox.Size = new Size(107, 66);
            _reportStyleBox.TabIndex = 19;
            _reportStyleBox.TabStop = false;
            _reportStyleBox.Text = "Report Type";
            // 
            // _moreSettingLink
            // 
            _moreSettingLink.AutoSize = true;
            _moreSettingLink.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _moreSettingLink.Location = new Point(90, 106);
            _moreSettingLink.Name = "_moreSettingLink";
            _moreSettingLink.Size = new Size(146, 13);
            _moreSettingLink.TabIndex = 22;
            _moreSettingLink.TabStop = true;
            _moreSettingLink.Text = "More Settings....................";
            // 
            // _rowWise
            // 
            __rowWise.AutoSize = true;
            __rowWise.Location = new Point(10, 43);
            __rowWise.Name = "__rowWise";
            __rowWise.Size = new Size(79, 17);
            __rowWise.TabIndex = 20;
            __rowWise.Text = "Row Wise";
            __rowWise.UseVisualStyleBackColor = true;
            // 
            // _columnWise
            // 
            __columnWise.AutoSize = true;
            __columnWise.Checked = true;
            __columnWise.Location = new Point(10, 20);
            __columnWise.Name = "__columnWise";
            __columnWise.Size = new Size(97, 17);
            __columnWise.TabIndex = 19;
            __columnWise.TabStop = true;
            __columnWise.Text = "Column Wise";
            __columnWise.UseVisualStyleBackColor = true;
            // 
            // _landScapeRadioButton
            // 
            __landScapeRadioButton.AutoSize = true;
            __landScapeRadioButton.Checked = true;
            __landScapeRadioButton.Location = new Point(6, 20);
            __landScapeRadioButton.Name = "__landScapeRadioButton";
            __landScapeRadioButton.Size = new Size(85, 17);
            __landScapeRadioButton.TabIndex = 25;
            __landScapeRadioButton.TabStop = true;
            __landScapeRadioButton.Text = "Landscape";
            __landScapeRadioButton.UseVisualStyleBackColor = true;
            // 
            // _portraitRadioButton
            // 
            __portraitRadioButton.AutoSize = true;
            __portraitRadioButton.Location = new Point(6, 43);
            __portraitRadioButton.Name = "__portraitRadioButton";
            __portraitRadioButton.Size = new Size(69, 17);
            __portraitRadioButton.TabIndex = 24;
            __portraitRadioButton.Text = "Portrait";
            __portraitRadioButton.UseVisualStyleBackColor = true;
            // 
            // _tabControl
            // 
            _tabControl.Controls.Add(_fieldSettings);
            _tabControl.Controls.Add(_programSettings);
            _tabControl.Controls.Add(_settings);
            _tabControl.Font = new Font("Trebuchet MS", 9.75f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _tabControl.Location = new Point(7, 190);
            _tabControl.Name = "_tabControl";
            _tabControl.SelectedIndex = 0;
            _tabControl.Size = new Size(498, 535);
            _tabControl.TabIndex = 27;
            // 
            // _fieldSettings
            // 
            _fieldSettings.BackColor = SystemColors.ControlLightLight;
            _fieldSettings.Controls.Add(_ChkShwFieldNum);
            _fieldSettings.Controls.Add(panelToleranceType);
            _fieldSettings.Controls.Add(__conditionalFormatting1);
            _fieldSettings.Controls.Add(__measurementListView);
            _fieldSettings.Controls.Add(_BtnMoveUp);
            _fieldSettings.Controls.Add(_BtnMoveDown);
            _fieldSettings.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _fieldSettings.Location = new Point(4, 27);
            _fieldSettings.Name = "_fieldSettings";
            _fieldSettings.Padding = new Padding(3);
            _fieldSettings.Size = new Size(490, 504);
            _fieldSettings.TabIndex = 0;
            _fieldSettings.Text = "Fields";
            // 
            // ChkShwFieldNum
            // 
            _ChkShwFieldNum.AutoSize = true;
            _ChkShwFieldNum.Location = new Point(299, 10);
            _ChkShwFieldNum.Name = "_ChkShwFieldNum";
            _ChkShwFieldNum.Size = new Size(132, 17);
            _ChkShwFieldNum.TabIndex = 94;
            _ChkShwFieldNum.Text = "Show Field Number";
            _ChkShwFieldNum.UseVisualStyleBackColor = true;
            // 
            // panelToleranceType
            // 
            panelToleranceType.BackColor = SystemColors.GradientInactiveCaption;
            panelToleranceType.BorderStyle = BorderStyle.Fixed3D;
            panelToleranceType.Controls.Add(_Btn_TolerancePanelClose);
            panelToleranceType.Controls.Add(__allowedRange);
            panelToleranceType.Controls.Add(__nominalMinMax);
            panelToleranceType.Controls.Add(__nomialPlusMinus);
            panelToleranceType.Location = new Point(6, 29);
            panelToleranceType.Name = "panelToleranceType";
            panelToleranceType.Size = new Size(159, 87);
            panelToleranceType.TabIndex = 31;
            panelToleranceType.Visible = false;
            // 
            // Btn_TolerancePanelClose
            // 
            _Btn_TolerancePanelClose.Anchor = AnchorStyles.None;
            _Btn_TolerancePanelClose.AutoSizeMode = AutoSizeMode.GrowAndShrink;
            _Btn_TolerancePanelClose.BackColor = Color.Tomato;
            _Btn_TolerancePanelClose.BackgroundImage = (Image)resources.GetObject("Btn_TolerancePanelClose.BackgroundImage");
            _Btn_TolerancePanelClose.BackgroundImageLayout = ImageLayout.Zoom;
            _Btn_TolerancePanelClose.Cursor = Cursors.Default;
            _Btn_TolerancePanelClose.FlatAppearance.BorderColor = SystemColors.ActiveCaptionText;
            _Btn_TolerancePanelClose.FlatAppearance.MouseDownBackColor = Color.Black;
            _Btn_TolerancePanelClose.FlatAppearance.MouseOverBackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(255)), Conversions.ToInteger(Conversions.ToByte(128)), Conversions.ToInteger(Conversions.ToByte(0)));
            _Btn_TolerancePanelClose.FlatStyle = FlatStyle.Flat;
            _Btn_TolerancePanelClose.Font = new Font("Symbol", 9.75f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(2));
            _Btn_TolerancePanelClose.ForeColor = SystemColors.ControlLightLight;
            _Btn_TolerancePanelClose.Location = new Point(138, 2);
            _Btn_TolerancePanelClose.Margin = new Padding(0);
            _Btn_TolerancePanelClose.Name = "_Btn_TolerancePanelClose";
            _Btn_TolerancePanelClose.Size = new Size(16, 16);
            _Btn_TolerancePanelClose.TabIndex = 91;
            _Btn_TolerancePanelClose.TextAlign = ContentAlignment.TopCenter;
            _Btn_TolerancePanelClose.TextImageRelation = TextImageRelation.ImageBeforeText;
            _Btn_TolerancePanelClose.UseVisualStyleBackColor = false;
            // 
            // _allowedRange
            // 
            __allowedRange.AutoSize = true;
            __allowedRange.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            __allowedRange.Location = new Point(4, 10);
            __allowedRange.Name = "__allowedRange";
            __allowedRange.Size = new Size(114, 17);
            __allowedRange.TabIndex = 24;
            __allowedRange.Text = "Allowed Range  ";
            __allowedRange.UseVisualStyleBackColor = true;
            // 
            // _nominalMinMax
            // 
            __nominalMinMax.AutoSize = true;
            __nominalMinMax.Location = new Point(4, 56);
            __nominalMinMax.Name = "__nominalMinMax";
            __nominalMinMax.Size = new Size(104, 17);
            __nominalMinMax.TabIndex = 25;
            __nominalMinMax.TabStop = true;
            __nominalMinMax.Text = "Show LSL/USL";
            __nominalMinMax.UseVisualStyleBackColor = true;
            // 
            // _nomialPlusMinus
            // 
            __nomialPlusMinus.AutoSize = true;
            __nomialPlusMinus.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            __nomialPlusMinus.Location = new Point(4, 33);
            __nomialPlusMinus.Name = "__nomialPlusMinus";
            __nomialPlusMinus.Size = new Size(141, 17);
            __nomialPlusMinus.TabIndex = 23;
            __nomialPlusMinus.Text = "Nominal ± Tolerance";
            __nomialPlusMinus.UseVisualStyleBackColor = true;
            // 
            // _conditionalFormatting1
            // 
            __conditionalFormatting1.AutoSize = true;
            __conditionalFormatting1.BackColor = SystemColors.GradientInactiveCaption;
            __conditionalFormatting1.Location = new Point(6, 10);
            __conditionalFormatting1.Name = "__conditionalFormatting1";
            __conditionalFormatting1.Size = new Size(192, 17);
            __conditionalFormatting1.TabIndex = 30;
            __conditionalFormatting1.Text = "Include Tolerance Calculation";
            __conditionalFormatting1.UseVisualStyleBackColor = false;
            // 
            // _measurementListView
            // 
            __measurementListView.AllowUserToAddRows = false;
            __measurementListView.AllowUserToDeleteRows = false;
            __measurementListView.AllowUserToResizeColumns = false;
            __measurementListView.AllowUserToResizeRows = false;
            DataGridViewCellStyle1.BackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(224)), Conversions.ToInteger(Conversions.ToByte(224)), Conversions.ToInteger(Conversions.ToByte(224)));
            __measurementListView.AlternatingRowsDefaultCellStyle = DataGridViewCellStyle1;
            __measurementListView.BackgroundColor = SystemColors.GradientInactiveCaption;
            DataGridViewCellStyle2.Alignment = DataGridViewContentAlignment.MiddleCenter;
            DataGridViewCellStyle2.BackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(118)), Conversions.ToInteger(Conversions.ToByte(157)), Conversions.ToInteger(Conversions.ToByte(208)));
            DataGridViewCellStyle2.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            DataGridViewCellStyle2.ForeColor = Color.White;
            DataGridViewCellStyle2.SelectionBackColor = SystemColors.Highlight;
            DataGridViewCellStyle2.SelectionForeColor = SystemColors.HighlightText;
            DataGridViewCellStyle2.WrapMode = DataGridViewTriState.True;
            __measurementListView.ColumnHeadersDefaultCellStyle = DataGridViewCellStyle2;
            __measurementListView.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            DataGridViewCellStyle3.Alignment = DataGridViewContentAlignment.MiddleRight;
            DataGridViewCellStyle3.BackColor = SystemColors.Window;
            DataGridViewCellStyle3.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            DataGridViewCellStyle3.ForeColor = SystemColors.ControlText;
            DataGridViewCellStyle3.SelectionBackColor = Color.Yellow;
            DataGridViewCellStyle3.SelectionForeColor = Color.Blue;
            DataGridViewCellStyle3.WrapMode = DataGridViewTriState.False;
            __measurementListView.DefaultCellStyle = DataGridViewCellStyle3;
            __measurementListView.EnableHeadersVisualStyles = false;
            __measurementListView.GridColor = SystemColors.ActiveCaption;
            __measurementListView.Location = new Point(6, 29);
            __measurementListView.MultiSelect = false;
            __measurementListView.Name = "__measurementListView";
            DataGridViewCellStyle4.Alignment = DataGridViewContentAlignment.MiddleLeft;
            DataGridViewCellStyle4.BackColor = SystemColors.Control;
            DataGridViewCellStyle4.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            DataGridViewCellStyle4.ForeColor = SystemColors.WindowText;
            DataGridViewCellStyle4.SelectionBackColor = Color.LightYellow;
            DataGridViewCellStyle4.SelectionForeColor = SystemColors.HighlightText;
            DataGridViewCellStyle4.WrapMode = DataGridViewTriState.True;
            __measurementListView.RowHeadersDefaultCellStyle = DataGridViewCellStyle4;
            __measurementListView.RowHeadersWidth = 20;
            __measurementListView.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            __measurementListView.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            __measurementListView.Size = new Size(440, 490);
            __measurementListView.TabIndex = 12;
            // 
            // BtnMoveUp
            // 
            _BtnMoveUp.Image = (Image)resources.GetObject("BtnMoveUp.Image");
            _BtnMoveUp.Location = new Point(452, 166);
            _BtnMoveUp.Name = "_BtnMoveUp";
            _BtnMoveUp.Size = new Size(32, 40);
            _BtnMoveUp.TabIndex = 91;
            _BtnMoveUp.UseVisualStyleBackColor = true;
            // 
            // BtnMoveDown
            // 
            _BtnMoveDown.Image = (Image)resources.GetObject("BtnMoveDown.Image");
            _BtnMoveDown.Location = new Point(452, 212);
            _BtnMoveDown.Name = "_BtnMoveDown";
            _BtnMoveDown.Size = new Size(32, 39);
            _BtnMoveDown.TabIndex = 92;
            _BtnMoveDown.UseVisualStyleBackColor = true;
            // 
            // _programSettings
            // 
            _programSettings.BackColor = SystemColors.GradientInactiveCaption;
            _programSettings.Controls.Add(_Button1);
            _programSettings.Controls.Add(_addNewTitle);
            _programSettings.Controls.Add(pnlTitlePredefinedList);
            _programSettings.Controls.Add(__extraInformationDataView);
            _programSettings.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _programSettings.Location = new Point(4, 27);
            _programSettings.Name = "_programSettings";
            _programSettings.Padding = new Padding(3);
            _programSettings.Size = new Size(490, 504);
            _programSettings.TabIndex = 1;
            _programSettings.Text = "Titles";
            _programSettings.UseVisualStyleBackColor = true;
            // 
            // Button1
            // 
            _Button1.Location = new Point(347, 478);
            _Button1.Name = "_Button1";
            _Button1.Size = new Size(134, 41);
            _Button1.TabIndex = 44;
            _Button1.Text = "Edit Predefined List";
            _Button1.UseVisualStyleBackColor = true;
            // 
            // addNewTitle
            // 
            _addNewTitle.Location = new Point(6, 478);
            _addNewTitle.Name = "_addNewTitle";
            _addNewTitle.Size = new Size(105, 41);
            _addNewTitle.TabIndex = 43;
            _addNewTitle.Text = "Add New Title";
            _addNewTitle.UseVisualStyleBackColor = true;
            // 
            // pnlTitlePredefinedList
            // 
            pnlTitlePredefinedList.BackColor = SystemColors.GradientInactiveCaption;
            pnlTitlePredefinedList.BorderStyle = BorderStyle.Fixed3D;
            pnlTitlePredefinedList.Controls.Add(txtPredefinedList);
            pnlTitlePredefinedList.Controls.Add(Label7);
            pnlTitlePredefinedList.Controls.Add(Label8);
            pnlTitlePredefinedList.Controls.Add(_btnSetList);
            pnlTitlePredefinedList.Location = new Point(208, 5);
            pnlTitlePredefinedList.Name = "pnlTitlePredefinedList";
            pnlTitlePredefinedList.Size = new Size(273, 185);
            pnlTitlePredefinedList.TabIndex = 42;
            pnlTitlePredefinedList.Visible = false;
            // 
            // txtPredefinedList
            // 
            txtPredefinedList.Location = new Point(2, 26);
            txtPredefinedList.Name = "txtPredefinedList";
            txtPredefinedList.Size = new Size(159, 135);
            txtPredefinedList.TabIndex = 31;
            txtPredefinedList.Text = "";
            // 
            // Label7
            // 
            Label7.AutoSize = true;
            Label7.Font = new Font("Tahoma", 8.25f, FontStyle.Regular, GraphicsUnit.Point, Conversions.ToByte(0));
            Label7.Location = new Point(6, 165);
            Label7.Name = "Label7";
            Label7.Size = new Size(156, 13);
            Label7.TabIndex = 28;
            Label7.Text = "Each line becomes a single item";
            // 
            // Label8
            // 
            Label8.AutoSize = true;
            Label8.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            Label8.Location = new Point(11, 6);
            Label8.Name = "Label8";
            Label8.Size = new Size(227, 13);
            Label8.TabIndex = 28;
            Label8.Text = "Enter the list of items for this field/title";
            // 
            // btnSetList
            // 
            _btnSetList.Location = new Point(170, 110);
            _btnSetList.Name = "_btnSetList";
            _btnSetList.Size = new Size(91, 52);
            _btnSetList.TabIndex = 30;
            _btnSetList.Text = "Set List";
            _btnSetList.UseVisualStyleBackColor = true;
            // 
            // _extraInformationDataView
            // 
            __extraInformationDataView.AllowUserToAddRows = false;
            DataGridViewCellStyle5.BackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(224)), Conversions.ToInteger(Conversions.ToByte(224)), Conversions.ToInteger(Conversions.ToByte(224)));
            __extraInformationDataView.AlternatingRowsDefaultCellStyle = DataGridViewCellStyle5;
            __extraInformationDataView.BackgroundColor = SystemColors.GradientInactiveCaption;
            DataGridViewCellStyle6.Alignment = DataGridViewContentAlignment.MiddleCenter;
            DataGridViewCellStyle6.BackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(118)), Conversions.ToInteger(Conversions.ToByte(157)), Conversions.ToInteger(Conversions.ToByte(208)));
            DataGridViewCellStyle6.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            DataGridViewCellStyle6.ForeColor = Color.White;
            DataGridViewCellStyle6.SelectionBackColor = SystemColors.Highlight;
            DataGridViewCellStyle6.SelectionForeColor = SystemColors.HighlightText;
            DataGridViewCellStyle6.WrapMode = DataGridViewTriState.True;
            __extraInformationDataView.ColumnHeadersDefaultCellStyle = DataGridViewCellStyle6;
            __extraInformationDataView.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            DataGridViewCellStyle7.Alignment = DataGridViewContentAlignment.MiddleLeft;
            DataGridViewCellStyle7.BackColor = SystemColors.Window;
            DataGridViewCellStyle7.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            DataGridViewCellStyle7.ForeColor = SystemColors.ControlText;
            DataGridViewCellStyle7.SelectionBackColor = Color.Yellow;
            DataGridViewCellStyle7.SelectionForeColor = Color.Blue;
            DataGridViewCellStyle7.WrapMode = DataGridViewTriState.False;
            __extraInformationDataView.DefaultCellStyle = DataGridViewCellStyle7;
            __extraInformationDataView.GridColor = SystemColors.ActiveCaption;
            __extraInformationDataView.Location = new Point(6, 3);
            __extraInformationDataView.Name = "__extraInformationDataView";
            __extraInformationDataView.RowHeadersWidth = 20;
            __extraInformationDataView.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            __extraInformationDataView.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            __extraInformationDataView.Size = new Size(478, 469);
            __extraInformationDataView.TabIndex = 1;
            // 
            // _settings
            // 
            _settings.BackColor = SystemColors.GradientInactiveCaption;
            _settings.Controls.Add(GroupBox1);
            _settings.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _settings.Location = new Point(4, 27);
            _settings.Name = "_settings";
            _settings.Padding = new Padding(3);
            _settings.Size = new Size(490, 504);
            _settings.TabIndex = 2;
            _settings.Text = "Settings";
            _settings.UseVisualStyleBackColor = true;
            // 
            // GroupBox1
            // 
            GroupBox1.Controls.Add(txtPartName_Row);
            GroupBox1.Controls.Add(txtPartName_Col);
            GroupBox1.Controls.Add(Label12);
            GroupBox1.Controls.Add(Label27);
            GroupBox1.Controls.Add(Label28);
            GroupBox1.Controls.Add(txtPartName_Label_Row);
            GroupBox1.Controls.Add(txtPartName_Label_Col);
            GroupBox1.Controls.Add(Label26);
            GroupBox1.Controls.Add(Label25);
            GroupBox1.Controls.Add(Label11);
            GroupBox1.Controls.Add(grpPasswordProtection);
            GroupBox1.Controls.Add(cbPaginateReport);
            GroupBox1.Controls.Add(_CheckBox1);
            GroupBox1.Controls.Add(Label24);
            GroupBox1.Controls.Add(Label23);
            GroupBox1.Controls.Add(PanelRcadPic);
            GroupBox1.Controls.Add(_ProgramInNewSheet);
            GroupBox1.Controls.Add(_DetailSelect);
            GroupBox1.Controls.Add(Label17);
            GroupBox1.Controls.Add(chkEarlierMadeReportsAlso);
            GroupBox1.Controls.Add(nudFormatdigits);
            GroupBox1.Controls.Add(Label16);
            GroupBox1.Controls.Add(Label15);
            GroupBox1.Controls.Add(Label14);
            GroupBox1.Controls.Add(txtMaxColumn);
            GroupBox1.Controls.Add(chkMakeSettingsDefault);
            GroupBox1.Controls.Add(txtMaxRow);
            GroupBox1.Controls.Add(txtInitialColumn);
            GroupBox1.Controls.Add(Label1);
            GroupBox1.Controls.Add(txtInitialRow);
            GroupBox1.Controls.Add(Label3);
            GroupBox1.Controls.Add(pnlInsertPicture);
            GroupBox1.Controls.Add(_chkInsertPicture);
            GroupBox1.Controls.Add(chkWriteprogramNameInReport);
            GroupBox1.Controls.Add(Label9);
            GroupBox1.Controls.Add(columnLabel);
            GroupBox1.Controls.Add(txtendRow);
            GroupBox1.Controls.Add(txtEndCol);
            GroupBox1.Controls.Add(Label6);
            GroupBox1.Controls.Add(_btnUpdateRange);
            GroupBox1.Controls.Add(_templateRangeLabel);
            GroupBox1.Controls.Add(Label18);
            GroupBox1.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            GroupBox1.Location = new Point(5, 6);
            GroupBox1.Name = "GroupBox1";
            GroupBox1.Size = new Size(478, 495);
            GroupBox1.TabIndex = 9;
            GroupBox1.TabStop = false;
            GroupBox1.Text = "Report Template Settings ";
            // 
            // txtPartName_Row
            // 
            txtPartName_Row.Location = new Point(136, 124);
            txtPartName_Row.Name = "txtPartName_Row";
            txtPartName_Row.Size = new Size(46, 21);
            txtPartName_Row.TabIndex = 54;
            // 
            // txtPartName_Col
            // 
            txtPartName_Col.Location = new Point(85, 124);
            txtPartName_Col.Name = "txtPartName_Col";
            txtPartName_Col.Size = new Size(42, 21);
            txtPartName_Col.TabIndex = 53;
            // 
            // Label12
            // 
            Label12.AutoSize = true;
            Label12.Location = new Point(18, 128);
            Label12.Name = "Label12";
            Label12.Size = new Size(66, 13);
            Label12.TabIndex = 52;
            Label12.Text = "Part Name";
            // 
            // Label27
            // 
            Label27.AutoSize = true;
            Label27.Location = new Point(402, 50);
            Label27.Name = "Label27";
            Label27.Size = new Size(31, 13);
            Label27.TabIndex = 51;
            Label27.Text = "Row";
            // 
            // Label28
            // 
            Label28.AutoSize = true;
            Label28.Location = new Point(355, 50);
            Label28.Name = "Label28";
            Label28.Size = new Size(24, 13);
            Label28.TabIndex = 50;
            Label28.Text = "Col";
            // 
            // txtPartName_Label_Row
            // 
            txtPartName_Label_Row.Location = new Point(136, 98);
            txtPartName_Label_Row.Name = "txtPartName_Label_Row";
            txtPartName_Label_Row.Size = new Size(46, 21);
            txtPartName_Label_Row.TabIndex = 49;
            // 
            // txtPartName_Label_Col
            // 
            txtPartName_Label_Col.Location = new Point(85, 98);
            txtPartName_Label_Col.Name = "txtPartName_Label_Col";
            txtPartName_Label_Col.Size = new Size(42, 21);
            txtPartName_Label_Col.TabIndex = 48;
            // 
            // Label26
            // 
            Label26.AutoSize = true;
            Label26.Location = new Point(17, 102);
            Label26.Name = "Label26";
            Label26.Size = new Size(64, 13);
            Label26.TabIndex = 47;
            Label26.Text = "Part Label";
            // 
            // Label25
            // 
            Label25.AutoSize = true;
            Label25.ForeColor = SystemColors.ControlDark;
            Label25.Location = new Point(17, 271);
            Label25.Name = "Label25";
            Label25.Size = new Size(441, 13);
            Label25.TabIndex = 46;
            Label25.Text = "______________________________________________________________";
            // 
            // Label11
            // 
            Label11.AutoSize = true;
            Label11.ForeColor = SystemColors.ControlDark;
            Label11.Location = new Point(12, 408);
            Label11.Name = "Label11";
            Label11.Size = new Size(441, 13);
            Label11.TabIndex = 45;
            Label11.Text = "______________________________________________________________";
            // 
            // grpPasswordProtection
            // 
            grpPasswordProtection.BackColor = SystemColors.GradientInactiveCaption;
            grpPasswordProtection.Controls.Add(Label10);
            grpPasswordProtection.Controls.Add(_filePassword);
            grpPasswordProtection.Controls.Add(filepasslabel);
            grpPasswordProtection.Controls.Add(passwordBox);
            grpPasswordProtection.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            grpPasswordProtection.Location = new Point(0, 424);
            grpPasswordProtection.Name = "grpPasswordProtection";
            grpPasswordProtection.Size = new Size(480, 70);
            grpPasswordProtection.TabIndex = 11;
            grpPasswordProtection.TabStop = false;
            grpPasswordProtection.Text = "Password Protection";
            // 
            // Label10
            // 
            Label10.AutoSize = true;
            Label10.Location = new Point(165, 12);
            Label10.Name = "Label10";
            Label10.Size = new Size(115, 13);
            Label10.TabIndex = 8;
            Label10.Text = "Excel File Password";
            // 
            // filePassword
            // 
            _filePassword.Location = new Point(307, 19);
            _filePassword.Name = "_filePassword";
            _filePassword.Size = new Size(84, 45);
            _filePassword.TabIndex = 7;
            _filePassword.Text = "Set";
            _filePassword.UseVisualStyleBackColor = true;
            // 
            // filepasslabel
            // 
            filepasslabel.AutoSize = true;
            filepasslabel.Location = new Point(101, 35);
            filepasslabel.Name = "filepasslabel";
            filepasslabel.Size = new Size(61, 13);
            filepasslabel.TabIndex = 4;
            filepasslabel.Text = "Password";
            // 
            // passwordBox
            // 
            passwordBox.Location = new Point(168, 32);
            passwordBox.Name = "passwordBox";
            passwordBox.PasswordChar = '*';
            passwordBox.Size = new Size(133, 21);
            passwordBox.TabIndex = 5;
            // 
            // cbPaginateReport
            // 
            cbPaginateReport.AutoSize = true;
            cbPaginateReport.Location = new Point(6, 18);
            cbPaginateReport.Name = "cbPaginateReport";
            cbPaginateReport.Size = new Size(118, 17);
            cbPaginateReport.TabIndex = 44;
            cbPaginateReport.Text = "Paginate Report";
            cbPaginateReport.UseVisualStyleBackColor = true;
            // 
            // CheckBox1
            // 
            _CheckBox1.AutoSize = true;
            _CheckBox1.Location = new Point(285, 256);
            _CheckBox1.Name = "_CheckBox1";
            _CheckBox1.Size = new Size(157, 17);
            _CheckBox1.TabIndex = 43;
            _CheckBox1.Text = "Create Raw Data Sheet";
            _CheckBox1.UseVisualStyleBackColor = true;
            // 
            // Label24
            // 
            Label24.AutoSize = true;
            Label24.Location = new Point(15, 288);
            Label24.Name = "Label24";
            Label24.Size = new Size(51, 13);
            Label24.TabIndex = 42;
            Label24.Text = "Image1";
            // 
            // Label23
            // 
            Label23.AutoSize = true;
            Label23.Location = new Point(269, 288);
            Label23.Name = "Label23";
            Label23.Size = new Size(51, 13);
            Label23.TabIndex = 41;
            Label23.Text = "Image2";
            // 
            // PanelRcadPic
            // 
            PanelRcadPic.Controls.Add(_UpdateRcadPic);
            PanelRcadPic.Controls.Add(RcadCol);
            PanelRcadPic.Controls.Add(Rcadwidth);
            PanelRcadPic.Controls.Add(Label19);
            PanelRcadPic.Controls.Add(Label20);
            PanelRcadPic.Controls.Add(Label21);
            PanelRcadPic.Controls.Add(RcadRow);
            PanelRcadPic.Controls.Add(Rcadheight);
            PanelRcadPic.Controls.Add(Label22);
            PanelRcadPic.Enabled = false;
            PanelRcadPic.Location = new Point(269, 304);
            PanelRcadPic.Name = "PanelRcadPic";
            PanelRcadPic.Size = new Size(182, 107);
            PanelRcadPic.TabIndex = 40;
            // 
            // UpdateRcadPic
            // 
            _UpdateRcadPic.Location = new Point(3, 63);
            _UpdateRcadPic.Name = "_UpdateRcadPic";
            _UpdateRcadPic.Size = new Size(176, 41);
            _UpdateRcadPic.TabIndex = 18;
            _UpdateRcadPic.Text = "Update picture details";
            _UpdateRcadPic.UseVisualStyleBackColor = true;
            // 
            // RcadCol
            // 
            RcadCol.Location = new Point(137, 39);
            RcadCol.Name = "RcadCol";
            RcadCol.Size = new Size(42, 21);
            RcadCol.TabIndex = 25;
            // 
            // Rcadwidth
            // 
            Rcadwidth.Location = new Point(51, 3);
            Rcadwidth.Name = "Rcadwidth";
            Rcadwidth.Size = new Size(42, 21);
            Rcadwidth.TabIndex = 19;
            // 
            // Label19
            // 
            Label19.AutoSize = true;
            Label19.Location = new Point(16, 6);
            Label19.Name = "Label19";
            Label19.Size = new Size(40, 13);
            Label19.TabIndex = 18;
            Label19.Text = "Width";
            // 
            // Label20
            // 
            Label20.AutoSize = true;
            Label20.Location = new Point(95, 6);
            Label20.Name = "Label20";
            Label20.Size = new Size(44, 13);
            Label20.TabIndex = 20;
            Label20.Text = "Height";
            // 
            // Label21
            // 
            Label21.AutoSize = true;
            Label21.Location = new Point(106, 42);
            Label21.Name = "Label21";
            Label21.Size = new Size(27, 13);
            Label21.TabIndex = 23;
            Label21.Text = "Col.";
            // 
            // RcadRow
            // 
            RcadRow.Location = new Point(51, 39);
            RcadRow.Name = "RcadRow";
            RcadRow.Size = new Size(42, 21);
            RcadRow.TabIndex = 24;
            // 
            // Rcadheight
            // 
            Rcadheight.Location = new Point(141, 3);
            Rcadheight.Name = "Rcadheight";
            Rcadheight.Size = new Size(38, 21);
            Rcadheight.TabIndex = 21;
            // 
            // Label22
            // 
            Label22.AutoSize = true;
            Label22.Location = new Point(16, 41);
            Label22.Name = "Label22";
            Label22.Size = new Size(31, 13);
            Label22.TabIndex = 22;
            Label22.Text = "Row";
            // 
            // ProgramInNewSheet
            // 
            _ProgramInNewSheet.AutoSize = true;
            _ProgramInNewSheet.Location = new Point(6, 235);
            _ProgramInNewSheet.Name = "_ProgramInNewSheet";
            _ProgramInNewSheet.Size = new Size(143, 17);
            _ProgramInNewSheet.TabIndex = 38;
            _ProgramInNewSheet.Text = "ProgramInNewSheet";
            _ProgramInNewSheet.UseVisualStyleBackColor = true;
            // 
            // DetailSelect
            // 
            _DetailSelect.AutoSize = true;
            _DetailSelect.Location = new Point(155, 235);
            _DetailSelect.Name = "_DetailSelect";
            _DetailSelect.Size = new Size(128, 17);
            _DetailSelect.TabIndex = 37;
            _DetailSelect.Text = "Show Detail Sheet";
            _DetailSelect.UseVisualStyleBackColor = true;
            // 
            // Label17
            // 
            Label17.AutoSize = true;
            Label17.ForeColor = SystemColors.ControlDark;
            Label17.Location = new Point(19, 215);
            Label17.Name = "Label17";
            Label17.Size = new Size(441, 13);
            Label17.TabIndex = 35;
            Label17.Text = "______________________________________________________________";
            // 
            // chkEarlierMadeReportsAlso
            // 
            chkEarlierMadeReportsAlso.AutoSize = true;
            chkEarlierMadeReportsAlso.Location = new Point(273, 162);
            chkEarlierMadeReportsAlso.Name = "chkEarlierMadeReportsAlso";
            chkEarlierMadeReportsAlso.Size = new Size(196, 17);
            chkEarlierMadeReportsAlso.TabIndex = 34;
            chkEarlierMadeReportsAlso.Text = "for earlier made reports also )";
            chkEarlierMadeReportsAlso.UseVisualStyleBackColor = true;
            // 
            // nudFormatdigits
            // 
            nudFormatdigits.Location = new Point(136, 160);
            nudFormatdigits.Maximum = new decimal(new int[] { 4, 0, 0, 0 });
            nudFormatdigits.Name = "nudFormatdigits";
            nudFormatdigits.Size = new Size(36, 21);
            nudFormatdigits.TabIndex = 33;
            nudFormatdigits.Value = new decimal(new int[] { 4, 0, 0, 0 });
            // 
            // Label16
            // 
            Label16.AutoSize = true;
            Label16.Location = new Point(174, 162);
            Label16.Name = "Label16";
            Label16.Size = new Size(99, 13);
            Label16.TabIndex = 32;
            Label16.Text = "Decimal Places (";
            // 
            // Label15
            // 
            Label15.AutoSize = true;
            Label15.Location = new Point(190, 162);
            Label15.Name = "Label15";
            Label15.Size = new Size(0, 13);
            Label15.TabIndex = 31;
            // 
            // Label14
            // 
            Label14.AutoSize = true;
            Label14.Location = new Point(1, 162);
            Label14.Name = "Label14";
            Label14.Size = new Size(133, 13);
            Label14.TabIndex = 30;
            Label14.Text = "Measurement Display:";
            // 
            // txtMaxColumn
            // 
            txtMaxColumn.Location = new Point(348, 101);
            txtMaxColumn.Name = "txtMaxColumn";
            txtMaxColumn.Size = new Size(42, 21);
            txtMaxColumn.TabIndex = 28;
            // 
            // chkMakeSettingsDefault
            // 
            chkMakeSettingsDefault.AutoSize = true;
            chkMakeSettingsDefault.Location = new Point(13, 195);
            chkMakeSettingsDefault.Name = "chkMakeSettingsDefault";
            chkMakeSettingsDefault.Size = new Size(147, 17);
            chkMakeSettingsDefault.TabIndex = 27;
            chkMakeSettingsDefault.Text = "Default for all reports";
            chkMakeSettingsDefault.UseVisualStyleBackColor = true;
            // 
            // txtMaxRow
            // 
            txtMaxRow.Location = new Point(400, 101);
            txtMaxRow.Name = "txtMaxRow";
            txtMaxRow.Size = new Size(42, 21);
            txtMaxRow.TabIndex = 25;
            // 
            // txtInitialColumn
            // 
            txtInitialColumn.Location = new Point(348, 74);
            txtInitialColumn.Name = "txtInitialColumn";
            txtInitialColumn.Size = new Size(42, 21);
            txtInitialColumn.TabIndex = 22;
            // 
            // Label1
            // 
            Label1.AutoSize = true;
            Label1.Location = new Point(279, 105);
            Label1.Name = "Label1";
            Label1.Size = new Size(66, 13);
            Label1.TabIndex = 21;
            Label1.Text = "Final Entry";
            // 
            // txtInitialRow
            // 
            txtInitialRow.Location = new Point(400, 73);
            txtInitialRow.Name = "txtInitialRow";
            txtInitialRow.Size = new Size(42, 21);
            txtInitialRow.TabIndex = 20;
            // 
            // Label3
            // 
            Label3.AutoSize = true;
            Label3.Location = new Point(272, 80);
            Label3.Name = "Label3";
            Label3.Size = new Size(73, 13);
            Label3.TabIndex = 19;
            Label3.Text = "Initial Entry";
            // 
            // pnlInsertPicture
            // 
            pnlInsertPicture.Controls.Add(_btnUpdatePicDetails);
            pnlInsertPicture.Controls.Add(txtColumn);
            pnlInsertPicture.Controls.Add(txtWidth);
            pnlInsertPicture.Controls.Add(lblWidth);
            pnlInsertPicture.Controls.Add(lblHeight);
            pnlInsertPicture.Controls.Add(lblTop);
            pnlInsertPicture.Controls.Add(txtRow);
            pnlInsertPicture.Controls.Add(txtHeight);
            pnlInsertPicture.Controls.Add(lblLeft);
            pnlInsertPicture.Enabled = false;
            pnlInsertPicture.Location = new Point(15, 304);
            pnlInsertPicture.Name = "pnlInsertPicture";
            pnlInsertPicture.Size = new Size(182, 107);
            pnlInsertPicture.TabIndex = 9;
            // 
            // btnUpdatePicDetails
            // 
            _btnUpdatePicDetails.Location = new Point(3, 63);
            _btnUpdatePicDetails.Name = "_btnUpdatePicDetails";
            _btnUpdatePicDetails.Size = new Size(176, 41);
            _btnUpdatePicDetails.TabIndex = 18;
            _btnUpdatePicDetails.Text = "Update picture details";
            _btnUpdatePicDetails.UseVisualStyleBackColor = true;
            // 
            // txtColumn
            // 
            txtColumn.Location = new Point(137, 39);
            txtColumn.Name = "txtColumn";
            txtColumn.Size = new Size(42, 21);
            txtColumn.TabIndex = 25;
            // 
            // txtWidth
            // 
            txtWidth.Location = new Point(51, 3);
            txtWidth.Name = "txtWidth";
            txtWidth.Size = new Size(42, 21);
            txtWidth.TabIndex = 19;
            // 
            // lblWidth
            // 
            lblWidth.AutoSize = true;
            lblWidth.Location = new Point(5, 7);
            lblWidth.Name = "lblWidth";
            lblWidth.Size = new Size(40, 13);
            lblWidth.TabIndex = 18;
            lblWidth.Text = "Width";
            // 
            // lblHeight
            // 
            lblHeight.AutoSize = true;
            lblHeight.Location = new Point(95, 6);
            lblHeight.Name = "lblHeight";
            lblHeight.Size = new Size(44, 13);
            lblHeight.TabIndex = 20;
            lblHeight.Text = "Height";
            // 
            // lblTop
            // 
            lblTop.AutoSize = true;
            lblTop.Location = new Point(106, 42);
            lblTop.Name = "lblTop";
            lblTop.Size = new Size(27, 13);
            lblTop.TabIndex = 23;
            lblTop.Text = "Col.";
            // 
            // txtRow
            // 
            txtRow.Location = new Point(51, 39);
            txtRow.Name = "txtRow";
            txtRow.Size = new Size(42, 21);
            txtRow.TabIndex = 24;
            // 
            // txtHeight
            // 
            txtHeight.Location = new Point(141, 3);
            txtHeight.Name = "txtHeight";
            txtHeight.Size = new Size(38, 21);
            txtHeight.TabIndex = 21;
            // 
            // lblLeft
            // 
            lblLeft.AutoSize = true;
            lblLeft.Location = new Point(16, 41);
            lblLeft.Name = "lblLeft";
            lblLeft.Size = new Size(31, 13);
            lblLeft.TabIndex = 22;
            lblLeft.Text = "Row";
            // 
            // chkInsertPicture
            // 
            _chkInsertPicture.AutoSize = true;
            _chkInsertPicture.Location = new Point(6, 256);
            _chkInsertPicture.Name = "_chkInsertPicture";
            _chkInsertPicture.Size = new Size(156, 17);
            _chkInsertPicture.TabIndex = 0;
            _chkInsertPicture.Text = "Insert picture in report";
            _chkInsertPicture.UseVisualStyleBackColor = true;
            // 
            // chkWriteprogramNameInReport
            // 
            chkWriteprogramNameInReport.AutoSize = true;
            chkWriteprogramNameInReport.Location = new Point(284, 235);
            chkWriteprogramNameInReport.Name = "chkWriteprogramNameInReport";
            chkWriteprogramNameInReport.Size = new Size(199, 17);
            chkWriteprogramNameInReport.TabIndex = 17;
            chkWriteprogramNameInReport.Text = "Write program name in Report";
            chkWriteprogramNameInReport.UseVisualStyleBackColor = true;
            // 
            // Label9
            // 
            Label9.AutoSize = true;
            Label9.Location = new Point(137, 50);
            Label9.Name = "Label9";
            Label9.Size = new Size(31, 13);
            Label9.TabIndex = 16;
            Label9.Text = "Row";
            // 
            // columnLabel
            // 
            columnLabel.AutoSize = true;
            columnLabel.Location = new Point(90, 50);
            columnLabel.Name = "columnLabel";
            columnLabel.Size = new Size(24, 13);
            columnLabel.TabIndex = 15;
            columnLabel.Text = "Col";
            // 
            // txtendRow
            // 
            txtendRow.Location = new Point(136, 73);
            txtendRow.Name = "txtendRow";
            txtendRow.Size = new Size(46, 21);
            txtendRow.TabIndex = 14;
            // 
            // txtEndCol
            // 
            txtEndCol.Location = new Point(84, 72);
            txtEndCol.Name = "txtEndCol";
            txtEndCol.Size = new Size(42, 21);
            txtEndCol.TabIndex = 12;
            // 
            // Label6
            // 
            Label6.AutoSize = true;
            Label6.Location = new Point(14, 76);
            Label6.Name = "Label6";
            Label6.Size = new Size(67, 13);
            Label6.TabIndex = 9;
            Label6.Text = "Ending Cell";
            // 
            // btnUpdateRange
            // 
            _btnUpdateRange.Location = new Point(311, 188);
            _btnUpdateRange.Name = "_btnUpdateRange";
            _btnUpdateRange.Size = new Size(157, 28);
            _btnUpdateRange.TabIndex = 7;
            _btnUpdateRange.Text = "Update";
            _btnUpdateRange.UseVisualStyleBackColor = true;
            // 
            // _templateRangeLabel
            // 
            _templateRangeLabel.AutoSize = true;
            _templateRangeLabel.Location = new Point(174, 33);
            _templateRangeLabel.Name = "_templateRangeLabel";
            _templateRangeLabel.Size = new Size(129, 13);
            _templateRangeLabel.TabIndex = 0;
            _templateRangeLabel.Text = "Page Selection Range";
            // 
            // Label18
            // 
            Label18.AutoSize = true;
            Label18.ForeColor = SystemColors.ControlDark;
            Label18.Location = new Point(12, 29);
            Label18.Name = "Label18";
            Label18.Size = new Size(441, 13);
            Label18.TabIndex = 36;
            Label18.Text = "______________________________________________________________";
            // 
            // ShowToolBar
            // 
            _ShowToolBar.AutoSize = true;
            _ShowToolBar.Location = new Point(628, 661);
            _ShowToolBar.Name = "_ShowToolBar";
            _ShowToolBar.Size = new Size(121, 17);
            _ShowToolBar.TabIndex = 10;
            _ShowToolBar.Text = "Show Excel Toolbar";
            _ShowToolBar.UseVisualStyleBackColor = true;
            // 
            // showExcelMenu
            // 
            _showExcelMenu.AutoSize = true;
            _showExcelMenu.Location = new Point(511, 661);
            _showExcelMenu.Name = "_showExcelMenu";
            _showExcelMenu.Size = new Size(112, 17);
            _showExcelMenu.TabIndex = 9;
            _showExcelMenu.Text = "Show Excel Menu";
            _showExcelMenu.UseVisualStyleBackColor = true;
            // 
            // _sheetBox
            // 
            _sheetBox.FormattingEnabled = true;
            _sheetBox.Location = new Point(92, 27);
            _sheetBox.Name = "_sheetBox";
            _sheetBox.Size = new Size(110, 21);
            _sheetBox.TabIndex = 31;
            // 
            // _reportSheets
            // 
            _reportSheets.AutoSize = true;
            _reportSheets.Location = new Point(7, 30);
            _reportSheets.Name = "_reportSheets";
            _reportSheets.Size = new Size(82, 13);
            _reportSheets.TabIndex = 29;
            _reportSheets.Text = "Report Sheet";
            // 
            // _saveButton
            // 
            __saveButton.BackColor = Color.Transparent;
            __saveButton.Font = new Font("Tahoma", 20.0f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            __saveButton.ForeColor = Color.ForestGreen;
            __saveButton.Location = new Point(890, 667);
            __saveButton.Name = "__saveButton";
            __saveButton.Size = new Size(182, 67);
            __saveButton.TabIndex = 29;
            __saveButton.Text = "Save";
            __saveButton.UseVisualStyleBackColor = false;
            // 
            // _programName
            // 
            _programName.AutoSize = true;
            _programName.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _programName.Location = new Point(-1, 7);
            _programName.Name = "_programName";
            _programName.Size = new Size(56, 13);
            _programName.TabIndex = 30;
            _programName.Text = "Program";
            // 
            // _programNameBox
            // 
            _programNameBox.Location = new Point(249, 3);
            _programNameBox.Name = "_programNameBox";
            _programNameBox.ReadOnly = true;
            _programNameBox.Size = new Size(256, 20);
            _programNameBox.TabIndex = 31;
            // 
            // _programNamePathBox
            // 
            _programNamePathBox.Location = new Point(54, 3);
            _programNamePathBox.Name = "_programNamePathBox";
            _programNamePathBox.ReadOnly = true;
            _programNamePathBox.Size = new Size(186, 20);
            _programNamePathBox.TabIndex = 32;
            _programNamePathBox.TextAlign = HorizontalAlignment.Right;
            // 
            // Label2
            // 
            Label2.AutoSize = true;
            Label2.Location = new Point(240, 7);
            Label2.Name = "Label2";
            Label2.Size = new Size(12, 13);
            Label2.TabIndex = 33;
            Label2.Text = @"\";
            // 
            // _reportName
            // 
            _reportName.AutoSize = true;
            _reportName.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _reportName.Location = new Point(8, 30);
            _reportName.Name = "_reportName";
            _reportName.Size = new Size(46, 13);
            _reportName.TabIndex = 34;
            _reportName.Text = "Report";
            // 
            // _reportNameBox
            // 
            _reportNameBox.Location = new Point(249, 26);
            _reportNameBox.Name = "_reportNameBox";
            _reportNameBox.ReadOnly = true;
            _reportNameBox.Size = new Size(256, 20);
            _reportNameBox.TabIndex = 35;
            // 
            // _reportPathNameBox
            // 
            _reportPathNameBox.Location = new Point(55, 26);
            _reportPathNameBox.Name = "_reportPathNameBox";
            _reportPathNameBox.ReadOnly = true;
            _reportPathNameBox.Size = new Size(186, 20);
            _reportPathNameBox.TabIndex = 36;
            _reportPathNameBox.TextAlign = HorizontalAlignment.Right;
            // 
            // Label5
            // 
            Label5.AutoSize = true;
            Label5.Location = new Point(239, 29);
            Label5.Name = "Label5";
            Label5.Size = new Size(12, 13);
            Label5.TabIndex = 37;
            Label5.Text = @"\";
            // 
            // _pageStyleBox
            // 
            _pageStyleBox.Controls.Add(__landScapeRadioButton);
            _pageStyleBox.Controls.Add(__portraitRadioButton);
            _pageStyleBox.Controls.Add(LinkLabel1);
            _pageStyleBox.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _pageStyleBox.Location = new Point(120, 52);
            _pageStyleBox.Name = "_pageStyleBox";
            _pageStyleBox.Size = new Size(119, 66);
            _pageStyleBox.TabIndex = 38;
            _pageStyleBox.TabStop = false;
            _pageStyleBox.Text = "Page Orientation";
            // 
            // LinkLabel1
            // 
            LinkLabel1.AutoSize = true;
            LinkLabel1.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            LinkLabel1.Location = new Point(90, 106);
            LinkLabel1.Name = "LinkLabel1";
            LinkLabel1.Size = new Size(146, 13);
            LinkLabel1.TabIndex = 22;
            LinkLabel1.TabStop = true;
            LinkLabel1.Text = "More Settings....................";
            // 
            // GroupBox2
            // 
            GroupBox2.Controls.Add(_reportStyleNameBox);
            GroupBox2.Controls.Add(_sheetBox);
            GroupBox2.Controls.Add(Label4);
            GroupBox2.Controls.Add(LinkLabel2);
            GroupBox2.Controls.Add(_reportSheets);
            GroupBox2.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            GroupBox2.Location = new Point(7, 122);
            GroupBox2.Name = "GroupBox2";
            GroupBox2.Size = new Size(498, 62);
            GroupBox2.TabIndex = 19;
            GroupBox2.TabStop = false;
            GroupBox2.Text = "Other Information";
            // 
            // _reportStyleNameBox
            // 
            _reportStyleNameBox.FormattingEnabled = true;
            _reportStyleNameBox.Location = new Point(294, 27);
            _reportStyleNameBox.Name = "_reportStyleNameBox";
            _reportStyleNameBox.Size = new Size(198, 21);
            _reportStyleNameBox.TabIndex = 3;
            // 
            // Label4
            // 
            Label4.AutoSize = true;
            Label4.Location = new Point(219, 30);
            Label4.Name = "Label4";
            Label4.Size = new Size(78, 13);
            Label4.TabIndex = 0;
            Label4.Text = "Report Style";
            // 
            // LinkLabel2
            // 
            LinkLabel2.AutoSize = true;
            LinkLabel2.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            LinkLabel2.Location = new Point(90, 106);
            LinkLabel2.Name = "LinkLabel2";
            LinkLabel2.Size = new Size(146, 13);
            LinkLabel2.TabIndex = 22;
            LinkLabel2.TabStop = true;
            LinkLabel2.Text = "More Settings....................";
            // 
            // panelButtonEnclosure
            // 
            panelButtonEnclosure.Anchor = AnchorStyles.None;
            panelButtonEnclosure.Controls.Add(btnPreview);
            panelButtonEnclosure.Location = new Point(792, 661);
            panelButtonEnclosure.Name = "panelButtonEnclosure";
            panelButtonEnclosure.Size = new Size(27, 74);
            panelButtonEnclosure.TabIndex = 39;
            // 
            // btnPreview
            // 
            btnPreview.BackColor = Color.Transparent;
            btnPreview.Font = new Font("Tahoma", 14.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            btnPreview.Location = new Point(5, 10);
            btnPreview.Name = "btnPreview";
            btnPreview.Size = new Size(19, 59);
            btnPreview.TabIndex = 30;
            btnPreview.Text = "Preview";
            btnPreview.UseVisualStyleBackColor = false;
            btnPreview.Visible = false;
            // 
            // fieldContextMenuStrip
            // 
            fieldContextMenuStrip.Items.AddRange(new ToolStripItem[] { DateToolStripMenuItem, TimeToolStripMenuItem, PredefinedListToolStripMenuItem, PassFailToolStripMenuItem, TextToolStripMenuItem });
            fieldContextMenuStrip.Name = "fieldContextMenuStrip";
            fieldContextMenuStrip.Size = new Size(153, 114);
            // 
            // DateToolStripMenuItem
            // 
            DateToolStripMenuItem.Name = "DateToolStripMenuItem";
            DateToolStripMenuItem.Size = new Size(152, 22);
            DateToolStripMenuItem.Text = "Date";
            // 
            // TimeToolStripMenuItem
            // 
            TimeToolStripMenuItem.Name = "TimeToolStripMenuItem";
            TimeToolStripMenuItem.Size = new Size(152, 22);
            TimeToolStripMenuItem.Text = "Time";
            // 
            // PredefinedListToolStripMenuItem
            // 
            PredefinedListToolStripMenuItem.Name = "PredefinedListToolStripMenuItem";
            PredefinedListToolStripMenuItem.Size = new Size(152, 22);
            PredefinedListToolStripMenuItem.Text = "Predefined List";
            // 
            // PassFailToolStripMenuItem
            // 
            PassFailToolStripMenuItem.Name = "PassFailToolStripMenuItem";
            PassFailToolStripMenuItem.Size = new Size(152, 22);
            PassFailToolStripMenuItem.Text = "Pass/Fail";
            // 
            // TextToolStripMenuItem
            // 
            TextToolStripMenuItem.Name = "TextToolStripMenuItem";
            TextToolStripMenuItem.Size = new Size(152, 22);
            TextToolStripMenuItem.Text = "Text";
            // 
            // titleDataTypeContextMenuList
            // 
            titleDataTypeContextMenuList.Items.AddRange(new ToolStripItem[] { PredefinedListToolStripMenuItem1, TextToolStripMenuItem1, DateToolStripMenuItem1, TimeToolStripMenuItem1, PassFailToolStripMenuItem1 });
            titleDataTypeContextMenuList.Name = "titleDataTypeContextMenuList";
            titleDataTypeContextMenuList.Size = new Size(153, 114);
            // 
            // PredefinedListToolStripMenuItem1
            // 
            PredefinedListToolStripMenuItem1.Name = "PredefinedListToolStripMenuItem1";
            PredefinedListToolStripMenuItem1.Size = new Size(152, 22);
            PredefinedListToolStripMenuItem1.Text = "Predefined List";
            // 
            // TextToolStripMenuItem1
            // 
            TextToolStripMenuItem1.Name = "TextToolStripMenuItem1";
            TextToolStripMenuItem1.Size = new Size(152, 22);
            TextToolStripMenuItem1.Text = "Text";
            // 
            // DateToolStripMenuItem1
            // 
            DateToolStripMenuItem1.Name = "DateToolStripMenuItem1";
            DateToolStripMenuItem1.Size = new Size(152, 22);
            DateToolStripMenuItem1.Text = "Date";
            // 
            // TimeToolStripMenuItem1
            // 
            TimeToolStripMenuItem1.Name = "TimeToolStripMenuItem1";
            TimeToolStripMenuItem1.Size = new Size(152, 22);
            TimeToolStripMenuItem1.Text = "Time";
            // 
            // PassFailToolStripMenuItem1
            // 
            PassFailToolStripMenuItem1.Name = "PassFailToolStripMenuItem1";
            PassFailToolStripMenuItem1.Size = new Size(152, 22);
            PassFailToolStripMenuItem1.Text = "Pass/Fail";
            // 
            // convertFieldToTitleList
            // 
            convertFieldToTitleList.Items.AddRange(new ToolStripItem[] { ConvertFieldAsTitleToolStripMenuItem, DeleteToolStripMenuItem, SetAsPassFailToolStripMenuItem });
            convertFieldToTitleList.Name = "convertFieldToTitle";
            convertFieldToTitleList.Size = new Size(184, 70);
            // 
            // ConvertFieldAsTitleToolStripMenuItem
            // 
            ConvertFieldAsTitleToolStripMenuItem.Name = "ConvertFieldAsTitleToolStripMenuItem";
            ConvertFieldAsTitleToolStripMenuItem.Size = new Size(183, 22);
            ConvertFieldAsTitleToolStripMenuItem.Text = "Convert Field as Title";
            // 
            // DeleteToolStripMenuItem
            // 
            DeleteToolStripMenuItem.Name = "DeleteToolStripMenuItem";
            DeleteToolStripMenuItem.Size = new Size(183, 22);
            DeleteToolStripMenuItem.Text = "Delete";
            // 
            // SetAsPassFailToolStripMenuItem
            // 
            SetAsPassFailToolStripMenuItem.Name = "SetAsPassFailToolStripMenuItem";
            SetAsPassFailToolStripMenuItem.Size = new Size(183, 22);
            SetAsPassFailToolStripMenuItem.Text = "Set as Pass/Fail";
            // 
            // FieldDeleteMenu
            // 
            FieldDeleteMenu.Items.AddRange(new ToolStripItem[] { DeleteToolStripMenuItem1 });
            FieldDeleteMenu.Name = "titleDeleteMenu";
            FieldDeleteMenu.Size = new Size(108, 26);
            // 
            // DeleteToolStripMenuItem1
            // 
            DeleteToolStripMenuItem1.Name = "DeleteToolStripMenuItem1";
            DeleteToolStripMenuItem1.Size = new Size(107, 22);
            DeleteToolStripMenuItem1.Text = "Delete";
            // 
            // GroupBox6
            // 
            GroupBox6.Controls.Add(_blackWhite);
            GroupBox6.Controls.Add(_colorPrinter);
            GroupBox6.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            GroupBox6.Location = new Point(374, 50);
            GroupBox6.Name = "GroupBox6";
            GroupBox6.Size = new Size(131, 66);
            GroupBox6.TabIndex = 41;
            GroupBox6.TabStop = false;
            GroupBox6.Text = "Printer Type";
            // 
            // blackWhite
            // 
            _blackWhite.AutoSize = true;
            _blackWhite.Location = new Point(6, 43);
            _blackWhite.Name = "_blackWhite";
            _blackWhite.Size = new Size(94, 17);
            _blackWhite.TabIndex = 1;
            _blackWhite.Text = "Black/White";
            _blackWhite.UseVisualStyleBackColor = true;
            // 
            // colorPrinter
            // 
            _colorPrinter.AutoSize = true;
            _colorPrinter.Checked = true;
            _colorPrinter.Location = new Point(6, 20);
            _colorPrinter.Name = "_colorPrinter";
            _colorPrinter.Size = new Size(57, 17);
            _colorPrinter.TabIndex = 0;
            _colorPrinter.TabStop = true;
            _colorPrinter.Text = "Color ";
            _colorPrinter.UseVisualStyleBackColor = true;
            // 
            // _cancelButton
            // 
            __cancelButton.BackColor = Color.Transparent;
            __cancelButton.Font = new Font("Tahoma", 18.0f, FontStyle.Regular, GraphicsUnit.Point, Conversions.ToByte(0));
            __cancelButton.ForeColor = Color.Red;
            __cancelButton.ImageAlign = ContentAlignment.MiddleLeft;
            __cancelButton.Location = new Point(1078, 671);
            __cancelButton.Name = "__cancelButton";
            __cancelButton.Size = new Size(142, 61);
            __cancelButton.TabIndex = 29;
            __cancelButton.Text = "Cancel";
            __cancelButton.UseVisualStyleBackColor = false;
            // 
            // GroupBox3
            // 
            GroupBox3.Controls.Add(Rawdataasrow);
            GroupBox3.Controls.Add(_Rawdataascol);
            GroupBox3.Controls.Add(LinkLabel3);
            GroupBox3.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            GroupBox3.Location = new Point(249, 52);
            GroupBox3.Name = "GroupBox3";
            GroupBox3.Size = new Size(119, 66);
            GroupBox3.TabIndex = 42;
            GroupBox3.TabStop = false;
            GroupBox3.Text = "Raw Data Sheet";
            // 
            // Rawdataasrow
            // 
            Rawdataasrow.AutoSize = true;
            Rawdataasrow.Location = new Point(6, 41);
            Rawdataasrow.Name = "Rawdataasrow";
            Rawdataasrow.Size = new Size(79, 17);
            Rawdataasrow.TabIndex = 27;
            Rawdataasrow.Text = "Row Wise";
            Rawdataasrow.UseVisualStyleBackColor = true;
            // 
            // Rawdataascol
            // 
            _Rawdataascol.AutoSize = true;
            _Rawdataascol.Location = new Point(6, 18);
            _Rawdataascol.Name = "_Rawdataascol";
            _Rawdataascol.Size = new Size(97, 17);
            _Rawdataascol.TabIndex = 26;
            _Rawdataascol.Text = "Column Wise";
            _Rawdataascol.UseVisualStyleBackColor = true;
            // 
            // LinkLabel3
            // 
            LinkLabel3.AutoSize = true;
            LinkLabel3.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            LinkLabel3.Location = new Point(90, 106);
            LinkLabel3.Name = "LinkLabel3";
            LinkLabel3.Size = new Size(146, 13);
            LinkLabel3.TabIndex = 22;
            LinkLabel3.TabStop = true;
            LinkLabel3.Text = "More Settings....................";
            // 
            // lblNoDSOFramer
            // 
            lblNoDSOFramer.AutoSize = true;
            lblNoDSOFramer.Font = new Font("Segoe UI Semibold", 9.75f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            lblNoDSOFramer.ForeColor = SystemColors.WindowFrame;
            lblNoDSOFramer.Location = new Point(694, 291);
            lblNoDSOFramer.Name = "lblNoDSOFramer";
            lblNoDSOFramer.Size = new Size(405, 17);
            lblNoDSOFramer.TabIndex = 43;
            lblNoDSOFramer.Text = "A software component to show the excel file could not be loaded";
            lblNoDSOFramer.Visible = false;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(6.0f, 13.0f);
            AutoScaleMode = AutoScaleMode.Font;
            BackColor = SystemColors.GradientInactiveCaption;
            ClientSize = new Size(1222, 737);
            ControlBox = false;
            Controls.Add(lblNoDSOFramer);
            Controls.Add(GroupBox3);
            Controls.Add(_ShowToolBar);
            Controls.Add(_reportPathNameBox);
            Controls.Add(_showExcelMenu);
            Controls.Add(_reportNameBox);
            Controls.Add(_programNameBox);
            Controls.Add(Label5);
            Controls.Add(_reportName);
            Controls.Add(Label2);
            Controls.Add(_programName);
            Controls.Add(_programNamePathBox);
            Controls.Add(_reportStyleBox);
            Controls.Add(GroupBox6);
            Controls.Add(GroupBox2);
            Controls.Add(__cancelButton);
            Controls.Add(_tabControl);
            Controls.Add(panelButtonEnclosure);
            Controls.Add(__saveButton);
            Controls.Add(_pageStyleBox);
            FormBorderStyle = FormBorderStyle.FixedToolWindow;
            Name = "Form1";
            ShowIcon = false;
            SizeGripStyle = SizeGripStyle.Hide;
            Text = "  Report Formatting";
            _reportStyleBox.ResumeLayout(false);
            _reportStyleBox.PerformLayout();
            _tabControl.ResumeLayout(false);
            _fieldSettings.ResumeLayout(false);
            _fieldSettings.PerformLayout();
            panelToleranceType.ResumeLayout(false);
            panelToleranceType.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)__measurementListView).EndInit();
            _programSettings.ResumeLayout(false);
            pnlTitlePredefinedList.ResumeLayout(false);
            pnlTitlePredefinedList.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)__extraInformationDataView).EndInit();
            _settings.ResumeLayout(false);
            GroupBox1.ResumeLayout(false);
            GroupBox1.PerformLayout();
            grpPasswordProtection.ResumeLayout(false);
            grpPasswordProtection.PerformLayout();
            PanelRcadPic.ResumeLayout(false);
            PanelRcadPic.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)nudFormatdigits).EndInit();
            pnlInsertPicture.ResumeLayout(false);
            pnlInsertPicture.PerformLayout();
            _pageStyleBox.ResumeLayout(false);
            _pageStyleBox.PerformLayout();
            GroupBox2.ResumeLayout(false);
            GroupBox2.PerformLayout();
            panelButtonEnclosure.ResumeLayout(false);
            fieldContextMenuStrip.ResumeLayout(false);
            titleDataTypeContextMenuList.ResumeLayout(false);
            convertFieldToTitleList.ResumeLayout(false);
            FieldDeleteMenu.ResumeLayout(false);
            GroupBox6.ResumeLayout(false);
            GroupBox6.PerformLayout();
            GroupBox3.ResumeLayout(false);
            GroupBox3.PerformLayout();
            Load += new EventHandler(Form1_Load);
            ResumeLayout(false);
            PerformLayout();
        }

        internal GroupBox _reportStyleBox;
        private RadioButton __rowWise;

        internal RadioButton _rowWise
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __rowWise;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__rowWise != null)
                {
                    __rowWise.CheckedChanged -= HandlingReportStylesForExcel;
                }

                __rowWise = value;
                if (__rowWise != null)
                {
                    __rowWise.CheckedChanged += HandlingReportStylesForExcel;
                }
            }
        }

        private RadioButton __columnWise;

        internal RadioButton _columnWise
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __columnWise;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__columnWise != null)
                {
                    __columnWise.CheckedChanged -= HandlingReportStylesForExcel;
                }

                __columnWise = value;
                if (__columnWise != null)
                {
                    __columnWise.CheckedChanged += HandlingReportStylesForExcel;
                }
            }
        }

        internal LinkLabel _moreSettingLink;
        private RadioButton __portraitRadioButton;

        internal RadioButton _portraitRadioButton
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __portraitRadioButton;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__portraitRadioButton != null)
                {
                    __portraitRadioButton.CheckedChanged -= HandlingReportStylesForExcel;
                }

                __portraitRadioButton = value;
                if (__portraitRadioButton != null)
                {
                    __portraitRadioButton.CheckedChanged += HandlingReportStylesForExcel;
                }
            }
        }

        private RadioButton __landScapeRadioButton;

        internal RadioButton _landScapeRadioButton
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __landScapeRadioButton;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__landScapeRadioButton != null)
                {
                    __landScapeRadioButton.CheckedChanged -= HandlingReportStylesForExcel;
                }

                __landScapeRadioButton = value;
                if (__landScapeRadioButton != null)
                {
                    __landScapeRadioButton.CheckedChanged += HandlingReportStylesForExcel;
                }
            }
        }

        internal TabControl _tabControl;
        internal TabPage _fieldSettings;
        internal TabPage _programSettings;
        private RadioButton __allowedRange;

        internal RadioButton _allowedRange
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __allowedRange;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__allowedRange != null)
                {
                    __allowedRange.CheckedChanged -= _allowedRange_CheckedChanged;
                }

                __allowedRange = value;
                if (__allowedRange != null)
                {
                    __allowedRange.CheckedChanged += _allowedRange_CheckedChanged;
                }
            }
        }

        private RadioButton __nomialPlusMinus;

        internal RadioButton _nomialPlusMinus
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __nomialPlusMinus;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__nomialPlusMinus != null)
                {
                    __nomialPlusMinus.CheckedChanged -= _nomialPlusMinus_CheckedChanged;
                }

                __nomialPlusMinus = value;
                if (__nomialPlusMinus != null)
                {
                    __nomialPlusMinus.CheckedChanged += _nomialPlusMinus_CheckedChanged;
                }
            }
        }

        private RadioButton __nominalMinMax;

        internal RadioButton _nominalMinMax
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __nominalMinMax;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__nominalMinMax != null)
                {
                    __nominalMinMax.CheckedChanged -= _nominalMinMax_CheckedChanged;
                }

                __nominalMinMax = value;
                if (__nominalMinMax != null)
                {
                    __nominalMinMax.CheckedChanged += _nominalMinMax_CheckedChanged;
                }
            }
        }

        private CheckBox __conditionalFormatting1;

        internal CheckBox _conditionalFormatting1
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __conditionalFormatting1;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__conditionalFormatting1 != null)
                {
                    __conditionalFormatting1.CheckedChanged -= ConditionalFormatting_CheckedChanged;
                }

                __conditionalFormatting1 = value;
                if (__conditionalFormatting1 != null)
                {
                    __conditionalFormatting1.CheckedChanged += ConditionalFormatting_CheckedChanged;
                }
            }
        }

        private DataGridView __measurementListView;

        internal DataGridView _measurementListView
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __measurementListView;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__measurementListView != null)
                {
                    //__measurementListView.RowHeaderMouseDoubleClick -= _measurementListView_RowHeaderMouseDoubleClick;
                    __measurementListView.CellContentClick -= _measurementListView_CellContentClick;
                }

                __measurementListView = value;
                if (__measurementListView != null)
                {
                    //__measurementListView.RowHeaderMouseDoubleClick += _measurementListView_RowHeaderMouseDoubleClick;
                    __measurementListView.CellContentClick += _measurementListView_CellContentClick;
                }
            }
        }

        private Button __saveButton;

        internal Button _saveButton
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __saveButton;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__saveButton != null)
                {
                    __saveButton.Click -= _saveButton_Click;
                }

                __saveButton = value;
                if (__saveButton != null)
                {
                    __saveButton.Click += _saveButton_Click;
                }
            }
        }

        internal Label _programName;
        internal TabPage _settings;
        internal GroupBox GroupBox1;
        private Button _btnUpdateRange;

        internal Button btnUpdateRange
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnUpdateRange;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnUpdateRange != null)
                {
                    _btnUpdateRange.Click -= btnUpdateRange_Click;
                }

                _btnUpdateRange = value;
                if (_btnUpdateRange != null)
                {
                    _btnUpdateRange.Click += btnUpdateRange_Click;
                }
            }
        }

        internal Label _templateRangeLabel;
        internal ComboBox _sheetBox;
        internal Label _reportSheets;
        internal TextBox _programNameBox;
        internal TextBox _programNamePathBox;
        internal Label Label2;
        internal Label _reportName;
        internal TextBox _reportNameBox;
        internal TextBox _reportPathNameBox;
        internal Label Label5;
        internal GroupBox _pageStyleBox;
        internal LinkLabel LinkLabel1;
        private DataGridView __extraInformationDataView;

        internal DataGridView _extraInformationDataView
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __extraInformationDataView;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__extraInformationDataView != null)
                {
                    __extraInformationDataView.DoubleClick -= GetSelectedCellInformation;
                    __extraInformationDataView.CellEnter -= Handle_PredefinedList;
                }

                __extraInformationDataView = value;
                if (__extraInformationDataView != null)
                {
                    __extraInformationDataView.DoubleClick += GetSelectedCellInformation;
                    __extraInformationDataView.CellEnter += Handle_PredefinedList;
                }
            }
        }

        internal Panel panelToleranceType;
        internal GroupBox GroupBox2;
        internal LinkLabel LinkLabel2;
        internal Panel panelButtonEnclosure;
        internal ComboBox _reportStyleNameBox;
        internal Label Label4;
        internal ContextMenuStrip fieldContextMenuStrip;
        internal ToolStripMenuItem DateToolStripMenuItem;
        internal ToolStripMenuItem TimeToolStripMenuItem;
        internal ToolStripMenuItem PredefinedListToolStripMenuItem;
        internal ToolStripMenuItem PassFailToolStripMenuItem;
        internal ToolStripMenuItem TextToolStripMenuItem;
        internal ContextMenuStrip titleDataTypeContextMenuList;
        internal ToolStripMenuItem PredefinedListToolStripMenuItem1;
        internal ToolStripMenuItem TextToolStripMenuItem1;
        internal ContextMenuStrip convertFieldToTitleList;
        internal ToolStripMenuItem ConvertFieldAsTitleToolStripMenuItem;
        internal Panel pnlTitlePredefinedList;
        internal Label Label7;
        internal Label Label8;
        private Button _btnSetList;

        internal Button btnSetList
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnSetList;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnSetList != null)
                {
                    _btnSetList.Click -= btnSetList_Click;
                }

                _btnSetList = value;
                if (_btnSetList != null)
                {
                    _btnSetList.Click += btnSetList_Click;
                }
            }
        }

        private Button _Btn_TolerancePanelClose;

        internal Button Btn_TolerancePanelClose
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _Btn_TolerancePanelClose;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_Btn_TolerancePanelClose != null)
                {
                    _Btn_TolerancePanelClose.Click -= Btn_TolerancePanelClose_Click;
                }

                _Btn_TolerancePanelClose = value;
                if (_Btn_TolerancePanelClose != null)
                {
                    _Btn_TolerancePanelClose.Click += Btn_TolerancePanelClose_Click;
                }
            }
        }

        private Button _BtnMoveDown;

        internal Button BtnMoveDown
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _BtnMoveDown;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_BtnMoveDown != null)
                {
                    _BtnMoveDown.Click -= BtnMoveUp_Click;
                }

                _BtnMoveDown = value;
                if (_BtnMoveDown != null)
                {
                    _BtnMoveDown.Click += BtnMoveUp_Click;
                }
            }
        }

        private Button _BtnMoveUp;

        internal Button BtnMoveUp
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _BtnMoveUp;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_BtnMoveUp != null)
                {
                    _BtnMoveUp.Click -= BtnMoveUp_Click;
                }

                _BtnMoveUp = value;
                if (_BtnMoveUp != null)
                {
                    _BtnMoveUp.Click += BtnMoveUp_Click;
                }
            }
        }

        internal ToolStripMenuItem DeleteToolStripMenuItem;
        internal TextBox txtendRow;
        internal TextBox txtEndCol;
        internal Label Label6;
        internal ContextMenuStrip FieldDeleteMenu;
        internal ToolStripMenuItem DeleteToolStripMenuItem1;
        internal ToolStripMenuItem DateToolStripMenuItem1;
        internal ToolStripMenuItem TimeToolStripMenuItem1;
        internal ToolStripMenuItem PassFailToolStripMenuItem1;
        private Button _addNewTitle;

        internal Button addNewTitle
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _addNewTitle;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_addNewTitle != null)
                {
                    //_addNewTitle.Click -= addNewTitle_Click;
                }

                _addNewTitle = value;
                if (_addNewTitle != null)
                {
                    //_addNewTitle.Click += addNewTitle_Click;
                }
            }
        }

        internal Label Label9;
        internal Label columnLabel;
        internal GroupBox grpPasswordProtection;
        internal Label Label10;
        private Button _filePassword;

        internal Button filePassword
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _filePassword;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_filePassword != null)
                {
                    _filePassword.Click -= filePassword_Click;
                }

                _filePassword = value;
                if (_filePassword != null)
                {
                    _filePassword.Click += filePassword_Click;
                }
            }
        }

        internal TextBox passwordBox;
        private CheckBox _ShowToolBar;

        internal CheckBox ShowToolBar
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _ShowToolBar;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_ShowToolBar != null)
                {
                    _ShowToolBar.CheckedChanged -= showExcelMenu_CheckedChanged;
                }

                _ShowToolBar = value;
                if (_ShowToolBar != null)
                {
                    _ShowToolBar.CheckedChanged += showExcelMenu_CheckedChanged;
                }
            }
        }

        private CheckBox _showExcelMenu;

        internal CheckBox showExcelMenu
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _showExcelMenu;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_showExcelMenu != null)
                {
                    _showExcelMenu.CheckedChanged -= showExcelMenu_CheckedChanged;
                }

                _showExcelMenu = value;
                if (_showExcelMenu != null)
                {
                    _showExcelMenu.CheckedChanged += showExcelMenu_CheckedChanged;
                }
            }
        }

        internal ToolStripMenuItem SetAsPassFailToolStripMenuItem;
        internal GroupBox GroupBox6;
        private RadioButton _blackWhite;

        internal RadioButton blackWhite
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _blackWhite;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_blackWhite != null)
                {
                    _blackWhite.CheckedChanged -= PrinterHandling;
                }

                _blackWhite = value;
                if (_blackWhite != null)
                {
                    _blackWhite.CheckedChanged += PrinterHandling;
                }
            }
        }

        private RadioButton _colorPrinter;

        internal RadioButton colorPrinter
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _colorPrinter;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_colorPrinter != null)
                {
                    _colorPrinter.CheckedChanged -= PrinterHandling;
                }

                _colorPrinter = value;
                if (_colorPrinter != null)
                {
                    _colorPrinter.CheckedChanged += PrinterHandling;
                }
            }
        }

        internal Button btnPreview;
        private Button _Button1;

        internal Button Button1
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _Button1;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_Button1 != null)
                {
                    _Button1.Click -= ShowPredefinedList_Click;
                }

                _Button1 = value;
                if (_Button1 != null)
                {
                    _Button1.Click += ShowPredefinedList_Click;
                }
            }
        }

        internal RichTextBox txtPredefinedList;
        internal CheckBox chkWriteprogramNameInReport;
        internal Label filepasslabel;
        internal TextBox txtHeight;
        internal Label lblHeight;
        internal TextBox txtWidth;
        internal Label lblWidth;
        private CheckBox _chkInsertPicture;

        internal CheckBox chkInsertPicture
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _chkInsertPicture;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_chkInsertPicture != null)
                {
                    _chkInsertPicture.CheckedChanged -= chkInsertPicture_CheckedChanged;
                }

                _chkInsertPicture = value;
                if (_chkInsertPicture != null)
                {
                    _chkInsertPicture.CheckedChanged += chkInsertPicture_CheckedChanged;
                }
            }
        }

        internal TextBox txtColumn;
        internal TextBox txtRow;
        internal Label lblTop;
        internal Label lblLeft;
        internal Panel pnlInsertPicture;
        private Button _btnUpdatePicDetails;

        internal Button btnUpdatePicDetails
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnUpdatePicDetails;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnUpdatePicDetails != null)
                {
                    _btnUpdatePicDetails.Click -= btnUpdatePicDetails_Click;
                }

                _btnUpdatePicDetails = value;
                if (_btnUpdatePicDetails != null)
                {
                    _btnUpdatePicDetails.Click += btnUpdatePicDetails_Click;
                }
            }
        }

        internal TextBox txtInitialColumn;
        internal Label Label1;
        internal TextBox txtInitialRow;
        internal Label Label3;
        internal TextBox txtMaxRow;
        internal CheckBox chkMakeSettingsDefault;
        internal TextBox txtMaxColumn;
        private Button __cancelButton;

        internal Button _cancelButton
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __cancelButton;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__cancelButton != null)
                {
                    __cancelButton.Click -= _cancelButton_Click;
                }

                __cancelButton = value;
                if (__cancelButton != null)
                {
                    __cancelButton.Click += _cancelButton_Click;
                }
            }
        }

        internal Label Label15;
        internal Label Label14;
        internal NumericUpDown nudFormatdigits;
        internal Label Label16;
        internal CheckBox chkEarlierMadeReportsAlso;
        internal Label Label17;
        internal Label Label18;
        private CheckBox _ChkShwFieldNum;

        internal CheckBox ChkShwFieldNum
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _ChkShwFieldNum;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_ChkShwFieldNum != null)
                {
                    _ChkShwFieldNum.CheckedChanged -= ChkShwFieldNum_CheckedChanged;
                }

                _ChkShwFieldNum = value;
                if (_ChkShwFieldNum != null)
                {
                    _ChkShwFieldNum.CheckedChanged += ChkShwFieldNum_CheckedChanged;
                }
            }
        }

        private CheckBox _DetailSelect;

        internal CheckBox DetailSelect
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _DetailSelect;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_DetailSelect != null)
                {
                    _DetailSelect.CheckedChanged -= DetailSelect_CheckedChanged;
                }

                _DetailSelect = value;
                if (_DetailSelect != null)
                {
                    _DetailSelect.CheckedChanged += DetailSelect_CheckedChanged;
                }
            }
        }

        private CheckBox _ProgramInNewSheet;

        internal CheckBox ProgramInNewSheet
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _ProgramInNewSheet;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_ProgramInNewSheet != null)
                {
                    _ProgramInNewSheet.CheckedChanged -= ProgramInNewSheet_CheckedChanged;
                }

                _ProgramInNewSheet = value;
                if (_ProgramInNewSheet != null)
                {
                    _ProgramInNewSheet.CheckedChanged += ProgramInNewSheet_CheckedChanged;
                }
            }
        }

        internal Panel PanelRcadPic;
        private Button _UpdateRcadPic;

        internal Button UpdateRcadPic
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _UpdateRcadPic;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_UpdateRcadPic != null)
                {
                    _UpdateRcadPic.Click -= UpdateRcadPic_Click;
                }

                _UpdateRcadPic = value;
                if (_UpdateRcadPic != null)
                {
                    _UpdateRcadPic.Click += UpdateRcadPic_Click;
                }
            }
        }

        internal TextBox RcadCol;
        internal TextBox Rcadwidth;
        internal Label Label19;
        internal Label Label20;
        internal Label Label21;
        internal TextBox RcadRow;
        internal TextBox Rcadheight;
        internal Label Label22;
        internal Label Label24;
        internal Label Label23;
        private CheckBox _CheckBox1;

        internal CheckBox CheckBox1
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _CheckBox1;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_CheckBox1 != null)
                {
                    _CheckBox1.CheckedChanged -= CheckBox1_CheckedChanged;
                }

                _CheckBox1 = value;
                if (_CheckBox1 != null)
                {
                    _CheckBox1.CheckedChanged += CheckBox1_CheckedChanged;
                }
            }
        }

        internal GroupBox GroupBox3;
        internal RadioButton Rawdataasrow;
        private RadioButton _Rawdataascol;

        internal RadioButton Rawdataascol
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _Rawdataascol;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_Rawdataascol != null)
                {
                    _Rawdataascol.CheckedChanged -= RadioButton3_CheckedChanged;
                }

                _Rawdataascol = value;
                if (_Rawdataascol != null)
                {
                    _Rawdataascol.CheckedChanged += RadioButton3_CheckedChanged;
                }
            }
        }

        internal LinkLabel LinkLabel3;
        internal CheckBox cbPaginateReport;
        internal Label Label25;
        internal Label Label11;
        internal Label lblNoDSOFramer;
        internal Label Label27;
        internal Label Label28;
        internal TextBox txtPartName_Label_Row;
        internal TextBox txtPartName_Label_Col;
        internal Label Label26;
        internal TextBox txtPartName_Row;
        internal TextBox txtPartName_Col;
        internal Label Label12;
    }
}