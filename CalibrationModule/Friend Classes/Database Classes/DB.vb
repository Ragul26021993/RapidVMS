Imports System.Data.OleDb
Public Class DB
    Implements IDatabaseInterac
    'The connection string to initialize the connection :: Note connection is yet not initialized 
    'Since implementation is of disconnected mode nature ,hence no oledb connection object is used 
    'Infact the fill method of DataAdapter will initialize the connection -- fill the dataset with the table -- and then terminates the connection 
    Private Sub StartConnection(ByVal ConnectionString As String) Implements IDatabaseInterac.StartConnection
        Me.ConnectionString = ConnectionString
    End Sub
    'Fill one specified table 
    Public Function FillTable(ByVal TableName As String) As System.Data.DataSet Implements IDatabaseInterac.FillTable
        Try
            If ConnectionString <> "" Then
                DataA = New OleDb.OleDbDataAdapter("SELECT * FROM " & "[" & TableName & "]", ConnectionString)
                If DataS.Tables.Contains(TableName) Then DataS.Tables.Remove(TableName)
                DataA.Fill(DataS, TableName)
            Else
                Throw New ApplicationException("The Connection String is Not Proper!! Please Verify and retry.")
            End If
        Catch ex As Exception
            MsgBox(ex.Message, MsgBoxStyle.Critical, "Rapid-I 5.0")
        End Try
        Return DataS

    End Function
    'Fill Range of Tables 
    Public Function FillTableRange(ByVal TableNames() As String) As System.Data.DataSet Implements IDatabaseInterac.FillTableRange
        For i As Integer = 0 To TableNames.Length - 1
            If DataS.Tables.Contains(TableNames(i)) Then DataS.Tables.Remove(TableNames(i))
            FillTable(TableNames(i))
        Next
        Return DataS
    End Function
    'to get the instance of the row 
    'get any specified Table (Overloaded)
    Public Function GetTable(ByVal TableName As String) As System.Data.DataTable Implements IDatabaseInterac.GetTable
        If Not DataS.Tables.Contains(TableName) Then FillTable(TableName)
        Return DataS.Tables(TableName)
    End Function
    Public Function GetTable(ByVal TableNo As Integer) As System.Data.DataTable Implements IDatabaseInterac.GetTable
        Return DataS.Tables(TableNo)
    End Function
    'To get any single row from the table(Overloaded)
    Public Function GetRow(ByVal TableNo As Integer, ByVal RowNo As Integer) As System.Data.DataRow Implements IDatabaseInterac.GetRow
        'If Not DataS.Tables.Contains(TableName) Then FillTable(TableName)
        Return DataS.Tables(TableNo).Rows(RowNo)
    End Function
    Public Function GetRow(ByVal TableName As String, ByVal RowNo As Integer) As System.Data.DataRow Implements IDatabaseInterac.GetRow
        Return DataS.Tables(TableName).Rows(RowNo)
    End Function
    'Gives the array of DataRows that satisfy the given condition (overloaded)
    Public Function Select_ChildControls(ByVal Tablename As String, ByVal ColumnName As String, ByVal Parameter As Object) As DataRow() Implements IDatabaseInterac.Select_ChildControls
        Return GetTable(Tablename).Select(ColumnName & "=" & "'" & CStr(Parameter) & "'")
    End Function
    Public Function Select_ChildControls(ByVal TableNo As Integer, ByVal ColumnName As Integer, ByVal Parameter As Object) As DataRow() Implements IDatabaseInterac.Select_ChildControls
        Return GetTable(TableNo).Select(ColumnName & "=" & "'" & CStr(Parameter) & "'")
    End Function
    Public Function Select_ChildControls(ByVal Tablename As String, ByVal ColumnName As String, ByVal Parameter As Object, ByVal OrderBy_ColumnName As String) As DataRow() Implements IDatabaseInterac.Select_ChildControls
        Return GetTable(Tablename).Select(ColumnName & "=" & "'" & CStr(Parameter) & "'").OrderBy(Function(TmpRow) CStr(TmpRow(OrderBy_ColumnName))).ToArray()
    End Function

    Public Function Select_ChildControls(ByVal Tablename As String, ByVal ColumnName As Integer, ByVal Parameter As Object) As DataRow() Implements IDatabaseInterac.Select_ChildControls
        Dim tName As String = GetTable(Tablename).Columns(ColumnName).ColumnName
        Return GetTable(Tablename).Select(tName & "=" & "'" & CStr(Parameter) & "'")
    End Function

    Public Function Select_ChildControlsMulti(ByVal TableName As String, ByVal Query As String) As DataRow() 'Implements IDatabaseInterac.Select_ChildControls
        Return GetTable(TableName).Select(Query)
    End Function

    'Get any specific column value from dataset loaded(Overloaded)
    Public ReadOnly Property GetRecord(ByVal TableNo As Integer, ByVal RowNo As Integer, ByVal ColumnNo As Integer) As Object Implements IDatabaseInterac.GetRecord
        Get
            Return DataS.Tables(TableNo).Rows(RowNo).Item(ColumnNo)
        End Get
    End Property
    Public ReadOnly Property GetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal columnNo As Integer) As Object Implements IDatabaseInterac.GetRecord
        Get
            Return DataS.Tables(TableName).Rows(RowNo).Item(columnNo)
        End Get
    End Property
    Public ReadOnly Property GetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal ColumnName As String) As Object Implements IDatabaseInterac.GetRecord
        Get
            Return DataS.Tables(TableName).Rows(RowNo).Item(ColumnName)
        End Get
    End Property
    'Set any particular value of specific column and row in dataset(Overloaded)
    Public WriteOnly Property SetRecord(ByVal TableNo As Integer, ByVal RowNo As Integer, ByVal ColumnNo As Integer) As Object Implements IDatabaseInterac.SetRecord
        Set(ByVal value As Object)
            DataS.Tables(TableNo).Rows(RowNo).Item(ColumnNo) = value
        End Set
    End Property
    Public WriteOnly Property SetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal columnNo As Integer) As Object Implements IDatabaseInterac.SetRecord
        Set(ByVal value As Object)
            DataS.Tables(TableName).Rows(RowNo).Item(columnNo) = value
        End Set
    End Property
    Public WriteOnly Property SetRecord(ByVal TableName As String, ByVal RowNo As Integer, ByVal ColumnName As String) As Object Implements IDatabaseInterac.SetRecord
        Set(ByVal value As Object)
            DataS.Tables(TableName).Rows(RowNo).Item(ColumnName) = value
        End Set
    End Property
    'Updates the Database
    Public Function Update_Table(ByVal TableName As String) As Boolean Implements IDatabaseInterac.Update_Table
        Try
            'DataAdapter is Mapped again to given table only ,to have the correct Update command being calculated by CommandBuilder object.
            'It is necessary.Since,we are loading many tables into dataset ,Hence DataAdapter must be in syncronization with a particular Table only , at a given time.
            'This Method is also recommended by MSDN and also it does not create any major/minor performance loss at all.
            'Albeit ,it is the most safer method to do updation of database without using stored SQL strings queries or to remember them by heart!!
            DataA = New OleDb.OleDbDataAdapter("SELECT * FROM " & "[" & TableName & "]", ConnectionString)
            CommB = New OleDb.OleDbCommandBuilder(DataA)
            DataA.Update(DataS, TableName)
            Return True
        Catch ex As Exception
            Throw New Exception(ex.Message, ex) 'The Table mappings are not valid <OR> The Update command autogenerated by command Builder object is Wrong <OR> The Table Name given is not into the dataset itself!!
            'Note: Error can also happen ,if any column name is a 'Reserved Keyword' <OR> The Column name contains Spaces in its name.
            'Those type of errors generally have exception message as Error into INSERT INTO command <or> UPDATE command. <OR> Error in SQL query.
            Return False
        End Try
    End Function

    Public Function AddColumnToTable(ByVal TableName As String, ByVal ColumnName As String, ByVal ColumnType As Type) As Boolean
        Dim conn As New OleDbConnection(Me.ConnectionString)
        Try
            Dim cmd As New OleDbCommand()
            conn.Open()
            cmd.Connection = conn
            Dim oT As String

            Dim colType As String = "Text"
            Select Case ColumnType.FullName
                Case "System.String"
                    oT = "CHAR(255)"
                Case "System.Int32"
                    oT = "INTEGER"
                Case Else
                    oT = "CHAR(255)"
            End Select
            cmd.CommandText = "ALTER TABLE " + TableName + " ADD COLUMN " + ColumnName + " " + oT
            'cmd.ComandType = CommandType.Text
            cmd.ExecuteNonQuery()
            conn.Close()
            Return True
        Catch ex As Exception
            conn.Close()
            Return False
        End Try

    End Function
