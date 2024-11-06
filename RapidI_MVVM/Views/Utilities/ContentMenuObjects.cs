namespace Rapid.Utilities
{
    public class ContentMenuObjects
    {
        public int Id;
        public int ParentId;
        public string Alignment = "";
        public string ItemName = "";
        public int Frequency = 0;
        public int ButtonType = 0; 
        public bool HideTool = false;
        public bool CheckedProperty = false;
        public bool ThreeDButtonType = false;
        public bool IgnoreOriginalStateDuringClearAll = false;
        public string ToolTip = "";       
        public bool HasClickEvent = true;
        public System.Windows.Controls.Image Image;
        public string ContextMenuname = "";
        public bool isSeperator = false;
    }
}
