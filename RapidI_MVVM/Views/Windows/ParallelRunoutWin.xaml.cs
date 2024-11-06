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
    /// Interaction logic for ParallelRunoutWin.xaml
    /// </summary>
    public partial class ParallelRunoutWin : Window
    {
        public event RoutedEventHandler WinClosing;
        public bool Result { get; set; }
        private static ParallelRunoutWin _MyInstance;
        public static ParallelRunoutWin MyInstance()
        {
            if (_MyInstance == null)
            {
                _MyInstance = new ParallelRunoutWin();
            }
            return _MyInstance;
        }

        private ParallelRunoutWin()
        {
            InitializeComponent();
        }
        private void btnOK_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                int NumberOfFlutes = Convert.ToInt32(txtValue.Text);
                bool ParallelRunout = Convert.ToBoolean(Parallelchbk.IsChecked);
                bool isGeometric = Convert.ToBoolean(IsGeometricchbk.IsChecked);
                if (ParallelRunout)
                {
                    if (isGeometric)
                    {
                        RWrapper.RW_MainInterface.MYINSTANCE().SetRunoutFlute(NumberOfFlutes, 2);
                    }
                    else
                        RWrapper.RW_MainInterface.MYINSTANCE().SetRunoutFlute(NumberOfFlutes, 3);
                }
                else
                {
                    RWrapper.RW_MainInterface.MYINSTANCE().SetRunoutFlute(NumberOfFlutes, 1);
                }
                RWrapper.RW_MainInterface.MYINSTANCE().HandleMeasure_Click("Parallel Runout");
                this.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PR01", ex);
            }
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                this.Close();
            }
            catch (Exception ex)
            {
                RWrapper.RW_MainInterface.MYINSTANCE().WriteErrorLog("FrontEndError:PR02", ex);
            }
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
            _MyInstance = null;
            e.Cancel = true;
        }
    }
}
