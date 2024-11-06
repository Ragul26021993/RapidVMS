Imports System.Windows.Forms
Friend Class CreateControl
    Protected Function CControl(ByVal ControlType As String) As System.Windows.Forms.Control
        Select Case ControlType.ToLower
            Case "button"
                Return New System.Windows.Forms.Button()
            Case "textbox"
                Return New System.Windows.Forms.TextBox()
            Case "label"
                Return New System.Windows.Forms.Label()
            Case "combobox"
                Return New System.Windows.Forms.ComboBox()
            Case "radiobutton"
                Return New System.Windows.Forms.RadioButton()
            Case "checkbox"
                Return New System.Windows.Forms.CheckBox()
            Case "groupbox"
                Return New System.Windows.Forms.GroupBox()
            Case "panel"
                Return New System.Windows.Forms.Panel()
            Case "tabpage"
                Return New System.Windows.Forms.TabPage()
            Case "tabcontrol"
                Return New System.Windows.Forms.TabControl()
        End Select
        Throw New ApplicationException("The Control Type Specified is 'NOT FOUND'.Verify It is Valid Control <OR> Control's Type is written properly in Database.")
    End Function

    Public Function Return_Control(ByVal ControlType As String, ByVal Width As Integer, ByVal Height As Integer, ByVal Text As String, ByVal Name As String, ByVal Location As Drawing.Point) As System.Windows.Forms.Control
        Try
            Dim C As Control = CControl(ControlType)
            C.Width = CInt(Width)
            C.Height = CInt(Height)
            C.Text = Text
            C.Name = Name
            C.Location = Location
            Return C
        Catch ex As Exception
            Throw New Exception(ex.Message, ex) 'The specified Control Corresponding to ControlType is not Found 
        End Try
    End Function
    Public Function Return_Control(ByVal ControlType As String, ByVal Width As Integer, ByVal Height As Integer, ByVal Text As String, ByVal Name As String, ByVal Location As Drawing.Point, ByVal BackColor As System.Drawing.Color) As System.Windows.Forms.Control
        Try
            Dim C As Control = Return_Control(ControlType, Width, Height, Text, Name, Location)
            If ((C.GetType Is GetType(System.Windows.Forms.TextBox) Or C.GetType Is GetType(System.Windows.Forms.ComboBox))) And (BackColor.A = Drawing.Color.Transparent.A) Then
                Return C
            End If
            C.BackColor = BackColor
            Return C
        Catch ex As Exception
            Throw New Exception(ex.Message, ex) 'The specified Control Corresponding to ControlType is not Found 
        End Try
    End Function
    Public Sub ReleaseClass()
        MyBase.Finalize()
    End Sub
End Class
Public Class DefineControl(Of T)
    Public Function CheckEquals(ByVal T1 As T, ByVal T2 As T) As Boolean
        Try
            IIf(T1.ToString = T2.ToString, True, False)
        Catch ex As Exception
            Throw New Exception(ex.Message, ex)
        End Try
    End Function
End Class

Public Class JustTesting
    Dim Ch As New DefineControl(Of Integer)
    Public Sub JustChecking()
        If Ch.CheckEquals(20, 30) Then
            MsgBox("They are equal")
        End If
        'If Function(T As Object)
    End Sub
    Sub Main()
        ' The following line will print Success, because 4 is even.
        testResult(4, Function(num) num Mod 2 = 0)
        ' The following line will print Failure, because 5 is not > 10.
        testResult(5, Function(num) num > 10)
    End Sub

    ' Sub testResult takes two arguments, an integer value and a 
    ' Boolean function. 
    ' If the function returns True for the integer argument, Success
    ' is displayed.
    ' If the function returns False for the integer argument, Failure
    ' is displayed.
    Sub testResult(ByVal value As Integer, ByVal fun As Func(Of Integer, Boolean))
        If fun(value) Then
            Console.WriteLine("Success")
        Else
            Console.WriteLine("Failure")
        End If
    End Sub

End Class