Imports System.Windows.Forms
Friend Module ConstantValues
    'The Connection string of the connection 
    Friend ConnectionString As String = Application.StartupPath & "\Database\RapidSettings.mdb;Persist Security Info=False"
    'Friend ConnectionString64 As String = "Provider=Microsoft.Ace.OLEDB.12.0;Data Source=" & Application.StartupPath & "\Database\RapidSettings.mdb;Persist Security Info=False"
    'The Names of the table to be loaded from the database
    Friend UI_TableNames() As String = {"UI_main_control", "UI_child_controls", "UI_Predefined_Values"}
    Friend Settings_TableNames() As String = {"Settings_Main_Controls", "Settings_Child_Controls", "Settings_Predefined_Values"}
    'The Enum structure to have the column no for the specific tables 
    Enum UI_Main
        'Table UI main control Columns name and Column index respectively
        P_Control_ID = 0
        P_Name = 1
        P_Text = 2
        P_Width = 3
        P_Height = 4
        P_LocationX = 5
        P_LocationY = 6
        P_Backcolor = 7
        P_Control_Type = 8
        P_Parent_ID = 9
        Handler_Function_Name = 10
        IsNumeric = 11
    End Enum
    Enum UI_Child
        'Table UI child controls 
        C_Control_ID = 0
        P_Control_ID = 1
        C_Width = 2
        C_Height = 3
        C_LocationX = 4
        C_LocationY = 5
        C_Control_Type = 6
        C_Control_Name = 7
        C_Control_Text = 8
        C_Backcolor = 9
        C_HandlerFunction = 10
        Has_Predefined_Value = 11
        Handler_Function_Name = 12
        IsNumeric = 13
    End Enum
    Enum UI_Predefined
        'Predefined values table
        Primary_ID = 0
        C_Control_ID = 1
        C_Value = 2
    End Enum
    Enum Set_Main
        Primary_ID = 0
        Main_Control_Name = 1
        Main_Control_Text = 2
        Main_Control_Width = 3
        Main_Control_Height = 4
        Main_Control_LocationX = 5
        Main_Control_LocationY = 6
        Main_Control_Backcolor = 7
        Main_Control_Type = 8
        Parent_ID = 9
        Handler_Function_Name = 10
        IsNumeric = 11
    End Enum
    Enum Set_Child
        Control_ID = 0
        Control_Type = 1
        Control_Text = 2
        Control_Name = 3
        Control_LocationX = 4
        Control_LocationY = 5
        Control_Width = 6
        Control_Height = 7
        Control_Backcolor = 8
        Control_Parent_ID = 9
        Has_Predefined_Values = 10
        Handler_Function_Name = 11
        IsNumeric = 12
    End Enum
    Enum Set_Predefined
        Primary_ID = 0
        S_Control_ID = 1
        S_Value = 2
    End Enum
End Module