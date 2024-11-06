using System;
using System.Collections;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using Microsoft.VisualBasic;
using Microsoft.VisualBasic.CompilerServices;
using NPOI.HSSF.UserModel;
using NPOI.SS.UserModel;
using NPOI.SS.Util;
using NPOI.XSSF.UserModel;

namespace rrm
{
    internal abstract class xlApp : IExcel, IDisposable
    {
        protected internal object xlA, xlW, xlS, xlSAdditional;
        protected internal FileStream file;
        protected internal IWorkbook workbook;
        protected internal ISheet sheet;
        protected internal IRow row;
        protected internal ICell cell;
        string SheetName;
        bool XssfWorkBook, HssfWorkBook;
        
        // ==Initializing the Excel Application Object 
        public void InitializeExcel()
        {
            try
            {
                string culture = System.Threading.Thread.CurrentThread.CurrentCulture.ToString();
                var ci = new CultureInfo(culture);
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1200: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }
        // ==Opening the Given Excel file (overloaded)

        private bool OpenInExcel = true;
        private bool OpenedExcel = false;

        public void Open_File(string FileName, string Password = "")
        {
            try
            {
                OpenInExcel = true;

                file = new FileStream(FileName, FileMode.Open, FileAccess.ReadWrite);
                if (FileName.IndexOf(".xlsx") > 0)
                {
                    workbook = new XSSFWorkbook(file);
                    XssfWorkBook = true;
                }
                else if (FileName.IndexOf(".xls") > 0)
                {
                    workbook = new HSSFWorkbook(file);
                    HssfWorkBook = true;
                }
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1200: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }   
        }

        public void Open_File(string FileName, bool Excel_Visible)
        {
            //try
            //{
            //    OpenInExcel = true;
            //    for (int i = 1, loopTo = Conversions.ToInteger(xlA.WorkBooks.Count); i <= loopTo; i++)
            //    {
            //        if (Conversions.ToBoolean(Operators.ConditionalCompareObjectEqual(xlA.WorkBooks.item(i).FullName, FileName, false)))
            //        {
            //            xlW = xlA.Workbooks(i);
            //            OpenInExcel = false;
            //        }
            //    }

            //    if (OpenInExcel)
            //    {
            //        xlW = xlA.Workbooks.Open(FileName);
            //    }

            //    if (Excel_Visible)
            //    {
            //        // Making The Excel Application visible 
            //        xlA.Visible = Excel_Visible;
            //        // Setting the Object refrences to nothing(This decrements RCW of COM object)
            //        xlW = null;
            //        xlA = null;
            //        // Calling GC so that as soon as excel is closed by user through MicroSoft Excel UI ,Excel objects are claimed by GC.
            //        GC.Collect();
            //        GC.WaitForPendingFinalizers();
            //    }
            //}
            //catch (Exception ex)
            //{
            //    Interaction.MsgBox("EE1202: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            //}          // handle here any error generated
        }
        // ===Opening Specified excel sheet from workbook(Overloaded)
        public void Open_Sheet()
        {
            try
            {
                int NoOfSheets = workbook.NumberOfSheets;
                for(int i =0; i< NoOfSheets; i++)
                {
                    sheet = workbook.GetSheetAt(i);
                }
                //xlS = xlW.Sheets(xlW.ActiveSheet.Name);
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1203: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void Open_Sheet(int SheetNo)
        {
            try
            {
                sheet = workbook.GetSheetAt(SheetNo);
                //xlS = xlW.Sheets(xlW.Sheets(SheetNo).Name);
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1204: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void Open_Sheet(string Sheetname)
        {
            try
            {
                sheet = workbook.GetSheet(Sheetname);
                //xlS = xlW.Sheets(SheetName);
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1205: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }
        public int GetColIndexTemp(string ColName)
        {
            var colIndex = default(int);
            if (Strings.Len(ColName) == 1)
            {
                colIndex = Strings.Asc(ColName) - 64;
            }
            else if (Strings.Len(ColName) == 2)
            {
                colIndex = (Strings.Asc(Strings.Mid(ColName, 1, 1)) - 64) * 26 + Strings.Asc(Strings.Mid(ColName, 2, 1)) - 64;
            }

            return colIndex;
        }
        // ====Getting the Range Object Itself 
        public ICell Get_Range(int Row, string Column)
        {
            try
            {
                cell = sheet.GetRow(Row).GetCell(GetColIndexTemp(Column));
                //var Range = xlS.Cells(Row, Column);
                return cell;
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1206: " + ex.Message + Constants.vbCrLf + ex.StackTrace);          // handle here any error generated
                return null;
            }
        }
        // ====The Overridable function to implement the Save as functionality.
        // There can be various variations of save as ... Sometime only file name want to be changed ,sometime password is needed,
        // Some time conversion to other file formats can also be required ,then some security and crypotographic actions can also be taken.
        public virtual void SaveAs()
        {
        }
        // ===Saving the excel Workbook and Closing it. 
        public void Save()
        {
            workbook.Write(file);
            //xlW.Save();
            if (OpenInExcel)
            {
                file.Close();
                //xlW.Close();
            }

            if (OpenedExcel)
            {
                file.Close();
                //xlA.Quit();
            }
        }

        public void Close()
        {
            if (OpenInExcel)
            {
                file.Close();
                //xlW.Close();
            }

            if (OpenedExcel)
            {
                //xlA.Quit();
            }
        }

        // ==Function to Exit the Excel Application neatly 
        public void ExitExcel()
        {
            // Making RCW count of COM objects as '0'
            if (OpenInExcel)
            {
                if (xlS is object)
                {
                    while (System.Runtime.InteropServices.Marshal.FinalReleaseComObject(xlS) != 0)
                    {
                    }
                }

                if (xlW is object)
                {
                    while (System.Runtime.InteropServices.Marshal.FinalReleaseComObject(xlW) != 0)
                    {
                    }
                }
            }

            if (OpenedExcel)
            {
                if (xlA is object)
                {
                    while (System.Runtime.InteropServices.Marshal.FinalReleaseComObject(xlA) != 0)
                    {
                    }
                }
            }
            // Making Them equal to NULL 
            if (xlS is object)
                xlS = null;
            if (xlW is object)
                xlW = null;
            if (xlA is object)
                xlA = null;
            // Calling Garbage Collector to claim these Unrefrenced Objects from Memory
            GC.Collect();
            GC.WaitForPendingFinalizers();
        }
        // ==The Property to have the Range's value 
        public object this[int Row, string Column]
        {
            get
            {
                return Get_Range(Row, Column);
            }

            set
            {
                Get_Range(Row, Column);
            }
        }
        ICellStyle cellStyle;
        // ==Setting the alignment of the range's object 
        public void set_Set_Alignment(int Row, string Column, int value)
        {
            Get_Range(Row, Column).CellStyle.Alignment = HorizontalAlignment.Center;
            //Get_Range(Row, Column).HorizontalAlignment = value;
        }
        // ==Setting the Color of the range's object 
        public void set_Set_Color(int Row, string column, int value)
        {
            if (value == 35)
            {
                Get_Range(Row, column).CellStyle.FillForegroundColor = IndexedColors.LightBlue.Index;
            }
            else if (value == 3)
            {
                Get_Range(Row, column).CellStyle.FillForegroundColor = IndexedColors.Red.Index;
            }
            else if (value == 4)
            {
                Get_Range(Row, column).CellStyle.FillForegroundColor = IndexedColors.Green.Index;
            }
            else if (value == 2)
            {
                Get_Range(Row, column).CellStyle.FillForegroundColor = IndexedColors.White.Index;
            }
            else if (value == 46)
            {
                Get_Range(Row, column).CellStyle.FillForegroundColor = IndexedColors.Orange.Index;
            }

            //Get_Range(Row, column).Interior.ColorIndex = value;
        }
        // ==Setting the autofit property of the Columns 
        public void set_Set_AutoColumnWidth(int row, string Column, bool value)
        {
            if (value)
            {
                sheet.AutoSizeColumn(GetColIndexTemp(Column));
            }
        }

        public void set_Set_Format(int Row, string Column, int value)
        {
        }

        private bool disposedValue = false;        // To detect redundant calls
                                                   // IDisposable
        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // This sub will free all the COM objects being used by this class.
                    ExitExcel();
                }
            }

            disposedValue = true;
        }

        public void SetChartRange(string SheetName, int ChartIndex, string ValueRange)
        {
            object xlSS;
        }


        #region  IDisposable Support 
        // This code added by Visual Basic to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code.  Put cleanup code in Dispose(ByVal disposing As Boolean) above.
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}