using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace Rapid.Windows.AttachedProperties
{
    class NumberRangeRule : ValidationRule
    {
        int _min;
        int _max;

        public int Min
        {
            get { return _min; }
            set { _min = value; }
        }

        public int Max
        {
            get { return _max; }
            set { _max = value; }
        }

        public override ValidationResult Validate(object value, System.Globalization.CultureInfo cultureInfo)
        {
            int number;
            if (!int.TryParse((string)value, out number))
            {
                return new ValidationResult(false, "Invalid number format");
            }
            if (number < _min || number > _max)
            {
                return new ValidationResult(false, string.Format("Number out of Range ({0}-{1})", _min, _max));
            }
            //throw new NotImplementedException();
            return ValidationResult.ValidResult;
        }
    }
}
