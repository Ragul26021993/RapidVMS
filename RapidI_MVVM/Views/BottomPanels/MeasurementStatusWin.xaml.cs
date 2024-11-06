using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Controls.Primitives;
using System.Linq;
using Rapid.Utilities;
using Rapid.DGItemClasses;
using Rapid.Windows;
using RapidI.Help;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for ShapeStatusWin.xaml
    /// </summary>
    public partial class MeasurementStatusWin : Grid
    {
        DrawDD MeasurementsScrList = new DrawDD();
        bool ProcessingSelection = false;
        ObservableCollection<MeasurementEntities> DGSourcing;
        
        string AScFileLocation;
        private bool _ToleranceMode = false;
        private bool SetAllNogo = false;
        public MeasurementStatusWin()
        {
            this.InitializeComponent();
            // Insert code required on object creation below this point.
            init();
        }
        void init()
        {
            try
            {
                #region Handling the events related to Add/Modify/Delete of Measurements
                RWrapper.RW_MeasureParameter.MYINSTANCE().MeasureParamUpdateEvent += new RWrapper.RW_MeasureParameter.MeasurementParameterEventHandler(MeasurementStatusWin_MeasureParamUpdate);

                #endregion
                MeasurementsDG.SelectionChanged += new SelectionChangedEventHandler(MeasurementsDG_SelectionChanged);
                MeasurementsDG.PreparingCellForEdit += new EventHandler<DataGridPreparingCellForEditEventArgs>(MeasurementsDG_PreparingCellForEdit);
                MeasurementsDG.CellEditEnding += new EventHandler<DataGridCellEditEndingEventArgs>(MeasurementsDG_CellEditEnding);
                MeasurementsDG.PreviewKeyDown += new KeyEventHandler(MeasurementsDG_PreviewKeyDown);

                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    RotaryGradePanel.Visibility = System.Windows.Visibility.Visible;
                }

                List<Image> MeasureP_ImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("Measure Parameters", 30, 30);
                //GlobalSettings.SettingsReader.ButtonBaseNameList
                List<string> MeasureP_ToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                List<int> MeasurePbuttonType = GlobalSettings.SettingsReader.ButtontypeList;
                List<string> MeasureP_ButtonNameList = GlobalSettings.SettingsReader.ButtonBaseNameList;
                if (GlobalSettings.IsButtonWithNames)
                {
                    ButtonsPanel.Height = 68;
                    ScrollBarBorber.Height = 78;
                    ScrollBarBorber.Margin = new Thickness(4, 0, 0, 2);
                    MainPanel.RowDefinitions[0].Height = new GridLength(80, GridUnitType.Pixel);
                    MainPanel.RowDefinitions[1].Height = new GridLength(.58, GridUnitType.Star);
                }
                else
                {
                    MainPanel.RowDefinitions[0].Height = new GridLength(58, GridUnitType.Pixel);
                    MainPanel.RowDefinitions[1].Height = new GridLength(.773, GridUnitType.Star);
                }
                //Panel MeasurePPanel = (Panel)ButtonsPanel;
                for (int i = 0; i < MeasureP_ImageList.Count; i++)
                {
                    if (MeasurePbuttonType[i] == 10)
                    {
                        Image Im = new Image();
                        Im = MeasureP_ImageList[i];
                        Im.Width = 5; Im.Height = 40;
                        Im.Margin = new Thickness(3, 0, 3, 0);
                        ButtonsPanel.Children.Add(Im);

                        //System.Windows.Shapes.Path sep = new System.Windows.Shapes.Path();
                        //sep.Style = VerticalSep;
                        //sep.IsHitTestVisible = false;
                        //sep.Width = 3;
                        //sep.Margin = new Thickness(5, 6, 5, 5);
                        //MeasurePPanel.Children.Add(sep);
                    }
                    else if (MeasurePbuttonType[i] == 2)
                    {
                        ToggleButton btn = new ToggleButton();
                        if (GlobalSettings.IsButtonWithNames)
                        { btn.Width = 80; }
                        else { btn.Width = 40; }
                        btn.Margin = new Thickness(2, 0, 2, 0);
                        btn.Style = GlobalSettings.ScrBar_ToggleButtonStyle;
                        btn.Content = MeasureP_ImageList[i];
                        btn.Tag = new string[] { "MeasureParameters", MeasureP_ToolTipList[i], MeasureP_ButtonNameList[i], "Measurement", GlobalSettings.SettingsReader.ButtonFeatureId[i].ToString() };
                        btn.Click += new RoutedEventHandler(HandleButtonsPanelsButtonsClick);
                        btn.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                        btn.ToolTip = MeasureP_ToolTipList[i];
                        btn.SetValue(AttachedProperties.AttachedButtonName.ButtonName, btn.ToolTip);
                        ButtonsPanel.Children.Add(btn);
                    }
                    else
                    {
                        Button btn = new Button();
                        if (GlobalSettings.IsButtonWithNames)
                        { btn.Width = 80; }
                        else { btn.Width = 40; }
                        btn.Margin = new Thickness(2, 0, 2, 0);
                        btn.Style = GlobalSettings.ScrBar_NormalButtonStyle;
                        btn.Content = MeasureP_ImageList[i];
                        btn.Tag = new string[] { "MeasureParameters", MeasureP_ToolTipList[i], MeasureP_ButtonNameList[i], "Measurement", GlobalSettings.SettingsReader.ButtonFeatureId[i].ToString() };
                        btn.Click += new RoutedEventHandler(HandleButtonsPanelsButtonsClick);
                        btn.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                        btn.ToolTip = MeasureP_ToolTipList[i];
                        btn.SetValue(AttachedProperties.AttachedButtonName.ButtonName, btn.ToolTip);
                        ButtonsPanel.Children.Add(btn);
                    }

                }
                DGSourcing = new ObservableCollection<MeasurementEntities>();
                MeasurementsDG.ItemsSource = DGSourcing;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS01", ex); }
        }

        #region Frontend event Handling

        //Handling the button click of buttons in the button panel
        private void HandleButtonsPanelsButtonsClick(object sender, RoutedEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                {
                    if (!(bool)GlobalSettings.FeatureLicenseInfo[Convert.ToInt32(tagStr[4])])
                    {
                        MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                        if (b.GetType() == typeof(ToggleButton))
                            ((ToggleButton)b).IsChecked = false;
                        if (b.GetType() == typeof(RadioButton))
                            ((RadioButton)b).IsChecked = false;
                        return;
                    }
                }
                RWrapper.RW_MainInterface.MYINSTANCE().HandleMeasureParameter_Click(tagStr[2]);
                if (tagStr[2] == "Data Transfer to Excel")
                {
                    System.Windows.Forms.SaveFileDialog sd = new System.Windows.Forms.SaveFileDialog();
                    sd.Filter = "(*.xls)Excel Files|*.xls";
                    sd.Title = "Save as";
                    sd.DefaultExt = ".xls";
                    if (sd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        //GlobalSettings.MainExcelInstance.MeasurementList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementList();
                        GlobalSettings.MainExcelInstance.ToleranceFormat = GlobalSettings.XLDataTransferTolMode;
                        GlobalSettings.MainExcelInstance.MeasurementStringList = RWrapper.RW_MeasureParameter.MYINSTANCE().GetTotal_MeasurementListWithNT();
                        GlobalSettings.MainExcelInstance.ReportName = sd.FileName;
                        GlobalSettings.MainExcelInstance.Reset_All();
                        GlobalSettings.MainExcelInstance.GenerateExcel(false, true);
                        //GlobalSettings.MainExcelInstance.GenerateExcel(true, true);
                        if (System.IO.File.Exists(sd.FileName))
                            System.Diagnostics.Process.Start(sd.FileName);
                        else
                            MessageBox.Show("The report file does not exist. It may have been created properly.", "Rapid-I 5.0");
                    }
                }
                else if (tagStr[2] == "Nominal Tolerance")
                {
                    if (MeasurementsDG.SelectedItem != null)
                        HandleEnterMeasurementNominalTolerance(((MeasurementEntities)MeasurementsDG.SelectedItem).DistanceMode);
                }
                else if (tagStr[2] == "Rename Measurement")
                {
                    RenameMeasurement();
                }
                else if (tagStr[2] == "Tolerance Mode")
                {
                    if ((bool)(b as ToggleButton).IsChecked)
                    {
                        _ToleranceMode = true;
                        foreach (MeasurementEntities MeasEntity in DGSourcing)
                        {
                            MeasEntity.ToleranceMode = true;
                        }
                        LoTolCol.Header = "Lo Tol";
                        UpTolCol.Header = "Up Tol";

                    }
                    else
                    {
                        _ToleranceMode = false;
                        foreach (MeasurementEntities MeasEntity in DGSourcing)
                        {
                            MeasEntity.ToleranceMode = false;
                        }
                        LoTolCol.Header = "Lsl";
                        UpTolCol.Header = "Usl";
                    }
                }
                else if (tagStr[2] == "Export to ASC")
                {
                    System.Windows.Forms.SaveFileDialog ofd = new System.Windows.Forms.SaveFileDialog();
                    ofd.Filter = "ASC Files(*.asc)|*.asc";
                    ofd.Title = "Save ASC File";
                    ofd.DefaultExt = ".asc";
                    if (ofd.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        AScFileLocation = ofd.FileName;
                    }
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS02", ex); }
        }
        //Update the framework with the measurement selection
        void MeasurementsDG_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (MeasurementsDG.SelectedItems.Count <= 0) return;
                if (ProcessingSelection) return;
                ProcessingSelection = true;

                List<int> SelectdMeasureID = new List<int>();
                bool MultiMeasurementSelected = false;

                for (int i = 0; i < MeasurementsDG.SelectedItems.Count; i++)
                {
                    SelectdMeasureID.Add(((MeasurementEntities)MeasurementsDG.SelectedItems[i]).ID);
                    if (((MeasurementEntities)MeasurementsDG.SelectedItems[i]).ID != ((MeasurementEntities)MeasurementsDG.SelectedItems[i]).ChildID)
                        MultiMeasurementSelected = true;
                }
                RWrapper.RW_MeasureParameter.MYINSTANCE().SelectMeasurement(SelectdMeasureID.ToArray());

                if (MultiMeasurementSelected)
                {
                    MeasurementsDG.SelectedItems.Clear();
                    for (int i = 0; i < DGSourcing.Count; i++)
                        for (int j = 0; j < SelectdMeasureID.Count; j++)
                            if (DGSourcing[i].ID == SelectdMeasureID[j])
                                MeasurementsDG.SelectedItems.Add(DGSourcing[i]);

                    if (AScFileLocation != "" && MeasurementsDG.SelectedItems.Count == 4) // ((MeasurementEntities)MeasurementsDG.SelectedItems[1]).TypeStr == "Angle")
                    {
                        System.IO.StreamWriter asc = new System.IO.StreamWriter(AScFileLocation);
                        asc.WriteLine("[ELECTRODE]");
                        string LineString = "N0001";
                        string[] axistag = { "X=", "Y=", "Z=", "C=" };
                        MeasurementEntities[] CurrentMeasurements = new MeasurementEntities[4];
                        if (((MeasurementEntities)MeasurementsDG.SelectedItems[0]).TypeStr == "Coordinate")
                        {
                            for (int j = 0; j < 4; j++)
                                CurrentMeasurements[j] = (MeasurementEntities)MeasurementsDG.SelectedItems[j];
                        }
                        else
                        {
                            CurrentMeasurements[0] = (MeasurementEntities)MeasurementsDG.SelectedItems[1];
                            CurrentMeasurements[1] = (MeasurementEntities)MeasurementsDG.SelectedItems[2];
                            CurrentMeasurements[2] = (MeasurementEntities)MeasurementsDG.SelectedItems[3];
                            CurrentMeasurements[3] = (MeasurementEntities)MeasurementsDG.SelectedItems[0];
                        }
                        for (int i = 0; i < 4; i++)
                        {
                            LineString += " " + axistag[i];
                            if (i == 3) break;
                            if (CurrentMeasurements[i].ActualValue < 0)
                                LineString += CurrentMeasurements[i].ValueStr;
                            else
                                LineString += "+" + CurrentMeasurements[i].ValueStr;
                        }
                        double angle = 0;
                        if (GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second || GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Radians)
                            angle = CurrentMeasurements[3].ActualValue * 180 / Math.PI;
                        else
                            angle = CurrentMeasurements[3].ActualValue;
                        if (angle < 0)
                            LineString += Math.Round(angle, GlobalSettings.CurrentDroPrecision);
                        else
                            LineString += "+" + Math.Round(angle, GlobalSettings.CurrentDroPrecision);
                        asc.WriteLine(LineString);
                        asc.Close();
                        AScFileLocation = "";
                        MessageBox.Show("Generated the ASC file successfully!", "Rapid-I 5.0", MessageBoxButton.OK, MessageBoxImage.Information);
                    }
                }

                SelectdMeasureID = null;
                ProcessingSelection = false;
            }
            catch (Exception ex) { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS06", ex); }
        }

        bool DegMinSecAngle = false;
        string OriName = "";
        //Handling formating of values when cell enters edit mode
        void MeasurementsDG_PreparingCellForEdit(object sender, DataGridPreparingCellForEditEventArgs e)
        {
            if (e.Column == NameCol)
            {
                OriName = ((TextBox)e.EditingElement).Text;
                return;
            }
            string ExistingStr = ((TextBox)e.EditingElement).Text;
            int DegSignIndx = ExistingStr.IndexOf('\u00B0'), MinSignIndx = ExistingStr.IndexOf('\'');

            if (DegSignIndx >= 0 && MinSignIndx >= 0)
            {
                ExistingStr = ExistingStr.Replace('\u00B0', ',');
                ExistingStr = ExistingStr.Replace('\'', ',');
                ExistingStr = ExistingStr.Substring(0, ExistingStr.Length - 2);
                DegMinSecAngle = true;
            }
            else
                DegMinSecAngle = false;
            ((TextBox)e.EditingElement).Text = ExistingStr;
            ((TextBox)e.EditingElement).SelectAll();
        }
        //Handling the edit of cell values
        void MeasurementsDG_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            string EnteredStr = ((TextBox)e.EditingElement).Text;
            MeasurementEntities MEntity = e.Row.Item as MeasurementEntities;
            RWrapper.RW_MeasureParameter.MYINSTANCE().SelectMeasurement(MEntity.ID);
            if (e.Column == NameCol)
            {
                if (OriName == EnteredStr) return;
                RWrapper.RW_MeasureParameter.MYINSTANCE().RenameMeasurement(EnteredStr);
            }
            else
            {
                if (e.Column == NominalCol)
                    MEntity.SetNominalToleranceValues(StrForEnteredValue(EnteredStr), MEntity.UpToleranceStr, MEntity.LoToleranceStr);
                else if (e.Column == UpTolCol)
                    MEntity.SetNominalToleranceValues(MEntity.NominalStr, StrForEnteredValue(EnteredStr), MEntity.LoToleranceStr);
                else if (e.Column == LoTolCol)
                    MEntity.SetNominalToleranceValues(MEntity.NominalStr, MEntity.UpToleranceStr, StrForEnteredValue(EnteredStr));
                if (MEntity.ToleranceMode)
                {
                    RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureNominalTolerance(MEntity.NominalValue, MEntity.UpToleranceValue, MEntity.LoToleranceValue, MEntity.ID);
                    //RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureNominalTolerance(MEntity.NominalValue, MEntity.UpToleranceValue, MEntity.LoToleranceValue);
                }
                else
                {
                    RWrapper.RW_MeasureParameter.MYINSTANCE().SetMeasureNominalTolerance(MEntity.NominalValue, MEntity.UpToleranceValue - MEntity.NominalValue, MEntity.LoToleranceValue - MEntity.NominalValue, MEntity.ID);
                }
            }
        }
        //String validating and formating
        string StrForEnteredValue(string EnteredStr)
        {
            if (DegMinSecAngle && GlobalSettings.CurrentAngleMode == GlobalSettings.AngleMode.Degree_Minute_Second)
            {
                string[] SplitStr;
                string ReturnVal = "", NegStr = "", ReturnVal_forFailure = "0\u00B0 00\' 00\'\'";
                int TmpVal = 0;

                SplitStr = EnteredStr.Split(',');
                for (int i = 0; i < SplitStr.Length; i++)
                {
                    if (i == 3) break;
                    if (!int.TryParse(SplitStr[i], out TmpVal))
                        return ReturnVal_forFailure;
                    if (i == 0)//-ve sign can only be entered for the degree term, not for min or sec.
                    {
                        ReturnVal += Math.Abs(TmpVal).ToString() + "\u00B0 ";
                        if (SplitStr[0].Contains("-")) NegStr = "-";
                    }
                    else if (i == 1 || i == 2)
                    {
                        string UnitSymbol;
                        if (i == 1) UnitSymbol = "\' "; //minute symbol: '
                        else UnitSymbol = "\'\'"; //second symbol: ''

                        TmpVal = Math.Abs(TmpVal);
                        if (TmpVal > 59) TmpVal = 59;
                        if (TmpVal < 10)
                            ReturnVal += "0" + TmpVal.ToString() + UnitSymbol;
                        else
                            ReturnVal += TmpVal.ToString() + UnitSymbol;
                    }
                }
                if (SplitStr.Length == 0)
                    ReturnVal = ReturnVal_forFailure;
                else if (SplitStr.Length == 1)
                    ReturnVal += "00\' 00\'\'";
                else if (SplitStr.Length == 2)
                    ReturnVal += "00\'\'";

                return NegStr + ReturnVal;
            }
            else
            {
                string ReturnVal_forFailure = "0";
                double TmpVal = 0;
                if (!double.TryParse(EnteredStr, out TmpVal))
                    return ReturnVal_forFailure;
                return TmpVal.ToString();
            }
        }
        //Handling delete press
        void MeasurementsDG_PreviewKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
            {
                e.Handled = true;
            }
        }
        //To show the tolerance entering window
        void HandleEnterMeasurementNominalTolerance(bool DistMode)
        {
            try
            {
                //AssignShapePanelToShapeInfoWindow(2, "Nominal;Tolerance", "NominalTolerance");
                GlobalSettings.NomTolWin.DistMode = DistMode;
                GlobalSettings.NomTolWin.SetAngleMode();
                Point MPos = Mouse.GetPosition(GlobalSettings.MainWin);
                if (MPos.X + GlobalSettings.NomTolWin.Width < GlobalSettings.MainWin.Width)
                    GlobalSettings.NomTolWin.Left = MPos.X;
                else
                    GlobalSettings.NomTolWin.Left = GlobalSettings.MainWin.Width - GlobalSettings.NomTolWin.Width - 5;
                if (MPos.Y - GlobalSettings.NomTolWin.ActualHeight > 0)
                    GlobalSettings.NomTolWin.Top = MPos.Y - GlobalSettings.NomTolWin.Height;
                else
                    GlobalSettings.NomTolWin.Top = 5;
                GlobalSettings.NomTolWin.ShowDialog();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07", ex); }
        }
        //To show Help window
        public void btn_PreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                string BtnName = tagStr[2];
                string BtnLocationName = tagStr[3];
                if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
                    HelpWindow.GetObj().Show();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07a", ex); }
        }

        protected void HeaderMouseClick(object sender, RoutedEventArgs e)
        {
            try
            {
                DataGridColumnHeader DGC = (DataGridColumnHeader)sender;
                if (DGC.Content.ToString() == "No Go")
                {
                    if (DGSourcing != null && DGSourcing.Count > 0)
                    {
                        foreach (MeasurementEntities MeasEntity in DGSourcing)
                        {
                            MeasEntity.NoGoMeasurement = !SetAllNogo;
                        }
                        RWrapper.RW_MainInterface.MYINSTANCE().SetNogoMeasurement(!SetAllNogo);
                        SetAllNogo = !SetAllNogo;
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MSChBk01a", ex);
            }
        }

        protected void NoGoChkBx_Checked(object Sender, EventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().HandleRightClick_OnEntities("RightClickOnMeasure", "SetPPAbortCheck");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MSChBk01b", ex);
            }
        }
        protected void NoGoChkBx_Unchecked(object sender, EventArgs e)
        {
            try
            {
                RWrapper.RW_MainInterface.MYINSTANCE().HandleRightClick_OnEntities("RightClickOnMeasure", "ResetPPAbortCheck");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MSChBk01c", ex);
            }
        }
        #endregion

        #region Framework event handling

        void MeasurementStatusWin_MeasureParamUpdate(RWrapper.RW_Enum.RW_MEASUREPARAM_TYPE MeasureParam)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    switch (MeasureParam)
                    {
                        case RWrapper.RW_Enum.RW_MEASUREPARAM_TYPE.MEASUREMENT_TABLE_UPDATE:
                            UpdateMeasureListTable();
                            break;
                        case RWrapper.RW_Enum.RW_MEASUREPARAM_TYPE.MEASUREMENT_ADDED:
                            AddMeasurement();
                            break;
                        case RWrapper.RW_Enum.RW_MEASUREPARAM_TYPE.MEASUREMENT_REMOVED:
                            RemoveMeasurement();
                            break;
                        case RWrapper.RW_Enum.RW_MEASUREPARAM_TYPE.MEASUREMENT_UPDATE:
                            UpdateMeasurement();
                            break;
                        case RWrapper.RW_Enum.RW_MEASUREPARAM_TYPE.MEASUREMENT_SELECTION_CHANGED:
                            HandleMeasureSelectionChanged();
                            break;
                    }
                    DelphiMeasureUpdate();
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_MeasureParameter.MeasurementParameterEventHandler(MeasurementStatusWin_MeasureParamUpdate), MeasureParam);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS02a", ex);
            }
        }
        //Measurement table updated
        void UpdateMeasureListTable()
        {
            try
            {
                DGSourcing.Clear();
                MeasurementEntities TmpMEntity;

                System.Data.DataView TmpTb = RWrapper.RW_MeasureParameter.MYINSTANCE().MeasurementParamTable.DefaultView;
                for (int i = 0; i < TmpTb.Count; i++)
                {
                    TmpMEntity = new MeasurementEntities();
                    TmpMEntity.SetValues(Convert.ToInt16(TmpTb[i].Row[0].ToString()), Convert.ToInt16(TmpTb[i].Row[1].ToString()), TmpTb[i].Row[2].ToString(), TmpTb[i].Row[3].ToString(), TmpTb[i].Row[4].ToString(),
                        TmpTb[i].Row[5].ToString(), TmpTb[i].Row[6].ToString(), TmpTb[i].Row[7].ToString(), Convert.ToBoolean(TmpTb[i].Row[8].ToString()), Convert.ToBoolean(TmpTb[i].Row[9].ToString()), _ToleranceMode);
                    DGSourcing.Add(TmpMEntity);
                    //TmpMEntity.PropertyChanged += 
                }

                //HandleMeasureSelectionChanged();//Update selection
                //if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                //{
                //    RRecordPanel.dgvValues.SetValue(DataGrid.ItemsSourceProperty, null);
                //}
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS03", ex);
            }
        }
        //A new measurement added
        void AddMeasurement()
        {
            try
            {
                List<ArrayList> ListOfParams = RWrapper.RW_MeasureParameter.MYINSTANCE().MeasureParameter_ArrayList;
                for (int i = 0; i < ListOfParams.Count; i++)
                {
                    ArrayList MParams = ListOfParams[i];

                    MeasurementEntities TmpMEntity = new MeasurementEntities();
                    TmpMEntity.SetValues(Convert.ToInt16(MParams[0]), Convert.ToInt16(MParams[1]), MParams[2].ToString(), MParams[3].ToString(), MParams[4].ToString(), MParams[5].ToString(), MParams[6].ToString(), MParams[7].ToString(), Convert.ToBoolean(MParams[8]), Convert.ToBoolean(MParams[9]), _ToleranceMode);

                    DGSourcing.Add(TmpMEntity);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS03", ex);
            }
        }
        //Existing measurement removed
        void RemoveMeasurement()
        {
            try
            {
                ProcessingSelection = true;
                List<MeasurementEntities> TmpRemoveEntities = new List<MeasurementEntities>();
                for (int i = 0; i < DGSourcing.Count; i++)
                {
                    if (DGSourcing[i].ID == RWrapper.RW_MeasureParameter.MYINSTANCE().Measure_EntityID)
                    {
                        //TmpRemoveEntities.Add(DGSourcing[i]);
                        DGSourcing.Remove(DGSourcing[i]);
                        i--;
                    }
                }
                ProcessingSelection = false;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS03", ex);
            }
        }
        //Existing measurement updated
        void UpdateMeasurement()
        {
            try
            {
                List<ArrayList> ListOfParams = RWrapper.RW_MeasureParameter.MYINSTANCE().MeasureParameter_ArrayList;
                for (int i = 0; i < ListOfParams.Count; i++)
                {
                    ArrayList MParams = ListOfParams[i];
                    int MeasureId = Convert.ToInt16(MParams[0]);
                    int MeasureChildId = Convert.ToInt16(MParams[1]);
                    for (int j = 0; j < DGSourcing.Count; j++)
                    {
                        if (DGSourcing[j].ID == MeasureId && DGSourcing[j].ChildID == MeasureChildId)
                        {
                            MeasurementEntities TmpMEntity = DGSourcing[j] as MeasurementEntities;
                            TmpMEntity.SetValues(MParams[2].ToString(), MParams[3].ToString(), MParams[4].ToString(), MParams[5].ToString(), MParams[6].ToString(), MParams[7].ToString(), Convert.ToBoolean(MParams[8]), Convert.ToBoolean(MParams[9]), _ToleranceMode);
                            break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS03", ex);
            }
        }
        //Renaming the entity
        public void RenameMeasurement()
        {
            try
            {
                if (MeasurementsDG.SelectedItems.Count > 1)
                {
                    for (int i = 0; i < MeasurementsDG.SelectedItems.Count; i++)
                        if (((MeasurementEntities)MeasurementsDG.SelectedItems[i]).ID == ((MeasurementEntities)MeasurementsDG.SelectedItems[i]).ChildID)
                        {
                            MessageBox.Show("Sorry! Cannot rename multiple measurements at a time.", "Rapid-I");
                            return;
                        }
                }
                //Show the shape rename window.
                ShapeRenameWin renameWin = new ShapeRenameWin();
                renameWin.txtNewName.Text = ((MeasurementEntities)MeasurementsDG.SelectedItem).NameStr;
                renameWin.ShowDialog();
                if (renameWin.Result)
                {
                    string MName = renameWin.txtNewName.Text.Replace(" ", "_");
                    RWrapper.RW_MeasureParameter.MYINSTANCE().RenameMeasurement(MName);
                    if (((MeasurementEntities)MeasurementsDG.SelectedItem).ID == ((MeasurementEntities)MeasurementsDG.SelectedItem).ChildID)
                        ((MeasurementEntities)MeasurementsDG.SelectedItem).NameStr = MName;
                }
                renameWin.Hide();
                renameWin.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS05", ex);
            }
        }
        //Updating the measurement selection
        void HandleMeasureSelectionChanged()
        {
            try
            {
                if (ProcessingSelection) return;
                ProcessingSelection = true;

                MeasurementsDG.SelectedItems.Clear();

                //Get the refrence to the selected measurement list from framewwork
                List<int> FrameworkMeasureList = RWrapper.RW_MeasureParameter.MYINSTANCE().SelectedMeasureIDList;
                for (int i = 0; i < DGSourcing.Count; i++)
                    for (int j = 0; j < FrameworkMeasureList.Count; j++)
                        if (DGSourcing[i].ID == FrameworkMeasureList[j])
                            MeasurementsDG.SelectedItems.Add(DGSourcing[i]);

                ScrollAppropriateMeasure_IntoView();

                ProcessingSelection = false;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS04", ex);
            }
        }

        #endregion

        void DelphiMeasureUpdate()
        {
            if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
            {
                RotaryGradePanel.dgvProgramSteps.ItemsSource = null;
                RotaryGradePanel.dgvProgramSteps.ItemsSource = RWrapper.RW_MeasureParameter.MYINSTANCE().GetMeasurementTableWithDelphi().DefaultView;
                if (RotaryGradePanel.dgvProgramSteps.Columns.Count > 0)
                {
                    RotaryGradePanel.dgvProgramSteps.Columns[0].Width = 130;
                    RotaryGradePanel.dgvProgramSteps.Columns[1].Width = 230;
                    RotaryGradePanel.dgvProgramSteps.Columns[2].Width = 130;
                    RotaryGradePanel.dgvProgramSteps.Columns[3].Width = 130;
                }
                if (RotaryGradePanel.dgvProgramSteps.Items.Count > 0)
                    RotaryGradePanel.dgvProgramSteps.ScrollIntoView(RotaryGradePanel.dgvProgramSteps.Items[RotaryGradePanel.dgvProgramSteps.Items.Count - 1]);
            }
        }

        public int SetPassFail()
        {
            try
            {
                bool Pass = true, ShowPassFail = false;
                bool AllMeasurementsCritical = true;
                for (int i = 0; i < MeasurementsDG.Items.Count; i++)
                {
                    if (!((MeasurementEntities)MeasurementsDG.Items[i]).IsCriticalMeasure)
                    {
                        AllMeasurementsCritical = false;
                        break;
                    }
                }

                for (int i = 0; i < MeasurementsDG.Items.Count; i++)
                {
                    MeasurementEntities currMeasurement = (MeasurementEntities)MeasurementsDG.Items[i];
                    if (currMeasurement.IsCriticalMeasure) // IsCriticalChkBxVisible == System.Windows.Visibility.Visible)
                    {
                        ShowPassFail = true;
                        if (AllMeasurementsCritical)
                            Pass = ThisMeasurementisPassed(currMeasurement);
                        else if (currMeasurement.IsCriticalMeasure)
                            Pass = ThisMeasurementisPassed(currMeasurement);
                    }
                }
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Rotary_Del_FI)
                {
                    ShowPassFail = true;
                    if (!RWrapper.RW_PartProgram.MYINSTANCE().GetDelphiPPSuccessStatus())
                        Pass = false;
                }
                if (ShowPassFail)
                {
                    if (PassFailBorder.Visibility == System.Windows.Visibility.Hidden)
                    {
                        PassFailBorder.Visibility = System.Windows.Visibility.Visible;
                        MainPanel.ColumnDefinitions[1].Width = new System.Windows.GridLength(125);
                    }
                    if (Pass)
                    {
                        PassFailTxt.Text = "PASS";
                        PassFailBorder.Background = Brushes.Green;
                        return 1;
                    }
                    else
                    {
                        PassFailTxt.Text = "FAIL";
                        PassFailBorder.Background = Brushes.Red;
                        return 2;
                    }
                }
                return 0;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS08", ex);
                return 0;
            }
        }


        public void HidePassFail()
        {
            try
            {
                PassFailBorder.Visibility = System.Windows.Visibility.Hidden;
                MainPanel.ColumnDefinitions[1].Width = new System.Windows.GridLength(0);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS09", ex); }
        }

        //Scroll to view either selected shape or last shape
        public void ScrollAppropriateMeasure_IntoView()
        {
            if (MeasurementsDG.SelectedItems.Count > 0)
                MeasurementsDG.ScrollIntoView(MeasurementsDG.SelectedItems[MeasurementsDG.SelectedItems.Count - 1]);
            else if (MeasurementsDG.Items.Count > 0)
                MeasurementsDG.ScrollIntoView(MeasurementsDG.Items[MeasurementsDG.Items.Count - 1]);
        }
        //Clear All function
        public void ClearAll()
        {
            try
            {
                HidePassFail();
                DGSourcing.Clear();
                ToggleButton TBtn = null;
                try
                {
                    TBtn = ButtonsPanel.Children.OfType<ToggleButton>()
                        .Where(T => (((string[])T.Tag)[2]) == "Tolerance Mode").First();
                }
                catch (Exception ex)
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS08", ex);
                }
                if (TBtn != null)
                {
                    TBtn.IsChecked = false;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS11", ex); }
        }

        //Functions for experimentation
        public DataGridRow GetRow(DataGrid grid, int index)
        {
            DataGridRow row = (DataGridRow)grid.ItemContainerGenerator.ContainerFromIndex(index);
            if (row == null)
            {
                // May be virtualized, bring into view and try again.
                grid.UpdateLayout();
                grid.ScrollIntoView(grid.Items[index]);
                row = (DataGridRow)grid.ItemContainerGenerator.ContainerFromIndex(index);
            }
            return row;
        }
        public DataGridCell GetCell(DataGrid grid, DataGridRow row, int column)
        {
            if (row != null)
            {
                DataGridCellsPresenter presenter = GetVisualChild<DataGridCellsPresenter>(row);

                if (presenter == null)
                {
                    grid.ScrollIntoView(row, grid.Columns[column]);
                    presenter = GetVisualChild<DataGridCellsPresenter>(row);
                }

                DataGridCell cell = (DataGridCell)presenter.ItemContainerGenerator.ContainerFromIndex(column);
                return cell;
            }
            return null;
        }
        public DataGridCell GetCell(DataGrid grid, int row, int column)
        {
            DataGridRow rowContainer = GetRow(grid, row);
            return GetCell(grid, rowContainer, column);
        }
        public T GetVisualChild<T>(Visual parent) where T : Visual
        {
            T child = default(T);
            int numVisuals = VisualTreeHelper.GetChildrenCount(parent);
            for (int i = 0; i < numVisuals; i++)
            {
                Visual v = (Visual)VisualTreeHelper.GetChild(parent, i);
                child = v as T;
                if (child == null)
                {
                    child = GetVisualChild<T>(v);
                }
                if (child != null)
                {
                    break;
                }
            }
            return child;
        }

        private bool ThisMeasurementisPassed(MeasurementEntities TmpMEntity)
        {
            if (TmpMEntity.ToleranceMode)
            {
                if (TmpMEntity.NominalValue + TmpMEntity.UpToleranceValue < TmpMEntity.ActualValue || TmpMEntity.NominalValue + TmpMEntity.LoToleranceValue > TmpMEntity.ActualValue)
                    return false; //thisMeasurementisPass = false;
            }
            else
            {
                if (TmpMEntity.ActualValue > TmpMEntity.UpToleranceValue || TmpMEntity.ActualValue < TmpMEntity.LoToleranceValue)
                    return false; //thisMeasurementisPass = false;
            }
            return true;
        }
    }
}