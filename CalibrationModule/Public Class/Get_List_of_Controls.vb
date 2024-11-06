Option Strict Off
'Due to Various late bindings it is turned off here.
'Otherwise ,For The whole project, it is turned ON. 
Imports System.Windows.Forms
Public Class Get_List_of_Controls
    'Function to get all the UI Controls.
    Public Function Get_UI_Lists() As List(Of System.Windows.Forms.Control)
        '==The true means the list is UI list of the controls 
        Return Create_List(True)
    End Function
    'Function to Get all the Calibartion Settings Controls
    Public Function GetSettingsControls() As List(Of System.Windows.Forms.Control)
        'Firstly Call the Function to add all the controls and return the main list of the controls 
        Dim MainList As List(Of System.Windows.Forms.Control) = Create_List(False) '==The False means the list is Calibration Settings control's List.
        'Since The Creation OF DRO tabpage controls cannot be read from the chid controls entry .Hence This Function Reads the no of entries fro the DRO axises for particular machine no. and creates the Controls for that.
        Dim DROTabPage As New TabPage With {.Text = "DRO Settings", .Name = "DROSettings"}
        Dim MagValueTabPage As New TabPage With {.Text = "Magnification Levels", .Name = "Mag Settings"}
        Dim Db_Ins As New DB(ConstantValues.ConnectionString)
        'Adding all the static labels to the tab page 
        Dim LabelNames() As String = {"Axis Names", "Axis Lash", "Axis Multiplier", "Axis Type", "Count per Unit"}
        Try
            For i As Integer = 0 To LabelNames.Length - 1
                Dim lbl As New Label
                lbl.Text = LabelNames(i)
                lbl.Width = 100 : lbl.Height = 15
                lbl.Location = New Drawing.Point(5 * (i + 1) + 100 * i, 5)
                DROTabPage.Controls.Add(lbl)
            Next
            'Now The Adding the Dynamic controls. 
            Dim TableNames() As String = {"DROSettings", "MachineDetails", "MagLevels", "CurrentMachine"}
            Db_Ins.FillTableRange(TableNames)
            Dim RowCount As Integer = Db_Ins.DataS.Tables("CurrentMachine").Rows.Count - 1
            Dim MachineNo As String = CStr(Db_Ins.GetRecord("CurrentMachine", RowCount, "MachineNo"))
            'Selecting all the rows from the table who have same machine no. here 
            Dim DataR() As Data.DataRow = Db_Ins.Select_ChildControls("DROSettings", "MachineNo", MachineNo)
            'Now Populate The Textboxes and Axis Names 
            For i As Integer = 0 To DataR.Length - 1
                Dim Lbl As New Label
                Lbl.Text = CStr(DataR(i).Item("AxisName")) : Lbl.Width = 100 : Lbl.Height = 15 : Lbl.Location = New Drawing.Point(5, 20 + 20 * i)
                DROTabPage.Controls.Add(Lbl)
                'Lash textboxes
                Dim txt As New TextBox
                txt.Width = 100 : txt.Height = 20 : txt.Name = "Lash" & CStr(DataR(i).Item("AxisName")) : txt.Text = DataR(i).Item("AxisLash")
                txt.Location = New Drawing.Point(110, 20 + 20 * i)
                DROTabPage.Controls.Add(txt)
                'Multiplier Textboxes
                Dim txt1 As New TextBox
                txt1.Width = 100 : txt1.Height = 20 : txt1.Name = "Multiplier" & CStr(DataR(i).Item("AxisName")) : txt1.Text = DataR(i).Item("AxisMultiplier")
                txt1.Location = New Drawing.Point(215, 20 + 20 * i)
                DROTabPage.Controls.Add(txt1)
                'Axis type 
                Dim Chk As New CheckBox
                Chk.Width = 100 : Chk.Height = 20 : Chk.Text = "Is Linear" : Chk.Name = "Type" & CStr(DataR(i).Item("AxisName")) : Chk.Checked = CBool(DataR(i).Item("AxisType"))
                Chk.Location = New Drawing.Point(320, 20 + 20 * i)
                DROTabPage.Controls.Add(Chk)
                'Count per unit
                Dim txt2 As New TextBox
                txt2.Width = 100 : txt2.Height = 20 : txt2.Name = "Count" & CStr(DataR(i).Item("AxisName")) : txt2.Text = DataR(i).Item("AxisCount")
                txt2.Location = New Drawing.Point(425, 20 + 20 * i)
                DROTabPage.Controls.Add(txt2)
            Next
            'Get The total count of The mag. levels for a particular machine no. 
            Dim MagR() As DataRow = Db_Ins.Select_ChildControls("MagLevels", "MachineNo", MachineNo)
            'Adding the Pixel Width Label 
            Dim lblPixelWidth As New Label With {.Text = "Pixel Width", .Width = 100, .Height = 13, .Location = New Drawing.Point(60, 5)}
            MagValueTabPage.Controls.Add(lblPixelWidth)
            'Create The Labels For The Machine Magnification values and the textboxes for their Values of pixel width.
            For i As Integer = 0 To MagR.Length - 1
                Dim L As New Label
                L.Width = 50 : L.Height = 20
                L.Location = New Drawing.Point(10, 20 + 20 * i) : L.Text = MagR(i).Item("MagLevelValue") & ":" : MagValueTabPage.Controls.Add(L)
                Dim txt As New TextBox
                txt.Width = 100 : txt.Height = 20
                txt.Text = MagR(i).Item("PixelWidth") : txt.Name = "Mag" & CStr(MagR(i).Item("MagLevelValue"))
                txt.Location = New Drawing.Point(60, 20 + 20 * i)
                MagValueTabPage.Controls.Add(txt)
            Next
        Catch ex As Exception
            Throw New Exception(ex.Message, ex)
        End Try
        'Releasing all the memory used by this Try-Catch.
        Db_Ins.FinalizeClass()
        'Adding the Dro Tab page and Mag Level Tab page  to the main collection 
        CType(MainList.Item(0), TabControl).TabPages.Add(DROTabPage)
        CType(MainList.Item(0), TabControl).TabPages.Add(MagValueTabPage)
        'Returning the main list
        Return MainList
    End Function
    'This Function Creates all the controls From the database.
    Private Function Create_List(ByVal Is_UI As Boolean) As List(Of System.Windows.Forms.Control)
        '=====The List to contain all the Controls to be made from the database
        Dim CollectionList As New List(Of System.Windows.Forms.Control)
        '=====Creating Instances of the required classes 
        Dim Get_Cont As New CreateControl
        Dim Con_Prop As New ControlProperties(Is_UI)
        Try
            '======Outer loop to create the all Main controls 
            For i As Integer = 0 To Con_Prop.Total_Main_Controls - 1
                '=======Instantiating the Main controls from the properties taken by DataBase
                Dim MainC As System.Windows.Forms.Control = Get_Cont.Return_Control(Con_Prop.Main_Control_Type(i), Con_Prop.Main_Control_Width(i), Con_Prop.Main_Control_Height(i), Con_Prop.Main_Control_Text(i), Con_Prop.Main_Control_Name(i), Con_Prop.Main_Control_Location(i), Con_Prop.Main_Control_Backcolor(i))
                '=====Adding the keytrapping for the main control ,if control is of type textbox and Supports only numeric values.
                If Con_Prop.Main_Control_IsNumeric(i) And TryCast(MainC, System.Windows.Forms.TextBox) IsNot Nothing Then
                    AddHandler CType(MainC, System.Windows.Forms.TextBox).KeyDown, AddressOf Key_Trap
                End If
                '=======The Tagmaker function returns a point as (main control primary ID, its Parent ID )
                MainC.Tag = New System.Drawing.Point(Con_Prop.Main_Control_ID(i), Con_Prop.Main_Control_ParentID(i))
                '=======Adding the control to the collection list 
                CollectionList.Add(MainC)
                '=======Condition to check whether the main control is going to be attached to form or it have some child controls also 
                If Con_Prop.Main_Control_ParentID(i) > 0 Then
                    '=====It means This control have some child control also ::Instantiating its all child controls 
                    For i1 As Integer = 0 To Con_Prop.Total_No_of_Child(Con_Prop.Main_Control_ID(i)) - 1
                        '======Instantiating the child control 
                        Dim ChildC As System.Windows.Forms.Control = Get_Cont.Return_Control(Con_Prop.Child_Type(i1), Con_Prop.Child_Width(i1), Con_Prop.Child_Height(i1), Con_Prop.Child_Text(i1), Con_Prop.Child_Name(i1), Con_Prop.Child_Location(i1), Con_Prop.Child_Backcolor(i1))
                        '=====Adding the handler for the control if the control is of type Button only.
                        '=====Adding the key trapping of the child control if control is of type textbox and supports only numeric values.
                        If Con_Prop.Child_IsNumeric(i1) And TryCast(ChildC, System.Windows.Forms.TextBox) IsNot Nothing Then
                            AddHandler CType(ChildC, System.Windows.Forms.TextBox).KeyDown, AddressOf Key_Trap
                            AddHandler CType(ChildC, System.Windows.Forms.TextBox).TextChanged, AddressOf Check_Value
                        End If
                        '======Checking for the predefined values for the control
                        If Con_Prop.Child_Has_PredefinedValues(i1) Then
                            Dim Values As ArrayList = Con_Prop.Get_PreDefined_Values(Con_Prop.Child_ID(i1))
                            For Count As Integer = 0 To Values.Count - 1
                                '======Here only one condition is taken for Combobox only -- It only support predefined values. 
                                'If situation is like for textboxes or other controls the some values must be taken at the time of load 
                                'Then edit here and add code for that,and define the values into the predefined vamlues.
                                CType(ChildC, System.Windows.Forms.ComboBox).Items.Add(Values.Item(Count))
                            Next
                        End If
                        '====Adding the Child control to the collection list
                        CollectionList.Item(i).Controls.Add(ChildC)
                    Next
                    '=========Adding the Child Control to its parent control
                    If Is_UI Then
                        Select_Parent_Control(Con_Prop.Main_Control_ParentID(i), CollectionList).Controls.Add(CollectionList.Item(i))
                    Else
                        Dim Parent_Control As System.Windows.Forms.Control = Select_Parent_Control(Con_Prop.Main_Control_ParentID(i), CollectionList)
                        If Parent_Control.GetType Is GetType(System.Windows.Forms.TabControl) Then
                            CType(Parent_Control, System.Windows.Forms.TabControl).Multiline = True
                            CType(Parent_Control, System.Windows.Forms.TabControl).TabPages.Add(CollectionList.Item(i))
                        Else
                            Parent_Control.Controls.Add(CollectionList.Item(i))
                        End If
                    End If
                End If
            Next
            '=====Instansiating a list that will have only the main controls (controls who have parent id < 0 );to be directly added on form of other application (who is using this function)
            Dim Main_List As New List(Of System.Windows.Forms.Control)
            '=====Checking the main controls from the collection list and adding them to the main list 
            For i As Integer = 0 To Select_Main_Control(CollectionList).Count - 1
                Main_List.Add(Select_Main_Control(CollectionList).Item(i))
            Next
            '======Release all the used Memory;that is not required now by this shared function 
            Get_Cont.ReleaseClass()
            Con_Prop.ReleaseClass()
            CollectionList.Clear()
            '====Returning the List of main controls 
            Return Main_List
        Catch ex As Exception
            Throw New Exception(ex.Message, ex) 'The controls are not found <OR> database structure is not good::This Exception should be caught by the Using Application 
        End Try
    End Function
