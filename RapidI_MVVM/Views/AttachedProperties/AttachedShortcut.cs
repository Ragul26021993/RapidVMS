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
    public class AttachedShortcut : DependencyObject
    {
        public static readonly DependencyProperty ShortcutProperty = DependencyProperty.RegisterAttached("Shortcut", typeof(string),
                                                                typeof(AttachedShortcut), new FrameworkPropertyMetadata(string.Empty));

        public static string GetShortcut(DependencyObject obj)
        {
            return (string)obj.GetValue(ShortcutProperty);
        }
        public static void SetShortcut(DependencyObject obj, string value)
        {
            obj.SetValue(ShortcutProperty, value);
        }
    }
}