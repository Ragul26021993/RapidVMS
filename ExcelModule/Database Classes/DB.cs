using System;
using System.Data;
using System.Data.OleDb;
using Microsoft.VisualBasic.CompilerServices;

namespace rrm
{
    internal partial class DB : IDatabaseInterac
    {
        // The connection string to initialize the connection :: Note connection is yet not initialized 
        // since implementation is of disconnected mode nature ,hence no oledb connection object is used 
        // Infact the fill method of DataAdapter will initialize the connection -- fill the dataset with the table -- and then terminates the connection 
        void IDatabaseInterac.StartConnection(string ConnectionString)
        {
            this.ConnectionString = ConnectionString;
        }
        // Fill one specified table
        public DataSet FillTable(string TableName)
        {
            if (!string.IsNullOrEmpty(ConnectionString))
            {
                DataA = new OleDbDataAdapter("SELECT * FROM " + "[" + TableName + "]", ConnectionString);
                if (DataS.Tables.Contains(TableName))
                    DataS.Tables.Remove(TableName);
                DataA.Fill(DataS, TableName);
                return DataS;
            }
            else
            {
                throw new ApplicationException("The Connection String is Not Proper !!Please Verify it and retry.");
            }
        }
        // Fill Range of tables
        public DataSet FillTableRange(string[] TableNames)
        {
            for (int i = 0, loopTo = TableNames.Length - 1; i <= loopTo; i++)
                FillTable(TableNames[i]);
            return DataS;
        }
        // get any specified table (Overloaded)
        public DataTable GetTable(string TableName)
        {
            if (DataS.Tables is null || !DataS.Tables.Contains(TableName))
            {
                FillTable(TableName);
            }

            return DataS.Tables[TableName];
        }

        public DataTable GetTable(int TableNo)
        {
            return DataS.Tables[TableNo];
        }
        // To get any single row from the table(Overloaded)
        public DataRow GetRow(int TableNo, int RowNo)
        {
            return DataS.Tables[TableNo].Rows[RowNo];
        }

        public DataRow GetRow(string TableName, int RowNo)
        {
            return DataS.Tables[TableName].Rows[RowNo];
        }
        // Gives the array of DataRows that satisfy the given condition (overloaded)
        public DataRow[] Select_Rows(string Tablename, string ColumnName, object Parameter)
        {
            return GetTable(Tablename).Select(ColumnName + "=" + "'" + Conversions.ToString(Parameter) + "'");
        }

        public DataRow[] Select_RowsInt(string Tablename, string ColumnName, object Parameter)
        {
            return GetTable(Tablename).Select(ColumnName + "=" + Conversions.ToString(Parameter));
        }

        public void Delete_RowInt(string Tablename, string ColumnName, object Parameter)
        {
            foreach (DataRow dr in GetTable(Tablename).Select(ColumnName + "=" + Conversions.ToString(Parameter)))
                dr.Delete();
        }

        public void Delete_Row(string Tablename, string ColumnName, object Parameter)
        {
            GetTable(Tablename).Select(ColumnName + "=" + "'" + Conversions.ToString(Parameter) + "'")[0].Delete();
        }

        public void Rename_Row(string Tablename, string ColumnName, object Parameter)
        {
            GetTable(Tablename).Select(ColumnName + "=" + "'" + Conversions.ToString(Parameter) + "'")[0].Delete();
        }

        public DataRow[] Select_Rows(int TableNo, int ColumnName, object Parameter)
        {
            return GetTable(TableNo).Select(ColumnName + "=" + "'" + Conversions.ToString(Parameter) + "'");
        }
        // Get any specific column value from dataset loaded(Overloaded)
        public object get_GetRecord(int TableNo, int RowNo, int ColumnNo)
        {
            return DataS.Tables[TableNo].Rows[RowNo][ColumnNo];
        }

        public object get_GetRecord(string TableName, int RowNo, int columnNo)
        {
            return DataS.Tables[TableName].Rows[RowNo][columnNo];
        }

