using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using Rapid.AttachedProperties;

namespace Rapid.Utilities
{
    public class ButtonProperties
    {
        public string ButtonName, ResetTo_BtnName, ToolBarName;
        public bool InitialChkStatus, RevertToInitialStatus;
        public int FeatureID;
        public bool IsRarelyUsed = false;
        public bool RememberLastState = false;
        public bool LastCheckedState = false;

        public ButtonProperties(string BtnName = "Default", bool IsChk = false, bool RevertToInitial = true, string Toolbarname = "", int Fid = 0, bool isRarelyUserBtn = false, bool _RememberLastState = false, bool _LastCheckedState = false)
        {
            ButtonName = BtnName;
            ResetTo_BtnName = BtnName;
            InitialChkStatus = IsChk;
            RevertToInitialStatus = RevertToInitial;
            ToolBarName = Toolbarname;
            FeatureID = Fid;
            IsRarelyUsed = isRarelyUserBtn;
            RememberLastState = _RememberLastState;
            LastCheckedState = _LastCheckedState;
        }
    }

    public partial class ScrToolbar : Border
    {
        /// <summary>
        /// ReadOnly Dependency property for getting ScrollAmount on this panel.
        /// </summary>
        public static readonly DependencyProperty ScrollAmountProperty = DependencyProperty.Register("ScrollAmount",
            typeof(double), typeof(ScrToolbar), new PropertyMetadata(60.0D));
        /// <summary>
        /// The no of pixels to be scrolled
        /// </summary>
        public double ScrollAmount
        {
            get { return (double)GetValue(ScrollAmountProperty); }
            set { SetValue(ScrollAmountProperty, value); }
        }
        /// <summary>
        /// ReadOnly Dependency property for getting AnimateDuration property on this panel.
        /// </summary>
        public static readonly DependencyProperty AnimateDurationProperty = DependencyProperty.Register("AnimateDuration",
            typeof(int), typeof(ScrToolbar), new PropertyMetadata(400));
        /// <summary>
        /// The total duration of animation in ms(milli-seconds).
        /// </summary>
        public int AnimateDuration
        {
            get { return (int)GetValue(AnimateDurationProperty); }
            set { SetValue(AnimateDurationProperty, value); }
        }
        /// <summary>
        ///ReadOnly Dependency property for getting FontSize property on this panel.
        /// </summary>
        public static readonly DependencyProperty FontSizeProperty = DependencyProperty.Register("FontSize",
            typeof(double), typeof(ScrToolbar), new FrameworkPropertyMetadata(24.0D, FrameworkPropertyMetadataOptions.Inherits));
        /// <summary>
        /// Gets-Sets the Font Size property of this control.
        /// </summary>
        public double FontSize
        {
            get { return (double)GetValue(FontSizeProperty); }
            set { SetValue(FontSizeProperty, value); }
        }
        /// <summary>
        /// ReadOnly Dependency property for getting Foreground property on this panel.
        /// </summary>
        public static readonly DependencyProperty ForegroundProperty = DependencyProperty.Register("Foreground",
            typeof(Brush), typeof(ScrToolbar), new FrameworkPropertyMetadata(Brushes.Black, FrameworkPropertyMetadataOptions.Journal));
        /// <summary>
        /// Gets-Sets the Foreground property of this control.
        /// </summary>
        public Brush Foreground
        {
            get { return (Brush)GetValue(ForegroundProperty); }
            set { SetValue(ForegroundProperty, value); }
        }
        /// <summary>
        /// Constructor
        /// </summary>
        /// 

