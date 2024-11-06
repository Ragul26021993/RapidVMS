using System.Windows.Controls;

namespace Rapid.Utilities
{
    /// <summary>
    /// Interaction logic for ToolbarWin.xaml
    /// </summary>
    public partial class ToolbarGrid : Grid
    {
        ScrToolbar MyChild;
        
        public ToolbarGrid()
        {
            InitializeComponent();
        }

        public void SetContent(ScrToolbar con)
        {
            Grid.SetColumn(con, 0);
            Grid.SetRow(con, 0);
            Grid.SetColumnSpan(con, 1);
            MyChild = con;
            this.Children.Add(con);
        }
        public ScrToolbar GetContent()
        {
            return MyChild;
        }
    }
}