        public object get_GetRecord(string TableName, int RowNo, string ColumnName)
        {
            return DataS.Tables[TableName].Rows[RowNo][ColumnName];
        }
        // Set any particular value of specific column and row in dataset(Overloaded)
        public void set_SetRecord(int TableNo, int RowNo, int ColumnNo, object value)
        {
            DataS.Tables[TableNo].Rows[RowNo][ColumnNo] = value;
        }

        public void set_SetRecord(string TableName, int RowNo, int columnNo, object value)
        {
            DataS.Tables[TableName].Rows[RowNo][columnNo] = value;
        }

        public void set_SetRecord(string TableName, int RowNo, string ColumnName, object value)
        {
            DataS.Tables[TableName].Rows[RowNo][ColumnName] = value;
        }
        // Updates the database
        public virtual bool Update_Table(string TableName)
        {
            try
            {
                // DataAdapter is Mapped again to given table only ,to have the correct Update command being calculated by CommandBuilder object.
                // It is necessary.Since,we are loading many tables into dataset ,Hence DataAdapter must be in syncronization with a particular Table only at a given time.
                // This Method is also recommended by MSDN and also it does not create any major/minor performance loss at all.
                // Albeit ,it is the most safer method to do updation of database without using stored SQL strings queries or to remember them by heart!!
                DataA = new OleDbDataAdapter("SELECT * FROM " + "[" + TableName + "]", ConnectionString);
                CommB = new OleDbCommandBuilder(DataA);
                // MsgBox(CommB.GetInsertCommand.CommandText)
                DataA.Update(DataS, TableName);
            }
            catch (Exception ex)
            {
                throw new Exception(ex.Message, ex);
            } // The Table mappings are not valid <OR> The Update command autogenerated by command Builder object is Wrong <OR> The Table Name given is not into the dataset itself!!

            return default;
        }

        public bool AddColumnToTable(string TableName, string ColumnName, Type ColumnType)
        {
            var conn = new OleDbConnection(ConnectionString);
            try
            {
                var cmd = new OleDbCommand();
                conn.Open();
                cmd.Connection = conn;
                string oT;
                string colType = "Text";
                switch (ColumnType.FullName ?? "")
                {
                    case "System.String":
                        {
                            oT = "CHAR(255)";
                            break;
                        }

                    case "System.Int32":
                        {
                            oT = "INTEGER";
                            break;
                        }

                    default:
                        {
                            oT = "CHAR(255)";
                            break;
                        }
                }

                cmd.CommandText = "ALTER TABLE " + TableName + " ADD COLUMN " + ColumnName + " " + oT;
                // cmd.ComandType = CommandType.Text
                cmd.ExecuteNonQuery();
                conn.Close();
                return true;
            }
            catch (Exception ex)
            {
                conn.Close();
                return false;
            }
        }
    }
    // Other Supportive methods and declarations for this class -- For good debugging support it is classified like this ;can be merged later,once debugging is fully done. 
    internal partial class DB
    {
        // Only these are required for the DisConnected Mode of database interaction
        internal OleDbDataAdapter DataA;
        protected OleDbCommandBuilder CommB;
        internal DataSet DataS = new DataSet();
        private string ConnectionString = "";

        void StartConnection(string ConnectionString)
        {
            this.ConnectionString = ConnectionString;
        }
        public DB(string ConnectStr)
        {
            this.StartConnection(ConnectStr);
        } // ==The Constructor with a connection string as parameter 
          // ==Function to check if any given value is equivalent to database NULL value or not 
        protected internal virtual bool IsNull(object O)
        {
            return Equals(O, DBNull.Value);
        }
        // ==To clean up the used database Objects 
        [System.Runtime.ConstrainedExecution.ReliabilityContract(System.Runtime.ConstrainedExecution.Consistency.WillNotCorruptState, System.Runtime.ConstrainedExecution.Cer.Success)]
        protected void FinalizeClass()
        {
            // clean Up neatly the database objects 
            if (DataA is object)
                DataA.Dispose();
            if (DataS is object)
                DataS.Dispose();
            if (CommB is object)
                CommB.Dispose();
            ConnectionString = null;
        }
    }
}