        public string ToolBarName; 
        public ScrToolbar()
            : base()
        {
            InitializeComponent();
            _btnLeft.Click += new System.Windows.RoutedEventHandler(HandleNavigation);
            _btnRight.Click += new System.Windows.RoutedEventHandler(HandleNavigation);
            _btnLeft.IsEnabled = false;

            //this.KeyUp += new System.Windows.Input.KeyEventHandler(ScrToolbar_KeyDown);
        }
        //Handling the clicks of the button.
        private void HandleNavigation(object sender, System.Windows.RoutedEventArgs e)
        {
            if (sender.Equals(_btnLeft)) { MainPanel.LineLeft(); }
            else { MainPanel.LineRight(); }
            SetIsEnable();
        }
        /// <summary>
        /// Gets the children collection of the panel.
        /// </summary>
        /// <returns></returns>
        public UIElementCollection Children
        {
            get { return MainPanel.Children; }
        }
        //Focusing the control when it mouse enters it.
        protected override void OnMouseEnter(System.Windows.Input.MouseEventArgs e)
        {
            this.Focus();
            this.Scr.Focus();
            base.OnMouseEnter(e);
        }
        //Setting IsEnable properties of buttons on scroll event
        protected override void OnPreviewMouseWheel(MouseWheelEventArgs e)
        {
            base.OnPreviewMouseWheel(e);
            SetIsEnable();
        }
        //Function to calculate the IsEnable property for the navigational buttons.
        private void SetIsEnable()
        {
            _btnLeft.IsEnabled = (MainPanel._offset.X > 0);
            _btnRight.IsEnabled = (MainPanel._offset.X < Scr.ScrollableWidth);
        }

