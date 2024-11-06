<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class GenericDialogWindow
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.MsgTxtLbl = New System.Windows.Forms.Label()
        Me.PositiveResponseBtn = New System.Windows.Forms.Button()
        Me.NegetiveResponseBtn = New System.Windows.Forms.Button()
        Me.SuspendLayout()
        '
        'MsgTxtLbl
        '
        Me.MsgTxtLbl.Font = New System.Drawing.Font("Tahoma", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.MsgTxtLbl.Location = New System.Drawing.Point(12, 19)
        Me.MsgTxtLbl.Name = "MsgTxtLbl"
        Me.MsgTxtLbl.Size = New System.Drawing.Size(462, 87)
        Me.MsgTxtLbl.TabIndex = 0
        Me.MsgTxtLbl.Text = "Message Text"
        Me.MsgTxtLbl.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'PositiveResponseBtn
        '
        Me.PositiveResponseBtn.Location = New System.Drawing.Point(86, 116)
        Me.PositiveResponseBtn.Name = "PositiveResponseBtn"
        Me.PositiveResponseBtn.Size = New System.Drawing.Size(127, 40)
        Me.PositiveResponseBtn.TabIndex = 1
        Me.PositiveResponseBtn.Text = "Button1"
        Me.PositiveResponseBtn.UseVisualStyleBackColor = True
        '
        'NegetiveResponseBtn
        '
        Me.NegetiveResponseBtn.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.NegetiveResponseBtn.Location = New System.Drawing.Point(281, 116)
        Me.NegetiveResponseBtn.Name = "NegetiveResponseBtn"
        Me.NegetiveResponseBtn.Size = New System.Drawing.Size(127, 40)
        Me.NegetiveResponseBtn.TabIndex = 2
        Me.NegetiveResponseBtn.Text = "Button2"
        Me.NegetiveResponseBtn.UseVisualStyleBackColor = True
        '
        'GenericDialogWindow
        '
        Me.AcceptButton = Me.PositiveResponseBtn
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.CancelButton = Me.NegetiveResponseBtn
        Me.ClientSize = New System.Drawing.Size(486, 168)
        Me.Controls.Add(Me.NegetiveResponseBtn)
        Me.Controls.Add(Me.PositiveResponseBtn)
        Me.Controls.Add(Me.MsgTxtLbl)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Name = "GenericDialogWindow"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "GenericDialogWindow"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents MsgTxtLbl As System.Windows.Forms.Label
    Friend WithEvents PositiveResponseBtn As System.Windows.Forms.Button
    Friend WithEvents NegetiveResponseBtn As System.Windows.Forms.Button
End Class
