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
    class AttachedCorner: DependencyObject
    {
        public static readonly DependencyProperty CornerProperty = DependencyProperty.RegisterAttached("Corner",typeof(CornerRadius),
                     typeof(AttachedCorner), new FrameworkPropertyMetadata(new CornerRadius(0)));

        public static CornerRadius GetCorner(DependencyObject obj)
        {
            return (CornerRadius)obj.GetValue(CornerProperty);
        }
        public static void SetCorner(DependencyObject obj, CornerRadius value)
        {
            obj.SetValue(CornerProperty, value);
        }
		
		
		//DRO text
        public static readonly DependencyProperty DROTextProperty = DependencyProperty.RegisterAttached("DROText", typeof(string),
                    typeof(AttachedCorner), new FrameworkPropertyMetadata("-888.8888"));

        public static string GetDROText(DependencyObject obj)
        {
            return (string)obj.GetValue(DROTextProperty);
        }
        public static void SetDROText(DependencyObject obj, string value)
        {
            obj.SetValue(DROTextProperty, value);
        }



        //DRO background colour
        public static readonly DependencyProperty DROTextBGProperty = DependencyProperty.RegisterAttached("DROTextBG", typeof(SolidColorBrush),
                    typeof(AttachedCorner), new FrameworkPropertyMetadata(Brushes.Black));

        public static Brush GetDROTextBG(DependencyObject obj)
        {
            return (Brush)obj.GetValue(DROTextBGProperty);
        }
        public static void SetDROTextBG(DependencyObject obj, Brush value)
        {
            obj.SetValue(DROTextBGProperty, value);
        }



        //DRO foreground colour
        public static readonly DependencyProperty DROTextFGProperty = DependencyProperty.RegisterAttached("DROTextFG", typeof(SolidColorBrush),
                   typeof(AttachedCorner), new FrameworkPropertyMetadata(Brushes.White));

        public static Brush GetDROTextFG(DependencyObject obj)
        {
            return (Brush)obj.GetValue(DROTextFGProperty);
        }
        public static void SetDROTextFG(DependencyObject obj, Brush value)
        {
            obj.SetValue(DROTextFGProperty, value);
        }
    }
}
