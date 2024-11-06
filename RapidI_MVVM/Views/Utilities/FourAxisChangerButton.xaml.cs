using System;
using System.Windows;
using System.Windows.Controls;

namespace Rapid.Views.Utilities
{
    /// <summary>
    /// Interaction logic for FourAxisChangerButton.xaml
    /// </summary>
    public partial class FourAxisChangerButton : UserControl
    {
        private int _horiz_Offset, _vert_Offset;
        public int HorizontalOffset
        {
            get
            {
                return _horiz_Offset;
            }

            set
            {
                _horiz_Offset = value;
                tb_h.Text = value.ToString();
            }
        }

        public int VerticalOffset
        {
            get
            {
                return _vert_Offset;
            }

            set
            {
                _vert_Offset = value;
                tb_v.Text = value.ToString();
            }
    }
        public delegate void OffsetChangedHandler(object sender, FourButtonEventArgs e); // int Position, int OffsetValue);
        public event OffsetChangedHandler OffsetChanged;
        public FourAxisChangerButton()
        {
            InitializeComponent();
        }

        private void ChangeOffset(object sender, RoutedEventArgs e)
        {
            Button btn = sender as Button;
            switch(btn.Name)
            {
                case "Left":
                    HorizontalOffset--;
                    break;
                case "Right":
                    HorizontalOffset++;
                    //OffsetChanged(this, HorizontalOffset);// Convert.ToInt32(this.Tag) * 2, HorizontalOffset); //Raise the event //(int)this.Tag * 
                    break;
                case "Top":
                    VerticalOffset++;
                    //OffsetChanged(Convert.ToInt32(this.Tag) * 2 + 1, VerticalOffset); //Raise the event //
                    break;
                case "Bottom":
                    VerticalOffset--;
                    //OffsetChanged(Convert.ToInt32(this.Tag) * 2 + 1, VerticalOffset); //Raise the event //(int)this.Tag * 2 + 1
                    break;
            }
            OffsetChanged(this, new FourButtonEventArgs(HorizontalOffset, VerticalOffset)); // Convert.ToInt32(this.Tag) * 2, HorizontalOffset); //Raise the event //
            //tb_h.Text = HorizontalOffset.ToString();tb_v.Text = VerticalOffset.ToString();
        }
    }

    public class FourButtonEventArgs :EventArgs
    {
        public int HorizontalOffset;
        public int VerticalOffset;

        public FourButtonEventArgs(int h, int v)
        {
            HorizontalOffset = h; VerticalOffset = v;
        }
    }
}
