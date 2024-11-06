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
    /// Interaction logic for CircularInterpolationPartProgWin.xaml
    /// </summary>
    public partial class CircularInterpolationPartProgWin : Window
    {
        private static CircularInterpolationPartProgWin Myobject;
        public static CircularInterpolationPartProgWin MyInstance()
        {
            if (Myobject == null)
            {
                Myobject = new CircularInterpolationPartProgWin();
            }
            return Myobject;
        }
        private CircularInterpolationPartProgWin()
        {
            InitializeComponent();
            TakePointBtn.Click+=new RoutedEventHandler(TakePointBtn_Click);
        }
        protected void TakePointBtn_Click(object sender, EventArgs e)
        {
            Button Btn = (Button)sender;
            switch (Btn.Content.ToString())
            {
                case "1":
                    {
                        Btn.Content = "2";
                    }
                    break;
                case "2":
                    {
                        Btn.Content = "3";
                    }
                    break;
                case "3":
                    {
                        Btn.Content = "1";
                    }
                    break;
            }
        }
        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            Myobject = null;          
        }
    }
}
