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
using System.ComponentModel;
using System.Collections.ObjectModel;
using Rapid.IO;
using Rapid.Interface;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for CalibrationWin.xaml
    /// </summary>
    public partial class FocusCalibrationWin : Window, INotifyPropertyChanged
    {
        //double dx = 0, dy = 0;
        public FocusCalibrationWin()
        {
            this.InitializeComponent();
            TextBox[] txtb = { RowHeightTxtBx, NumOfRowsTxtBx, NumOfColumnsTxtBx, ColWidthTxtBx, GapTxtBx, SpanTxtBx };
            foreach (TextBox tb in txtb)
            {
                Validation.AddErrorHandler(tb, Textbox_Errorhandler);
            }
            ReadParameters();
            MaxAllowedCellsTxtbx.Text += GlobalSettings.LimitRowColumProduct.ToString();
            // Insert code required on object creation below this point.
        }

        public event PropertyChangedEventHandler PropertyChanged;
        void Notify(string PropName)
        {
            if (PropName != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        int _NumberofColumns = 20;
        public int NumberofColumns
        {
            get
            {
                return _NumberofColumns;
            }
            set
            {
                //if (value < 20)
                //{
                //    value = 20;
                //}
                //if (value > 800)
                //{
                //    value = 800;
                //}
                if (value <= 0)
                {
                    MaxAllowedRow = GlobalSettings.LimitRowColumProduct;
                    _NumberofColumns = 0;
                }
                else
                {
                    _NumberofColumns = value;
                    MaxAllowedRow = (int)(GlobalSettings.LimitRowColumProduct / value);
                }
                Notify("NumberofColumns");
            }
        }

        int _NumberOfRows = 15;
        public int NumberOfRows
        {
            get
            {
                return _NumberOfRows;
            }
            set
            {
                //if (value < 15)
                //{
                //    value = 15;
                //}
                //if (value > 800)
                //{
                //    value = 800;
                //}
                if (value <= 0)
                {
                    MaxAllowedColumn = GlobalSettings.LimitRowColumProduct;
                    _NumberOfRows = 0;
                }
                else
                {
                    _NumberOfRows = value;
                    MaxAllowedColumn = (int)(GlobalSettings.LimitRowColumProduct / value);
                }
                Notify("NumberOfRows");
            }
        }

        int _MaxAllowedRow = 1000;
        public int MaxAllowedRow
        {
            get
            {
                return _MaxAllowedRow;
            }
            set
            {
                if (value < NumberOfRows)
                {
                    NumberOfRows = value;
                }
                _MaxAllowedRow = value;
                Notify("MaxAllowedRow");
            }
        }

        int _MaxAllowedColumn = 1000;
        public int MaxAllowedColumn
        {
            get
            {
                return _MaxAllowedColumn;
            }
            set
            {
                if (value < NumberofColumns)
                {
                    NumberofColumns = value;
                }
                _MaxAllowedColumn = value;
                Notify("MaxAllowedColumn");
            }
        }

        int _Gap = 40;
        public int Gap
        {
            get
            {
                return _Gap;
            }
            set
            {
                if (value <= 0)
                {
                    value = 1;
                }
                if (value > 400)
                {
                    value = 400;
                }
                _Gap = value;
                Notify("Gap");
            }
        }

        double _Span = 1;
        public double Span
        {
            get
            {
                return _Span;
            }
            set
            {
                if (value < 0.001 || value > 400)
                {
                    value = 0.001;
                }
                _Span = value;
                Notify("Span");
            }
        }

        int _RowHeight = 40;
        public int RowHeight
        {
            get
            {
                return _RowHeight;
            }
            set
            {
                if (value < 20)
                {
                    value = 20;
                }
                if (value > 200)
                {
                    value = 200;
                }
                _RowHeight = value;
                Notify("RowHeight");
            }
        }

        int _ColumnWidth = 40;
        public int ColumnWidth
        {
            get
            {
                return _ColumnWidth;
            }
            set
            {
                if (value < 20)
                {
                    value = 20;
                }
                if (value > 200)
                {
                    value = 200;
                }
                _ColumnWidth = value;
                Notify("ColumnWidth");
            }
        }

        void CalibrateBtn_Click(object sender, RoutedEventArgs e)
        {
            InterpretTxtBxVals(false);
        }
        void CalibrateDeviationBtn_Click(object sender, RoutedEventArgs e)
        {
            InterpretTxtBxVals(true);
        }
        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        void InterpretTxtBxVals(bool FocusDeviationCalc)
        {
            double dvalue1 = 0, dvalue2 = 0, dvalue3 = 0, valueZ = 0;
            //if (info.txtValue.Text.Length == 0 || info.txtValue2.Text.Length == 0) { return; }
            if (MoveInXTxtBx.Text.Length > 0)
                dvalue1 = Convert.ToDouble(MoveInXTxtBx.Text);
            else dvalue1 = 0;
            if (MoveInYTxtBx.Text.Length > 0)
                dvalue2 = Convert.ToDouble(MoveInYTxtBx.Text);
            else dvalue2 = 0;
            if (SpeedTxtBx.Text.Length > 0)
                dvalue3 = Convert.ToDouble(SpeedTxtBx.Text);
            else dvalue3 = 0;
            if (MoveInZTxtBx.Text.Length > 0)
                valueZ = Convert.ToDouble(MoveInZTxtBx.Text);
            else valueZ = 0;
            RWrapper.RW_FocusDepth.MYINSTANCE().DoFocusCalibration(dvalue1, dvalue2, valueZ, NumberOfRows, NumberofColumns, Gap, ColumnWidth, RowHeight, Span, dvalue3);
        }
        void XCorrectionBtn_Click(object sender, RoutedEventArgs e)
        {
            XYCorrection correction = new XYCorrection();
            correction.Show();
            this.Close();
        }
        void Textbox_Errorhandler(object sender, ValidationErrorEventArgs e)
        {
            TextBox _this = (sender as TextBox);
            object obj = ((TextBox)sender).CommandBindings;
            _this.ToolTip = (string)e.Error.ErrorContent;
        }
        protected override void OnClosing(CancelEventArgs e)
        {
            SaveParameters();
        }

        #region To save the Windows Parameters
        public string TextFilePath { get; set; }
        public void ReadParameters()
        {
            try
            {
                if (GlobalSettings.SaveWindowsParameterFolderPath != "" && GlobalSettings.SaveWindowsParameterFolderPath != null)
                {
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\FocusCalibrationWin.text";
                    if (System.IO.File.Exists(TextFilePath))
                    {
                        List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                        ListOfParameters.Add(new WindowsParameters<string>("MoveInX", MoveInXTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MoveInY", MoveInYTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("MoveInZ", MoveInZTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Speed", SpeedTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Gap", GapTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NumOfRows", NumOfRowsTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("NumOfColumns", NumOfColumnsTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("RowHeight", RowHeightTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("ColWidth", ColWidthTxtBx.Text.ToString()));
                        ListOfParameters.Add(new WindowsParameters<string>("Span", SpanTxtBx.Text.ToString()));

                        ReadWriteText.ReadFileWhenWindowOpens(TextFilePath, ref ListOfParameters);
                        if (ListOfParameters.Count > 0)
                        {
                            for (int i = 0; i < ListOfParameters.Count; i++)
                            {
                                if (ListOfParameters[i].Name == "MoveInX")
                                {
                                    MoveInXTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "MoveInY")
                                {
                                    MoveInYTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "MoveInZ")
                                {
                                    MoveInZTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "Speed")
                                {
                                    SpeedTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "Gap")
                                {
                                    GapTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "NumOfRows")
                                {
                                    NumOfRowsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "NumOfColumns")
                                {
                                    NumOfColumnsTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "RowHeight")
                                {
                                    RowHeightTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "ColWidth")
                                {
                                    ColWidthTxtBx.Text = ListOfParameters[i].Value.ToString();
                                }
                                if (ListOfParameters[i].Name == "Span")
                                {
                                    SpanTxtBx.Text = ListOfParameters[i].Value.ToString();
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
                    TextFilePath = GlobalSettings.SaveWindowsParameterFolderPath + "\\FocusCalibrationWin.text";
                    List<WindowsParameters<string>> ListOfParameters = new List<WindowsParameters<string>>();
                    ListOfParameters.Add(new WindowsParameters<string>("MoveInX", MoveInXTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MoveInY", MoveInYTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("MoveInZ", MoveInZTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Speed", SpeedTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Gap", GapTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NumOfRows", NumOfRowsTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("NumOfColumns", NumOfColumnsTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("RowHeight", RowHeightTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("ColWidth", ColWidthTxtBx.Text.ToString()));
                    ListOfParameters.Add(new WindowsParameters<string>("Span", SpanTxtBx.Text.ToString()));
                    ReadWriteText.WriteFileWhenWindowClose(TextFilePath, ListOfParameters);
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