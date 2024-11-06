using System;
using System.Windows.Data;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows;

namespace Rapid.Converters
{

    public class ContentToPathConverter : IValueConverter
    {
        readonly static ContentToPathConverter value = new ContentToPathConverter();
        public static ContentToPathConverter Value
        {
            get { return value; }
        }

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            ContentPresenter cp = (ContentPresenter)value;
            double h = cp.ActualHeight > 10 ? 32 : 10;
            double w = cp.ActualWidth > 10 ? cp.ActualWidth : 10;
            PathSegmentCollection ps = new PathSegmentCollection(4)
            {
                new LineSegment(new Point(1,0.7* h), true),
                new BezierSegment(new Point(1, 0.9 * h), new Point(0.1 * h, h), new Point(0.3 * h, h), true),
                new LineSegment(new Point(w - 2, h), true),
                new BezierSegment(new Point(w + 0.6 * h, h), new Point(w + h, 0), new Point(w + h * 1, 0), true)
            };
            //cp.Height = 28;
            return ps;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
        #endregion
    }

    public class ContentToMarginConverter : IValueConverter
    {
        readonly static ContentToMarginConverter value = new ContentToMarginConverter();
        public static ContentToMarginConverter Value
        {
            get { return value; }
        }

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return new Thickness(0, 0, -((ContentPresenter)value).ActualHeight, 0);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
        #endregion
    }

    public class StringToDoubleConverter : IValueConverter
    {
        readonly static StringToDoubleConverter value = new StringToDoubleConverter();
        public static StringToDoubleConverter Value
        {
            get { return value; }
        }

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double ConvertedVal = 0;
            if (double.TryParse(value.ToString(), out ConvertedVal))
                return ConvertedVal;
            return 0;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return value.ToString();
        }
        #endregion
    }

    public class DoubleToStringConverter : IValueConverter
    {
        readonly static DoubleToStringConverter value = new DoubleToStringConverter();
        public static DoubleToStringConverter Value
        {
            get { return value; }
        }

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return value.ToString();
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            double ConvertedVal = 0;
            if (double.TryParse(value.ToString(), out ConvertedVal))
                return ConvertedVal;
            return 0;
        }
        #endregion
    }
    public class ReverseBoolToVisibilityConvertor : IValueConverter
    {
        readonly static ReverseBoolToVisibilityConvertor value = new ReverseBoolToVisibilityConvertor();
        public static ReverseBoolToVisibilityConvertor Value
        {
            get { return value; }
        }

        #region IValueConverter Members

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            if (value != null)
            {
                if ((bool)value)
                {
                    return System.Windows.Visibility.Collapsed;
                }
                else
                {
                    return System.Windows.Visibility.Visible;
                }
            }
            else
                return System.Windows.Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            throw new NotImplementedException();
        }
        #endregion
    }
}
