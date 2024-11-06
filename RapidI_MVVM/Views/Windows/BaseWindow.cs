using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Animation;
using System.Windows.Media;

namespace Rapid.Windows
{
    class BaseWindow : Window
    {
        public BaseWindow()
            : base()
        {
            this.ShowInTaskbar = false;
			this.ResizeMode = ResizeMode.NoResize;
        }
    }
}
