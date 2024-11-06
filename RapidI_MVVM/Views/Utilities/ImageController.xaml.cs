using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Imaging;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for ImageController.xaml
    /// </summary>
    public partial class ImageController : Grid
    {
        public RoutedEventHandler ButtonClicked;
        public ImageController()
        {
            InitializeComponent();
        }

        public byte[] MyImageSource
        {
            get { return base.GetValue(MyImageSourceProperty) as byte[]; }
            set { base.SetValue(MyImageSourceProperty, value); }
        }

        public bool ShowButton
        {
            get { return (bool)GetValue(ShowButtonProperty); }
            set { SetValue(ShowButtonProperty, value); }
        }
       
        public static readonly DependencyProperty MyImageSourceProperty =
            DependencyProperty.Register("MyImageSource", typeof(byte[]), typeof(ImageController),
            new FrameworkPropertyMetadata(new PropertyChangedCallback(OnImageSourceChanged)));

        private static readonly DependencyProperty ShowButtonProperty = 
            DependencyProperty.Register("ShowButton",typeof(bool), typeof(ToleranceControl),
            new PropertyMetadata(false, new PropertyChangedCallback(HandleShowBtnChange)));

        private static void OnImageSourceChanged(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            ImageController userControl = (ImageController)sender;
            userControl.IconImg.Source = new BitmapImage((Uri)e.NewValue);
        }
        private static void HandleShowBtnChange(DependencyObject sender, DependencyPropertyChangedEventArgs e)
        {
            ImageController userControl = (ImageController)sender;
            if (userControl.ShowButton)
                userControl.MainGrid.ColumnDefinitions[1].Width = new GridLength(0);
            userControl.IconImg.Source = new BitmapImage((Uri)e.NewValue);
        }

        protected void OnButtonClick(object Sender, RoutedEventArgs e)
        {
            if (ButtonClicked != null)
            {
                ButtonClicked(this, null);
            }
        }
    }
}
