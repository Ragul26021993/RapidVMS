using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Controls.Primitives;
using System.ComponentModel;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for RapidDropDownControl.xaml
    /// </summary>
    public partial class RapidDropDownControl : UserControl, INotifyPropertyChanged
    {
        //This Button is Exposed so that you can Set the style from out Side this Control ... Hence this control can be used at multiple places with different lookend style.
        public ToggleButton TglBtn;
        public Popup PopupContainer;
        Brush SwitchOff, SwitchOn;

        #region Function To notify UI that value is Changed

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string prop)
        {
            if (PropertyChanged != null)
            {
                var e = new PropertyChangedEventArgs(prop);
                PropertyChanged(this, e);
            }
        }

        #endregion

        #region Event is Exposed so that click Event of Toggle Button can be Handled outside this Control where-ever it is used.
        public event RoutedEventHandler OnButtonClick;
        #endregion

        #region Public Property to Handle the UI

        private bool _Hasitem;
        private bool _IsDropDownOpen = false;
        private string _header;
        private bool _SwitchColorOn = false;
        private double _MaxPopupWidth;

        public bool IsDropDownOpen
        {
            get
            {
                return _IsDropDownOpen;
            }
            set
            {
                _IsDropDownOpen = value;               
                OnPropertyChanged("IsDropDownOpen");
            }
        }
        public bool HasItem
        {
            get
            {
                return _Hasitem;
            }
            set
            {
                _Hasitem = value;
            }
        }
        public string Header
        {
            get
            {
                return _header;
            }
            set
            {
                _header = value;
                OnPropertyChanged("Header");
            }
        }
        public double MaximumDropDownHeight
        {
            get
            {
                return PopupContainer.MaxHeight;
            }
            set
            {
                PopupContainer.MaxHeight = value;
            }
        }
        public double MaxPopupWidth
        {
            get
            {
                return _MaxPopupWidth;
            }
            set
            {
                _MaxPopupWidth = value;
            }
        }
        public bool SwitchOnColor
        {
            get
            {
                return _SwitchColorOn;
            }
            set
            {
                if ((bool)value)
                    this.BackGroundBrush = SwitchOn;
                else
                    this.BackGroundBrush = SwitchOff;
                _SwitchColorOn = value;
            }
        }
        public Brush BackGroundBrush
        {
            get { return (Brush)GetValue(BackGroundBrushProperty); }
            set { SetValue(BackGroundBrushProperty, value); }
        }

        public static readonly DependencyProperty BackGroundBrushProperty =
            DependencyProperty.Register("BackGroundBrush", typeof(Brush), typeof(RapidDropDownControl), new UIPropertyMetadata(Brushes.Red));

        #endregion

        public RapidDropDownControl()
        {
            this.InitializeComponent();
            Scr.ApplyTemplate();
            TglBtn = (ToggleButton)Scr.Template.FindName("MainTglBtn", Scr);
            PopupContainer = (Popup)Scr.Template.FindName("PART_Popup1", Scr);
            TglBtn.Click += new RoutedEventHandler(TglBtn_Click);           
            init();
            this.Loaded+=new RoutedEventHandler(RapidDropDownControl_Loaded);
        }

        private void RapidDropDownControl_Loaded(object sender, RoutedEventArgs e)
        {
            try
            {
                ItemCollection ICL = this.Scr.Items;
                foreach (Control Ctrl in ICL)
                {
                    Ctrl.PreviewMouseLeftButtonDown+=new MouseButtonEventHandler(Ctrl_PreviewMouseLeftButtonDown);
                }
            }
            catch (Exception)
            {                
                throw;
            }
        }

        private void init()
        {
            try
            {
                SwitchOff = (Brush)TryFindResource("SnapRedStyle");
                SwitchOn = (Brush)TryFindResource("SnapGreenStyle");
                BackGroundBrush = SwitchOff;                
            }
            catch (Exception)
            {
                throw;
            }
        }

        protected void TglBtn_Click(object sender, RoutedEventArgs e)
        {
            if (OnButtonClick != null)
            {
                OnButtonClick(this, null);
            }
        }

        public void ClosePopup()
        {
            if (PopupContainer.IsOpen)
                PopupContainer.IsOpen = false;
        }

        protected void Ctrl_PreviewMouseLeftButtonDown(object sender, RoutedEventArgs e)
        {
            try
            {
                string Tag = ((Control)sender).Tag.ToString();
                this.Header = Tag;
            }
            catch (Exception)
            {
                
                throw;
            }
        }
    }
}