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

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ShapeInfoWin.xaml
    /// </summary>
    public partial class FGandCldPtSizeWin : Window, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }

        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        double Svalue = 3;
        public double SliderValue
        {
            get
            {
                return Svalue;
            }
            set
            {
                Svalue = value;
                Notify("SliderValue");
                SetSliderValue((double)value);
            }
        }
        public FGandCldPtSizeWin()
        {
            this.InitializeComponent();
            this.PtSizeValueSlider.ValueChanged += new RoutedPropertyChangedEventHandler<double>(PtSizeValueSlider_ValueChanged);
            this.KeyDown += new KeyEventHandler(FGandCldPtSizeWin_KeyDown);
            GetSliderValue();
            // Insert code required on object creation below this point.
        }
        //On ok click just return the dialog result as true.
        private void btnOK_Click(object sender, RoutedEventArgs e)
        { this.Result = true; this.Close(); }
        //if user presses enter key in textbox just make the dialog result to be true.
        private void FGandCldPtSizeWin_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) { this.Result = true; e.Handled = true; this.Close(); }
            else if (e.Key == Key.Escape) { this.Result = false; e.Handled = true; this.Close(); }
        }
        private void RaiseEvent()
        {
            if (WinClosing != null)
                WinClosing(this, null);
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RaiseEvent();
            this.Hide();
            e.Cancel = true;
        }
        void PtSizeValueSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            RWrapper.RW_MainInterface.MYINSTANCE().SetFGandCloudPointSize((int)PtSizeValueSlider.Value);
        }
        public void ClearAll()
        {
            PtSizeValueSlider.Value = 3;
            Svalue = 3;
        }
        private void GetSliderValue()
        {
            try
            {               
                Svalue = Convert.ToDouble(GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("MachineDetails")[0]["FGandCldPtSize"]);
                if (Svalue == 1)
                    PtSizeValueSlider_ValueChanged(null, null);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FGANDCLDPOINTSIZE01", ex);
            }
        }

        private void SetSliderValue(double _value)
        {
            try
            {
                GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("MachineDetails")[0]["FGandCldPtSize"] = _value;
                GlobalSettings.SettingsReader.UpdateTable("MachineDetails");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:FGANDCLDPOINTSIZE02", ex);
            }
        }
    }
}