End Class
'The all other Supportive Methods to do calculations 
Partial Class Get_List_of_Controls
    Private Shared Function Select_Parent_Control(ByVal Tag As Object, ByRef CollectionList As List(Of System.Windows.Forms.Control)) As System.Windows.Forms.Control
        For i As Integer = 0 To CollectionList.Count - 1
            Dim p As System.Drawing.Point = CType((CollectionList.Item(i).Tag), Drawing.Point)
            If p.X = CInt(Tag) Then
                Return CollectionList.Item(i) 'Required Parent control was found 
            End If
        Next
        'The element was not found -- Some Error in database structure <or> Not correct entry made.So throw an exception
        Throw New Exception("Corresponding Parent Element Was not Found !! Verify the Database Entries" & vbCrLf & "This Can only occur when ,into the database the child controls have first enteries than their Parent Controls.Parent Controls needs to be instantiated first than its child controls." & vbCrLf & "So that the child controls can be added onto their parent control.")
    End Function
    Private Shared Function Select_Main_Control(ByRef CollectionList As List(Of System.Windows.Forms.Control)) As List(Of System.Windows.Forms.Control)
        Dim list As New List(Of System.Windows.Forms.Control)
        For i As Integer = 0 To CollectionList.Count - 1
            If CollectionList.Item(i).Tag.Y < 0 Then
                list.Add(CollectionList.Item(i))
            End If
        Next
        Return list
    End Function
    'Trapping the keys for the Numeric textboxes only 
    Private Shared Sub Key_Trap(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs)
        Select Case e.KeyCode
            Case Keys.NumPad0 To Keys.NumPad9, Keys.Back, Keys.Delete, Keys.Decimal, Keys.D0 To Keys.D9, Keys.OemPeriod, Keys.OemMinus
                e.SuppressKeyPress = False
            Case Keys.Up, Keys.Down, Keys.Left, Keys.Right
                e.SuppressKeyPress = False
            Case Else
                e.SuppressKeyPress = True
        End Select
    End Sub
    Private Shared Sub Check_Value(ByVal sender As Object, ByVal e As EventArgs)
        If CType(sender, System.Windows.Forms.TextBox).Text Is Nothing Or CType(sender, System.Windows.Forms.TextBox).Text = "" Then
            MsgBox("Values Cannot be Empty or Null !!" & vbCrLf & "Please renter the Correct Values.", MsgBoxStyle.Critical)
        End If
    End Sub
End Class