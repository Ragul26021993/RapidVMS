Friend Interface IDatabaseInterac

    'Methods To initialize database connections and Fill the tables into dataset 
    Sub StartConnection(ByVal ConnectionString As String)
    Function FillTable(ByVal TableName As String) As System.Data.DataSet
    Function FillTableRange(ByVal TableNames() As String) As System.Data.DataSet

    'Methods to get whole table <OR> to get one particular row of table 
    Function GetTable(ByVal TableName As String) As System.Data.DataTable
    Function GetTable(ByVal TableNo As Integer) As System.Data.DataTable
    Function GetRow(ByVal TableNo As Integer, ByVal RowNo As Integer) As System.Data.DataRow
    Function GetRow(ByVal TableName As String, ByVal RowNo As Integer) As System.Data.DataRow

    'Methods to select certain Rows from a specified table based on given certain criterias (SQL Queries)
    Function Select_ChildControls(ByVal Tablename As String, ByVal ColumnName As String, ByVal Parameter As Object) As DataRow()
    Function Select_ChildControls(ByVal TableNo As Integer, ByVal ColumnName As Integer, ByVal Parameter As Object) As DataRow()
    Function Select_ChildControls(ByVal Tablename As String, ByVal ColumnName As String, ByVal Parameter As Object, ByVal OrderBy_ColumnName As String) As DataRow()
    Function Select_ChildControls(ByVal TableNo As String, ByVal ColumnName As Integer, ByVal Parameter As Object) As DataRow()

    'ReadOnly properties to Get the value of only one record in a partcular row of particular column
    ReadOnly Property GetRecord(ByVal TableNo As Integer, ByVal RowNo As Integer, ByVal ColumnNo As Integer) As Object
    ReadOnly Property GetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal columnNo As Integer) As Object
    ReadOnly Property GetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal ColumnName As String) As Object

    'Writeonly properties to set the value of a record in a particular row of particular column 
    WriteOnly Property SetRecord(ByVal TableNo As Integer, ByVal RowNo As Integer, ByVal ColumnNo As Integer) As Object
    WriteOnly Property SetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal columnNo As Integer) As Object
    WriteOnly Property SetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal ColumnName As String) As Object
    '<NOTE:: the above properties can be merged with each other -- only for better debugging support i have splitted them 
    'Once everything is completed and done with debugging ,these can be merged with each other,as a single property >

    'Method to Update the database 
    Function Update_Table(ByVal TableName As String) As Boolean
End Interface