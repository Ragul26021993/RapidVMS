using System;
using System.Collections;
using Microsoft.VisualBasic;
using Microsoft.VisualBasic.CompilerServices;

namespace rrm
{
    internal class xl_DB : DB
    {
        public xl_DB() : base(DatabaseConstants.ConnectionString)
        {
        }
        // To get any primary ID of a table by using SQL Queries .
        public int ReturnsID(string SQL_query)
        {
            var DB_Connec = new System.Data.OleDb.OleDbConnection(DatabaseConstants.ConnectionString);
            var DB_Command = new System.Data.OleDb.OleDbCommand(SQL_query, DB_Connec);
            DB_Connec.Open();
            try
            {
                if (!ReferenceEquals(DB_Command.ExecuteScalar(), DBNull.Value))
                {
                    return Conversions.ToInteger(DB_Command.ExecuteScalar());
                }
                else
                {
                    Interaction.MsgBox("The Value Is Not found !! Please verify the SQL query ");
                }
            }
            catch (Exception ex)
            {
                // On exception Closing the connection.And returning -1 from the function.
                DB_Connec.Close();
                return -1;
            }
            // If Everything goes Well then 
            // Closing and Disposing the Used database objects
            DB_Connec.Close();
            DB_Command.Dispose();
            DB_Connec.Dispose();
            return default;
        }
        // To check that any record is alreay existing in database or not.
        public bool Find_Record(string TableName, string Record, int ColumnNo)
        {
            try
            {
                // If DataS.Tables Is Nothing OrElse Not DataS.Tables.Contains(TableName) Then

                // End If
                // If record is found then return true ,otherwise returns false 
                string colName = DataS.Tables[TableName].Columns[ColumnNo].ColumnName;
                // Return DataS.Tables(TableName).Rows.Contains(Record)

                var foundrows = DataS.Tables[TableName].Select(colName + " = '" + Record + "'");
                if (foundrows.Length > 0)
                {
                    return true;
                }
            }
            // For i As Integer = 0 To DataS.Tables(TableName).Rows.Count - 1
            // If DataS.Tables(TableName).Rows(i).Item(ColumnNo).ToString() = Record Then Return True
            // Next
            catch (Exception ex)
            {
            }

            return false;
        }

        public void DeleteRecords(string SQL_query)
        {
            using (var DB_Connec = new System.Data.OleDb.OleDbConnection(DatabaseConstants.ConnectionString))
            {
                using (var DB_Command = new System.Data.OleDb.OleDbCommand(SQL_query, DB_Connec))
                {
                    DB_Connec.Open();
                    DB_Command.ExecuteNonQuery();
                }
            }
        }
        // Function to get all the Report Style Names from the  
        public ArrayList GetAllReportsStyleNames(int ProgID) // 'ByVal ProgramName As String
        {
            var Ar = new ArrayList();
            try
            {
                // Dim Id As Integer
                // 'Gaining the required ID from the given program name
                // Try
                // Id = ReturnsID("SELECT Program_Id FROM ProgramInformation WHERE Program_Name=" & "'" & ProgramName & "'")
                // Catch ex As Exception
                // Return Ar
                // End Try
                // Selecting all rows from the report style information where Program_Id = Id
                var dataR = Select_Rows("ReportStyleInformation", "Program_Id", ProgID); // ' Id)
                                                                                         // Adding only reportStyle Names to the arraylist
                for (int i = 0, loopTo = dataR.Length - 1; i <= loopTo; i++)
                    Ar.Add(dataR[i][2]);
            }
            catch (Exception ex)
            {
                Interaction.MsgBox(ex.Message);
            }

            // Returning all report style names 
            return Ar;
        }
    }
}