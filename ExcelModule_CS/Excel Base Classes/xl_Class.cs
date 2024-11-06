using System;
using System.Collections.Generic;
using Microsoft.VisualBasic;
using NPOI.SS.UserModel;
using NPOI.SS.Util;

namespace rrm
{
    internal class xl_Class : xlApp
    {
        public void InitializeExcelApplication(string ReportName, string sheetName, string Password = "")
        {
            try                                                      // File is Not open and is avaliable for writing.So open that file with the our excel application  
            {
                InitializeExcel();                                    // Initializing the excel application
                Open_File(ReportName, Password);                      // opening the report 
                if (!string.IsNullOrEmpty(sheetName))
                {
                    Open_Sheet(sheetName);
                }
                else
                {
                    Open_Sheet(1);
                }
            }
            // opening the active sheet of the workbook.
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1100: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }
        // ________________________________________________Write Values to the excel file_____________________________________________________________________________________________________________ 

        public void WriteValuesInExcel(string data, string coloumn, int row, bool color = false, string SuperSub = null, int Alignment = -4152)
        {
            try
            {
                if (string.IsNullOrEmpty(SuperSub))
                {
                    double FieldDbl;
                    if (double.TryParse(data, out FieldDbl))
                    {
                        this[row, coloumn] = FieldDbl;
                    }
                    else
                    {
                        this[row, coloumn] = data;
                    }

                    set_Set_Alignment(row, coloumn, -4152);
                    if (color)
                    {
                        set_Set_Color(row, coloumn, 35);
                    }
                }
                else if (SuperSub == "SuperSub")
                {
                    var Range = Get_Range(row, coloumn);
                    int _posIndex = data.IndexOf("@");
                    int _negIndex = data.IndexOf(";");
                    var ss = data.Split('@');
                    string tempString = "";
                    for (int i = 0, loopTo = ss.Length - 1; i <= loopTo; i++)
                        tempString += ss[i] + " ";
                    data = tempString;
                    tempString = "";
                    ss = data.Split(';');
                    for (int i = 0, loopTo1 = ss.Length - 1; i <= loopTo1; i++)
                        tempString += ss[i] + " ";
                    data = tempString;
                    // data.Remove(data.IndexOf("@"), 1) ', " ")
                    // data.Remove(data.IndexOf(";"), 1) ', " ")

                    double FieldDbl;
                    Range.SetCellValue(data);
                    //if (double.TryParse(data, out FieldDbl))
                    //{
                    //    Range.Value2 = FieldDbl;
                    //}
                    //else
                    //{
                    //    Range.Value2 = data;
                    //}

                    Range.CellStyle.Alignment = NPOI.SS.UserModel.HorizontalAlignment.Center;
                    //Range.HorizontalAlignment = -4108;
                    //Range.Characters(_posIndex + 1, _negIndex - _posIndex).Font.Superscript = true;
                    //Range.Characters(_negIndex + 2, data.Length - _negIndex).Font.Subscript = true;
                    //System.Runtime.InteropServices.Marshal.FinalReleaseComObject(Range);
                }
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1101: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        private object XLDataSheet, XLDataSheetSecond;

        public void Copy(string FinalRange, string InitialRange, string TarGetSheet, string TargetRange, bool OnlyCopy = false)
        {
            try
            {
                sheet = workbook.GetSheet(TarGetSheet);
                //XLDataSheet = xlW.Sheets(xlW.Sheets(TarGetSheet).Name);
                //xlS.Range(InitialRange, FinalRange).Copy();
                //if (!OnlyCopy)
                //{
                //    XLDataSheet.Range(TargetRange).PasteSpecial(13);
                //}
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1102: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void CopyBetweenTwoSheets(string SourceSheet, string FinalRange, string InitialRange, string TarGetSheet, string TargetRange, bool OnlyCopy = false)
        {
            try
            {
                var firstSheet = workbook.GetSheet(TarGetSheet);
                var secondSheet = workbook.GetSheet(SourceSheet);
                string tempRange = InitialRange + ":" + FinalRange;
                var Range = tempRange;
                var cellRange = CellRangeAddress.ValueOf(Range);
                
                for (var i = cellRange.FirstRow; i <= cellRange.LastRow; i++)
                {
                    var row = firstSheet.GetRow(i);
                    IRow secondRow = secondSheet.CreateRow(i);
                    for (var j = cellRange.FirstColumn; j <= cellRange.LastColumn; j++)
                    {
                        var tempValue = row.GetCell(j).StringCellValue.ToString();
                        secondRow.CreateCell(j).SetCellValue(tempValue);     
                    }
                }
 
                //XLDataSheet = xlW.Sheets(xlW.Sheets(TarGetSheet).Name);
                //XLDataSheetSecond = xlW.Sheets(xlW.Sheets(SourceSheet).Name);
                //XLDataSheetSecond.Range(InitialRange, FinalRange).Copy();
                //if (!OnlyCopy)
                //{
                //    XLDataSheet.Range(TargetRange).PasteSpecial(13);
                //}
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1103: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public int GetColIndexTemp1(string ColName)
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
        public void ClearFormatCondition(string CSheet, string FinalRange, string InitialRange)
        {
            sheet = workbook.GetSheet(CSheet);
            //XLDataSheet = xlW.Sheets(xlW.Sheets(CSheet).Name);
            string tempRange = InitialRange + ":" + FinalRange;
            var Range = tempRange;
            var cellRange = CellRangeAddress.ValueOf(Range);

            for (var i = cellRange.FirstRow; i <= cellRange.LastRow; i++)
            {
                row = sheet.GetRow(i);
                for (var j = cellRange.FirstColumn; j <= cellRange.LastColumn; j++)
                {
                    row.GetCell(j).SetCellValue(string.Empty);
                }
            }

            //XLDataSheet.Range(InitialRange + ":" + FinalRange).FormatConditions.Delete();
        }

        public void ClearData(string CSheet, string FinalRange, string InitialRange)
        {
            try
            {
                sheet = workbook.GetSheet(CSheet);
                //XLDataSheet = xlW.Sheets(xlW.Sheets(CSheet).Name);
                string tempRange = InitialRange + ":" + FinalRange;
                var Range = tempRange;
                var cellRange = CellRangeAddress.ValueOf(Range);
                for (var i = cellRange.FirstRow; i <= cellRange.LastRow; i++)
                {
                    row = sheet.GetRow(i);
                    for (var j = cellRange.FirstColumn; j <= cellRange.LastColumn; j++)
                    {
                        cell = row.CreateCell(j);
                        row.RemoveCell(cell);
                    }
                }
                //XLDataSheet.Range(InitialRange + ":" + FinalRange).Value = "";
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1104: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void CopyDetail(string FinalRange, bool OnlyCopy = false)
        {
            try
            {
                //XLDataSheet = xlW.Sheets(xlW.Sheets("Data").Name);
                //XLDataSheet.Range("A1", FinalRange).Copy();
                //if (!OnlyCopy)
                //{
                //    xlS.Range("A1").PasteSpecial(13);
                //}
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1105: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void CopyImageSheet(string FinalRange, bool OnlyCopy = false)
        {
            try
            {
                try
                {
                    sheet = workbook.GetSheet("ImageSheet");
                    //XLDataSheet = xlW.Sheets(xlW.Sheets("ImageSheet").Name);
                }
                catch (Exception ex1)
                {
                    sheet = workbook.GetSheet("Data");
                    //XLDataSheet = xlW.Sheets(xlW.Sheets("Data").Name);
                }

                //XLDataSheet.Range("A1", FinalRange).Copy();
                //if (!OnlyCopy)
                //{
                //    xlS.Range("A1").PasteSpecial(13);
                //}
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1106: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void Paste(string SourceSheet, string FinalRange, string InitialRange, string TargetRange)
        {
            try
            {
                //XLDataSheet = xlW.Sheets(xlW.Sheets(SourceSheet).Name);
                //XLDataSheet.Range(InitialRange, FinalRange).Copy();
                //xlS.Range(TargetRange).PasteSpecial(13); // Special(-4105)
            }
            catch (Exception ex) // Handle here all the exceptions ocurring.
            {
                Interaction.MsgBox("EE1107: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void PasteDetail(string FinalRange, string InitialRange)
        {
            try
            {
                //XLDataSheet = xlW.Sheets(xlW.Sheets("DetailSheet").Name);
                //XLDataSheet.Range("A1", FinalRange).Copy();
                //xlS.Range(InitialRange).PasteSpecial(13); // Special(-4105)
            }
            catch (Exception ex) // Handle here all the exceptions ocurring.
            {
                Interaction.MsgBox("EE1108: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }

        public void ReleaseExtraDatasheet()
        {
            try
            {
                // Make the RCW =0
                if (XLDataSheet is object)
                    System.Runtime.InteropServices.Marshal.FinalReleaseComObject(XLDataSheet);
                GC.Collect();
                GC.WaitForPendingFinalizers();
            }
            catch (Exception ex)
            {
                Interaction.MsgBox("EE1109: " + ex.Message + Constants.vbCrLf + ex.StackTrace);
            }          // handle here any error generated
        }
    }
}