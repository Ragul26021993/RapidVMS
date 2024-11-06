using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for LastMeasurementDG.xaml
    /// </summary>
    public partial class LastMeasurementDG : Grid
	{
		int GradeIndex = 0;
		public LastMeasurementDG()
		{
			this.InitializeComponent();
			init();
		}
		void init()
		{
			GradeIndex = GlobalSettings.SettingsReader.GetRotaryGradeSelection();
			GradeSelectionCBx.Items.Add("Grade Type 1");
			GradeSelectionCBx.Items.Add("Grade Type 2");
			GradeSelectionCBx.SelectedIndex = GradeIndex;
			RWrapper.RW_DBSettings.MYINSTANCE().UpdateGradeSelection(GradeIndex);
			this.MouseRightButtonDown += new MouseButtonEventHandler(LastMeasurementDG_MouseRightButtonDown);
			OkBtn.Click += new RoutedEventHandler(OkBtn_Click);
			CancelBtn.Click += new RoutedEventHandler(CancelBtn_Click);
		}
		void LastMeasurementDG_MouseRightButtonDown(object sender, RoutedEventArgs e)
		{
			GradeSelectionCBx.SelectedIndex = GradeIndex;
			GradeSelectionGrid.Visibility = System.Windows.Visibility.Visible;
		}
		void OkBtn_Click(object sender, RoutedEventArgs e)
		{
			GradeSelectionGrid.Visibility = System.Windows.Visibility.Hidden;
			GradeIndex = GradeSelectionCBx.SelectedIndex;
			RWrapper.RW_DBSettings.MYINSTANCE().UpdateGradeSelection(GradeIndex);
			GlobalSettings.SettingsReader.SetRotaryGradeSelection(GradeIndex);
		}
		void CancelBtn_Click(object sender, RoutedEventArgs e)
		{
			GradeSelectionGrid.Visibility = System.Windows.Visibility.Hidden;
		}
	}
}