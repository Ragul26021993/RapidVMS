using System;

namespace rrm
{
    static class DatabaseConstants
    {
        // Dim AppDataFolderPath As String = Application.StartupPath
        internal static string AppDataFolderPath = Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData) + @"\Rapid-I 5.0";
        internal static string TemplatePath = AppDataFolderPath + @"\Database";
        internal static string TemporaryFilePath = AppDataFolderPath + @"\Temp";
        internal static string AppDataFdrPathforHC = AppDataFolderPath + @"\Reports";
        // Friend ConnectionString As String = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" & AppDataFolderPath & "\Database\ExcelDoc.accdb" '& ";Persist Security Info=False;Jet OLEDB:Database Password=excelforctpl"
        internal static string ConnectionString;
        internal static string[] TableNames = new string[] { "ProgramInformation", "TitleInformation", "FieldInformation", "ReportStyleinformation", "ReportInformation", "MasterTemplateSettings", "MultiProgramReport", "Settings" };
    }
}