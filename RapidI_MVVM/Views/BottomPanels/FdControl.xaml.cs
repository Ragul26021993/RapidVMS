using System;
using System.Globalization;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Rapid.Utilities;
using RapidI.Help;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for FdControl.xaml
    /// </summary>
    public partial class FdControl : Grid
    {
        DrawDD DepthShpScrList = new DrawDD();
        System.Collections.Hashtable FocusDatumTable;
        bool FocusOn = false;

        public FdControl()
        {
            try
            {
                this.InitializeComponent();
                this.focusGrid.ValueChange += new RoutedEventHandler(focusGrid_ValueChange);
                ZAxis.IsChecked = true;
                FocusDatumTable = new System.Collections.Hashtable();
                init();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD01", ex); }
        }
        void init()
        {
            try
            {
                this.PreviewMouseDown += new MouseButtonEventHandler(FdControl_PreviewMouseDown);
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FocusGrpBx", "FocusGrpBx", 32, 32);
                grpFocus.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("DepthGrpBx", "DepthGrpBx", 32, 32);
                grpFocusDepths.Header = GlobalSettings.SettingsReader.ButtonToolTip;

                DepthListBorder.Child = DepthShpScrList;
                Button[] focusButtons = { btnFocus, btnFocusOnOff, btnReset, btnSet, 
                                            btnNew, btnSetZ1, btnSetZ2, btnResetFD};
                for (int i = 0; i < focusButtons.Length; i++)
                {
                    focusButtons[i].Tag = new string[] { "Focus", "Tooltip", "Name" };
                    focusButtons[i].Click += new RoutedEventHandler(FocusPanelClicks);
                    focusButtons[i].PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                }
                txtRows.ValueChanged += new RoutedEventHandler(HandleFocusRowsColumns_Change);
                txtColumns.ValueChanged += new RoutedEventHandler(HandleFocusRowsColumns_Change);
                //txtRows._valueBox.PreviewTextInput += new TextCompositionEventHandler(LimitRowsColumns);
                //txtColumns._valueBox.PreviewTextInput += new TextCompositionEventHandler(LimitRowsColumns);
                txtWidth.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
                txtHeight.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
                txtGap.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
                txtSpan.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
                txtSpeed.TextChanged += new TextChangedEventHandler(txtValues_TextChanged);
                XAxis.Checked += new RoutedEventHandler(FocusAxis_Checked);
                YAxis.Checked += new RoutedEventHandler(FocusAxis_Checked);
                ZAxis.Checked += new RoutedEventHandler(FocusAxis_Checked);
                FocusShapeCBx.Items.Add("Point");
                FocusShapeCBx.Items.Add("Plane");
                FocusShapeCBx.Items.Add("Sphere");
                FocusShapeCBx.Items.Add("Cylinder");
                FocusShapeCBx.SelectedIndex = 0;
                FocusShapeCBx.SelectionChanged += new SelectionChangedEventHandler(FocusShapeCBx_SelectionChanged);

                btnFocusOnOff.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus Off", "FocusPanel", 30, 30);
                btnFocusOnOff.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnFocusOnOff.Tag)[2] = "Focus Off";
                btnSettings.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus Settings", "FocusPanel", 30, 30);
                btnSettings.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnSettings.Tag = new string[] { "Focus", "Tooltip", "Focus Settings" };
                btnFocus.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Do Focus", "FocusPanel", 30, 30);
                btnFocus.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnFocus.Tag)[2] = "Do Focus";
                btnReset.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Reset Focus", "FocusPanel", 30, 30);
                btnReset.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnReset.Tag)[2] = "Reset Focus";
                btnNew.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("New Depth", "FocusPanel", 30, 30);
                btnNew.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnNew.Tag)[2] = "New Depth";
                btnResetFD.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Reset Depth", "FocusPanel", 30, 30);
                btnResetFD.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnResetFD.Tag)[2] = "Reset Depth";

                btnSetZ1.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Set Z1", "FocusPanel", 25, 25);
                btnSetZ1.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnSetZ1.Tag)[2] = "Set Z1";
                btnSetZ2.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Set Z2", "FocusPanel", 25, 25);
                btnSetZ2.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnSetZ2.Tag)[2] = "Set Z2";
                btnSet.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Save Focus Settings", "FocusPanel", 30, 30);
                btnSet.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnSet.Tag)[2] = "Save Focus Settings";
                btnCancel.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Close Focus Settings", "FocusPanel", 30, 30);
                btnCancel.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnCancel.Tag = new string[] { "Focus", "Tooltip", "Close Focus Settings" };

                RWrapper.RW_FocusDepth.MYINSTANCE().FocusValueChanged += new RWrapper.RW_FocusDepth.FocusEventHandler(Handle_FocusValueChanged);
                RWrapper.RW_FocusDepth.MYINSTANCE().FocusFinished += new RWrapper.RW_FocusDepth.FocusEventHandler(Handle_FocusFinished);
                RWrapper.RW_FocusDepth.MYINSTANCE().GenerateFocusBtnClick += new RWrapper.RW_FocusDepth.FocusButtonClickEventHandler(Handle_FocusModeChange);
                //Handling the Automatic focus
                RWrapper.RW_FocusDepth.MYINSTANCE().UpdateDepthValues += new RWrapper.RW_FocusDepth.FocusEventHandler(HandleUpdate_FocusDepthvalue);
                txtSpan.Text = String.Format("{0:0.##}", 0.4);
                txtSpeed.Text = String.Format("{0:0.##}", 0.2);

                //Handling the Focus depth cilcks
                //RWrapper.RW_MainInterface.AddNewDepthMeasure += new RWrapper.RW_MainInterface.RWrapperEventHandler(Handle_AddNewDepthMeasure);
                //Handle the Focus depth measurement selection change
                //This is especiually for depth datum implementation in frameowrk
                RWrapper.RW_FocusDepth.MYINSTANCE().DepthShapeAdded += new RWrapper.RW_FocusDepth.FocusEventHandler(Handle_DepthShapeAdded);
                RWrapper.RW_FocusDepth.MYINSTANCE().DepthShapeRemoved += new RWrapper.RW_FocusDepth.FocusEventHandler(Handle_DepthShapeRemoved);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD02", ex); }
        }
        void FdControl_PreviewMouseDown(object sender, RoutedEventArgs e)
        {
            if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
            {
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.DSP ||
                    GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal_DSP ||
                    GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Magnifier_DSP)
                {
                    return;
                }
                if (!(bool)GlobalSettings.FeatureLicenseInfo[6])
                {
                    MessageBox.Show("Sorry! You do not have license for this feature.", "Rapid-I");
                    e.Handled = true;
                }
            }
        }


        void focusGrid_ValueChange(object sender, RoutedEventArgs e)
        {
            try
            {
                this.txtRows.Value = focusGrid.NoOfSelectedRows;
                this.txtColumns.Value = focusGrid.NoOfSelectedColumns;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD03", ex); }
        }
        void btnSet_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(txtRows.Value, txtColumns.Value,
                    Convert.ToDouble(txtGap.Text), Convert.ToDouble(txtWidth.Text), Convert.ToDouble(txtHeight.Text),
                    Convert.ToDouble(txtSpan.Text), Convert.ToDouble(txtSpeed.Text));
                grpSettings.Visibility = System.Windows.Visibility.Hidden;
                OuterGrid.Visibility = System.Windows.Visibility.Visible;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD04", ex); }
        }
        void btnSettings_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                OuterGrid.Visibility = System.Windows.Visibility.Hidden;
                grpSettings.Visibility = System.Windows.Visibility.Visible;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD05", ex); }
        }
        public void btn_PreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                string BtnName = tagStr[2];
                string BtnLocationName = tagStr[0];
                if (HelpWindow.GetObj().ShowTopic(BtnLocationName, BtnName))
                    HelpWindow.GetObj().Show();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:MS07a", ex); }
        }

        void FocusPanelClicks(object sender, RoutedEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                switch (tagStr[2])
                {
                    //For Focus Depth panel
                    case "Do Focus":
                        if (!FocusOn)
                            FocusPanelClicks(btnFocusOnOff, null);
                        RWrapper.RW_FocusDepth.MYINSTANCE().FocusClicked();
                        b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Abort Focus", "FocusPanel", 30, 30);
                        b.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                        ((string[])b.Tag)[2] = "Abort Focus";
                        break;
                    case "Focus On":
                        FocusOn = false;
                        RWrapper.RW_FocusDepth.MYINSTANCE().ShowHideFocus(false);
                        btnFocusOnOff.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus Off", "FocusPanel", 30, 30);
                        btnFocusOnOff.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                        ((string[])btnFocusOnOff.Tag)[2] = "Focus Off";
                        break;
                    case "Focus Off":
                        FocusOn = true;
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(txtRows.Value, txtColumns.Value,
                            Convert.ToDouble(txtGap.Text), Convert.ToDouble(txtWidth.Text), Convert.ToDouble(txtHeight.Text),
                            Convert.ToDouble(txtSpan.Text), Convert.ToDouble(txtSpeed.Text));
                        RWrapper.RW_FocusDepth.MYINSTANCE().ShowHideFocus(true);
                        btnFocusOnOff.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus On", "FocusPanel", 30, 30);
                        btnFocusOnOff.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                        ((string[])btnFocusOnOff.Tag)[2] = "Focus On";
                        break;
                    case "Abort Focus":
                        RWrapper.RW_FocusDepth.MYINSTANCE().AbortClicked();
                        b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Do Focus", "FocusPanel", 30, 30);
                        b.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                        ((string[])b.Tag)[2] = "Do Focus";
                        break;
                    case "Reset Focus":
                        //RWrapper.RW_FocusDepth.MYINSTANCE().ResetClicked();
                        RWrapper.RW_FocusDepth.MYINSTANCE().FocusResetClicked();
                        break;
                    case "Close Focus Settings":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(txtRows.Value, txtColumns.Value,
                    Convert.ToDouble(txtGap.Text), Convert.ToDouble(txtWidth.Text), Convert.ToDouble(txtHeight.Text),
                    Convert.ToDouble(txtSpan.Text), Convert.ToDouble(txtSpeed.Text));
                        break;
                    case "New Depth":
                        FocusShapeCBx.SelectedIndex = 0;
                        //remove the already added items from list.
                        RWrapper.RW_FocusDepth.MYINSTANCE().NewClicked();
                        break;
                    case "Set Z1":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetZ1Clicked();
                        break;
                    case "Set Z2":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetZ2Clicked();
                        break;
                    case "Reset Depth":
                        if (RWrapper.RW_FocusDepth.MYINSTANCE().DepthResetClicked())
                        {
                            txtSetZ1.Text = null;
                            txtsetZ2.Text = null;
                            txtDepth.Text = null;
                            FocusShapeCBx.SelectedIndex = 0;
                        }
                        break;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD06", ex); }
        }
        void Handle_FocusValueChanged()
        {
            try
            {
                this.pgFocusMeter.Dispatcher.Invoke(new System.Windows.Forms.MethodInvoker(GetFocusValue));
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD07", ex); }

        }
        void GetFocusValue()
        {
            try
            {
                this.pgFocusMeter.Value = RWrapper.RW_FocusDepth.MYINSTANCE().FocusValue;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD08", ex); }
        }

        void Handle_FocusFinished()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    this.txtFocusValue.Text = RWrapper.RW_FocusDepth.MYINSTANCE().CurrentFocusValue;
                    btnFocus.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Do Focus", "FocusPanel", 30, 30);
                    btnFocus.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                    ((string[])btnFocus.Tag)[2] = "Do Focus";
                }
                else
                {
                    this.Dispatcher.Invoke(new System.Windows.Forms.MethodInvoker(Handle_FocusFinished));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD09", ex); }
        }
        void Handle_FocusModeChange(string FocusMode)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (FocusMode == "FOCUS")
                    {
                        btnFocus.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Abort Focus", "FocusPanel", 30, 30);
                        btnFocus.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                        ((string[])btnFocus.Tag)[2] = "Abort Focus";
                        RWrapper.RW_FocusDepth.MYINSTANCE().FocusClicked();
                    }
                    else
                    {
                        btnFocus.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Do Focus", "FocusPanel", 30, 30);
                        btnFocus.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                        ((string[])btnFocus.Tag)[2] = "Do Focus";
                        RWrapper.RW_FocusDepth.MYINSTANCE().AbortClicked();
                    }
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_FocusDepth.FocusButtonClickEventHandler(Handle_FocusModeChange), FocusMode);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD10", ex); }
        }
        void HandleFocusRowsColumns_Change(object sender, RoutedEventArgs e)
        {
            try
            {
                NumericUpDown Txtbx = (NumericUpDown)sender;
                ValueWithinLimit(ref Txtbx);
                RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(txtRows.Value, txtColumns.Value,
                        Convert.ToDouble(txtGap.Text, CultureInfo.CurrentCulture), Convert.ToDouble(txtWidth.Text, CultureInfo.CurrentCulture),
                        Convert.ToDouble(txtHeight.Text, CultureInfo.CurrentCulture),
                        Convert.ToDouble(txtSpan.Text, CultureInfo.CurrentCulture), Convert.ToDouble(txtSpeed.Text, CultureInfo.CurrentCulture));
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD11", ex); }
        }
        void txtValues_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                double TmpGap = 0, TmpWidth = 0, TmpHeight = 0, TmpSpan = 0, TmpSpeed = 0;
                if (txtGap.Text != "")
                    TmpGap = Convert.ToDouble(txtGap.Text, CultureInfo.CurrentCulture);
                if (txtWidth.Text != "")
                    TmpWidth = Convert.ToDouble(txtWidth.Text, CultureInfo.CurrentCulture);
                if (txtHeight.Text != "")
                    TmpHeight = Convert.ToDouble(txtHeight.Text, CultureInfo.CurrentCulture);
                if (txtSpan.Text != "")
                    TmpSpan = Convert.ToDouble(txtSpan.Text, CultureInfo.CurrentCulture);
                if (txtSpeed.Text != "")
                    TmpSpeed = Convert.ToDouble(txtSpeed.Text, CultureInfo.CurrentCulture);
                RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(txtRows.Value, txtColumns.Value, TmpGap, TmpWidth, TmpHeight, TmpSpan, TmpSpeed);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD12", ex); }
        }
        void HandleUpdate_FocusDepthvalue()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    txtSetZ1.Text = RWrapper.RW_FocusDepth.MYINSTANCE().CurrentDepthZ1Value;
                    txtsetZ2.Text = RWrapper.RW_FocusDepth.MYINSTANCE().CurrentDepthZ2Value;
                    txtDepth.Text = RWrapper.RW_FocusDepth.MYINSTANCE().CurrentDepthValue;
                }
                else
                    this.Dispatcher.Invoke(new System.Windows.Forms.MethodInvoker(HandleUpdate_FocusDepthvalue));
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD13", ex); }
        }
        void Handle_DepthShapeAdded()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    RadioButton rb = new RadioButton();
                    rb.Style = GlobalSettings.RadioButtonStyle;
                    rb.Content = RWrapper.RW_MainInterface.EntityName;
                    rb.ToolTip = RWrapper.RW_MainInterface.EntityName;
                    rb.Height = 40;
                    rb.Width = 40;
                    rb.Click += new RoutedEventHandler(HandleFocusListSelection);
                    DepthShpScrList.ScrT.Children.Add(rb);
                    rb.Tag = RWrapper.RW_ShapeParameter.MYINSTANCE().Shape_EntityID;
                    FocusDatumTable.Add(RWrapper.RW_ShapeParameter.MYINSTANCE().Shape_EntityID, rb);
                }
                else
                    this.Dispatcher.Invoke(new System.Windows.Forms.MethodInvoker(Handle_DepthShapeAdded));
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD14", ex); }
        }
        void Handle_DepthShapeRemoved()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    DepthShpScrList.ScrT.Children.Remove((UIElement)FocusDatumTable[RWrapper.RW_ShapeParameter.MYINSTANCE().Shape_EntityID]);
                    FocusDatumTable.Remove(RWrapper.RW_ShapeParameter.MYINSTANCE().Shape_EntityID);
                }
                else
                    this.Dispatcher.Invoke(new System.Windows.Forms.MethodInvoker(Handle_DepthShapeRemoved));
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD15", ex); }
        }
        void HandleFocusListSelection(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_FocusDepth.MYINSTANCE().SelectDepthDatum((int)((RadioButton)sender).Tag);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD16", ex); }
        }
        void FocusAxis_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                if (sender.Equals(XAxis))
                    RWrapper.RW_FocusDepth.MYINSTANCE().SetSelectedAxis(0);
                else if (sender.Equals(YAxis))
                    RWrapper.RW_FocusDepth.MYINSTANCE().SetSelectedAxis(1);
                else if (sender.Equals(ZAxis))
                    RWrapper.RW_FocusDepth.MYINSTANCE().SetSelectedAxis(2);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD17", ex); }
        }
        void FocusShapeCBx_SelectionChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                switch (FocusShapeCBx.SelectedItem.ToString())
                {
                    case "Point":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetShapeForDepth(3);
                        break;
                    case "Plane":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetShapeForDepth(0);
                        break;
                    case "Sphere":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetShapeForDepth(1);
                        break;
                    case "Cylinder":
                        RWrapper.RW_FocusDepth.MYINSTANCE().SetShapeForDepth(2);
                        break;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD17a", ex); }
        }

        public void DoFocusOn()
        {
            try
            {
                FocusOn = true;
                RWrapper.RW_FocusDepth.MYINSTANCE().SetFocusRectangleProperties(txtRows.Value, txtColumns.Value,
                    Convert.ToDouble(txtGap.Text), Convert.ToDouble(txtWidth.Text), Convert.ToDouble(txtHeight.Text),
                    Convert.ToDouble(txtSpan.Text), Convert.ToDouble(txtSpeed.Text));
                RWrapper.RW_FocusDepth.MYINSTANCE().ShowHideFocus(true);
                btnFocusOnOff.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus On", "FocusPanel", 30, 30);
                btnFocusOnOff.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnFocusOnOff.Tag)[2] = "Focus On";
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD18", ex); }
        }
        public void FocusOff()
        {
            try
            {
                if (FocusOn)
                {
                    btnFocusOnOff.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus Off", "FocusPanel", 30, 30);
                    btnFocusOnOff.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                    ((string[])btnFocusOnOff.Tag)[2] = "Focus Off";
                    FocusOn = false;
                    RWrapper.RW_FocusDepth.MYINSTANCE().ShowHideFocus(false);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD19", ex); }
        }
        public void ClearAll()
        {
            try
            {
                //Clear all depth values
                DepthShpScrList.ScrT.Children.Clear();
                FocusDatumTable.Clear();
                txtSetZ1.Text = "";
                txtsetZ2.Text = "";
                txtDepth.Text = "";
                txtRows.Value = 1; txtColumns.Value = 1;
                txtHeight.Text = "40"; txtWidth.Text = "40";
                txtSpan.Text = String.Format("{0:0.##}", 0.4);
                txtSpeed.Text = String.Format("{0:0.##}", 0.2);
                txtGap.Text = "40";
                this.txtFocusValue.Text = "";
                this.pgFocusMeter.Value = 0;
                focusGrid.ResetConditions();

                //Focus button reset
                FocusOn = false;
                btnFocusOnOff.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Focus Off", "FocusPanel", 30, 30);
                btnFocusOnOff.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                ((string[])btnFocusOnOff.Tag)[2] = "Focus Off";
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FD20", ex); }
        }

        private void ValueWithinLimit(ref NumericUpDown SenderNumericUpDown)
        {
            if (SenderNumericUpDown == txtRows)
            {
                double value = 1;
                if (double.TryParse(txtRows._valueBox.Text, out value))
                {
                    if (value == 0)
                        txtColumns.Maximum = GlobalSettings.LimitRowColumProduct;
                    txtColumns.Maximum = (int)(GlobalSettings.LimitRowColumProduct / value);
                }
            }
            else if (SenderNumericUpDown == txtColumns)
            {
                double value = 1;
                if (double.TryParse(txtColumns._valueBox.Text, out value))
                {
                    if (value == 0)
                        txtRows.Maximum = GlobalSettings.LimitRowColumProduct;
                    txtRows.Maximum = (int)(GlobalSettings.LimitRowColumProduct / value);
                }
            }
        }
    }
}