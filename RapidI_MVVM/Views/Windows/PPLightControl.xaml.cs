using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for PPLightControl.xaml
    /// </summary>
    public partial class PPLightControl : Window, INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        void Notify(string PropName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(PropName));
            }
        }
        //int _ProfileChangeValue = 2, _SurfaceChangeValue = 2;

        //public int SurfaceChangeValue
        //{
        //    get
        //    {
        //        return _SurfaceChangeValue;
        //    }
        //    set
        //    {
        //        _SurfaceChangeValue = value;
        //        Notify("SurfaceValueChange");
        //        SetSliderValue(SurfaceLightModifier, value);
        //    }
        //}

        //public int ProfileChangeValue
        //{
        //    get
        //    {
        //        return _ProfileChangeValue;
        //    }
        //    set
        //    {
        //        _ProfileChangeValue = value;
        //        Notify("ProfileValueChange");
        //        SetSliderValue(ProfileLightModifier, value);
        //    }
        //}


        public PPLightControl()
        {
            InitializeComponent();
            //this.SurfaceLightModifier.ValueChanged += new RoutedPropertyChangedEventHandler<double>(SurfaceSlider_ValueChanged);
            System.Data.DataRow[] sdsd = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings"); //[0];

            nud_Surface.Value = (int)sdsd[0][7];
            nud_Profile.Value = (int)sdsd[0][8];
        }

        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(true, (int)nud_Surface.Value);
            RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(false, (int)nud_Profile.Value);
            //if (thisSlider.Name.Contains("Surface"))
            System.Data.DataRow[] sdsd = GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings"); //[0];
            //[0]["PP_SF_ChangeValue"] = (int)nud_Surface.Value;
            ////else
            //GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings")[0]["PP_PL_ChangeValue"] = (int)nud_Profile.Value;
            sdsd[0][7] = (int)nud_Surface.Value; sdsd[0][8] = (int)nud_Profile.Value;
            GlobalSettings.SettingsReader.UpdateTable("LightingSettings");
            this.Hide();
        }
        //void SurfaceSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        //{
        //    RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(true, (int)SurfaceLightModifier.Value);
        //}
        //void ProfileSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        //{
        //    RWrapper.RW_PartProgram.MYINSTANCE().SetLightChangeValue(false, (int)SurfaceLightModifier.Value);
        //}


        private void SetSliderValue(Slider thisSlider, int _value)
        {
            try
            {
                if (thisSlider.Name.Contains("Surface"))
                    GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings")[0]["PP_SF_ChangeValue"] = _value;
                else
                    GlobalSettings.SettingsReader.GetRowsAccordingToCurrentMachineNumber("LightingSettings")[0]["PP_SF_ChangeValue"] = _value;
                GlobalSettings.SettingsReader.UpdateTable("LightingSettings");
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PPLightControl001", ex);
            }
        }

    }
}
