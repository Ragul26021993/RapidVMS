using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Controls.Primitives;

namespace Rapid.Panels
{
    /// <summary>
    /// Interaction logic for SAOI.xaml
    /// </summary>
    public partial class SAOI : Grid
    {
        public ToggleButton SAOIRBtn;
        double CellWidth = 1, CellHeight = 1, GridWidth = 10, GridHeight = 10, currentTopleftX, currentTopLeftY, currentBottomRightX, currentBottomRightY;
        public double BottomRightX = 10, BottomRightY = 10, TopLeftX = 166, TopLeftY = 153;
        public bool isTopLeftSet = true, isBottomRightset = true;
        bool isbottomflagset = false , isCNCModeSet=false; //iscellflagset=false ,
        RWrapper.RW_SAOIParameter SAOIOBJ;
        public SAOI()
        {
            InitializeComponent();
            SAOIOBJ = new RWrapper.RW_SAOIParameter();

            init();
        }
        private void init()
        {
            try
            {
                GlobalSettings.SettingsReader.UpdateSingleBtnInfo("SpeedsGrpBx", "SpeedsGrpBx", 32, 32);
                grpFeedrates.Header = GlobalSettings.SettingsReader.ButtonToolTip;
                GroupBox gb = grpGridTop;
                Button[] SAOIButtons = { BtnSet, BtnHome, BtnNext, BtnStart, btnShowGrid, btnSetGridCoordinates, BtnRightbot };
                for (int i = 0; i < SAOIButtons.Length; i++)
                {
                    SAOIButtons[i].Tag = new string[] { "SAIO", "Tooltip", "Name" };
                    SAOIButtons[i].Click += new RoutedEventHandler(HandleButtonsClick);
                }

                double[] CNCFeedRates = { GlobalSettings.DROSpeeds[0], GlobalSettings.DROSpeeds[1], GlobalSettings.DROSpeeds[2], GlobalSettings.DROSpeeds[3], GlobalSettings.DROSpeeds[0] };
                TextBox[] saoitextbox = { txtTopX, txtTopY, txtCellWidth, txtCellHeight, txtGridWidth, txtGrigHeight };
                for (int i = 0; i < saoitextbox.Length; i++)
                {
                    saoitextbox[i].PreviewLostKeyboardFocus += new KeyboardFocusChangedEventHandler(SAOI_PreviewLostKeyboardFocus);
                }
                btnShowGrid.IsEnabled = false;
                txtCellWidth.Tag = "Width";
                txtCellHeight.Tag = "Height";
                txtGridWidth.Tag = "Width";
                txtGrigHeight.Tag = "Height";
                Measure(new Size(double.PositiveInfinity, double.PositiveInfinity));
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SAOI001", ex);
            }
        }
        //TextBox Value Change Handler
        private void SAOI_PreviewLostKeyboardFocus(object sender, KeyboardEventArgs e)
        {
            try
            {
                TextBox tb = (TextBox)sender;
                if (tb.Equals(txtTopX))
                {

                    if (!isBottomRightset)
                    {
                        this.BottomRightX = Convert.ToDouble(txtTopX.Text);
                    }
                    else
                    {
                        this.TopLeftX = Convert.ToDouble(txtTopX.Text);
                    }
                }
                if (tb.Equals(txtTopY))
                {

                    if (!isBottomRightset)
                    {
                        this.BottomRightY = Convert.ToDouble(txtTopY.Text);
                    }
                    else
                    {
                        this.TopLeftY = Convert.ToDouble(txtTopY.Text);
                    }
                }
                if (tb.Equals(txtCellHeight))
                {
                    this.CellHeight = Convert.ToDouble(txtCellHeight.Text);
                    calculateAndSet();
                }
                if (tb.Equals(txtCellWidth))
                {
                    this.CellWidth = Convert.ToDouble(txtCellWidth.Text);
                    calculateAndSet();
                }
                if (tb.Equals(txtGridWidth))
                {
                    this.GridWidth = Convert.ToDouble(txtGridWidth.Text);
                }
                if (tb.Equals(txtGrigHeight))
                {
                    this.GridHeight = Convert.ToDouble(txtGrigHeight.Text);
                }
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SAOI01", ex);
            }
        }
        //Button Click Handler
        private void HandleButtonsClick(object sender, RoutedEventArgs e)
        {
            try
            {
                ContentControl b = (ContentControl)sender;
                string[] tagStr = (string[])b.Tag;
                //Buttton click event to set the Left Top  Coordinate of grid 
                if (b.Equals(btnSetGridCoordinates))
                {
                    isTopLeftSet = false;
                    isBottomRightset = true;

                }
                //Buttton click event to Move the Machine to the  First Rectangle of Grid  
                else if (b.Equals(BtnHome))
                {
                    if(isCNCModeSet)
                    SAOIOBJ.MachineGotoDefaultPosition();
                }             
                //Buttton click event to SetUp the connection with the Machine 
                else if (b.Equals(BtnStart))
                {
                    SAOIOBJ.StartCNC();
                    isCNCModeSet = true;
                }

                //Buttton click event to Move the machine to next cell in grid 
                else if (b.Equals(BtnNext))
                {
                    if(isCNCModeSet)
                    SAOIOBJ.IncrementGridSelection();
                }

                //Buttton click event to set the Grid and display it on OpenGl Window 
                else if (b.Equals(BtnSet))
                {
                    calculateAndSet();
                    SAOIOBJ.InitialilseGrid(currentTopleftX, currentTopLeftY, this.GridWidth, this.GridHeight, this.CellWidth, this.CellHeight);
                    btnShowGrid.IsEnabled = true;
                }
                //Buttton click event to Show and Hide the Grid 
                else if (b.Equals(btnShowGrid))
                {
                    SAOIOBJ.ShowHideGrid();
                    if (btnShowGrid.Content.ToString() == "Show")
                    {
                        btnShowGrid.Content = "Hide";
                        isCNCModeSet = false;
                    }
                    else
                    {
                        btnShowGrid.Content = "Show";
                        isCNCModeSet = true;
                    }

                }
                //Buttton click event to set the Right Bottom  Coordinate of grid 
                else if (b.Equals(BtnRightbot))
                {
                    isTopLeftSet = true;
                    isBottomRightset = false;
                    isbottomflagset = true;
                }

            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SAOI002", ex);
            }
        }
        void HandleCNCFeedrate_ValueChanged(object sender, RoutedEventArgs e)
        {
            try
            {

                //RWrapper.RW_CNC.MYINSTANCE().SetCNCFeedRate(Cncfeeds);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:SAOI003", ex);
            }
        }

