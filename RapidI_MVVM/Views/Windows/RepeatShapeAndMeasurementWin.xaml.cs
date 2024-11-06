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
using Rapid.Utilities;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for RepeatShapeAndMeasurementWin.xaml
    /// </summary>
    public partial class RepeatShapeAndMeasurementWin : Window
    {
        //double CellWidth, CellHeight, CellGap;

        bool InRunningProcess = false;

        List<double> Coordinade = new List<double>();

        private static RepeatShapeAndMeasurementWin _MyInstance;
        public static RepeatShapeAndMeasurementWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new RepeatShapeAndMeasurementWin();
            }
            return _MyInstance;
        }

        public RepeatShapeAndMeasurementWin()
        {
            InitializeComponent();
            init();
        }

        void init()
        {
            try
            {
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().ShapeMeasureEvent+=new RWrapper.RW_RepeatShapeMeasure.RepeatShapeMeasureEventHandler(RepeatShapeAndMeasurementWin_ShapeMeasureEvent);
            }
            catch (Exception)
            {
                throw;
            }
        }

        void RepeatShapeAndMeasurementWin_ShapeMeasureEvent(int status)
        {
            if (status == 0)
            {
                //RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().StartRecording(false);
                StartRecordingBtn.IsChecked = false;
            }
            else if (status == 1)
            {
                EnableDisableControls(true);
            }
            else if (status == 2)
            {
                SetOrientationBtn.IsChecked = true;
            }   
        }

        void generateList(ref List<double> CoordinateList)
        {
            try
            {
                CoordinateList.Clear();
                RapidGrid Grd = rapidGrid;
                int rowsCount = rapidGrid.TotalRows, columnscount = rapidGrid.TotalColumns;
                if (rowsCount != 0 && columnscount != 0)
                {
                    for (int i = 0; i < this.rapidGrid.Children.Count; i++)
                    {
                        System.Windows.Shapes.Rectangle R = (System.Windows.Shapes.Rectangle)this.rapidGrid.Children[i];
                        RectTagproperty CuttentRectangle = (RectTagproperty)R.Tag;
                        Point pp = CuttentRectangle.position;
                        if (CuttentRectangle.isSelected)
                        {
                            int ival = (int)pp.X, jval = (int)pp.Y;
                            double Xposition = jval * CellWidthNumVal.Value, Yposition = ival * CellHeightNumVal.Value;
                            //if (jval > 0)
                            //{
                            //    Xposition += jval * GapNumVal.Value;
                            //}
                            //if (ival > 0)
                            //{
                            //    Yposition += ival * GapNumVal.Value;
                            //}
                            CoordinateList.Add(Xposition);
                            CoordinateList.Add(-Yposition);
                            CoordinateList.Add(0);
                        }
                    }
                }

            }
            catch (Exception)
            {
                throw;
            }
        }

        void EnableDisableControls(bool enable)
        {
            rapidGrid.IsEnabled = enable;
            RowsNumVal.IsEnabled = enable;
            ColumsNumVal.IsEnabled = enable;
            GapNumVal.IsEnabled = enable;
            CellWidthNumVal.IsEnabled = enable;
            CellHeightNumVal.IsEnabled = enable;  
            SetOrientationBtn.IsEnabled = enable;
            NextGridBtn.IsEnabled = enable;
            SelectAllBtn.IsEnabled = enable;
        }

        private void RunBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                generateList(ref Coordinade);
                if (Coordinade.Count > 0)
                {
                    EnableDisableControls(false);
                    InRunningProcess = true;
                    RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().setCoordinateListForAutoShapemeasure(Coordinade);
                }
            }
            catch (Exception)
            {
                throw;
            }
        }

        private void StopBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                EnableDisableControls(true);
                RunBtn.Content = "Run";
                InRunningProcess = false;                
            }
            catch (Exception)
            {
                throw;
            }
        }        

        private void ClearBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                rapidGrid.ResetConditions();               
                Coordinade.Clear();
                EnableDisableControls(true);
                InRunningProcess = false;
                StartRecordingBtn.IsChecked = false;
                SetOrientationBtn.IsChecked = false;
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().StartRecording(false);
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().ClearAll();                
            }
            catch (Exception)
            {
                throw;
            }
        }        

        private void SelectAllBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                if (!InRunningProcess)
                    rapidGrid.SelectAll();
            }
            catch (Exception)
            {
                throw;
            }
        }

        private void UploadExcelBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            System.Windows.Forms.OpenFileDialog sd = new System.Windows.Forms.OpenFileDialog();
            sd.Filter = "Old Excel Files(*.xls)|*.xls|New Excel Files(*.xlsx)|*.xlsx";
            sd.Title = "Open File";
            sd.DefaultExt = ".xls";
            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                GlobalSettings.MainExcelInstance.AutoMeasurmentFileRead(sd.FileName, ref Coordinade);
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().setCoordinateListForAutoShapemeasure(Coordinade);
            }
        }        

        private void StartRecordingBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                bool CheckStatus = (bool)StartRecordingBtn.IsChecked;
                EnableDisableControls(!CheckStatus);
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().StartRecording(CheckStatus);
            }
            catch (Exception)
            {
                throw;
            }
        }

        private void SetOrientationBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                SetOrientationBtn.IsChecked = false;
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().RW_SetOrientationOfComponent();
            }
            catch (Exception)
            {
                throw;
            }
        }

        private void NextGridBtn_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_RepeatShapeMeasure.MYINSTANCE().CreateNextGrid();
            }
            catch (Exception)
            {
                throw;
            }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                _MyInstance = null;
                RWrapper.RW_RepeatShapeMeasure.Close_RepeatShapeMeasureWindow();
            }
            catch (Exception)
            {
                throw;
            }
        }
    }
}