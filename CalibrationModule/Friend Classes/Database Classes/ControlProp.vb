'This is the final class ,implemented as according to the need and current structure of the database
Friend NotInheritable Class ControlProperties
    Inherits DB
    '====Is_UI will be true if properties of controls is needed for the UI controls 
    '====IS_UI will be false if properties of controls is needed for the Calibration Settings Tabbed controls
    Protected Is_UI As Boolean
    Public Sub New(ByVal _Is_UI As Boolean)
        'Specifying the connection string form constantValues module
        MyBase.New(ConstantValues.ConnectionString)

        'Filling the range of the tables
        Me.Is_UI = _Is_UI
        If _Is_UI Then
            FillTableRange(ConstantValues.UI_TableNames)
        Else
            FillTableRange(ConstantValues.Settings_TableNames)
        End If
    End Sub
    '======This Property gives the total count of the Main Controls
    Public ReadOnly Property Total_Main_Controls() As Integer
        Get
            If Is_UI Then
                Return GetTable(ConstantValues.UI_TableNames(0)).Rows.Count
            Else
                Return DataS.Tables(ConstantValues.Settings_TableNames(0)).Rows.Count
            End If
        End Get
    End Property
    '======It gives the Type of the main control
    '======Here <index> is  Row No. As of Current implementation it is the count of for loop typically (of the get_List_of_Controls Class)
    Public ReadOnly Property Main_Control_Type(ByVal index As Integer) As String
        Get
            If Is_UI Then
                Return GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Control_Type).ToString
            Else
                Return GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_Type).ToString
            End If
        End Get
    End Property
    '=======The text that is to be displayed by the main control
    Public ReadOnly Property Main_Control_Text(ByVal Index As Integer) As String
        Get
            If Is_UI Then
                Return GetRecord(ConstantValues.UI_TableNames(0), Index, ConstantValues.UI_Main.P_Text).ToString
            Else
                Return GetRecord(ConstantValues.Settings_TableNames(0), Index, ConstantValues.Set_Main.Main_Control_Text).ToString
            End If
        End Get
    End Property
    '========The name of the main control.
    Public ReadOnly Property Main_Control_Name(ByVal Index As Integer) As String
        Get
            If Is_UI Then
                Return GetRecord(ConstantValues.UI_TableNames(0), Index, ConstantValues.UI_Main.P_Name).ToString
            Else
                Return GetRecord(ConstantValues.Settings_TableNames(0), Index, ConstantValues.Set_Main.Main_Control_Name).ToString
            End If
        End Get
    End Property
    '========The width of the main control
    Public ReadOnly Property Main_Control_Width(ByVal index As Integer) As Double
        Get
            If Is_UI Then
                Return CDbl(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Width))
            Else
                Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_Width))
            End If
        End Get
    End Property
    '========the height of the main control
    Public ReadOnly Property Main_Control_Height(ByVal index As Integer) As Double
        Get
            If Is_UI Then
                Return CDbl(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Height))
            Else
                Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_Height))
            End If
        End Get
    End Property
    '=========the location of the main control
    Public ReadOnly Property Main_Control_Location(ByVal index As Integer) As Drawing.Point
        Get
            If Is_UI Then
                Return New Drawing.Point(CInt(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_LocationX)), CInt(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_LocationY)))
            Else
                Return New Drawing.Point(CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_LocationX)), CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_LocationY)))
            End If
        End Get
    End Property
    '==========The backcolor of the main control 
    Public ReadOnly Property Main_Control_Backcolor(ByVal index As Integer) As System.Drawing.Color
        Get
            If Is_UI Then
                'This field is optional;can be left blank by the user also --Hence checking the Value before proceeding
                If Not IsNull(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Backcolor)) Then
                    Return Drawing.Color.FromArgb(CInt(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Backcolor)))
                Else
                    Return Drawing.Color.Transparent
                End If
            Else
                If Not IsNull(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_Backcolor)) Then
                    Return Drawing.Color.FromArgb(CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Main_Control_Backcolor)))
                Else
                    Return Drawing.Color.Transparent
                End If
            End If
        End Get
    End Property
    '===========The Primary ID(key) of the control from the table 
    Public ReadOnly Property Main_Control_ID(ByVal index As Integer) As Integer
        Get
            If Is_UI Then
                Return CInt(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Control_ID))
            Else
                Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Primary_ID))
            End If
        End Get
    End Property
    '=========== The Id of the Parent of the control from the table
    Public ReadOnly Property Main_Control_ParentID(ByVal index As Integer) As Integer
        Get
            If Is_UI Then
                Return CInt(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.P_Parent_ID))
            Else
                Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Parent_ID))
            End If
        End Get
    End Property
    '======The main control handler function
    Public ReadOnly Property Main_Control_Handler(ByVal index As Integer) As String
        Get
            If Is_UI Then
                Return CStr(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.Handler_Function_Name))
            Else
                Return CStr(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.Handler_Function_Name))
            End If
        End Get
    End Property
    '=======The main controls Is numeric value 
    Public ReadOnly Property Main_Control_IsNumeric(ByVal index As Integer) As Boolean
        Get
            If Is_UI Then
                Return CBool(GetRecord(ConstantValues.UI_TableNames(0), index, ConstantValues.UI_Main.IsNumeric))
            Else
                Return CBool(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_Main.IsNumeric))
            End If
        End Get
    End Property
    Dim DataR() As DataRow
    '===========The total no of children corresponding to given ParentID as ;in Child table 
    Public ReadOnly Property Total_No_of_Child(ByVal ParentID As Integer) As Integer
        Get
            If Is_UI Then
                DataR = Select_ChildControls(ConstantValues.UI_TableNames(1), [Enum].GetName(GetType(ConstantValues.UI_Child), ConstantValues.UI_Child.P_Control_ID), ParentID)
                Return DataR.Length
            Else
                DataR = Select_ChildControls(ConstantValues.Settings_TableNames(1), [Enum].GetName(GetType(ConstantValues.Set_Child), ConstantValues.Set_Child.Control_Parent_ID), ParentID)
                Return DataR.Length
            End If
        End Get
    End Property
    '===========The type of the child control
    Public ReadOnly Property Child_Type(ByVal index As Integer) As String
        Get
            If Is_UI Then
                Return DataR(index).Item(ConstantValues.UI_Child.C_Control_Type).ToString
            Else
                Return DataR(index).Item(ConstantValues.Set_Child.Control_Type).ToString
            End If
        End Get
    End Property
    '=========The width of the child controls 
    Public ReadOnly Property Child_Width(ByVal index As Integer) As Double
        Get
            If Is_UI Then
                Return CDbl(DataR(index).Item(ConstantValues.UI_Child.C_Width))
            Else
                Return CInt(DataR(index).Item(ConstantValues.Set_Child.Control_Width))
            End If
        End Get
    End Property
    '=========== The height of the Child controls 
    Public ReadOnly Property Child_Height(ByVal index As Integer) As Double
        Get
            If Is_UI Then
                Return CDbl(DataR(index).Item(ConstantValues.UI_Child.C_Height))
            Else
                Return CInt(DataR(index).Item(ConstantValues.Set_Child.Control_Height))
            End If
        End Get
    End Property
    '=========== The Location of the child control
    Public ReadOnly Property Child_Location(ByVal index As Integer) As Drawing.Point
        Get
            If Is_UI Then
                Return New Drawing.Point(CInt(DataR(index).Item(ConstantValues.UI_Child.C_LocationX)), CInt(DataR(index).Item(ConstantValues.UI_Child.C_LocationY)))
            Else
                Return New Drawing.Point(CInt(DataR(index).Item(ConstantValues.Set_Child.Control_LocationX)), CInt(DataR(index).Item(ConstantValues.Set_Child.Control_LocationY)))
            End If
        End Get
    End Property
    '=========== The text to be displayed by the child control 
    Public ReadOnly Property Child_Text(ByVal index As Integer) As String
        Get
            If Is_UI Then
                Return DataR(index).Item(ConstantValues.UI_Child.C_Control_Text).ToString
            Else
                Return DataR(index).Item(ConstantValues.Set_Child.Control_Text).ToString
            End If
        End Get
    End Property
    '===========The Name of the control child 
    Public ReadOnly Property Child_Name(ByVal Index As Integer) As String
        Get
            If Is_UI Then
                Return DataR(Index).Item(ConstantValues.UI_Child.C_Control_Name).ToString
            Else
                Return DataR(Index).Item(ConstantValues.Set_Child.Control_Name).ToString
            End If
        End Get
    End Property
    '===========The backcolor of the child 
    Public ReadOnly Property Child_Backcolor(ByVal index As Integer) As System.Drawing.Color
        Get
            If Is_UI Then
                'This field is optional;can be left blank by the user also --Hence checking the Value before proceeding
                If Not IsNull((DataR(index).Item(ConstantValues.UI_Child.C_Backcolor))) Then
                    Return Drawing.Color.FromArgb(CInt(DataR(index).Item(ConstantValues.UI_Child.C_Backcolor)))
                Else
                    Return Drawing.Color.Transparent
                End If
            Else
                If Not IsNull(DataR(index).Item(ConstantValues.Set_Child.Control_Backcolor)) Then
                    Return Drawing.Color.FromArgb(CInt(DataR(index).Item(ConstantValues.Set_Child.Control_Backcolor)))
                Else
                    Return Drawing.Color.Transparent
                End If
            End If
        End Get
    End Property
    '=========== The Parent ID of the child control 
    Public ReadOnly Property Child_ParentID(ByVal index As Integer) As Integer
        Get
            Return CInt(DataR(index).Item(ConstantValues.UI_Child.P_Control_ID))
        End Get
    End Property
    '========== The Primary ID(key) of the child control from the table 
    Public ReadOnly Property Child_ID(ByVal index As Integer) As Integer
        Get
            If Is_UI Then
                Return CInt(DataR(index).Item(ConstantValues.UI_Child.C_Control_ID))
            Else
                Return CInt(DataR(index).Item(ConstantValues.Set_Child.Control_ID))
            End If
        End Get
    End Property
    '==========The property of IsNumeric or not 
    Public ReadOnly Property Child_IsNumeric(ByVal Index As Integer) As Boolean
        Get
            If Is_UI Then
                Return CBool(DataR(Index).Item(ConstantValues.UI_Child.IsNumeric))
            Else
                Return CBool(DataR(Index).Item(ConstantValues.Set_Child.IsNumeric))
            End If
        End Get
    End Property
    '========Gets the boolean value for the Has predefined values field 
    Public ReadOnly Property Child_Has_PredefinedValues(ByVal index As Integer) As Boolean
        Get
            If Is_UI Then
                Return CBool(DataR(index).Item(ConstantValues.UI_Child.Has_Predefined_Value))
            Else
                Return CBool(DataR(index).Item(ConstantValues.Set_Child.Has_Predefined_Values))
            End If
        End Get
    End Property
    '=======Gets all the predefined values from the field 
    Public Function Get_PreDefined_Values(ByVal ChildID As Integer) As ArrayList
        Dim a As New ArrayList
        If Is_UI Then
            Dim NameOfColumn As String = [Enum].GetName(GetType(ConstantValues.UI_Predefined), ConstantValues.UI_Predefined.C_Control_ID)
            Dim DataR() As DataRow = GetTable(ConstantValues.UI_TableNames(2)).Select(NameOfColumn & "=" & "'" & ChildID & "'")
            For i As Integer = 0 To DataR.Length - 1
                a.Add(DataR(i).Item(ConstantValues.UI_Predefined.C_Value))
            Next
        Else
            Dim NameOfColumn As String = [Enum].GetName(GetType(ConstantValues.Set_Predefined), ConstantValues.Set_Predefined.S_Control_ID)
            Dim DataR() As DataRow = GetTable(ConstantValues.Settings_TableNames(2)).Select(NameOfColumn & "=" & "'" & ChildID & "'")
            For i As Integer = 0 To DataR.Length - 1
                a.Add(DataR(i).Item(ConstantValues.Set_Predefined.S_Value))
            Next
        End If
        Return a
    End Function
    '======Property to Get the Handler function name 
    Public ReadOnly Property Child_Control_Handler(ByVal index As Integer) As String
        Get
            If Is_UI Then
                Return CStr(GetRecord(ConstantValues.UI_TableNames(1), index, ConstantValues.UI_Child.Handler_Function_Name))
            Else
                Return CStr(GetRecord(ConstantValues.Settings_TableNames(1), index, ConstantValues.Set_Child.Handler_Function_Name))
            End If
        End Get
    End Property
    '=====Release all the Memory Used By this class 
    Protected Friend Sub ReleaseClass()
        MyBase.FinalizeClass()
    End Sub
End Class