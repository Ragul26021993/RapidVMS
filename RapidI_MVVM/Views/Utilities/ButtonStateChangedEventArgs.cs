using System;

namespace Rapid.Views.Windows
{
    public class ButtonStateChangedEventArgs : EventArgs
    {
        public ButtonState NewValue { get; set; }

        public ButtonState OldValue { get; set; }

        public ButtonStateChangedEventArgs(ButtonState OldValue, ButtonState NewValue)
        {
            this.OldValue = OldValue;
            this.NewValue = NewValue;
        }
    }

    public class DrillCheckerButtonClickEventArgs : EventArgs
    {
        public int ButtonIndex { get; set; }

        public int State { get; set; }

        public DrillCheckerButtonClickEventArgs(int ButtonIndex, int State)
        {
            this.ButtonIndex = ButtonIndex;
            this.State = State;
        }
    }
}
