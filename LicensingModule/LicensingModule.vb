'Imports LicensingDLLProj

Public Class LicensingModule
    
    Dim LicensingDLLObj As LicensingDLL

    Sub New(ByVal DBConnectionStr As String, ByVal LogFilePath As String)
        LicensingDLLObj = New LicensingDLL(DBConnectionStr, LogFilePath)
    End Sub

    ''Function to start all the interactions
    Public Function InteractWithUser() As Boolean
        Dim LType As LicensingDLL.LicenseType = LicensingDLLObj.CheckLicense()
        Dim DialogResponse As System.Windows.Forms.DialogResult

        ''If user has a full license key, run the software with no questions asked

        If LType = LicensingDLL.LicenseType.No Then
            ''If user has not entered any key, tell him that and display the installation key

            Dim NoLicenseBox As New KeyDialogWindow
            NoLicenseBox.InitialiseWindow("You have not aquired any license to run this software. Please e-mail the below installation key to Customised Technologies (P) Ltd to get a license key", _
                                            "I have the license Key", "Cancel")
            NoLicenseBox.SetKeyTxt(LicensingDLLObj.GetInstallationKey, True)
            DialogResponse = NoLicenseBox.ShowDialog()
            If DialogResponse = System.Windows.Forms.DialogResult.OK Then
                If GetLicenseKeyFromUser() Then
                    Return True
                Else
                    Return False
                End If
            Else
                Return False
            End If
        Else
            Dim ErType As LicensingDLL.ErrorType
            ErType = LicensingDLLObj.CheckForError()
            If ErType = LicensingDLL.ErrorType.LogFileCorrupted Or ErType = LicensingDLL.ErrorType.LogFileNotFound Then
                If LicensingDLLObj.FeatureWithLimitedLicensePresent() Then
                    MsgBox("Sorry!! There is some problem with the Service Log. Please contact Customised Technologies (P) Ltd for assistance")
                    Return False
                End If
            ElseIf ErType = LicensingDLL.ErrorType.ServiceDisabled Or ErType = LicensingDLL.ErrorType.ServiceNotFound Then
                MsgBox("Sorry!! There is some problem with the Rapid I windows service. Please contact Customised Technologies (P) Ltd for assistance")
                Return False
            End If

            If LType = LicensingDLL.LicenseType.Limited Then
                ''If the user has limited trial period, then display accordingly
                Dim NoOfDays As Integer = LicensingDLLObj.NoOfDaysOfValidityRemaining()
                If NoOfDays = 0 Then
                    Dim TrialExpiaryBox As New GenericDialogWindow
                    TrialExpiaryBox.InitialiseWindow("Sorry, your trial period for the software has expired. Please contact Customised Technologies (P) Ltd to renew it.", 2, "I have a new Key", "Cancel")
                    DialogResponse = TrialExpiaryBox.ShowDialog()
                    If DialogResponse = System.Windows.Forms.DialogResult.OK Then
                        If GetLicenseKeyFromUser() Then
                            Return True
                        Else
                            Return False
                        End If
                    Else
                        Return False
                    End If
                Else
                    Return True
                End If
            ElseIf LType = LicensingDLL.LicenseType.Full Then
                Return True
            End If
        End If
    End Function

    Private Function GetLicenseKeyFromUser() As Boolean
        Dim DialogResponse As System.Windows.Forms.DialogResult
        Dim FirstRun As Boolean = True
        Dim KeyTxtBxTxt As String = ""

        While True
            Dim GetNewLicenseBox As New KeyDialogWindow
            If FirstRun Then
                GetNewLicenseBox.InitialiseWindow("Enter the license key below.", "Submit Key", "Cancel")
                FirstRun = False
            Else
                GetNewLicenseBox.InitialiseWindow("Re-enter the license key below.", "Submit Key", "Cancel")
                GetNewLicenseBox.KeyTxtBx.Text = KeyTxtBxTxt
                GetNewLicenseBox.KeyTxtBx.Select()
            End If

            GetNewLicenseBox.SetKeyTxt("", False)
            DialogResponse = GetNewLicenseBox.ShowDialog()
            KeyTxtBxTxt = GetNewLicenseBox.KeyTxtBx.Text

            If DialogResponse = System.Windows.Forms.DialogResult.OK Then
                Dim TrueKey As Boolean = LicensingDLLObj.CheckKey(GetNewLicenseBox.KeyTxtBx.Text.ToString.ToUpper)
                If TrueKey Then
                    Dim FullKeyBox As New GenericDialogWindow
                    FullKeyBox.InitialiseWindow("Congradulations!! You now have aquired additional license to use this software.", 1, "OK", "")
                    FullKeyBox.ShowDialog()
                    Return True
                Else
                    Dim InvalidKeyBox As New GenericDialogWindow
                    InvalidKeyBox.InitialiseWindow("Sorry, the key you have entered is not valid", 2, "Re-enter Key", "Cancel")
                    DialogResponse = InvalidKeyBox.ShowDialog()
                    If DialogResponse = System.Windows.Forms.DialogResult.OK Then
                        Continue While ''Continue with next loop
                    Else
                        Return False
                    End If
                End If
            Else
                Return False
            End If
        End While
    End Function
End Class
