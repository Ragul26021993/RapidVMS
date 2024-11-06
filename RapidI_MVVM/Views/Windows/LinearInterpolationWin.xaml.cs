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
using System.Linq;

namespace Rapid.Windows
{
	/// <summary>
	/// Interaction logic for CalibrationWin.xaml
	/// </summary>
	public partial class LinearInterpolationWin : Window
	{
        public LinearInterpolationWin()
		{
			this.InitializeComponent();
			// Insert code required on object creation below this point.
            TargetPtBtn.Click += new RoutedEventHandler(TargetPtBtn_Click);
            GoBtn.Click += new RoutedEventHandler(GoBtn_Click);
        }

        void TargetPtBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_LinearInterPolation.MYINSTANCE().SetTargetPosition();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CIW01", ex);
            }
        }

        void GoBtn_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                RWrapper.RW_LinearInterPolation.MYINSTANCE().StartLinearInterpolation();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:CIW01", ex);
            }
        }
    }
}