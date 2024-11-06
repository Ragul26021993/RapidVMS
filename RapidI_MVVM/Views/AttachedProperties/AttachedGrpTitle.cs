using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;

namespace Rapid.AttachedProperties
{
    class AttachedGrpTitle: DependencyObject
    {
        //Text displayed in the group box title combo box
        public static readonly DependencyProperty GrpTitleTextProperty = DependencyProperty.RegisterAttached("GrpTitleText", typeof(String),
                                                               typeof(AttachedGrpTitle), new FrameworkPropertyMetadata(""));

        public static String GetGrpTitleText(DependencyObject obj)
        {
            return (String)obj.GetValue(GrpTitleTextProperty);
        }
        public static void SetGrpTitleText(DependencyObject obj, String value)
        {
            obj.SetValue(GrpTitleTextProperty, value);
        }
    }
}
