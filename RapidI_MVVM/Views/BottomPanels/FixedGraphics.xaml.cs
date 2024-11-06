using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using Rapid.Utilities;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for FixedGraphics.xaml
    /// </summary>
    public partial class FixedGraphicsPanel : Grid
	{
        DrawDD FixedGShpScrList = new DrawDD();
        bool FixedGrapicsMode = false;
        
        //Constructor
        public FixedGraphicsPanel()
		{
            try
            {
                this.InitializeComponent();
                this.rdbDiameter.IsChecked = true;
                this.rdbThroughOrigin.IsChecked = true;

                FixedGShpScrList.Margin = new Thickness(0);
                init();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG01", ex); }
		}
        private void init()
        {
            try
            {
                ShpListBorder.Child = FixedGShpScrList;
                LineAngleDegree.Maximum = 360;
                LineAngleDegree.Minimum = -360;
                LineAngleMin.Maximum = 59;
                List<Image> FixGImageList = GlobalSettings.SettingsReader.UpdateBtnInfoList("FixedGraphics_Toolbar", 30, 30);
                List<string> FixGToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                List<int> FixGbuttonType = GlobalSettings.SettingsReader.ButtontypeList;
                List<string> FixGButtonNameList = GlobalSettings.SettingsReader.ButtonBaseNameList;
                for (int i = 0; i < FixGImageList.Count; i++)
                {
                    if (FixGbuttonType[i] == 0)
                    {
                        Button rb = new Button();
                        rb.Width = 35;
                        rb.Style = GlobalSettings.NormalButtonStyle;
                        rb.Background = null;
                        rb.BorderBrush = null;
                        rb.BorderThickness = new Thickness(0);
                        rb.Content = FixGImageList[i];
                        rb.ToolTip = FixGToolTipList[i];
                        rb.Tag = new string[] { "FixedGraphics", FixGToolTipList[i], FixGButtonNameList[i] };
                        rb.Click += new RoutedEventHandler(FixedGraphics_Click);
                        //FgToolbar.Children.Add(rb);
                        ButtonsPanel.Children.Add(rb);
                    }
                    else
                    {
                        ToggleButton rb = new ToggleButton();
                        rb.Width = 35;
                        rb.Style = GlobalSettings.ToggleButtonStyle;
                        rb.Background = null;
                        rb.BorderBrush = null;
                        rb.BorderThickness = new Thickness(0);
                        rb.Content = FixGImageList[i];
                        rb.ToolTip = FixGToolTipList[i];
                        rb.Tag = new string[] { "FixedGraphics", FixGToolTipList[i], FixGButtonNameList[i] };
                        rb.Click += new RoutedEventHandler(FixedGraphics_Click);
                        //FgToolbar.Children.Add(rb);
                        ButtonsPanel.Children.Add(rb);
                    }
                }
                TextBox[] FixedGraphicsTxts = { txtLineOffset, txtcircleX,
                                              txtCircleY, txtCircleRadDia,txtText };
                for (int i = 0; i < FixedGraphicsTxts.Length; i++)
                    FixedGraphicsTxts[i].TextChanged += new TextChangedEventHandler(FixedGraphics_TextChanged);
                LineAngleDegree.ValueChanged += new RoutedEventHandler(LineAngle_ValueChanged);
                LineAngleMin.ValueChanged += new RoutedEventHandler(LineAngle_ValueChanged);

                RadioButton[] RdbsFgs = { rdbFromOrigin, rdbThroughOrigin, rdbXoffset, rdbYOffset };

                for (int i = 0; i < RdbsFgs.Length; i++)
                    RdbsFgs[i].Checked += new RoutedEventHandler(FixedGraphics_Checked);
                rdbDiameter.Click += new RoutedEventHandler(FixedGraphics_Checked);

                btnLine.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Fixed Line", "FixedGraphics_Line", 30, 30);
                btnLine.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnLine.Tag = new string[] { "FixedGraphics", GlobalSettings.SettingsReader.ButtonToolTip, "Fixed Line" };
                btnLine.Click += new RoutedEventHandler(FixedGraphics_Click);
                rdbFromOrigin.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("From Origin", "FixedGraphics_Line", 20, 20);
                rdbFromOrigin.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                rdbThroughOrigin.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Through Origin", "FixedGraphics_Line", 20, 20);
                rdbThroughOrigin.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                rdbXoffset.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("X Offset", "FixedGraphics_Line", 20, 20);
                rdbXoffset.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                rdbYOffset.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Y Offset", "FixedGraphics_Line", 20, 20);
                rdbYOffset.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnCircle.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Fixed Circle", "FixedGraphics_Circle", 30, 30);
                btnCircle.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnCircle.Tag = new string[] { "FixedGraphics", GlobalSettings.SettingsReader.ButtonToolTip, "Fixed Circle" };
                btnCircle.Click += new RoutedEventHandler(FixedGraphics_Click);
                rdbDiameter.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedDiameter", "FixedGraphics_Circle", 30, 30);
                rdbDiameter.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnText.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("Fixed Text", "FixedGraphics_Text", 30, 30);
                btnText.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnText.Tag = new string[] { "FixedGraphics", GlobalSettings.SettingsReader.ButtonToolTip, "Fixed Text" };
                btnText.Click += new RoutedEventHandler(FixedGraphics_Click);
                btnInsertCharacter.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedTextInsertChar", "FixedGraphics_Text", 30, 30);
                btnInsertCharacter.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                btnInsertCharacter.Tag = new string[] { "FixedGraphics", GlobalSettings.SettingsReader.ButtonToolTip, "FixedTextInsertChar" };
                btnInsertCharacter.Click += new RoutedEventHandler(FixedGraphics_Click);
                btnAppendCurrentTime.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedTextInsertChar", "FixedGraphics_Text", 30, 30);
                btnAppendCurrentTime.ToolTip = "Append Current Time";
                btnAppendCurrentTime.Click += new RoutedEventHandler(btnAppendCurrentTime_Click);
                

                RWrapper.RW_FixedGraph.MYINSTANCE().ClearFixedShapeSelection += new RWrapper.RW_FixedGraph.FixedGraphEventHandler(FixedGraphicsPanel_ClearFixedShapeSelection);
                RWrapper.RW_FixedGraph.MYINSTANCE().UpdateSelectedShapeParam += new RWrapper.RW_FixedGraph.FixedGraphEventHandler(FixedGraphicsPanel_UpdateSelectedShapeParam);

                //ResetDateTimeInTxtBx();
                //this.IsVisibleChanged += new DependencyPropertyChangedEventHandler(this_IsVisibleChanged);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG02", ex); }
        }

        private void FixedGraphics_Checked(object sender, RoutedEventArgs e)
        {
            try
            {
                if (FixedGrapicsMode) return;
                if (sender.GetType() == typeof(RadioButton))
                {
                    RadioButton rb = (RadioButton)sender;
                    if (!(bool)rb.IsChecked)
                        return;
                }
                if (sender.Equals(rdbFromOrigin) || sender.Equals(rdbThroughOrigin))
                {
                    txtLineOffset.Text = "";
                    txtLineOffset.IsEnabled = false;
                }
                else
                {
                    txtLineOffset.IsEnabled = true;
                }
                double RadDia = 0;
                double Angle = 0;
                double Offset = 0;
                double CenterX = 0;
                double CenterY = 0;
                Double.TryParse(txtCircleRadDia.Text, out RadDia);
                Angle = LineAngleDegree.Value + LineAngleMin.Value / 60;
                Double.TryParse(txtLineOffset.Text, out Offset);
                Double.TryParse(txtcircleX.Text, out CenterX);
                Double.TryParse(txtCircleY.Text, out CenterY);

                if (sender.Equals(rdbFromOrigin))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged(false, true, false, false, Angle, Offset);
                }
                else if (sender.Equals(rdbDiameter))
                {
                    if ((bool)rdbDiameter.IsChecked)
                    {
                        rdbDiameter.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedDiameter", "FixedGraphics_Circle", 30, 30);
                        rdbDiameter.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                    }
                    else
                    {
                        rdbDiameter.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedRadius", "FixedGraphics_Circle", 30, 30);
                        rdbDiameter.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                    }
                    //RWrapper.RW_FixedGraph.MYINSTANCE().CircleCheckedChanged(false, RadDia);
                    RWrapper.RW_FixedGraph.MYINSTANCE().CircleParameterChanged(CenterX, CenterY, RadDia, (bool)rdbDiameter.IsChecked);
                }
                else if (sender.Equals(rdbThroughOrigin))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged(true, false, false, false, Angle, Offset);
                }
                else if (sender.Equals(rdbXoffset))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged(false, false, true, false, Angle, Offset);
                }
                else if (sender.Equals(rdbYOffset))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged(false, false, false, true, Angle, Offset);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG03", ex);
            }
        }
        private void FixedGraphics_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                if (FixedGrapicsMode) return;
                double RadDia = 0;
                Double.TryParse(txtCircleRadDia.Text, out RadDia);
                double Angle = 0;
                double Offset = 0;
                Angle = LineAngleDegree.Value + LineAngleMin.Value / 60;
                Double.TryParse(txtLineOffset.Text, out Offset);
                double CenterX = 0;
                double CenterY = 0;
                Double.TryParse(txtcircleX.Text, out CenterX);
                Double.TryParse(txtCircleY.Text, out CenterY);

                if (sender.Equals(txtLineOffset))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged((bool)rdbThroughOrigin.IsChecked,
                        (bool)rdbFromOrigin.IsChecked, (bool)rdbXoffset.IsChecked,
                        (bool)rdbXoffset.IsChecked, Angle, Offset);
                }
                else if (sender.Equals(txtCircleRadDia))
                {
                    //RWrapper.RW_FixedGraph.MYINSTANCE().CircleCheckedChanged((bool)rdbDiameter.IsChecked, RadDia);
                    RWrapper.RW_FixedGraph.MYINSTANCE().CircleParameterChanged(CenterX, CenterY, RadDia, (bool)rdbDiameter.IsChecked);
                }
                else if (sender.Equals(txtcircleX))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().CircleParameterChanged(CenterX, CenterY, RadDia, (bool)rdbDiameter.IsChecked);
                }
                else if (sender.Equals(txtCircleY))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().CircleParameterChanged(CenterX, CenterY, RadDia, (bool)rdbDiameter.IsChecked);
                }
                else if (sender.Equals(txtText))
                {
                    RWrapper.RW_FixedGraph.MYINSTANCE().TextParameterChanged(txtText.Text, 0);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG04", ex);
            }

        }
        private void LineAngle_ValueChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                if (FixedGrapicsMode) return;
                double Angle = 0;
                double Offset = 0;
                Angle = LineAngleDegree.Value + LineAngleMin.Value / 60;
                Double.TryParse(txtLineOffset.Text, out Offset);

                RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged((bool)rdbThroughOrigin.IsChecked,
                        (bool)rdbFromOrigin.IsChecked, (bool)rdbXoffset.IsChecked,
                        (bool)rdbXoffset.IsChecked, Angle, Offset);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG05", ex);
            }
        }
        private void FixedGraphics_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (FixedGrapicsMode) return;
                ContentControl b = (ContentControl)sender;
                string[] TagStr = (string[])b.Tag;
                switch (TagStr[2])
                {
                    case "Fixed Line":
                    case "Fixed Circle":
                    case "Fixed Text":
                        SetCheckState_of_HatchedBtn(false);
                        RWrapper.RW_FixedGraph.MYINSTANCE().HandleFixedGraphics(TagStr[2]);
                        RadioButton rb = new RadioButton();
                        rb.Style = GlobalSettings.RadioButtonStyle;
                        rb.Click += new RoutedEventHandler(FixedGraphicsList_Click);
                        rb.Background = null;
                        rb.Content = RWrapper.RW_FixedGraph.MYINSTANCE().FixedShapeName;
                        rb.Height = 30;
                        rb.Width = 30;
                        rb.Tag = TagStr[2];
                        FixedGShpScrList.ScrT.Children.Add(rb);
                        rb.IsChecked = true;
                        if (TagStr[2] == "Fixed Line")
                        {
                            double Angle = 0;
                            double Offset = 0;
                            Angle = LineAngleDegree.Value + LineAngleMin.Value / 60;
                            Double.TryParse(txtLineOffset.Text, out Offset);
                            RWrapper.RW_FixedGraph.MYINSTANCE().LineParameterChanged((bool)rdbThroughOrigin.IsChecked,
                                (bool)rdbFromOrigin.IsChecked, (bool)rdbXoffset.IsChecked,
                                (bool)rdbXoffset.IsChecked, Angle, Offset);
                        }
                        else if (TagStr[2] == "Fixed Circle")
                        {
                            double RadDia = 0;
                            Double.TryParse(txtCircleRadDia.Text, out RadDia);
                            double CenterX = 0;
                            double CenterY = 0;
                            Double.TryParse(txtcircleX.Text, out CenterX);
                            Double.TryParse(txtCircleY.Text, out CenterY);
                            RWrapper.RW_FixedGraph.MYINSTANCE().CircleParameterChanged(CenterX, CenterY, RadDia, (bool)rdbDiameter.IsChecked);
                        }
                        else if (TagStr[2] == "Fixed Text")
                        {
                            RWrapper.RW_FixedGraph.MYINSTANCE().TextParameterChanged(txtText.Text, 0);
                        }
                        break;
                    case "Delete Fixed Shape":
                        for (int i = 0; i < FixedGShpScrList.ScrT.Children.Count; i++)
                        {
                            RadioButton MyRbs = (RadioButton)FixedGShpScrList.ScrT.Children[i];
                            if ((bool)MyRbs.IsChecked)
                            {
                                RWrapper.RW_FixedGraph.MYINSTANCE().RemoveSelected(MyRbs.Content.ToString());
                                FixedGShpScrList.ScrT.Children.RemoveAt(i);
                                return;
                            }
                        }
                        break;
                    case "Fixed Shape Color":
                        RWrapper.RW_FixedGraph.MYINSTANCE().HandleFixedGraphics(TagStr[2]);
                        break;
                    case "Clear All Fixed Shape":
                        FixedGShpScrList.ScrT.Children.Clear();
                        ((ToggleButton)ButtonsPanel.Children[0]).IsChecked = false;
                        ((ToggleButton)ButtonsPanel.Children[1]).IsChecked = false;
                        ((ToggleButton)rdbDiameter).IsChecked = true;
                        RWrapper.RW_FixedGraph.MYINSTANCE().HandleFixedGraphics(TagStr[2]);
                        break;
                    case "Fixed Hatced Graphics":
                        RWrapper.RW_FixedGraph.MYINSTANCE().HandleFixedGraphics(TagStr[2]);
                        break;
                    case "Non-Clearable Fixed Shape":
                        if ((bool)((ToggleButton)b).IsChecked)
                            RWrapper.RW_FixedGraph.MYINSTANCE().DontClear = true;
                        else
                            RWrapper.RW_FixedGraph.MYINSTANCE().DontClear = false;
                        break;
                    case "Fixed Graphics Nudge":
                        RWrapper.RW_FixedGraph.MYINSTANCE().HandleFixedGraphics(TagStr[2]);
                        break;
                    case "FixedTextInsertChar":
                        ////Define the variable
                        //Object WsShell;

                        ////Set the varible
                        //WsShell = CreateObject("WScript.Shell");

                        ////Run the variable
                        //WsShell.Run("charmap");

                        ////Clean up objects
                        //WsShell = null;
                        try
                        {
                            System.Diagnostics.Process.Start("charmap.exe");
                        }
                        catch (System.ComponentModel.Win32Exception ex)
                        {
                            RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG04", ex);           // show message
                        }

                        break;
                    case "Import Fixed DXF":
                        RWrapper.RW_MainInterface.MYINSTANCE().DXFImport(true);
                        break;

                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG06", ex);
            }
        }
        private void FixedGraphicsList_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RadioButton rb = (RadioButton)sender;
                if (!(bool)rb.IsChecked)
                    return;
                FixedGrapicsMode = true;
                string SelectedToolPanel = rb.Tag.ToString();
                RWrapper.RW_FixedGraph.MYINSTANCE().SelectFixedShape(rb.Content.ToString());
                SetCheckState_of_HatchedBtn(RWrapper.RW_FixedGraph.MYINSTANCE().Hatched);
                SetCheckState_of_NonClearableBtn(RWrapper.RW_FixedGraph.MYINSTANCE().NonClearable);
                switch (SelectedToolPanel)
                {

                    case "Fixed Line":
                        {
                            rdbFromOrigin.IsChecked = RWrapper.RW_FixedGraph.MYINSTANCE().Line_FromOrigin;
                            rdbThroughOrigin.IsChecked = RWrapper.RW_FixedGraph.MYINSTANCE().Line_ThroughOrigin;
                            rdbXoffset.IsChecked = RWrapper.RW_FixedGraph.MYINSTANCE().Line_Xoffset;
                            rdbYOffset.IsChecked = RWrapper.RW_FixedGraph.MYINSTANCE().Line_Yoffset;
                            LineAngleDegree.Value = (int)RWrapper.RW_FixedGraph.MYINSTANCE().Angle;
                            LineAngleMin.Value = (int)((RWrapper.RW_FixedGraph.MYINSTANCE().Angle - (int)RWrapper.RW_FixedGraph.MYINSTANCE().Angle) * 60);
                            txtLineOffset.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().Offset);
                        }
                        break;
                    case "Fixed Circle":
                        {
                            rdbDiameter.IsChecked = RWrapper.RW_FixedGraph.MYINSTANCE().Circle_DiameterType;
                            if ((bool)rdbDiameter.IsChecked)
                            {
                                rdbDiameter.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedDiameter", "FixedGraphics_Circle", 30, 30);
                                rdbDiameter.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                            }
                            else
                            {
                                rdbDiameter.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("FixedRadius", "FixedGraphics_Circle", 30, 30);
                                rdbDiameter.ToolTip = GlobalSettings.SettingsReader.ButtonToolTip;
                            }
                            //rdbRadius.IsChecked = RWrapper.RW_FixedGraph.MYINSTANCE().Circle_RadiusType;
                            txtcircleX.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().CenterX);
                            txtCircleY.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().CenterY);
                            txtCircleRadDia.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().Radius);
                        }
                        break;
                    case "Fixed Text":
                        txtText.Text = RWrapper.RW_FixedGraph.MYINSTANCE().CurrentText;
                        break;
                }
                FixedGrapicsMode = false;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG07", ex); }
        }
        private void FixedGraphicsPanel_ClearFixedShapeSelection()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    for (int i = 0; i < FixedGShpScrList.ScrT.Children.Count; i++)
                    {
                        ((RadioButton)FixedGShpScrList.ScrT.Children[i]).IsChecked = false;
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_FixedGraph.FixedGraphEventHandler(FixedGraphicsPanel_ClearFixedShapeSelection));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG08", ex); }
        }
        private void FixedGraphicsPanel_UpdateSelectedShapeParam()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    FixedGrapicsMode = true;
                    for (int i = 0; i < FixedGShpScrList.ScrT.Children.Count; i++)
                    {
                        RadioButton rb = (RadioButton)FixedGShpScrList.ScrT.Children[i];
                        if (!(bool)rb.IsChecked) continue;
                        string SelectedToolPanel = rb.Tag.ToString();
                        switch (SelectedToolPanel)
                        {

                            case "Fixed Line":
                                {
                                    txtLineOffset.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().Offset);

                                    txtcircleX.Text = "";
                                    txtCircleY.Text = "";
                                }
                                break;
                            case "Fixed Circle":
                                {
                                    txtLineOffset.Text = "";

                                    txtcircleX.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().CenterX);
                                    txtCircleY.Text = String.Format("{0:0.0000}", RWrapper.RW_FixedGraph.MYINSTANCE().CenterY);
                                }
                                break;
                        }
                        break;
                    }
                    FixedGrapicsMode = false;
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_FixedGraph.FixedGraphEventHandler(FixedGraphicsPanel_UpdateSelectedShapeParam));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG09", ex); }
        }
        private void btnAppendCurrentTime_Click(object sender, RoutedEventArgs e)
        {
            ResetDateTimeInTxtBx();
        }
        private void SetCheckState_of_HatchedBtn(bool ChkState)
        {
            for (int i = 0; i < ButtonsPanel.Children.Count; i++)
            {
                if (((string[])((ContentControl)ButtonsPanel.Children[i]).Tag)[2] == "Fixed Hatced Graphics")
                    ((ToggleButton)ButtonsPanel.Children[i]).IsChecked = ChkState;
            }
        }
        private void SetCheckState_of_NonClearableBtn(bool ChkState)
        {
            for (int i = 0; i < ButtonsPanel.Children.Count; i++)
            {
                if (((string[])((ContentControl)ButtonsPanel.Children[i]).Tag)[2] == "Non-Clearable Fixed Shape")
                    ((ToggleButton)ButtonsPanel.Children[i]).IsChecked = ChkState;
            }
        }
        
        public void ResetDateTimeInTxtBx()
        {
            try
            {
                DateTime TmpDT = DateTime.Now;
                txtText.Text = txtText.Text + " - " + TmpDT.Date.ToString("dd MMM yyyy") + ", " + TmpDT.ToShortTimeString();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG10", ex); }
        }
        public void ClearAll()
        {
            try
            {
                TextBox[] fixedgraphicstexts = { txtText, txtCircleRadDia, txtcircleX,
                                                    txtCircleY,txtLineOffset};
                for (int i = 0; i < fixedgraphicstexts.Length; i++)
                    fixedgraphicstexts[i].Text = "";
                LineAngleDegree.Value = 0;
                LineAngleMin.Value = 0;

                ((ToggleButton)ButtonsPanel.Children[0]).IsChecked = false;
                ((ToggleButton)ButtonsPanel.Children[1]).IsChecked = false;
                rdbFromOrigin.IsChecked = false;
                rdbDiameter.IsChecked = true;
                rdbXoffset.IsChecked = false;
                rdbYOffset.IsChecked = false;
                rdbThroughOrigin.IsChecked = true;
                System.Collections.Hashtable FShpHashTable = RWrapper.RW_FixedGraph.MYINSTANCE().FixedShapeCollection;
                for (int i = 0; i < FixedGShpScrList.ScrT.Children.Count; i++)
                {
                    if (FShpHashTable[((RadioButton)FixedGShpScrList.ScrT.Children[i]).Content.ToString()] != null)
                        continue;
                    FixedGShpScrList.ScrT.Children.RemoveAt(i);
                    i--;
                }
                //FixedGShpScrList.ScrT.Children.Clear();
                //ResetDateTimeInTxtBx();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FG11", ex); }
        }
    }
}