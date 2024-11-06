Public Class KeyDialogWindow

    Public Sub InitialiseWindow(ByVal MsgTxt As String, ByVal Btn1Txt As String, ByVal Btn2Txt As String)
        MsgTxtLbl.Text = MsgTxt
        PositiveResponseBtn.Text = Btn1Txt
        NegetiveResponseBtn.Text = Btn2Txt
    End Sub

    Public Sub SetKeyTxt(ByVal KeyTxt As String, ByVal ReadOnlyTxtBx As Boolean)
        For i As Integer = 0 To KeyTxt.Length Step 6
            KeyTxtBx.Text &= KeyTxt.Substring(i, 6) & "-"
        Next
        Dim tc() As Char = {"-"}
        KeyTxtBx.Text.Trim(tc)
        KeyTxtBx.ReadOnly = ReadOnlyTxtBx
    End Sub

    Private Sub PositiveResponseBtn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles PositiveResponseBtn.Click
        Me.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.Hide()
    End Sub

    Private Sub NegetiveResponseBtn_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NegetiveResponseBtn.Click
        Me.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Hide()
    End Sub

End Class