        void TxtBx_KeyDown(Object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Delete)
                e.Handled = true;
        }
        public void ClickGoTo()
        {
            // HandleButtonsClick(btnDroGoto, null);
        }
        void calculateAndSet()
        {
            SAOIOBJ.SetTopLeftAccToGetPointer(TopLeftX, TopLeftY);
            currentTopleftX = SAOIOBJ.xvalue;
            currentTopLeftY = SAOIOBJ.yvalue;
            if (isbottomflagset)
            {
                SAOIOBJ.SetTopLeftAccToGetPointer(BottomRightX, BottomRightY);
                currentBottomRightX = SAOIOBJ.xvalue;
                currentBottomRightY = SAOIOBJ.yvalue;
                this.GridWidth = this.currentBottomRightX - this.currentTopleftX;
                this.GridHeight = this.currentTopLeftY - this.currentBottomRightY;
                isbottomflagset = false;
            }
            if (this.CellWidth > this.GridWidth)
            {
                this.CellWidth = 0;
            }
            if (this.CellHeight > this.GridHeight)
            {
                this.CellHeight = 0;
            }
            txtCellWidth.Text = CellWidth.ToString();
            txtCellHeight.Text = CellHeight.ToString();
            txtGridWidth.Text = GridWidth.ToString();
            txtGrigHeight.Text = GridHeight.ToString();
        }
    }
}
