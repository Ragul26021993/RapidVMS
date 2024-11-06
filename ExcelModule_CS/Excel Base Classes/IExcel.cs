
using NPOI.SS.UserModel;

namespace rrm
{
    internal interface IExcel
    {
        // //Methods
        // Create the Excel Application 
        void InitializeExcel();
        // Open a certain Excel file 
        void Open_File(string FileName, string Password = "");
        void Open_File(string FileName, bool Excel_Visible);
        // Open Specific Sheet of excel workbook
        void Open_Sheet();
        void Open_Sheet(string SheetName);
        void Open_Sheet(int SheetNo);
        // Function to get the Range object itself 
        ICell Get_Range(int Row, string Column);
        // To save the Excel File 
        void Save();
        void SaveAs();
        // Exit the Excel Application 
        void ExitExcel();


        // //Properties 
        // Get or Set the Range object's value
        object this[int Row, string Column] { get; set; }
        // Setting the Alignment Of the Range object's value
        void set_Set_Alignment(int Row, string Column, int value);
        // Setting the color of the range 
        void set_Set_Color(int Row, string column, int value);
        // setting the format of the range object 
        void set_Set_Format(int Row, string Column, int value);
        // Setting the Auto-Column Width property 
        void set_Set_AutoColumnWidth(int row, string Column, bool value);
    }
}