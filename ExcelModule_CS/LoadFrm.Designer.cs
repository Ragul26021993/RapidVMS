using System;
using System.Diagnostics;
using System.Drawing;
using System.Runtime.CompilerServices;
using System.Windows.Forms;
using Microsoft.VisualBasic.CompilerServices;

namespace rrm
{
    [DesignerGenerated()]
    public partial class LoadFrm : Form
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
            var resources = new System.ComponentModel.ComponentResourceManager(typeof(LoadFrm));
            var DataGridViewCellStyle1 = new DataGridViewCellStyle();
            __okButt = new Button();
            __okButt.Click += new EventHandler(_okButt_Click);
            _componentLabel = new Label();
            _reportLabel = new Label();
            __cancelButton = new Button();
            __cancelButton.Click += new EventHandler(_cancelButton_Click);
            _chooseProgram = new Button();
            _chooseProgram.Click += new EventHandler(chooseProgram_Click);
            _ChooseReport = new Button();
            _ChooseReport.Click += new EventHandler(ChooseReport_Click);
            _ReportNamelabel = new Label();
            _btnDeleteProgram = new Button();
            _btnDeleteProgram.Click += new EventHandler(btnDeleteProgram_Click);
            _btnRename = new Button();
            _btnRename.Click += new EventHandler(Rename_Click);
            __reportStyleBox = new ListBox();
            __reportStyleBox.SelectedIndexChanged += new EventHandler(_reportStyleBox_SelectedIndexChanged);
            __ReportBox = new ListBox();
            __ReportBox.SelectedIndexChanged += new EventHandler(Handle_SelectedReport);
            _btnMakeNewReport = new Button();
            _btnMakeNewReport.Click += new EventHandler(ShowNewReportGroupBox);
            grpMakeNewReport = new GroupBox();
            _btnClose = new Button();
            _btnClose.Click += new EventHandler(btnClose_Click);
            _btnSet = new Button();
            _btnSet.Click += new EventHandler(btnSet_Click);
            txtNewReportName = new TextBox();
            TitleName = new DataGridViewTextBoxColumn();
            TitleValue = new DataGridViewTextBoxColumn();
            _txtPgInitials = new TextBox();
            _txtPgInitials.TextChanged += new EventHandler(txtPgInitials_TextChanged);
            Label1 = new Label();
            _rdbShowDateWise = new RadioButton();
            _rdbShowDateWise.CheckedChanged += new EventHandler(ProgarmSort_CheckedChanged);
            _rdbShowNameWise = new RadioButton();
            _rdbShowNameWise.CheckedChanged += new EventHandler(ProgarmSort_CheckedChanged);
            dgPrograms = new DataGridView();
            grpMakeNewReport.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)dgPrograms).BeginInit();
            SuspendLayout();
            // 
            // _okButt
            // 
            __okButt.BackColor = Color.PaleGreen;
            __okButt.Enabled = false;
            __okButt.Font = new Font("Tahoma", 18.0f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            __okButt.Location = new Point(3, 621);
            __okButt.Name = "__okButt";
            __okButt.Size = new Size(200, 52);
            __okButt.TabIndex = 0;
            __okButt.Text = "Load";
            __okButt.UseVisualStyleBackColor = false;
            __okButt.Visible = false;
            // 
            // _componentLabel
            // 
            _componentLabel.AutoSize = true;
            _componentLabel.Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _componentLabel.Location = new Point(3, 7);
            _componentLabel.Name = "_componentLabel";
            _componentLabel.Size = new Size(62, 13);
            _componentLabel.TabIndex = 1;
            _componentLabel.Text = "Program :";
            // 
            // _reportLabel
            // 
            _reportLabel.AutoSize = true;
            _reportLabel.Location = new Point(3, 333);
            _reportLabel.Name = "_reportLabel";
            _reportLabel.Size = new Size(84, 13);
            _reportLabel.TabIndex = 3;
            _reportLabel.Text = "Report Style :";
            _reportLabel.Visible = false;
            // 
            // _cancelButton
            // 
            __cancelButton.BackColor = SystemColors.ButtonHighlight;
            __cancelButton.Font = new Font("Trebuchet MS", 12.0f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            __cancelButton.ForeColor = Color.Red;
            __cancelButton.Location = new Point(697, 635);
            __cancelButton.Name = "__cancelButton";
            __cancelButton.Size = new Size(125, 35);
            __cancelButton.TabIndex = 5;
            __cancelButton.Text = "Cancel";
            __cancelButton.UseVisualStyleBackColor = false;
            // 
            // chooseProgram
            // 
            _chooseProgram.Location = new Point(748, 263);
            _chooseProgram.Name = "_chooseProgram";
            _chooseProgram.Size = new Size(74, 37);
            _chooseProgram.TabIndex = 6;
            _chooseProgram.Text = "...";
            _chooseProgram.TextAlign = ContentAlignment.BottomCenter;
            _chooseProgram.UseVisualStyleBackColor = true;
            // 
            // ChooseReport
            // 
            _ChooseReport.Location = new Point(748, 494);
            _ChooseReport.Name = "_ChooseReport";
            _ChooseReport.Size = new Size(74, 31);
            _ChooseReport.TabIndex = 8;
            _ChooseReport.Text = "...";
            _ChooseReport.UseVisualStyleBackColor = true;
            _ChooseReport.Visible = false;
            // 
            // _ReportNamelabel
            // 
            _ReportNamelabel.AutoSize = true;
            _ReportNamelabel.Location = new Point(6, 392);
            _ReportNamelabel.Name = "_ReportNamelabel";
            _ReportNamelabel.Size = new Size(52, 13);
            _ReportNamelabel.TabIndex = 10;
            _ReportNamelabel.Text = "Reports";
            _ReportNamelabel.Visible = false;
            // 
            // btnDeleteProgram
            // 
            _btnDeleteProgram.BackColor = SystemColors.ButtonHighlight;
            _btnDeleteProgram.Font = new Font("Trebuchet MS", 10.0f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _btnDeleteProgram.ForeColor = Color.Red;
            _btnDeleteProgram.Location = new Point(407, 635);
            _btnDeleteProgram.Name = "_btnDeleteProgram";
            _btnDeleteProgram.Size = new Size(70, 35);
            _btnDeleteProgram.TabIndex = 11;
            _btnDeleteProgram.Text = "Delete";
            _btnDeleteProgram.UseVisualStyleBackColor = false;
            _btnDeleteProgram.Visible = false;
            // 
            // btnRename
            // 
            _btnRename.BackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(128)), Conversions.ToInteger(Conversions.ToByte(255)), Conversions.ToInteger(Conversions.ToByte(128)));
            _btnRename.Font = new Font("Trebuchet MS", 10.0f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            _btnRename.ForeColor = Color.Black;
            _btnRename.Location = new Point(274, 635);
            _btnRename.Name = "_btnRename";
            _btnRename.Size = new Size(72, 35);
            _btnRename.TabIndex = 12;
            _btnRename.Text = "Rename";
            _btnRename.UseVisualStyleBackColor = false;
            _btnRename.Visible = false;
            // 
            // _reportStyleBox
            // 
            __reportStyleBox.FormattingEnabled = true;
            __reportStyleBox.Location = new Point(5, 349);
            __reportStyleBox.Name = "__reportStyleBox";
            __reportStyleBox.Size = new Size(736, 43);
            __reportStyleBox.TabIndex = 13;
            // 
            // _ReportBox
            // 
            __ReportBox.FormattingEnabled = true;
            __ReportBox.Location = new Point(6, 404);
            __ReportBox.Name = "__ReportBox";
            __ReportBox.Size = new Size(736, 121);
            __ReportBox.TabIndex = 14;
            // 
            // btnMakeNewReport
            // 
            _btnMakeNewReport.Location = new Point(748, 404);
            _btnMakeNewReport.Name = "_btnMakeNewReport";
            _btnMakeNewReport.Size = new Size(74, 31);
            _btnMakeNewReport.TabIndex = 15;
            _btnMakeNewReport.Text = "New";
            _btnMakeNewReport.UseVisualStyleBackColor = true;
            // 
            // grpMakeNewReport
            // 
            grpMakeNewReport.Controls.Add(_btnClose);
            grpMakeNewReport.Controls.Add(_btnSet);
            grpMakeNewReport.Controls.Add(txtNewReportName);
            grpMakeNewReport.Location = new Point(6, 404);
            grpMakeNewReport.Name = "grpMakeNewReport";
            grpMakeNewReport.Size = new Size(736, 121);
            grpMakeNewReport.TabIndex = 16;
            grpMakeNewReport.TabStop = false;
            grpMakeNewReport.Text = "Enter new  Report Name";
            grpMakeNewReport.Visible = false;
            // 
            // btnClose
            // 
            _btnClose.BackColor = Color.WhiteSmoke;
            _btnClose.Image = (Image)resources.GetObject("btnClose.Image");
            _btnClose.Location = new Point(684, 53);
            _btnClose.Name = "_btnClose";
            _btnClose.Size = new Size(43, 24);
            _btnClose.TabIndex = 2;
            _btnClose.UseVisualStyleBackColor = false;
            // 
            // btnSet
            // 
            _btnSet.BackColor = Color.SpringGreen;
            _btnSet.Location = new Point(639, 53);
            _btnSet.Name = "_btnSet";
            _btnSet.Size = new Size(39, 24);
            _btnSet.TabIndex = 1;
            _btnSet.Text = "OK";
            _btnSet.UseVisualStyleBackColor = false;
            // 
            // txtNewReportName
            // 
            txtNewReportName.Location = new Point(6, 53);
            txtNewReportName.Name = "txtNewReportName";
            txtNewReportName.Size = new Size(627, 21);
            txtNewReportName.TabIndex = 0;
            // 
            // TitleName
            // 
            TitleName.HeaderText = "Title Name";
            TitleName.Name = "TitleName";
            TitleName.ReadOnly = true;
            // 
            // TitleValue
            // 
            TitleValue.HeaderText = "Title Value";
            TitleValue.Name = "TitleValue";
            // 
            // txtPgInitials
            // 
            _txtPgInitials.Location = new Point(65, 1);
            _txtPgInitials.Name = "_txtPgInitials";
            _txtPgInitials.Size = new Size(100, 21);
            _txtPgInitials.TabIndex = 18;
            // 
            // Label1
            // 
            Label1.AutoSize = true;
            Label1.Location = new Point(171, 7);
            Label1.Name = "Label1";
            Label1.Size = new Size(214, 13);
            Label1.TabIndex = 19;
            Label1.Text = "Enter Program Name for quick acess.";
            // 
            // rdbShowDateWise
            // 
            _rdbShowDateWise.AutoSize = true;
            _rdbShowDateWise.Location = new Point(3, 307);
            _rdbShowDateWise.Name = "_rdbShowDateWise";
            _rdbShowDateWise.Size = new Size(115, 17);
            _rdbShowDateWise.TabIndex = 20;
            _rdbShowDateWise.TabStop = true;
            _rdbShowDateWise.Text = "Show Date Wise";
            _rdbShowDateWise.UseVisualStyleBackColor = true;
            // 
            // rdbShowNameWise
            // 
            _rdbShowNameWise.AutoSize = true;
            _rdbShowNameWise.Location = new Point(136, 307);
            _rdbShowNameWise.Name = "_rdbShowNameWise";
            _rdbShowNameWise.Size = new Size(120, 17);
            _rdbShowNameWise.TabIndex = 21;
            _rdbShowNameWise.TabStop = true;
            _rdbShowNameWise.Text = "Show Name Wise";
            _rdbShowNameWise.UseVisualStyleBackColor = true;
            // 
            // dgPrograms
            // 
            dgPrograms.AllowUserToAddRows = false;
            dgPrograms.AllowUserToDeleteRows = false;
            dgPrograms.AllowUserToResizeColumns = false;
            DataGridViewCellStyle1.BackColor = Color.FromArgb(Conversions.ToInteger(Conversions.ToByte(255)), Conversions.ToInteger(Conversions.ToByte(255)), Conversions.ToInteger(Conversions.ToByte(192)));
            DataGridViewCellStyle1.ForeColor = Color.Black;
            DataGridViewCellStyle1.SelectionBackColor = Color.Blue;
            DataGridViewCellStyle1.SelectionForeColor = Color.White;
            dgPrograms.AlternatingRowsDefaultCellStyle = DataGridViewCellStyle1;
            dgPrograms.CellBorderStyle = DataGridViewCellBorderStyle.Sunken;
            dgPrograms.ClipboardCopyMode = DataGridViewClipboardCopyMode.Disable;
            dgPrograms.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            dgPrograms.EditMode = DataGridViewEditMode.EditProgrammatically;
            dgPrograms.Location = new Point(6, 28);
            dgPrograms.MultiSelect = false;
            dgPrograms.Name = "dgPrograms";
            dgPrograms.ReadOnly = true;
            dgPrograms.RowHeadersVisible = false;
            dgPrograms.ScrollBars = ScrollBars.Vertical;
            dgPrograms.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
            dgPrograms.Size = new Size(736, 273);
            dgPrograms.TabIndex = 24;
            // 
            // LoadFrm
            // 
            AutoScaleDimensions = new SizeF(7.0f, 13.0f);
            AutoScaleMode = AutoScaleMode.Font;
            BackColor = SystemColors.GradientInactiveCaption;
            ClientSize = new Size(834, 674);
            ControlBox = false;
            Controls.Add(dgPrograms);
            Controls.Add(_rdbShowNameWise);
            Controls.Add(_rdbShowDateWise);
            Controls.Add(Label1);
            Controls.Add(_txtPgInitials);
            Controls.Add(grpMakeNewReport);
            Controls.Add(_btnMakeNewReport);
            Controls.Add(__ReportBox);
            Controls.Add(__reportStyleBox);
            Controls.Add(_btnRename);
            Controls.Add(_btnDeleteProgram);
            Controls.Add(_ReportNamelabel);
            Controls.Add(_ChooseReport);
            Controls.Add(_chooseProgram);
            Controls.Add(__cancelButton);
            Controls.Add(_reportLabel);
            Controls.Add(_componentLabel);
            Controls.Add(__okButt);
            Font = new Font("Tahoma", 8.25f, FontStyle.Bold, GraphicsUnit.Point, Conversions.ToByte(0));
            FormBorderStyle = FormBorderStyle.FixedToolWindow;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "LoadFrm";
            ShowIcon = false;
            ShowInTaskbar = false;
            StartPosition = FormStartPosition.CenterScreen;
            Text = "  Program & Report";
            grpMakeNewReport.ResumeLayout(false);
            grpMakeNewReport.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)dgPrograms).EndInit();
            Load += new EventHandler(LoadFrm_Load);
            ResumeLayout(false);
            PerformLayout();
        }

        private Button __okButt;

        internal Button _okButt
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __okButt;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__okButt != null)
                {
                    __okButt.Click -= _okButt_Click;
                }

                __okButt = value;
                if (__okButt != null)
                {
                    __okButt.Click += _okButt_Click;
                }
            }
        }

        internal Label _componentLabel;
        internal Label _reportLabel;
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

        private Button _chooseProgram;

        internal Button chooseProgram
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _chooseProgram;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_chooseProgram != null)
                {
                    _chooseProgram.Click -= chooseProgram_Click;
                }

                _chooseProgram = value;
                if (_chooseProgram != null)
                {
                    _chooseProgram.Click += chooseProgram_Click;
                }
            }
        }

        private Button _ChooseReport;

        internal Button ChooseReport
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _ChooseReport;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_ChooseReport != null)
                {
                    _ChooseReport.Click -= ChooseReport_Click;
                }

                _ChooseReport = value;
                if (_ChooseReport != null)
                {
                    _ChooseReport.Click += ChooseReport_Click;
                }
            }
        }

        internal Label _ReportNamelabel;
        private Button _btnDeleteProgram;

        internal Button btnDeleteProgram
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnDeleteProgram;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnDeleteProgram != null)
                {
                    _btnDeleteProgram.Click -= btnDeleteProgram_Click;
                }

                _btnDeleteProgram = value;
                if (_btnDeleteProgram != null)
                {
                    _btnDeleteProgram.Click += btnDeleteProgram_Click;
                }
            }
        }

        private Button _btnRename;

        internal Button btnRename
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnRename;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnRename != null)
                {
                    _btnRename.Click -= Rename_Click;
                }

                _btnRename = value;
                if (_btnRename != null)
                {
                    _btnRename.Click += Rename_Click;
                }
            }
        }

        private ListBox __reportStyleBox;

        internal ListBox _reportStyleBox
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __reportStyleBox;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__reportStyleBox != null)
                {
                    __reportStyleBox.SelectedIndexChanged -= _reportStyleBox_SelectedIndexChanged;
                }

                __reportStyleBox = value;
                if (__reportStyleBox != null)
                {
                    __reportStyleBox.SelectedIndexChanged += _reportStyleBox_SelectedIndexChanged;
                }
            }
        }

        private ListBox __ReportBox;

        internal ListBox _ReportBox
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return __ReportBox;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (__ReportBox != null)
                {
                    __ReportBox.SelectedIndexChanged -= Handle_SelectedReport;
                }

                __ReportBox = value;
                if (__ReportBox != null)
                {
                    __ReportBox.SelectedIndexChanged += Handle_SelectedReport;
                }
            }
        }

        private Button _btnMakeNewReport;

        internal Button btnMakeNewReport
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnMakeNewReport;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnMakeNewReport != null)
                {
                    _btnMakeNewReport.Click -= ShowNewReportGroupBox;
                }

                _btnMakeNewReport = value;
                if (_btnMakeNewReport != null)
                {
                    _btnMakeNewReport.Click += ShowNewReportGroupBox;
                }
            }
        }

        internal GroupBox grpMakeNewReport;
        private Button _btnClose;

        internal Button btnClose
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnClose;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnClose != null)
                {
                    _btnClose.Click -= btnClose_Click;
                }

                _btnClose = value;
                if (_btnClose != null)
                {
                    _btnClose.Click += btnClose_Click;
                }
            }
        }

        private Button _btnSet;

        internal Button btnSet
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _btnSet;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_btnSet != null)
                {
                    _btnSet.Click -= btnSet_Click;
                }

                _btnSet = value;
                if (_btnSet != null)
                {
                    _btnSet.Click += btnSet_Click;
                }
            }
        }

        internal TextBox txtNewReportName;
        internal DataGridViewTextBoxColumn TitleName;
        internal DataGridViewTextBoxColumn TitleValue;
        private TextBox _txtPgInitials;

        internal TextBox txtPgInitials
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _txtPgInitials;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_txtPgInitials != null)
                {
                    _txtPgInitials.TextChanged -= txtPgInitials_TextChanged;
                }

                _txtPgInitials = value;
                if (_txtPgInitials != null)
                {
                    _txtPgInitials.TextChanged += txtPgInitials_TextChanged;
                }
            }
        }

        internal Label Label1;
        private RadioButton _rdbShowDateWise;

        internal RadioButton rdbShowDateWise
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _rdbShowDateWise;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_rdbShowDateWise != null)
                {
                    _rdbShowDateWise.CheckedChanged -= ProgarmSort_CheckedChanged;
                }

                _rdbShowDateWise = value;
                if (_rdbShowDateWise != null)
                {
                    _rdbShowDateWise.CheckedChanged += ProgarmSort_CheckedChanged;
                }
            }
        }

        private RadioButton _rdbShowNameWise;

        internal RadioButton rdbShowNameWise
        {
            [MethodImpl(MethodImplOptions.Synchronized)]
            get
            {
                return _rdbShowNameWise;
            }

            [MethodImpl(MethodImplOptions.Synchronized)]
            set
            {
                if (_rdbShowNameWise != null)
                {
                    _rdbShowNameWise.CheckedChanged -= ProgarmSort_CheckedChanged;
                }

                _rdbShowNameWise = value;
                if (_rdbShowNameWise != null)
                {
                    _rdbShowNameWise.CheckedChanged += ProgarmSort_CheckedChanged;
                }
            }
        }

        internal DataGridView dgPrograms;
    }
}