        void ScrToolbar_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.System)
            {
                MainPanel.LineLeft();
            }
            else if (e.Key == Key.BrowserForward)
            {
                MainPanel.LineRight();
            }
        }

        void OnManipulationBoundaryFeedback(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
        }

        public delegate void ButtonHandler_Pointer(object obj, RoutedEventArgs e);
        private ButtonHandler_Pointer MyBtnsHandler;
        private ButtonHandler_Pointer DropdownBtnHandler;

        private void AssignSingleBtnProp(ContentControl Btn, string BaseName, string ToolTip, bool IsChk, bool RevertToInitial, string ToolBarHelpName, Image Ico, Style BtnStyle, double Width, double Height, Thickness Margin, int Fid, bool IsRarelyUsed, bool RememberLastState = false, bool LastCheckedState = false)
        {
            try
            {
                Btn.Height = Height;
                Btn.Width = Width;
                Btn.Margin = Margin;
                Btn.Style = BtnStyle;
                Btn.Content = null;
                Btn.Content = Ico;
                Btn.ToolTip = ToolTip;
                Btn.SetValue(AttachedButtonName.ButtonName, ToolTip);
                Btn.Tag = new ButtonProperties(BaseName, IsChk, RevertToInitial, ToolBarHelpName, Fid, IsRarelyUsed, RememberLastState, LastCheckedState);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB001" + "; " + BaseName + "; " + ToolTip + "; " + ToolBarHelpName, ex);
            }
        }
        public void AssignSingleBtnProp(ContentControl Btn, string BaseName, string ToolTip, Image Ico)
        {
            try
            {
                Btn.Content = null;
                Btn.Content = Ico;
                Btn.ToolTip = ToolTip;
                Btn.SetValue(AttachedButtonName.ButtonName, ToolTip);
                if (Btn.Tag == null)
                {
                    Btn.Tag = new ButtonProperties(BaseName);
                }
                else
                {
                    ButtonProperties Prop = Btn.Tag as ButtonProperties;
                    if (Prop != null)    Prop.ButtonName = BaseName;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB002", ex);
            }
        }
        public void FillScrollBarButtons(string ToolBarName, string ToolBarHelpName, ButtonHandler_Pointer BtnHandler, bool FillNameList = false, List<string> BtnNameList = null, ButtonHandler_Pointer CheckBoxClickHandler = null)
        {
            try
            {
                MyBtnsHandler = BtnHandler;
                DropdownBtnHandler = CheckBoxClickHandler;
                this.ToolBarName = ToolBarName;
                GlobalSettings.SettingsReader.UpdateBtnInfoList(ToolBarName, GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                List<Image> ImageList = GlobalSettings.SettingsReader.ButtonImageList;
                List<string> ToolTipList = GlobalSettings.SettingsReader.ButtonToolTipList;
                List<int> buttonType = GlobalSettings.SettingsReader.ButtontypeList;
                List<string> BaseNameList = GlobalSettings.SettingsReader.ButtonBaseNameList;
                List<bool> ButtonCheckedList = GlobalSettings.SettingsReader.ButtonCheckedPropertyList;
                List<bool> ButtonRevertToInitialPropertyList = GlobalSettings.SettingsReader.ButtonRevertToInitialPropertyList;
                List<int> ButtonFeatureIdList = GlobalSettings.SettingsReader.ButtonFeatureId;
                List<bool> RarelyUsedButtonsList = GlobalSettings.SettingsReader.RerelyUsedButtonsList;
                List<bool> BtnRememberLastState = GlobalSettings.SettingsReader.RememberLastState;
                List<bool> LastCheckedState = GlobalSettings.SettingsReader.LastCheckedState;

                for (int i = 0; i < ImageList.Count; i++)
                {
                    if (GlobalSettings.RapidMachineType == GlobalSettings.MachineType.DSP || 
                        GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Magnifier_DSP || 
                        GlobalSettings.RapidMachineType == GlobalSettings.MachineType.Horizontal_DSP)
                        if (BaseNameList[i] == "Auto Focus" || BaseNameList[i] == "Focus Scan" || BaseNameList[i] == "Contour Scan")
                            continue;
                    if (ToolBarName == "RCAD Toolbar")
                        if (BaseNameList[i] == "Hide Probe")
                            if (GlobalSettings.SettingsReader.CheckButtonStatus("MachineDetails", "ProbePresent") == false)
                            {
                                continue;
                            }

                    switch (buttonType[i])
                    {
                        case 0:
                            {
                                System.Windows.Controls.Button btn = new System.Windows.Controls.Button();
                                AssignSingleBtnProp(btn, BaseNameList[i], ToolTipList[i], ButtonCheckedList[i], ButtonRevertToInitialPropertyList[i], ToolBarHelpName, ImageList[i], GlobalSettings.ScrBar_NormalButtonStyle, GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight, new Thickness(2, 0, 2, 0), ButtonFeatureIdList[i], RarelyUsedButtonsList[i], BtnRememberLastState[i], LastCheckedState[i]);
                                btn.Click += new RoutedEventHandler(MyBtnsHandler);
                                this.Children.Add(btn);
                                if (FillNameList) BtnNameList.Add(ToolTipList[i]);
                                if (RarelyUsedButtonsList[i])
                                {
                                    btn.Visibility = System.Windows.Visibility.Collapsed;
                                }
                            }
                            break;
                        case 1:
                            {
                                RadioButton RBtn = new RadioButton();
                                AssignSingleBtnProp(RBtn, BaseNameList[i], ToolTipList[i], ButtonCheckedList[i], ButtonRevertToInitialPropertyList[i], ToolBarHelpName, ImageList[i], GlobalSettings.ScrBar_RadioButtonStyle, GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight, new Thickness(2, 0, 2, 0), ButtonFeatureIdList[i], RarelyUsedButtonsList[i], BtnRememberLastState[i], LastCheckedState[i]);
                                RBtn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(MyBtnsHandler);
                                this.Children.Add(RBtn);
                                if (FillNameList) BtnNameList.Add(ToolTipList[i]);
                                if (RarelyUsedButtonsList[i])
                                {
                                    RBtn.Visibility = System.Windows.Visibility.Collapsed;
                                }
                            }
                            break;
                        case 2:
                            {
                                ToggleButton ToggBtn = new ToggleButton();
                                AssignSingleBtnProp(ToggBtn, BaseNameList[i], ToolTipList[i], ButtonCheckedList[i], ButtonRevertToInitialPropertyList[i], ToolBarHelpName, ImageList[i], GlobalSettings.ScrBar_ToggleButtonStyle, GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight, new Thickness(2, 0, 2, 0), ButtonFeatureIdList[i], RarelyUsedButtonsList[i], BtnRememberLastState[i], LastCheckedState[i]);
                                ToggBtn.Click += new RoutedEventHandler(MyBtnsHandler);
                                this.Children.Add(ToggBtn);
                                if (FillNameList) BtnNameList.Add(ToolTipList[i]);
                                if (RarelyUsedButtonsList[i])
                                {
                                    ToggBtn.Visibility = System.Windows.Visibility.Collapsed;
                                }
                            }
                            break;
                        case 3:
                            {
                                try
                                {
                                    List<System.Windows.Controls.Image> DDButtonImageList;
                                    List<string> DDButtonBaseNameList, DDButtonToolTipList;
                                    List<int> DDButtontypeList, DDButtonId, DDButtonParentId, DDButtonFeatureId;
                                    List<bool> DDButtonCheckedPropertyList, DDButttonHasClick;
                                    List<bool> DDButtonRevertToInitialPropertyList;

                                    DDButtonToolTipList = new List<string>();
                                    DDButtontypeList = new List<int>();
                                    DDButtonCheckedPropertyList = new List<bool>();
                                    DDButtonRevertToInitialPropertyList = new List<bool>();
                                    DDButtonBaseNameList = new List<string>();
                                    DDButtonId = new List<int>();
                                    DDButtonParentId = new List<int>();
                                    DDButttonHasClick = new List<bool>();
                                    DDButtonFeatureId = new List<int>();
                                    DDButtonImageList = new List<Image>();

                                    System.Data.DataRow[] ToolStripRows = GlobalSettings.SettingsReader.GetRowsAccordingToFilter("ToolStripButton", "Alignment", BaseNameList[i]);
                                    ToolStripRows = GlobalSettings.SettingsReader.SortRowAccordingToFreq(ToolStripRows);
                                    string ToolTipColName = "ButtonToolTip" + GlobalSettings.Language;

                                    for (int BtnCnt = 0; BtnCnt < ToolStripRows.Length; BtnCnt++)
                                    {
                                        if (GlobalSettings.CompanyName != "Customised Technologies (P) Ltd")
                                        {
                                            bool ThreeDBtn = Convert.ToBoolean(ToolStripRows[BtnCnt]["ThreeDButtonType"]);
                                            bool DisableBtn = Convert.ToBoolean(ToolStripRows[BtnCnt]["HideTool"]);
                                            if (DisableBtn)
                                                continue;

                                            if (!GlobalSettings.Is3DSoftware)
                                                if (ThreeDBtn) continue;
                                        }

                                        DDButtonToolTipList.Add(ToolStripRows[BtnCnt][ToolTipColName].ToString());

                                        DDButtontypeList.Add(Convert.ToInt16(ToolStripRows[BtnCnt]["ButtonType"]));

                                        DDButtonCheckedPropertyList.Add(Convert.ToBoolean(ToolStripRows[BtnCnt]["CheckedProperty"]));

                                        DDButtonRevertToInitialPropertyList.Add(!Convert.ToBoolean(ToolStripRows[BtnCnt]["IgnoreOriginalStateDuringClearAll"]));

                                        DDButtonBaseNameList.Add(ToolStripRows[BtnCnt]["ButtonName"].ToString());

                                        DDButtonId.Add(Convert.ToInt16(ToolStripRows[BtnCnt]["ButtonId"]));

                                        DDButtonParentId.Add(Convert.ToInt16(ToolStripRows[BtnCnt]["ButtonParentId"]));

                                        DDButttonHasClick.Add(Convert.ToBoolean(ToolStripRows[BtnCnt]["HasClick"]));

                                        DDButtonFeatureId.Add(Convert.ToInt16(ToolStripRows[BtnCnt]["FeatureID"]));

                                        byte[] Source = (byte[])ToolStripRows[BtnCnt]["ButtonIcon"];

                                        System.Windows.Controls.Image img = new System.Windows.Controls.Image();
                                        System.Windows.Media.Imaging.BitmapImage bmp = new System.Windows.Media.Imaging.BitmapImage();
                                        bmp.CacheOption = System.Windows.Media.Imaging.BitmapCacheOption.OnLoad;
                                        bmp.BeginInit();
                                        bmp.StreamSource = (new System.IO.MemoryStream(Source));
                                        bmp.EndInit();
                                        img.Source = bmp;
                                        img.Width = GlobalSettings.BtnIcoStdWidth;
                                        img.Height = GlobalSettings.BtnIcoStdHeight;
                                        DDButtonImageList.Add(img);
                                    }

                                    DropDownList DDL = new DropDownList();
                                    DDL.btnTg.Content = GlobalSettings.SettingsReader.UpdateSingleBtnInfo(BaseNameList[i], ToolBarName, GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                                    DDL.btnTg.Background = Brushes.Transparent;
                                    DDL.btnTg.Foreground = (Brush)TryFindResource("UpperLowerMainToolbarTextcolor");
                                    DDL.lstDropDownItems.ItemContainerStyle = (Style)TryFindResource("DropDownListBoxCheckedItem");
                                    DDL.Width = 100;

                                    for (int j = 0; j < DDButtonImageList.Count; j++)
                                    {
                                        switch (DDButtontypeList[j])
                                        {
                                            case 0:
                                                {
                                                    System.Windows.Controls.Button btn = new System.Windows.Controls.Button();
                                                    AssignSingleBtnProp(btn, DDButtonBaseNameList[j], DDButtonToolTipList[j], DDButtonCheckedPropertyList[j], DDButtonRevertToInitialPropertyList[j], BaseNameList[i], DDButtonImageList[j], GlobalSettings.ScrBar_NormalButtonStyle, GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight, new Thickness(2, 0, 2, 0), DDButtonFeatureId[j], false);
                                                    if (DropdownBtnHandler != null)
                                                        btn.Click += new RoutedEventHandler(DropdownBtnHandler);
                                                    DDL.lstDropDownItems.Items.Add(btn);
                                                }
                                                break;
                                            case 1:
                                                {
                                                    RadioButton RBtn = new RadioButton();
                                                    AssignSingleBtnProp(RBtn, DDButtonBaseNameList[j], DDButtonToolTipList[j], DDButtonCheckedPropertyList[j], DDButtonRevertToInitialPropertyList[j], BaseNameList[i], DDButtonImageList[j], GlobalSettings.ScrBar_RadioButtonStyle, GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight, new Thickness(2, 0, 2, 0), DDButtonFeatureId[j], false);
                                                    if (DropdownBtnHandler != null)
                                                        RBtn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(DropdownBtnHandler);
                                                    DDL.lstDropDownItems.Items.Add(RBtn);
                                                }
                                                break;
                                            case 2:
                                                {
                                                    ToggleButton ToggBtn = new ToggleButton();
                                                    AssignSingleBtnProp(ToggBtn, DDButtonBaseNameList[j], DDButtonToolTipList[j], DDButtonCheckedPropertyList[j], DDButtonRevertToInitialPropertyList[j], BaseNameList[i], DDButtonImageList[j], GlobalSettings.ScrBar_ToggleButtonStyle, GlobalSettings.ToolbarBtnStdWidth, GlobalSettings.ToolbarBtnStdHeight, new Thickness(2, 0, 2, 0), DDButtonFeatureId[j], false);
                                                    if (DropdownBtnHandler != null)
                                                        ToggBtn.Click += new RoutedEventHandler(DropdownBtnHandler);
                                                    DDL.lstDropDownItems.Items.Add(ToggBtn);
                                                }
                                                break;
                                        }
                                    }
                                    this.Children.Add(DDL);
                                }
                                catch (Exception ex)
                                {
                                    RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STBDDL003", ex);
                                }
                            }
                            break;
                        case 10:
                            {
                                Image SepImg = new Image();
                                SepImg = ImageList[i];
                                SepImg.Width = 5; SepImg.Height = 40;
                                SepImg.Margin = new Thickness(3, 0, 3, 0);
                                this.Children.Add(SepImg);
                            }
                            break;
                    }
                }
                SetInitialChkStatus();
                SetLastChkStatus();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB003", ex);
            }
        }
        public ContentControl Check_Togg_or_Radio_Btn(string BtnName, bool ButtonStatus)
        {
            try
            {
                ContentControl Btn = this.Children.OfType<ContentControl>()
                                        .Where(T => (((ButtonProperties)T.Tag).ButtonName) == BtnName).First();
                if (Btn == null) return null;
                if (Btn.GetType() == typeof(ToggleButton)) ((ToggleButton)Btn).IsChecked = ButtonStatus;
                else if (Btn.GetType() == typeof(RadioButton)) ((RadioButton)Btn).IsChecked = ButtonStatus;
                return Btn;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB004", ex);
                return null;
            }
        }

        public ContentControl AddNewButton(string BtnAllignment, string BaseName, double BtnWidth, double BtnHeight, double IcoWidth, double IcoHeight, Thickness Margin)
        {
            try
            {
                ContentControl Btn;
                Style BtnStyle;
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo(BaseName, BtnAllignment, IcoWidth, IcoHeight);
                if (GlobalSettings.SettingsReader.ButtonType == 0)
                {
                    Btn = new Button();
                    BtnStyle = GlobalSettings.ScrBar_NormalButtonStyle;
                    ((Button)Btn).Click += new RoutedEventHandler(MyBtnsHandler);
                }
                else if (GlobalSettings.SettingsReader.ButtonType == 1)
                {
                    Btn = new RadioButton();
                    BtnStyle = GlobalSettings.ScrBar_RadioButtonStyle;
                    ((RadioButton)Btn).PreviewMouseLeftButtonDown += new MouseButtonEventHandler(MyBtnsHandler);
                }
                else if (GlobalSettings.SettingsReader.ButtonType == 2)
                {
                    Btn = new ToggleButton();
                    BtnStyle = GlobalSettings.ScrBar_ToggleButtonStyle;
                    ((ToggleButton)Btn).Click += new RoutedEventHandler(MyBtnsHandler);
                }
                else
                    return null;
                AssignSingleBtnProp(Btn, BaseName, GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonCheckedProperty, GlobalSettings.SettingsReader.ButtonRevertToInitialProperty, BtnAllignment, GlobalSettings.SettingsReader.ButtonImage,
                    BtnStyle, BtnWidth, BtnHeight, new Thickness(2, 0, 2, 0), 0, false);
                this.Children.Add(Btn);
                return Btn;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB005", ex);
                return null;
            }
        }
        public void Remove_Button_from_End()
        {
            try
            {
                if (this.Children.Count > 0)
                {
                    this.Children.RemoveAt(this.Children.Count - 1);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB006", ex);
            }
        }
        public void Remove_Button(string ButtonName)
        {
            try
            {
                ContentControl Btn = this.Children.OfType<ContentControl>()
                                        .Where(T => (((ButtonProperties)T.Tag).ButtonName) == ButtonName).First();
                if (Btn == null) return;
                else this.Children.Remove(Btn);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB007", ex);
            }
        }

        public ContentControl Get_Button(string ButtonName)
        {
            try
            {
                //ContentControl Btn;
                //ContentControl Btn = this.Children.OfType<System.Windows.Controls.Button>()
                //                        .Where(T => (((ButtonProperties)T.Tag).ButtonName) == ButtonName).First();
                foreach (UIElement obj in this.Children)
                {
                    if (obj.GetType() == typeof(Button))
                    {
                        Button btn = (Button)obj;
                        if (((ButtonProperties)btn.Tag).ButtonName == ButtonName)
                            return (ContentControl)btn;
                    }
                    else if (obj.GetType() == typeof(ToggleButton))
                    {
                        ToggleButton btn = (ToggleButton)obj;
                        if (((ButtonProperties)btn.Tag).ButtonName == ButtonName)
                            return (ContentControl)btn;
                    }
                }
                //return Btn;
                return null;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB008a : ButtonName : " + ButtonName, ex);
                return null;
            }
        }
        public void ShowHideRarelyUsedButtons()
        {
            try
            {
                for (int i = 0; i < this.Children.Count - 1; i++)
                {
                    if (this.Children[i].GetType() == typeof(Button) || this.Children[i].GetType() == typeof(RadioButton) || this.Children[i].GetType() == typeof(ToggleButton))
                    {
                        ContentControl Btn = (ContentControl)this.Children[i];
                        ButtonProperties BProp = (ButtonProperties)Btn.Tag;
                        if (BProp.IsRarelyUsed)
                        {
                            if (Btn.Visibility == System.Windows.Visibility.Collapsed)
                            {
                                Btn.Visibility = System.Windows.Visibility.Visible;
                            }
                            else
                                Btn.Visibility = System.Windows.Visibility.Collapsed;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STBRarelyUsed001 :", ex);
            }
        }

        public void Hide_Button(string ButtonName)
        {
            try
            {

                ContentControl Btn;
                 try
                {
                    Btn = this.Children.OfType<ContentControl>()
                                           .Where(T => (((ButtonProperties)T.Tag).ButtonName) == ButtonName).First();
                    if (Btn == null) return;
                    else Btn.Visibility = Visibility.Collapsed;
                }
                catch (Exception exx)
                {
                    for (int i = 0; i < this.Children.Count; i++)
                    {
                        string gfg = this.Children[i].GetType().ToString();
                        if (gfg.Contains("ContentControl") || gfg.Contains("Button") || gfg.Contains("Toggle"))
                        {
                            Btn = (ContentControl)this.Children[i];
                            if (Btn.Tag.GetType().ToString().Contains("String"))
                            {
                                var myTempArray = Btn.Tag as IEnumerable<string>;
                                string[] ss = (string[])myTempArray;
                                if (ss[2] == ButtonName)
                                {
                                    Btn.Visibility = Visibility.Collapsed; return;
                                }
                            }
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB007", ex);
            }

        }

        public bool CheckIfButtonExists(string ButtonName)
        {
            try
            {
                ContentControl Btn = this.Children.OfType<ContentControl>()
                                        .Where(T => (((ButtonProperties)T.Tag).ButtonName) == ButtonName).First();
                return true;
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STb028", ex);
                return false;
            }
        }

        public void SetInitialChkStatus()
        {
            try
            {
                for (int i = 0; i < this.Children.Count; i++)
                {
                    if (this.Children[i].GetType() == typeof(RadioButton))
                    {
                        if (((ButtonProperties)((RadioButton)this.Children[i]).Tag).RevertToInitialStatus)
                            if (((ButtonProperties)((RadioButton)this.Children[i]).Tag).InitialChkStatus)
                            {
                                ((RadioButton)this.Children[i]).IsChecked = true;
                                MyBtnsHandler(this.Children[i], null);
                            }
                    }
                    else if (this.Children[i].GetType() == typeof(ToggleButton))
                    {
                        if (((ButtonProperties)((ToggleButton)this.Children[i]).Tag).RevertToInitialStatus)
                            if (((ButtonProperties)((ToggleButton)this.Children[i]).Tag).InitialChkStatus)
                            {
                                ((ToggleButton)this.Children[i]).IsChecked = true;
                                MyBtnsHandler(this.Children[i], null);
                            }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB008b", ex);
            }
        }

        public void SetLastChkStatus()
        {
            try
            {
                for (int i = 0; i < this.Children.Count; i++)
                {
                    if (this.Children[i].GetType() == typeof(RadioButton))
                    {
                        if (((ButtonProperties)((RadioButton)this.Children[i]).Tag).RememberLastState)
                            if (((ButtonProperties)((RadioButton)this.Children[i]).Tag).LastCheckedState)
                            {
                                ((RadioButton)this.Children[i]).IsChecked = true;
                                MyBtnsHandler(this.Children[i], null);
                            }
                            else
                            {
                                ((RadioButton)this.Children[i]).IsChecked = false;
                                MyBtnsHandler(this.Children[i], null);
                            }
                    }
                    else if (this.Children[i].GetType() == typeof(ToggleButton))
                    {
                        if (((ButtonProperties)((ToggleButton)this.Children[i]).Tag).RememberLastState)
                            if (((ButtonProperties)((ToggleButton)this.Children[i]).Tag).LastCheckedState)
                            {
                                ((ToggleButton)this.Children[i]).IsChecked = true;
                                MyBtnsHandler(this.Children[i], null);
                            }
                            else
                            {
                                ((ToggleButton)this.Children[i]).IsChecked = false;
                                MyBtnsHandler(this.Children[i], null);
                            }
                    }
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB008C", ex);
            }
        }

        public void ClearAllBtnSelection()
        {
            try
            {
                for (int i = 0; i < this.Children.Count; i++)
                {
                    if (this.Children[i].GetType() == typeof(RadioButton))
                        ((RadioButton)this.Children[i]).IsChecked = false;
                    else if (this.Children[i].GetType() == typeof(ToggleButton))
                        ((ToggleButton)this.Children[i]).IsChecked = false;
                }
                ClearNormalBtnState();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB009", ex);
            }
        }
        public void ClearNormalBtnState()
        {
            for (int i = 0; i < this.Children.Count; i++)
            {
                if (this.Children[i].GetType() == typeof(Button))
                {
                    Button TmpBtn = this.Children[i] as Button;
                    if (((ButtonProperties)TmpBtn.Tag).RevertToInitialStatus)
                        if (((ButtonProperties)TmpBtn.Tag).ButtonName != ((ButtonProperties)TmpBtn.Tag).ResetTo_BtnName)
                        {
                            GlobalSettings.SettingsReader.UpdateSingleBtnInfo(((ButtonProperties)TmpBtn.Tag).ResetTo_BtnName,
                                ((ButtonProperties)TmpBtn.Tag).ResetTo_BtnName, GlobalSettings.BtnIcoStdWidth, GlobalSettings.BtnIcoStdHeight);
                            AssignSingleBtnProp(TmpBtn, ((ButtonProperties)TmpBtn.Tag).ResetTo_BtnName, GlobalSettings.SettingsReader.ButtonToolTip, GlobalSettings.SettingsReader.ButtonImage);
                        }
                }
            }
        }
        public void ClearRadioBtnSelection()
        {
            try
            {
                for (int i = 0; i < this.Children.Count; i++)
                {
                    if (this.Children[i].GetType() == typeof(RadioButton))
                        ((RadioButton)this.Children[i]).IsChecked = false;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB010", ex);
            }
        }
        public void ClearToggBtnSelection()
        {
            try
            {
                for (int i = 0; i < this.Children.Count; i++)
                {
                    if (this.Children[i].GetType() == typeof(ToggleButton))
                        ((ToggleButton)this.Children[i]).IsChecked = false;
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:STB011", ex);
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
                RBtn.Width = Width;
                RBtn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_RadioButton2");
                RBtn.Content = ComponentName;
                RBtn.Tag = new ButtonProperties("ZeroingAngle", false, true, ZeroingAngle);
                RBtn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(MyBtnsHandler);
                RBtn.Margin = new Thickness(5, 0, 0, 0);
                RBtn.FontWeight = FontWeights.Normal;
                this.Children.Add(RBtn);
            }
            if (DRow.Length > 0)
            {
                Button Btn = new Button();
                Btn.MinWidth = 60;
                Btn.FontSize = 30;
                Btn.Content = "Zero";
                Btn.Style = (System.Windows.Style)GlobalSettings.MainWin.TryFindResource("ScrollToolbar_NormalButton2");
                Btn.PreviewMouseLeftButtonDown += new MouseButtonEventHandler(MyBtnsHandler);
                Btn.FontWeight = FontWeights.Bold;
                Btn.Tag = new ButtonProperties("Finalize Zeroing Angle", false, true, "Rotary Measurement");
                Btn.Margin = new Thickness(5, 0, 0, 0);
                this.Children.Add(Btn);
            }
        }
    }
}