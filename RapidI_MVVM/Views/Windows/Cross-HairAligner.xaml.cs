using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using Rapid.Views.Utilities;

namespace Rapid.Windows
{
    /// <summary>
    /// Interaction logic for Cross_HairAligner.xaml
    /// </summary>
    public partial class Cross_HairAligner : Window
    {
        int[] XH_Offsets = { 0, 0, 0, 0, 0, 0, 0, 0 };
        public Cross_HairAligner()
        {
            InitializeComponent();
            this.Left_Line.OffsetChanged += new FourAxisChangerButton.OffsetChangedHandler(HandleOffsetChanged);
            this.Right_Line.OffsetChanged += new FourAxisChangerButton.OffsetChangedHandler(HandleOffsetChanged);
            this.Top_Line.OffsetChanged += new FourAxisChangerButton.OffsetChangedHandler(HandleOffsetChanged);
            this.Bottom_Line.OffsetChanged += new FourAxisChangerButton.OffsetChangedHandler(HandleOffsetChanged);
            RWrapper.RW_MainInterface.MYINSTANCE().CrossHairOffsets(XH_Offsets, true);
            this.Left_Line.HorizontalOffset = XH_Offsets[0];
            this.Left_Line.VerticalOffset = XH_Offsets[1];
            this.Top_Line.HorizontalOffset = XH_Offsets[2];
            this.Top_Line.VerticalOffset = XH_Offsets[3];
            this.Right_Line.HorizontalOffset = XH_Offsets[4];
            this.Right_Line.VerticalOffset = XH_Offsets[5];
            this.Bottom_Line.HorizontalOffset = XH_Offsets[6];
            this.Bottom_Line.VerticalOffset = XH_Offsets[7];
        }

        public void HandleOffsetChanged(object sender, FourButtonEventArgs e) // int Position, int Value)
        {
            FourAxisChangerButton fb = sender as FourAxisChangerButton;
            int Position = Convert.ToInt32(fb.Tag) * 2;

            XH_Offsets[Position] = e.HorizontalOffset;
            XH_Offsets[Position + 1] = e.VerticalOffset;
            RWrapper.RW_MainInterface.MYINSTANCE().CrossHairOffsets(XH_Offsets, false);
        }

        private void BtnClose_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
