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
using Rapid.AttachedProperties;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for DelfiRotaryContainerWindow.xaml
    /// </summary>
    public partial class DelfiRotaryContainerWindow : Window
    {
        Brush BGColor;
        private static DelfiRotaryContainerWindow _MyInstance;
        public static DelfiRotaryContainerWindow MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new DelfiRotaryContainerWindow();
            }
            return _MyInstance;
        }

        private DelfiRotaryContainerWindow()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(DelfiRotaryContainerWindow_Loaded);
        }

        public TextBlock tbb;

        private void DelfiRotaryContainerWindow_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                AddAdditionalButtons();
                BGColor = (Brush)this.TryFindResource("BorderBrush_Title");
                GlobalSettings.MainWin.DropDownPanel.Visibility = System.Windows.Visibility.Visible;
                GlobalSettings.MainWin.DropDownPanel.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;

                if (GlobalSettings.MainWin.Video__Grid.Children.Contains(GlobalSettings.MainWin.DropDownPanel))
                {
                    GlobalSettings.MainWin.Video__Grid.Children.Remove(GlobalSettings.MainWin.DropDownPanel);
                    this.DropDownPanelHost.Children.Add(GlobalSettings.MainWin.DropDownPanel);
                }
                else
                    this.DropDownPanelHost.Children.Add(GlobalSettings.MainWin.DropDownPanel);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelfiRotaryContainerWindow01", ex);
            }
        }
        public void AddAdditionalButtons()
        {
            RadioButton RBtn;
            System.Data.DataRow[] DRow = GlobalSettings.SettingsReader.GetRowsAccordingToSelectedMachineNumber("RotaryMeasurementTypeDetails", GlobalSettings.MachineNo);
            for (int i = 0; i < DRow.Length; i++)
            {
                RBtn = new RadioButton();
                string ComponentName = DRow[i]["ComponentName"].ToString();
                string ZeroingAngle = DRow[i]["ZeroingAngle"].ToString();
                RBtn.MinWidth = 60;
                RBtn.Height = 70;
                RBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_RadioButton2");
                RBtn.Content = ComponentName;
                RBtn.Tag = new ButtonProperties("ZeroingAngle", false, true, ZeroingAngle);
                RBtn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(OnButtonClick);
                RBtn.Margin = new Thickness(5, 0, 0, 0);
                RBtn.FontWeight = FontWeights.Bold;
                RBtn.FontSize = 30;
                ButtonHost.Children.Add(RBtn);
            }
            if (DRow.Length > 0)
            {
                string[] Content = { "Zero", "Goto Offset Position" };
                string[] BtnName = { "Finalize Zeroing Angle", "Goto Offset Position" };

                for (int i = 0; i < Content.Length; i++)
                {
                    Button Btn = new Button();
                    Btn.Height = 70;
                    Btn.FontSize = 30;
                    Btn.Content = Content[i];
                    Btn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                    Btn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(OnButtonClick);
                    Btn.FontWeight = FontWeights.Bold;
                    Btn.Tag = new ButtonProperties(BtnName[i], false, true, "Rotary Measurement");
                    Btn.Margin = new Thickness(5, 0, 0, 0);
                    ButtonHost.Children.Add(Btn);
                }
            }
            tbb = new TextBlock();
            tbb.Width = ButtonHost.Width;
            tbb.Margin = new Thickness(4, 12, 4, 4);
            tbb.Text = "";
            tbb.FontSize = 20;
            ButtonHost.Children.Add(tbb);
        }
        private void OnButtonClick(object Sender, RoutedEventArgs e)
        {
            try
            {

                ContentControl Btn = (ContentControl)Sender;
                string BtnName = ((ButtonProperties)Btn.Tag).ButtonName;
                System.Data.DataRow[] Drow = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("ZeroingOffset");
                double[] Offset = { 0, 0, 0 };
                if (Drow.Length == 0)
                {
                    Offset[0] = 0;
                    Offset[1] = 0;
                    Offset[2] = 0;
                }
                else
                {
                    Offset[0] = Convert.ToDouble(Drow[0]["OffsetX"]);
                    Offset[1] = Convert.ToDouble(Drow[0]["OffestY"]);
                    Offset[2] = Convert.ToDouble(Drow[0]["OffsetZ"]);
                }

                switch (BtnName)
                {
                    case "ZeroingAngle":
                        RadioButton Rbtn = (RadioButton)Btn;
                        ButtonProperties BtnProp = (ButtonProperties)Btn.Tag;
                        double ZeroingAngle = 0.0;
                        Double.TryParse(BtnProp.ToolBarName, out ZeroingAngle);
                        GlobalSettings.ZeroingAngle = ZeroingAngle;
                        break;
                    case "Finalize Zeroing Angle":
                        double rvalue = 0;
                        if (GlobalSettings.DROIsLinearList[3])
                        {
                            rvalue = GlobalSettings.ZeroingAngle;
                        }
                        else
                        {
                            rvalue = GlobalSettings.ZeroingAngle * Math.PI / 180;
                        }
                        double[] RefDot = { 0, 0, 0, 0 };
                        double[] target = { 0, 0, 0, 0 };
                        double[] currentvalue = { Convert.ToDouble(GlobalSettings.MainWin.btnX.GetValue(AttachedDROProperties.DROTextProperty)),
                                                    Convert.ToDouble(GlobalSettings.MainWin.btnY.GetValue(AttachedDROProperties.DROTextProperty)),
                                                    Convert.ToDouble(GlobalSettings.MainWin.btnZ.GetValue(AttachedDROProperties.DROTextProperty)), 0};
                        RWrapper.RW_MainInterface.MYINSTANCE().GetRefDotParams(0, RefDot);
                        for (int ii = 0; ii < 3; ii ++)
                        {
                            target[ii] = Offset[ii] + RefDot[ii];// -currentvalue[ii];
                        }
                        double[] DroValue = { target[0], target[1], target[2], rvalue };
                        RWrapper.RW_DRO.MYINSTANCE().ResetDroValue(DroValue, true);

                        break;
                    case "Goto Offset Position":
                        RWrapper.RW_DRO.MYINSTANCE().SendMachineAtZeroingPosition(Offset);
                        break;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelfiRotaryContainerWindow02", ex);
            }
        }

        private void btnSaveOffset_Click(object Sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_DRO.MYINSTANCE().ResetOffsetPosition();
            }
            catch (Exception ex)
            {

                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelfiRotaryContainerWindow003", ex);
            }
        }

        void btnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                e.Cancel = true;
                this.Hide();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:DelfiRotaryContainerWindow003", ex);
            }

        }
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);
            if (e.ButtonState == MouseButtonState.Pressed)
                DragMove();
        }
    }
}
