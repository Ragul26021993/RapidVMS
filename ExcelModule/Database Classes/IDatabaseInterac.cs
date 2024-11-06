using System.Data;

namespace rrm
{
    internal interface IDatabaseInterac
    {

        // Methods To initialize database connections and Fill the tables into dataset 
        void StartConnection(string ConnectionString);
        DataSet FillTable(string TableName);
        DataSet FillTableRange(string[] TableNames);

        // Methods to get whole table <OR> to get one particular row of table 
        DataTable GetTable(string TableName);
        DataTable GetTable(int TableNo);
        DataRow GetRow(int TableNo, int RowNo);
        DataRow GetRow(string TableName, int RowNo);
        void Delete_Row(string Tablename, string ColumnName, object Parameter);
        void Rename_Row(string Tablename, string ColumnName, object Parameter);
        // Methods to select certain Rows from a specified table based on given certain criterias (SQL Queries)
        DataRow[] Select_Rows(string Tablename, string ColumnName, object Parameter);
        DataRow[] Select_Rows(int TableNo, int ColumnName, object Parameter);

        // ReadOnly properties to Get the value of only one record in a partcular row of particular column
        object get_GetRecord(int TableNo, int RowNo, int ColumnNo);
        object get_GetRecord(string TableName, int RowNo, int columnNo);
        object get_GetRecord(string TableName, int RowNo, string ColumnName);

        // Writeonly properties to set the value of a record in a particular row of particular column 
        void set_SetRecord(int TableNo, int RowNo, int ColumnNo, object value);
        void set_SetRecord(string TableName, int RowNo, int columnNo, object value);
        void set_SetRecord(string TableName, int RowNo, string ColumnName, object value);
        // <NOTE:: the above properties can be merged with each other -- only for better debugging support i have splitted them 
        // Once everything is,completed and done with debugging ,these can be merged with each other,as a single property >

        // Method to Update the database 
        bool Update_Table(string TableName);
    }
}