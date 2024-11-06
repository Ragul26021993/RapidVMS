using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Rapid.Views.Windows
{
    public enum ButtonState
    {
        Up,
        Down
    }

    /// <summary>
    /// Interaction logic for CustomButton.xaml
    /// </summary>
    public partial class CustomButton : Button
    {
        public CustomButton()
        {
            InitializeComponent();
        }

        public delegate void ButtonStateChangedEventHandler(object sender, ButtonStateChangedEventArgs e);

        public event ButtonStateChangedEventHandler ButtonStateChanged;

        public static DependencyProperty ButtonStateProperty = DependencyProperty.Register("ButtonState", typeof(ButtonState), typeof(CustomButton), new FrameworkPropertyMetadata(ButtonState.Down) { PropertyChangedCallback = new PropertyChangedCallback(ButtonState_Changed) });

        public ButtonState ButtonState
        {
            get
            {
                return (ButtonState)GetValue(ButtonStateProperty);
            }
            set
            {
                SetValue(ButtonStateProperty, value);
            }
        }

        private static void ButtonState_Changed(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            ((CustomButton)d).ButtonStateChanged?.Invoke(d, new ButtonStateChangedEventArgs((ButtonState)e.OldValue, (ButtonState)e.NewValue));
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            switch (ButtonState)
            {
                case ButtonState.Up:
                    ButtonState = ButtonState.Down;
                    Background = new SolidColorBrush(Colors.Blue);
                    Foreground = new SolidColorBrush(Colors.White);
                    break;
                case ButtonState.Down:
                    ButtonState = ButtonState.Up;
                    Background = new SolidColorBrush(Colors.LightBlue);
                    Foreground = new SolidColorBrush(Colors.Black);
                    break;
                default:
                    break;
            }
        }

        private void Button_Loaded(object sender, RoutedEventArgs e)
        {
            Background = new SolidColorBrush(Colors.Blue);
        }
    }

}
