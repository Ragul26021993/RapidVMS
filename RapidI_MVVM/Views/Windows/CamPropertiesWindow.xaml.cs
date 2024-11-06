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
using CalibrationModule;
using System.Data;
using Rapid.Utilities;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for PPBuildWin.xaml
	/// </summary>
	public partial class CamPropertiesWindow :OpenGlWin
	{
		//*********************************//
		//Property IDs

		//Brightness = 0
		//Contrast = 1
		//Saturation = 2
		//Sharpness = 3
		//Gain = 4
		//ColorEnable = 5
		//White_Balance = 6
		//Gamma = 7
		//Backlight_Compensation = 8
		//Gamma = 9 // Taking the place of Hue
		//Exposure = 10
		//*********************************//
		bool RefreshingFrontEndValuesOnly = false;
        //int[] BaseChannelValues = { 0, 0, 0, 0 };

		public CamPropertiesWindow(bool ShowSetDefault)
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
			init();
			if (ShowSetDefault)
			{
				Btn_SetDefault_Profile.Visibility = Visibility.Visible;
				Btn_SetDefault_Surface.Visibility = Visibility.Visible;
                Ch1_DGain.Visibility = Visibility.Visible;
                Ch2_DGain.Visibility = Visibility.Visible;
                Ch3_DGain.Visibility = Visibility.Visible;
                Ch4_DGain.Visibility = Visibility.Visible;
                Btn_CrossHair_Offset.Visibility = Visibility.Visible;
                Btn_SetDefault_RefDot.Visibility = Visibility.Visible;
            }
		}
		void init()
		{
			ExposureSlider.Maximum = 128; ExposureSlider.Minimum = 1; ExposureSlider.Value = 40;
			BrightnessSlider.Maximum = 255; BrightnessSlider.Minimum = 0; BrightnessSlider.Value = 0;
			Slider[] AllPropSliders = { BrightnessSlider, ContrastSlider, GammaSlider, SaturationSlider, SharpnessSlider, GainSlider, ExposureSlider };
			TextBox[] AllPropValueTxtBxs = { BrightnessTxtBx, ContrastTxtBx, GammaTxtBx, SaturationTxtBx, SharpnessTxtBx, GainTxtBx, ExposureTxtBx };
			int[] PropIds = { 0, 1, 7, 2, 3, 4, 10 };
			for (int j = 0; j < PropIds.Length; j++)
			{
				AllPropSliders[j].Value = RWrapper.RW_MainInterface.MYINSTANCE().GetCamProperty(PropIds[j]);
				AllPropValueTxtBxs[j].Text = AllPropSliders[j].Value.ToString();
				AllPropSliders[j].ValueChanged += new RoutedPropertyChangedEventHandler<double>(CamPropertiesSlider_ValueChanged);
			}

			DefaultBtn.Click += new RoutedEventHandler(DefaultBtn_Click);
			DefaultBtn_Profile.Click += new RoutedEventHandler(DefaultBtn_Click);

			Btn_SetDefault_Profile.Click += new RoutedEventHandler(Btn_SetDefault_Click);

			OKBtn.Click += new RoutedEventHandler(OKBtn_Click);
			btnClose.Click += new RoutedEventHandler(OKBtn_Click);
			if (RWrapper.RW_MainInterface.MYINSTANCE().GetCurrentCameraType() == 3)
			{
				//this.Height += 120;
				//this.MainGrid.RowDefinitions[2].Height = new GridLength(140);
				Slider[] ChannelSliders = { Ch1_Slider, Ch2_Slider, Ch3_Slider, Ch4_Slider };
				int[] chPropIDs = { 3, 6, 8, 9 };
				TextBox[] chTBs = { txb_Ch1, txb_Ch2, txb_Ch3, txb_Ch4 };
				for (int ii = 0; ii < ChannelSliders.Length; ii++)
				{
                    ChannelSliders[ii].Value = RWrapper.RW_MainInterface.MYINSTANCE().GetCamProperty(chPropIDs[ii]);
                    chTBs[ii].Text = ChannelSliders[ii].Value.ToString();
					ChannelSliders[ii].ValueChanged += new RoutedPropertyChangedEventHandler<double>(SpGain_Slider_ValueChanged);
				}
				int[] cbsPropIDs = { 14, 15, 16, 17 };
				NumericUpDown[] cbs = { Ch1_DGain, Ch2_DGain, Ch3_DGain, Ch4_DGain };
				for (int j = 0; j < cbs.Length; j++)
				{
					cbs[j].Minimum = 1; cbs[j].Maximum = 7;
					cbs[j].Value = RWrapper.RW_MainInterface.MYINSTANCE().GetCamProperty(cbsPropIDs[j]);
					cbs[j].ValueChanged += new RoutedEventHandler(ChangeDigitalGain);
				}

			}
			RWrapper.RW_MainInterface.MYINSTANCE().UpdateCamSettingsEvent += new RWrapper.RW_MainInterface.UpdateCamSettingsHandler(RefreshCamSettings);
		}

		void CamPropertiesSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			Slider[] AllPropSliders = { BrightnessSlider, ContrastSlider, GammaSlider, SaturationSlider, SharpnessSlider, GainSlider, ExposureSlider };
			TextBox[] AllPropValueTxtBxs = { BrightnessTxtBx, ContrastTxtBx, GammaTxtBx, SaturationTxtBx, SharpnessTxtBx, GainTxtBx, ExposureTxtBx };
			int[] PropIds = { 0, 1, 7, 2, 3, 4, 10 };
            for (int i = 0; i < AllPropSliders.Length; i++)
            {
                if (sender.Equals(AllPropSliders[i]))
                {
                    //if (i == 3)
                    //{
                    //    Slider[] ChannelSliders = { Ch1_Slider, Ch2_Slider, Ch3_Slider, Ch4_Slider };
                    //    for (int j = 0; j < ChannelSliders.Length; j++)
                    //        ChannelSliders[j].Value += (int)e.NewValue - (int)e.OldValue;
                    //}
                    AllPropValueTxtBxs[i].Text = e.NewValue.ToString();
                    if (!RefreshingFrontEndValuesOnly)
                        RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(PropIds[i], (int)e.NewValue);
                    break;
                }
            }
		}

		void DefaultBtn_Click(object sender, RoutedEventArgs e)
		{
			int thisPropval = 0;
			Slider[] AllPropSliders = { BrightnessSlider, ContrastSlider, SaturationSlider, Ch1_Slider, SharpnessSlider, GainSlider, Ch2_Slider, GammaSlider, Ch3_Slider, Ch4_Slider, ExposureSlider };
			TextBox[] AllPropValueTxtBxs = { BrightnessTxtBx, ContrastTxtBx, SaturationTxtBx, txb_Ch1, SharpnessTxtBx, GainTxtBx, txb_Ch2, GammaTxtBx, txb_Ch3, txb_Ch4, ExposureTxtBx };
			string defCamSettings = GlobalSettings.DefaultSurfaceLightSettings;
			if (((Button)sender).Content.ToString().Contains("Profile"))
				defCamSettings = GlobalSettings.DefaultProfileLightSettings;

            if (defCamSettings == "") return;

			for (int i = 0; i < defCamSettings.Length / 2; i++)
			{
				thisPropval = Convert.ToInt32(defCamSettings.Substring(i * 2, 2), 16);
				AllPropSliders[i].Value = thisPropval;
				AllPropValueTxtBxs[i].Text = thisPropval.ToString();
			}
		}

		void OKBtn_Click(object sender, RoutedEventArgs e)
		{
            if ((bool)DefaultBtn_RefDot.IsChecked)
                DefaultBtn_RefDot_Click(DefaultBtn_RefDot, null);
            this.Close();
		}

		void SpGain_Slider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
		{
			//int Value1 = (int)Ch1_Slider.Value;
			//int Value2 = (int)Ch3_Slider.Value;
			int propID = 100;
			Slider thisSlider = (Slider)sender;
			if (thisSlider.Name.Contains("1"))
			{
				txb_Ch1.Text = thisSlider.Value.ToString();
				propID = 3;
			}
			else if (thisSlider.Name.Contains("2"))
			{
				txb_Ch2.Text = thisSlider.Value.ToString();
				propID = 6;
			}
			else if (thisSlider.Name.Contains("3"))
			{
				txb_Ch3.Text = thisSlider.Value.ToString();
				propID = 8;
			}
			else if (thisSlider.Name.Contains("4"))
			{
				txb_Ch4.Text = thisSlider.Value.ToString();
				propID = 9;
			}
			//Value1 += ((int)Ch2_Slider.Value) * 16;
			//Value2 += ((int)Ch4_Slider.Value) * 16;
			if (!RefreshingFrontEndValuesOnly)
				RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(propID, (int)thisSlider.Value);
			//RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(9, Value2);
		}

		void RefreshCamSettings(string NewCamSettings)
		{
			RefreshingFrontEndValuesOnly = true;
			int thisPropval = 0;
			Slider[] AllPropSliders = { BrightnessSlider, ContrastSlider, SaturationSlider, Ch1_Slider, SharpnessSlider, GainSlider, Ch2_Slider, GammaSlider, Ch3_Slider, Ch4_Slider, ExposureSlider };
			TextBox[] AllPropValueTxtBxs = { BrightnessTxtBx, ContrastTxtBx, SaturationTxtBx, txb_Ch1, SharpnessTxtBx, GainTxtBx, txb_Ch2, GammaTxtBx, txb_Ch3, txb_Ch4, ExposureTxtBx };
			for (int i = 0; i < NewCamSettings.Length / 2; i ++)
			{
				thisPropval = Convert.ToInt32(NewCamSettings.Substring(i * 2, 2), 16);
				AllPropSliders[i].Value = thisPropval;
				AllPropValueTxtBxs[i].Text = thisPropval.ToString();
			}
			RefreshingFrontEndValuesOnly = false;
		}

		private void Btn_SetDefault_Click(object sender, RoutedEventArgs e)
		{
			Slider[] AllPropSliders = { BrightnessSlider, ContrastSlider, SaturationSlider, Ch1_Slider, SharpnessSlider, GainSlider, Ch2_Slider, GammaSlider, Ch3_Slider, Ch4_Slider, ExposureSlider };
			TextBox[] AllPropValueTxtBxs = { BrightnessTxtBx, ContrastTxtBx, SaturationTxtBx, txb_Ch1, SharpnessTxtBx, GainTxtBx, txb_Ch2, GammaTxtBx, txb_Ch3, txb_Ch4, ExposureTxtBx };
			string currCamSettings = "", tempstr = "";
			for (int i = 0; i < GlobalSettings.DefaultProfileLightSettings.Length / 2; i++)
			{
				tempstr = Convert.ToString((int)AllPropSliders[i].Value, 16);
				if (tempstr.Length < 2) tempstr = "0" + tempstr;
				currCamSettings += tempstr;
			}
			currCamSettings = currCamSettings.ToUpper();
			DB Settings_DataB;
            string ConnStr; 
            if (System.Environment.Is64BitOperatingSystem)
                ConnStr = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";
            else
                ConnStr = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + "\\Rapid-I 5.0\\Database\\RapidSettings.mdb;Persist Security Info=False;Jet OLEDB:Database Password=cTpL_4577ri";

            Settings_DataB = new DB(ConnStr);
			Settings_DataB.FillTable("DefaultCamSettings");
			DataRow thisEntry;
			DataRow[] allentriesforthismachine = Settings_DataB.Select_ChildControls("DefaultCamSettings", "MachineNo", GlobalSettings.MachineNo);
			if (allentriesforthismachine.Length > 0)
				thisEntry = allentriesforthismachine[allentriesforthismachine.Length - 1];
			else
				thisEntry = Settings_DataB.GetTable("DefaultCamSettings").NewRow();

            string[] fbtn = { " " };
            string BtnName = ((Button)sender).Content.ToString().Split(fbtn, StringSplitOptions.None)[1];
            switch (BtnName)
            {
                case "Profile":
                    GlobalSettings.DefaultProfileLightSettings = currCamSettings;
                    thisEntry["ProfileLightSettings"] = currCamSettings;
                    RWrapper.RW_MainInterface.MYINSTANCE().SetdefaultCamSettings(0, currCamSettings);
                    break;

                case "Surface":
                    GlobalSettings.DefaultSurfaceLightSettings = currCamSettings;
                    thisEntry["SurfaceLightSettings"] = currCamSettings;
                    RWrapper.RW_MainInterface.MYINSTANCE().SetdefaultCamSettings(1, currCamSettings);
                    break;

                case "RefDot":
                    GlobalSettings.RefDotLightSettings = currCamSettings;
                    thisEntry["RefDotCamSettings"] = currCamSettings;
                    RWrapper.RW_MainInterface.MYINSTANCE().SetdefaultCamSettings(2, currCamSettings);
                    break;
            }

            NumericUpDown[] cbs = { Ch1_DGain, Ch2_DGain, Ch3_DGain, Ch4_DGain };
			currCamSettings = "";
			currCamSettings = RWrapper.RW_MainInterface.MYINSTANCE().GetCamProperty(12).ToString();
			currCamSettings += RWrapper.RW_MainInterface.MYINSTANCE().GetCamProperty(13).ToString();

			for (int j = 0; j < cbs.Length; j++)
			{
				tempstr = Convert.ToString((int)cbs[j].Value, 16);
				if (tempstr.Length < 2) tempstr = "0" + tempstr;
				currCamSettings += tempstr;
			}
			thisEntry["DigitalGain_RCam3"] = currCamSettings;

			Settings_DataB.Update_Table("DefaultCamSettings");
		}

		private void ChangeDigitalGain(object sender, RoutedEventArgs e)
		{
			NumericUpDown[] cbs = { Ch1_DGain, Ch2_DGain, Ch3_DGain, Ch4_DGain };
			for (int j = 0; j < cbs.Length; j++)
			{
				if ((NumericUpDown)sender == cbs[j])
				{
					RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(14 + j, (int)cbs[j].Value);
					break;
				}
			}
		}

		private void btn_Test_Click(object sender, RoutedEventArgs e)
		{
			RWrapper.RW_MainInterface.MYINSTANCE().SetCamProperty(11, 1);
		}

        bool CamON = false;
        private void Btn_CrossHair_Offset_Click(object sender, RoutedEventArgs e)
        {
            //Cross_HairAligner cha = new Cross_HairAligner();
            //cha.Left = GlobalSettings.MainWin.MainLayout.ColumnDefinitions[0].ActualWidth + 8;
            //cha.Top = 132; GlobalSettings.MainWin.SetOwnerofWindow(cha); cha.Show();

            //Temporary Camera operator testing for Relay operations in OneShot
            if (!CamON)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().OperateSpecialPins(2, true); CamON = true;
            }
            else
            {
                RWrapper.RW_MainInterface.MYINSTANCE().OperateSpecialPins(2, false); CamON = false;
            }
            //System.Threading.Thread.Sleep(2000);

        }

        private void DefaultBtn_RefDot_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_CNC.MYINSTANCE().RefDotCamMode = (bool)DefaultBtn_RefDot.IsChecked;
            }
            catch (Exception ex)
            {

            }
        }
    }
}