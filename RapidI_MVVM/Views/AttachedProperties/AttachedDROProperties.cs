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

namespace Rapid.AttachedProperties
{
    class AttachedDROProperties : DependencyObject
    {
        //DRO text
        public static readonly DependencyProperty DROTextProperty = DependencyProperty.RegisterAttached("DROText", typeof(String),
                    typeof(AttachedDROProperties), new FrameworkPropertyMetadata("-888.8888"));

        public static String GetDROText(DependencyObject obj)
        {
            return (String)obj.GetValue(DROTextProperty);
        }
        public static void SetDROText(DependencyObject obj, String value)
        {
            value = String.Format("{0:F" + GlobalSettings.CurrentDroPrecision + "}", value);
            obj.SetValue(DROTextProperty, value);
        }
        

        //DRO background colour
        public static readonly DependencyProperty DROTxtBGProperty = DependencyProperty.RegisterAttached("DROTextBG", typeof(SolidColorBrush),
                    typeof(AttachedDROProperties), new FrameworkPropertyMetadata(Brushes.Black));

        public static Brush GetDROTextBG(DependencyObject obj)
        {
            return (Brush)obj.GetValue(DROTxtBGProperty);
        }
        public static void SetDROTextBG(DependencyObject obj, Brush value)
        {
            obj.SetValue(DROTxtBGProperty, value);
        }



        //DRO foreground colour
        public static readonly DependencyProperty DROTxtFGProperty = DependencyProperty.RegisterAttached("DROTextFG", typeof(SolidColorBrush),
                   typeof(AttachedDROProperties), new FrameworkPropertyMetadata(Brushes.White));

        public static Brush GetDROTextFG(DependencyObject obj)
        {
            return (Brush)obj.GetValue(DROTxtFGProperty);
        }
        public static void SetDROTextFG(DependencyObject obj, Brush value)
        {
            obj.SetValue(DROTxtFGProperty, value);
        }
    }
}
