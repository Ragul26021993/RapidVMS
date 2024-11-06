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
using System.Windows.Navigation;
using System.Windows.Shapes;
using RapidI.Help;

namespace Rapid.Panels
{
	/// <summary>
	/// Interaction logic for LightingPanel.xaml
	/// </summary>
	public partial class LightingPanel : Grid
	{
        Brush LightOnBG, LightOnBackground, LightOffBG, LightOffBackground;
        //to determine whether lighing width/height & gaps should be changed for the UI.
        private bool ChangeLightingCNCValues { get; set; }
        string[] Lightingchannel = { "R", "G", "B" };
        //Constructor
		public LightingPanel()
		{
            try
            {
                this.InitializeComponent();
                LightOnBG = (Brush)TryFindResource("LightButtonOnBrush_BG");
                LightOnBackground = (Brush)TryFindResource("LightButtonOnBrush_Border");
                LightOffBG = (Brush)TryFindResource("CloseButtonBrush_BG");
                LightOffBackground = (Brush)TryFindResource("CloseButtonBrush_Border");

                btnLock.Content = null;
                btnLock.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("LightingUnlock", "Lighting", 32, 32);
                btnLock.ToolTip = "Click to Lock";
                btnLock.Tag = new string[] { "Lighting", "Unlock", "LightingLock" };
                btnLock.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                this.Background = new SolidColorBrush(Color.FromArgb(01, 255, 255, 255));

                btnSave.Content = null;
                btnSave.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("LightingSave", "Lighting", 32, 32);
                btnSave.ToolTip = "Save";
                btnDelete.Content = null;
                btnDelete.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("LightingDelete", "Lighting", 32, 32);
                btnDelete.ToolTip = "Delete";
                btnPPLightingControl.Content = null;
                btnPPLightingControl.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("PPLighting", "Lighting", 32, 32);
                btnPPLightingControl.ToolTip = "Change Lighting in Part Program";
                btnPPLightingControl.Visibility = Visibility.Hidden;

                btnONOFF_Click(btnONOFF, null);

                init();
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI01", ex); }
		}
        //Initialisation
        void init()
        {
            try
            {
                //Handling the change in lighting values.
                RWrapper.RW_CNC.MYINSTANCE().LightCommandUpdated += new RWrapper.RW_CNC.CNCEventHandler(HandleLightUpdated);
                btnONOFF.Tag = new string[] { "Lighting", "Tooltip", "ON/OFF" };
                btnONOFF.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                MyLighting.Width = 215; MyLighting.Height = 215;
                if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot || GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                {
                    MyLighting.OneShotLighting = true;
                    btnLock.Visibility = System.Windows.Visibility.Hidden;

                    MyLighting.Width = 180;
                    MyLighting.Height = 180;
                }
                else
                {
                    MyLighting.IsAuxillaryLightingPresent = GlobalSettings.AuxillaryLightPresent;
                    MyLighting.IsCoAxialLightingPresent = GlobalSettings.CoAxialLightPresent;
                    MyLighting.TotalCircles = 2;
                    MyLighting.ShapeThickness = 32;
                    if (GlobalSettings.Light_Zone_Nos[0] + 1 <= 8)
                        MyLighting.TotalCircles = 2;
                    else if (GlobalSettings.Light_Zone_Nos[0] + 1 <= 12)
                        MyLighting.TotalCircles = 3;
                    else if (GlobalSettings.Light_Zone_Nos[0] + 1 <= 16)
                        MyLighting.TotalCircles = 4;
                    else if (GlobalSettings.Light_Zone_Nos[0] + 1 <= 20)  
                        MyLighting.TotalCircles = 5;
                    else if (GlobalSettings.Light_Zone_Nos[0] + 1 <= 32)
                        MyLighting.TotalCircles = 6;
                    if (MyLighting.TotalCircles > 4)
                    {
                        //btnONOFF.Width = 40; btnONOFF.Height = 40;
                        //btnONOFF.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
                        //btnONOFF.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                        //btnONOFF.Margin = new Thickness(6, 0, 0, 0);
                        //btnONOFF.FontSize = 14;
                        MyLighting.ShapeThickness = 18;
                        MyLighting.HorizontalAlignment = System.Windows.HorizontalAlignment.Left; MyLighting.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                        MyLighting.Width = 320; MyLighting.Height = 320;
                        //this.Width = 456; this.Height = 328;
                    }
                    //MyLighting.TotalCircles = 
                }
                //btnONOFF.Click += new RoutedEventHandler(HandleLowerPanelsButtonsClick);
                MyLighting.ValueChanged += new EventHandler(MyLighting_ValueChanged);
                if (this.ChangeLightingCNCValues)
                {
                    MyLighting.ShapeThickness = 20;
                    btnONOFF.Width = 45;
                    btnONOFF.Height = 45;
                    btnONOFF.FontSize = 16;
                    MyLighting.Width = 190;
                    MyLighting.Height = 190;
                }
                LightingSettingsCBx.ItemsSource = GlobalSettings.LightingConfigNameList;
                LightingSettingsCBx.SelectionChanged += new SelectionChangedEventHandler(cbSettings_SelectionChanged);
                LightingSettingsCBx.TextInput += new TextCompositionEventHandler(LightingSettingsCBx_TextInput);
                LightingChannelCbx.ItemsSource = Lightingchannel;
                LightingChannelCbx.SelectedIndex = 1;
                LightingChannelCbx.SelectionChanged +=new SelectionChangedEventHandler(LightingChannelCbx_SelectionChanged);
                btnSave.Click += new RoutedEventHandler(btnSave_Click);
                btnDelete.Click += new RoutedEventHandler(btnDelete_Click);
                btnPPLightingControl.Click += new RoutedEventHandler(btnPPLighting_Click);
                btnSave.Tag = new string[] {"Lighting","","LightingSave"};
                btnDelete.Tag = new string[] { "Lighting","", "LightingDelete" };
                btnSave.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);
                btnDelete.PreviewMouseRightButtonDown += new MouseButtonEventHandler(btn_PreviewMouseRightButtonDown);

                System.Data.DataRow[] sdsd = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings"); //[0];

                nud_Surface.Value = (int)sdsd[0][7];
                nud_Profile.Value = (int)sdsd[0][8];

                //Initialise in Framework
                RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(false, (int)nud_Profile.Value);
                RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(true, (int)nud_Surface.Value);

                //Do this to get proper height & width
                Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI02", ex); }
        }

        //Handling the On-Off button click.
        private void btnONOFF_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button b = (Button)sender;
                if (b.Content.ToString() == "ON")
                {
                    b.Background = LightOffBG;
                    b.BorderBrush = LightOffBackground;
                    b.Content = "OFF";
                    RWrapper.RW_CNC.MYINSTANCE().SwitchOnOffLight(false);
                    LightingSettingsCBx.Text = "";
                }
                else
                {
                    b.Background = LightOnBG;
                    b.BorderBrush = LightOnBackground;
                    b.Content = "ON";
                    RWrapper.RW_CNC.MYINSTANCE().SwitchOnOffLight(true);
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI03", ex); }
        }


        //Updated By Vikas
        private void MyLighting_ValueChanged(object sender, EventArgs e)
        {
            try
            {
                if (btnONOFF.Content.ToString() == "OFF") return;
                int[] tmpVals = new int[GlobalSettings.LightZonesForThisController]; // { MyLighting.Values[0], 0, 0, 0, MyLighting.Values[1], 0, 0, 0 };
                ///tempVals[8]
                //if (GlobalSettings.RapidMachineType != GlobalSettings.MachineType.One_Shot)
                //{
                    int index = GlobalSettings.LightZonesForThisController - 1; //= MyLighting.TotalCircles - 4;
                          ///index = 8-1 =7
                          ///Now This Module is Running! Have to check the value.
                if (index > 7)//False
                {




                    
                    //if (MyLighting.IsCoAxialLightingPresent) //if true 
                    //    index = MyLighting.Values.Length - 4;//index = 3 [0,1,2,3]
                    //else
                    //    index = MyLighting.Values.Length - 3;//else index = 4 [0,1,2,3,4]

                    //for (int j = 0; j < index; j++)
                    //    tmpVals[j] = MyLighting.Values[j];

                    //for (int j = 0; j < 4; j++)
                    //{
                    //    tmpVals[GlobalSettings.Light_Zone_Nos[j]] = MyLighting.Values[index++];
                    //    if (index == MyLighting.Values.Length) break;
                    //}
                    ////Since the CoAxial Light and Profile Light is Already Working SO what we can do is we
                    //can write the overall factor again
                    for (int i = 0; i < index; i++) 
                    {
                        tmpVals[GlobalSettings.TheLightArrayfromdatabase[i]] = MyLighting.Values[i];
                    }
                                    }
                else//True Case While Mind Debugging Stuff.
                {
                    //Idk It might be true no one knows
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot)
                    {
                        if (MyLighting.IsCoAxialLightingPresent)
                            index = MyLighting.Values.Length - 2;
                        else
                            index = MyLighting.Values.Length - 1;

                        tmpVals[0] = MyLighting.Values[0];

                        for (int j = 0; j < 4; j++)
                        {
                            tmpVals[GlobalSettings.Light_Zone_Nos[j]] = MyLighting.Values[index++];
                            if (index == MyLighting.Values.Length) break;
                        }
                    }
                    else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            tmpVals[j] = MyLighting.Values[0];
                            if (index == MyLighting.Values.Length) break;
                            
                        }
                    }
                    else
                    {
                        //This model is responsible for the filling of Temp Values.
                        for (int j = 0; j < MyLighting.Values.Length; j++)
                            tmpVals[GlobalSettings.TheLightArrayfromdatabase[j]] = MyLighting.Values[j];
                        
                    }
                }
                //}
                //This is the place values going for the Update and there is Light Protocal.
                
