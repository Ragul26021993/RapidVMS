Friend NotInheritable Class SettingsProp
    Inherits DB
    Public Sub New()
        'Initializing the base class constructor with paramater
        MyBase.New(ConstantValues.ConnectionString)
        'Filling the required tables for the settings
        FillTableRange(ConstantValues.Settings_TableNames)
    End Sub
    Public ReadOnly Property Total_Main_Controls() As Integer
        Get
            Return DataS.Tables(ConstantValues.Settings_TableNames(0)).Rows.Count
        End Get
    End Property
    Public ReadOnly Property Main_Control_Type(ByVal Index As Integer) As String
        Get
            Return GetRecord(ConstantValues.Settings_TableNames(0), Index, ConstantValues.Set_TabsNames.Control_Type).ToString
        End Get
    End Property
    Public ReadOnly Property Main_Control_Text(ByVal index As Integer) As String
        Get
            Return GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Control_Name).ToString
        End Get
    End Property
    Public ReadOnly Property Main_Control_Width(ByVal index As Integer) As Integer
        Get
            Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Width))
        End Get
    End Property
    Public ReadOnly Property Main_Control_Height(ByVal index As Integer) As Integer
        Get
            Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Height))
        End Get
    End Property
    Public ReadOnly Property Main_Control_Location(ByVal index As Integer) As Drawing.Point
        Get
            Return New Drawing.Point(CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.LocationX)), CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.LocationY)))
        End Get
    End Property
    Public ReadOnly Property Main_Control_Backcolor(ByVal index As Integer) As Drawing.Color
        Get
            If Not IsNull(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Backcolor)) Then
                Return Drawing.Color.FromArgb(CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Backcolor)))
            Else
                Return Drawing.Color.Transparent
            End If
        End Get
    End Property
    Public ReadOnly Property Main_Control_ParentID(ByVal index As Integer) As Integer
        Get
            Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Parent_ID))
        End Get
    End Property
    Public ReadOnly Property Main_Control_ID(ByVal index As Integer) As Integer
        Get
            Return CInt(GetRecord(ConstantValues.Settings_TableNames(0), index, ConstantValues.Set_TabsNames.Primary_ID))
        End Get
    End Property
    Dim DataR() As DataRow
    Public ReadOnly Property Total_No_of_Child(ByVal Control_Parent_ID As Integer) As Integer
        Get
            DataR = Select_ChildControls(ConstantValues.Settings_TableNames(1), [Enum].GetName(GetType(ConstantValues.Set_ControlTable), ConstantValues.Set_ControlTable.Control_Parent_ID), Control_Parent_ID)
            Return DataR.Length
        End Get
    End Property
    Public ReadOnly Property Child_Type(ByVal index As Integer) As String
        Get
            Return DataR(index).Item(ConstantValues.Set_ControlTable.Control_Type).ToString
        End Get
    End Property
    Public ReadOnly Property Child_Width(ByVal index As Integer) As Integer
        Get
            Return CInt(DataR(index).Item(ConstantValues.Set_ControlTable.Control_Width))
        End Get
    End Property
    Public ReadOnly Property Child_Height(ByVal index As Integer) As Integer
        Get
            Return CInt(DataR(index).Item(ConstantValues.Set_ControlTable.Control_Height))
        End Get
    End Property
    Public ReadOnly Property Child_Text(ByVal index As Integer) As String
        Get
            Return DataR(index).Item(ConstantValues.Set_ControlTable.Control_Text).ToString
        End Get
    End Property
    Public ReadOnly Property Child_Location(ByVal index As Integer) As Drawing.Point
        Get
            Return New Drawing.Point(CInt(DataR(index).Item(ConstantValues.Set_ControlTable.Control_LocationX)), CInt(DataR(index).Item(ConstantValues.Set_ControlTable.Control_LocationY)))
        End Get
    End Property
    Public ReadOnly Property Child_ID(ByVal index As Integer) As Integer
        Get
            Return CInt(DataR(index).Item(ConstantValues.Set_ControlTable.Control_ID))
        End Get
    End Property
    Public ReadOnly Property Child_Backcolor(ByVal index As Integer) As Drawing.Color
        Get
            If Not IsNull(DataR(index).Item(ConstantValues.Set_ControlTable.Backcolor)) Then
                Return Drawing.Color.FromArgb(CInt(DataR(index).Item(ConstantValues.Set_ControlTable.Backcolor)))
            Else
                Return Drawing.Color.Transparent
            End If
        End Get
    End Property
    Public ReadOnly Property Child_Has_PredefinedValues(ByVal index As Integer) As Boolean
        Get
            Return CBool(DataR(index).Item(ConstantValues.Set_ControlTable.Has_Predefined_Values))
        End Get
    End Property
    Public Function Get_PreDefined_Values(ByVal ChildID As Integer) As ArrayList
        Dim a As New ArrayList
        Dim NameOfColumn As String = [Enum].GetName(GetType(ConstantValues.Settings_Predefined_Values), ConstantValues.Settings_Predefined_Values.S_Control_ID)
        Dim DataR() As DataRow = GetTable(ConstantValues.Settings_TableNames(2)).Select(NameOfColumn & "=" & "'" & ChildID & "'")
        For i As Integer = 0 To DataR.Length - 1
            a.Add(DataR(i).Item(ConstantValues.Settings_Predefined_Values.S_Value))
        Next
        Return a
    End Function
End Class