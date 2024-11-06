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

namespace Rapid.AttachedProperties
{
	public class AttachedButtonName : DependencyObject
	{
        public static readonly DependencyProperty ButtonName = DependencyProperty.RegisterAttached("ButtonName", typeof(string),
                   typeof(AttachedButtonName), new FrameworkPropertyMetadata("BName"));
        public static string GetButtonName(DependencyObject obj)
        {
            return (string)obj.GetValue(ButtonName);
        }
        public static void SetButtonName(DependencyObject obj, string value)
        {
            obj.SetValue(ButtonName, value);
        }
	}
}