                RWrapper.RW_CNC.MYINSTANCE().UpdateLight(tmpVals);
                RWrapper.RW_CNC.MYINSTANCE().CalcualteSurfaceorProfileLight(tmpVals);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI04", ex); }

        }

        #region Lighting
        //handling the delete button click
        void btnDelete_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (LightingSettingsCBx.Items.Contains(LightingSettingsCBx.Text))
                {
                    GlobalSettings.SettingsReader.Delete_LightSettings(LightingSettingsCBx.SelectedItem.ToString());
                    LightingSettingsCBx.ItemsSource = null;
                    LightingSettingsCBx.ItemsSource = GlobalSettings.LightingConfigNameList;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI05", ex); }
        }
        //handling the Save Settings button click
        void btnSave_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if (LightingSettingsCBx.Text != "")
                {
                    GlobalSettings.SettingsReader.Update_Or_Save_LightSettings(LightingSettingsCBx.Text, MyLighting.Values);
                    LightingSettingsCBx.ItemsSource = null;
                    LightingSettingsCBx.ItemsSource = GlobalSettings.LightingConfigNameList;
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI06", ex); }
        }


        //handling the lighting setttings selection change events
        void cbSettings_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                if (LightingSettingsCBx.SelectedIndex >= 0)
                    GlobalSettings.SettingsReader.Select_LightSettings(e.AddedItems[0].ToString());
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI07", ex); }
        }
        //handling the text input to negate the selection
        void LightingSettingsCBx_TextInput(object sender, RoutedEventArgs e)
        {
        }

        void RefreshcbSettings_Update()
        {
            try
            {
                LightingSettingsCBx.Items.Clear();
                List<string> LightSett = RWrapper.RW_DBSettings.MYINSTANCE().SavedLightSettings_NameList;
                for (int i = 0; i < LightSett.Count; i++)
                    LightingSettingsCBx.Items.Add(LightSett[i]);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI08", ex); }
        }

        
        //Handling the updation of Light values given from the Framework.
        void HandleLightUpdated()
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.One_Shot)
                    {
                        int[] tmpVals = { RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty[0], RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty[4], 0, 0, 0, 0 };
                        MyLighting.UpdateValues(tmpVals);
                    }
                    else if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.OnlineSystem)
                    {
                        int[] tmpVals = { 4, 4, 4, 4, 4, 4, 4 ,4 };
                        for (int i = 0; i < tmpVals.Length; i++)
                            tmpVals[i] = RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty[0];
                        MyLighting.UpdateValues(tmpVals);
                    }
                    else
                    {
                        int[] temparray = new int[RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty.Length];
                        for (int i = 0; i < RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty.Length; i++) 
                        {
                            temparray[i] = RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty[i];
                        }
                        //Transfer
                        for(int i = 0; i < RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty.Length; i++)
                            RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty[GlobalSettings.TheLightArrayfromdatabase[i]] = temparray[i];
                        

                        
                        //From here the data will go.
                        
                        MyLighting.UpdateValues(RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty);
                        //MessageBox.Show(RWrapper.RW_CNC.MYINSTANCE().CurrentLightProperty.Length.ToString());
                    }
                }
                else
                {
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_CNC.CNCEventHandler(HandleLightUpdated));
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI09", ex); }
        }


        #endregion
        
        private void btnLock_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Button b = (Button)sender;
                if (((string[])b.Tag)[1] == "Unlock")
                {
                    b.Content = null;
                    b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("LightingLock", "Lighting", 32, 32);
                    b.ToolTip = "Click to Unlock";
                    MyLighting.IsLocked = true;
                    ((string[])b.Tag)[1] = "Lock";
                }
                else
                {
                    b.Content = null;
                    b.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo("LightingUnlock", "Lighting", 32, 32);
                    b.ToolTip = "Click to Lock";
                    MyLighting.IsLocked = false;
                    ((string[])b.Tag)[1] = "Unlock";
                }
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI10", ex); }
        }

        protected override void OnPreviewKeyDown(KeyEventArgs e)
        {
            try
            {
                if (MyLighting.EnterText.Visibility == System.Windows.Visibility.Visible && e.Key == Key.Escape)
                    MyLighting.EnterText.Visibility = System.Windows.Visibility.Hidden;
                base.OnPreviewKeyDown(e);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI11", ex); }
        }
        protected override void OnMouseLeftButtonUp(MouseButtonEventArgs e)
        {
            try
            {
                if (MyLighting.EnterText.Visibility == System.Windows.Visibility.Visible)
                    MyLighting.EnterText.Visibility = System.Windows.Visibility.Hidden;
                base.OnMouseLeftButtonUp(e);
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI12", ex); }
        }

        public void ClearAll()
        {
            LightingSettingsCBx.Text = "";
            //RWrapper.RW_MainInterface.MYINSTANCE().SetRGBChannelColor(1);
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
        void LightingChannelCbx_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                //if (LightingChannelCbx.SelectedIndex >= 0)
                //    RWrapper.RW_MainInterface.MYINSTANCE().SetRGBChannelColor(LightingChannelCbx.SelectedIndex);
            }
            catch (Exception ex)
            { 
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI14", ex); 
            }
        }


        void btnPPLighting_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                //GlobalSettings.PPLightControlWin.ShowDialog();
                gLightControl.Visibility = Visibility.Visible;
            }
            catch (Exception ex)
            { RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:LI06a", ex); }
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(true, (int)nud_Surface.Value);
            RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(false, (int)nud_Profile.Value);
            System.Data.DataRow[] sdsd = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings"); //[0];
            sdsd[0][7] = (int)nud_Surface.Value; sdsd[0][8] = (int)nud_Profile.Value;
            GlobalSettings.SettingsReader.UpdateTable("LightingSettings");
            gLightControl.Visibility = Visibility.Hidden;
        }

    }
} 