End Class
'Other Supportive methods and declarations for this class -- For good debugging support it is classified like this ;can be merged later,once debugging of this class is fully done. 
Partial Class DB
    'Only these are required for the DisConnected Mode of database interaction
    Public DataA As Data.OleDb.OleDbDataAdapter
    Public CommB As Data.OleDb.OleDbCommandBuilder
    Public DataS As New Data.DataSet
    Dim ConnectionString As String = ""
    Public Sub New(ByVal ConnectStr As String)
        Dim fullConnectString As String
        If System.Environment.Is64BitOperatingSystem Then
            fullConnectString = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" & ConnectStr
        Else
            fullConnectString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & ConnectStr
        End If
        StartConnection(fullConnectString)
    End Sub '==The Constructor with a connection string as parameter 
    '==Function to check if any given value is equivalent to database NULL value or not 
    Public Overridable Function IsNull(ByVal O As Object) As Boolean
        Return Object.Equals(O, DBNull.Value)
    End Function
    '==To clean up the used database Objects 
    <Runtime.ConstrainedExecution.ReliabilityContract(Runtime.ConstrainedExecution.Consistency.WillNotCorruptState, Runtime.ConstrainedExecution.Cer.Success)> _
    Public Sub FinalizeClass()
        'clean Up neatly the database objects 
        If DataA IsNot Nothing Then DataA.Dispose()
        If DataS IsNot Nothing Then DataS.Dispose()
        If CommB IsNot Nothing Then CommB.Dispose()
        ConnectionString = Nothing
        'Finally Call base class finalize method
        MyBase.Finalize()
    End Sub
    Public Function GetRowCount(ByVal TableName As String) As Integer
        Return DataS.Tables(TableName).Rows.Count
    End Function
End Class