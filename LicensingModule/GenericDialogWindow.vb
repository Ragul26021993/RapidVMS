Public Class GenericDialogWindow

    Public Sub InitialiseWindow(ByVal MsgTxt As String, ByVal NoOfBtns As Integer, ByVal Btn1Txt As String, ByVal Btn2Txt As String)
        MsgTxtLbl.Text = MsgTxt
        PositiveResponseBtn.Text = Btn1Txt
        NegetiveResponseBtn.Text = Btn2Txt

        If NoOfBtns = 1 Then
            NegetiveResponseBtn.Hide()
            PositiveResponseBtn.Location = New System.Drawing.Point(Me.Size.Width / 2 - PositiveResponseBtn.Size.Width / 2, PositiveResponseBtn.Location.Y)
        End If
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