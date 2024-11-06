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

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for ImageDelayCaliberation.xaml
    /// </summary>
    public partial class ImageDelayCaliberation : Window
    {
        Brush BrushSuccessful, BrushFailed;
        public RoutedEventHandler WinClosing;
        private static ImageDelayCaliberation _MyInstance;
        public static ImageDelayCaliberation MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new ImageDelayCaliberation();
            }
            return _MyInstance;
        }
        private ImageDelayCaliberation()
        {
            InitializeComponent();
            init();
        }

        void init()
        {
            OKBtn.Click += new RoutedEventHandler(OKBtn_Click);
            CancelBtn.Click += new RoutedEventHandler(CancelBtn_Click);
            RWrapper.RW_ImageDelayCalibration.MYINSTANCE().ImageDelayEvent += new RWrapper.RW_ImageDelayCalibration.ImageDelayEventHandler(ImageDelayCaliberation_ImageDelayEvent);
            BrushSuccessful = (Brush)TryFindResource("SnapGreenStyle");
            BrushFailed = (Brush)TryFindResource("SnapRedStyle");
        }

        #region Button Click
        void OKBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                if ((bool)OKBtn.IsChecked)
                {
                    double _Distance = 0.0, _Speed = 0.0, _DroChange = 0.0, _AverageImageChange = 0.0;
                    Double.TryParse(DistanceTxtBx.Text, out _Distance);
                    Double.TryParse(SpeedTxtBx.Text, out _Speed);
                    Double.TryParse(AvgImageChangeTxtBx.Text, out _AverageImageChange);
                    Double.TryParse(DroChangeTxtBx.Text, out _DroChange);
                    _DroChange = _DroChange / 1000;
                    StatusTxtBlk.Foreground = Brushes.Black;
                    StatusTxtBlk.Text = "In Process...";
                    DelayTxtBlk.Text = "";
                    RWrapper.RW_ImageDelayCalibration.MYINSTANCE().StartImageDelayCalibration(_Distance, _Speed, _DroChange, _AverageImageChange);
                }
                if (!(bool)OKBtn.IsChecked)
                {
                    OKBtn.IsChecked = true;
                }
            }
            catch (Exception ex)
            {
                StatusTxtBlk.Text = "Error";
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:IDC01", ex);
            }

        }
        void CancelBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                this.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:IDC02", ex);
            }
        }
        #endregion

        #region Events
        void ImageDelayCaliberation_ImageDelayEvent(double DelayValue, bool SuccessFul)
        {
            try
            {
                if (this.Dispatcher.Thread == System.Threading.Thread.CurrentThread)
                {
                    if (SuccessFul)
                    {
                        StatusTxtBlk.Text = "Successful";
                        StatusTxtBlk.Foreground = BrushSuccessful;
                        DelayTxtBlk.Text = DelayValue.ToString();
                        OKBtn.IsChecked = false;
                    }
                    else
                    {
                        StatusTxtBlk.Text = "Failed";
                        StatusTxtBlk.Foreground = BrushFailed;
                        DelayTxtBlk.Text = DelayValue.ToString();
                        OKBtn.IsChecked = false;
                    }
                }
                else
                    this.Dispatcher.Invoke(System.Windows.Threading.DispatcherPriority.Normal,
                    new RWrapper.RW_ImageDelayCalibration.ImageDelayEventHandler(ImageDelayCaliberation_ImageDelayEvent), DelayValue, SuccessFul);
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:IDC03", ex);
            }
        }

        void RaiseClosingEvent()
        {
            if (WinClosing != null)
            {
                WinClosing(this, null);
            }
        }
        #endregion

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            RWrapper.RW_ImageDelayCalibration.MYINSTANCE().StopImageCalibration();
            RWrapper.RW_ImageDelayCalibration.Close_ImageDelayWindow();
            RaiseClosingEvent();
            _MyInstance = null;
        }
    }
}
