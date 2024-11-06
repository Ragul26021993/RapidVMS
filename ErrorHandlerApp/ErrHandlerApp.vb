Imports System.Windows.Forms
Imports System
Imports System.IO
Imports System.Security
Imports System.Security.Cryptography
Imports System.Drawing.Imaging
Imports System.Drawing
Imports System.Security.AccessControl
Imports System.Security.Permissions

Public Class ErrHandlerApp
    Dim errorFilePath As String
    Dim userlogFolderName As String
    Dim errorScreenPath As String
    Dim ErLogFolderPath As String
    Dim CommandSendFilePath As String
    Dim ModuleInitialisationPath As String

    Public Sub New(ByVal LogFolderPath As String)
        ErLogFolderPath = LogFolderPath
    End Sub

    Public Sub ReportError(ByVal errorCode As String, ByVal errorName As String, ByVal errorStackTrace As String, ByVal CaptureScreen As Boolean)
        writeErrorLogFile(errorCode, errorName, errorStackTrace)
        If CaptureScreen Then
            captureErrorScreen(errorCode)
        End If
    End Sub

    Private Property ErrorLogFilePath() As String
        Get
            Return errorFilePath
        End Get
        Set(ByVal value As String)
            errorFilePath = value
        End Set
    End Property

    Private Property UserLogFilePath() As String
        Get
            Return userlogFolderName
        End Get
        Set(ByVal value As String)
            userlogFolderName = value
        End Set
    End Property
    
    Private Property ErrorScrrenShotFilePath() As String
        Get
            Return errorScreenPath
        End Get
        Set(ByVal value As String)
            errorScreenPath = value
        End Set
    End Property

  

    Private Sub getHour(ByRef hour As Integer, ByRef AMPM As String)
        Try
            If hour >= 12 Then
                hour = hour - 12
                AMPM = "PM"
            ElseIf hour = 24 Then
                hour = hour - 24
                AMPM = "AM"
            End If
        Catch ex As Exception

        End Try
    End Sub

    Private Sub writeErrorLogFile(ByVal errorCode As String, ByVal errorName As String, ByVal errorStackTrace As String)
        Try
            'createFolder()
            Dim fileNamePath As String = ErrorLogFilePath & "\ErrorLog " & Now.Date.ToString("dd-MM-yyyy") & ".Log"
            Dim FileName As String = ErrorLogFilePath & "\ErrorLog" & Now.Date.ToString("dd-MM-yyyy") & "_Debug" & ".txt"
            Dim BinaryWriteFile As BinaryWriter
            Dim TextWriteFile As StreamWriter

            Dim hour As Integer = Now.Hour
            Dim AMPM As String = "AM"
            getHour(hour, AMPM)


            If Not (fileNamePath = "") Then
                If (Not (File.Exists(fileNamePath))) Then
                    BinaryWriteFile = New BinaryWriter(File.Open(fileNamePath, FileMode.CreateNew))
                    TextWriteFile = New StreamWriter(File.Open(FileName, FileMode.CreateNew))
                Else
                    BinaryWriteFile = New BinaryWriter(File.Open(fileNamePath, FileMode.Append))
                    TextWriteFile = New StreamWriter(File.Open(FileName, FileMode.Append))
                End If


                TextWriteFile.WriteLine(Format(Now.Date, "dd-MMM-yyyy") & " " & hour & ":" & Now.Minute & " " & AMPM)
                TextWriteFile.WriteLine("Error Code-: " & errorCode)
                TextWriteFile.WriteLine(errorName)
                TextWriteFile.WriteLine(errorStackTrace)
                TextWriteFile.WriteLine("---------------------------------------------------------" & Environment.NewLine & Environment.NewLine)

                'If InStr(errorCode, "DEBUG") < 0 Then
                writeBytes(BinaryWriteFile, Format(Now.Date, "dd-MMM-yyyy") & " " & hour & ":" & Now.Minute & " " & AMPM & Environment.NewLine)
                    writeBytes(BinaryWriteFile, "Error Code-: " & errorCode & Environment.NewLine)
                    writeBytes(BinaryWriteFile, errorName & Environment.NewLine)
                    writeBytes(BinaryWriteFile, errorStackTrace & Environment.NewLine)
                    writeBytes(BinaryWriteFile, "---------------------------------------------------------" & Environment.NewLine & Environment.NewLine)
                'End If

                BinaryWriteFile.Close()
                TextWriteFile.Close()

            End If
        Catch ex As Exception

        End Try
    End Sub

    Public Sub Err_WriteCommandSendLog(ByVal command As String)
        Try
            'createFolder()
            Dim fileNamePath As String = CommandSendFilePath & "\CommandSent " & Now.Date.ToString("dd-MM-yyyy") & ".Log"
            Dim BinaryWriteFile As BinaryWriter
            If Not (fileNamePath = "") Then
                If (Not (File.Exists(fileNamePath))) Then
                    BinaryWriteFile = New BinaryWriter(File.Open(fileNamePath, FileMode.CreateNew))
                Else
                    BinaryWriteFile = New BinaryWriter(File.Open(fileNamePath, FileMode.Append))
                End If
                Dim hour As Integer = Now.Hour
                Dim AMPM As String = "AM"
                getHour(hour, AMPM)
                writeBytes(BinaryWriteFile, " " & hour & ":" & Now.Minute & ":" & Now.Second & " " & AMPM & "  " & command & Environment.NewLine)
                BinaryWriteFile.Close()
            End If
        Catch ex As Exception

        End Try
    End Sub

    Public Sub Err_WriteModuleInitialisation(ByVal command As String)
        Try
            'createFolder()
            Dim fileNamePath As String = ModuleInitialisationPath & "\InitializationLog " & Now.Date.ToString("dd-MM-yyyy") & ".Log"
            Dim BinaryWriteFile As BinaryWriter
            If Not (fileNamePath = "") Then
                If (Not (File.Exists(fileNamePath))) Then
                    BinaryWriteFile = New BinaryWriter(File.Open(fileNamePath, FileMode.CreateNew))
                Else
                    BinaryWriteFile = New BinaryWriter(File.Open(fileNamePath, FileMode.Append))
                End If
                writeBytes(BinaryWriteFile, command & Environment.NewLine)
                BinaryWriteFile.Close()
            End If
        Catch ex As Exception

        End Try
    End Sub

    Public Sub WriteUserLogFile(ByVal functionName As String, ByVal action As String, ByVal position As String)
        Try
            'createFolder()
            Dim fileNamePath As String = UserLogFilePath & "\UserLog " & Now.Date.ToString("dd-MM-yyyy") & ".Log"
            Dim StreamWriteFile As StreamWriter
            If Not (fileNamePath = "") Then
                If (Not (File.Exists(fileNamePath))) Then
                    StreamWriteFile = New StreamWriter(File.Open(fileNamePath, FileMode.CreateNew))
                Else
                    StreamWriteFile = New StreamWriter(File.Open(fileNamePath, FileMode.Append))
                End If

                Dim hour As Integer = Now.Hour
                Dim AMPM As String = "AM"
                getHour(hour, AMPM)
                StreamWriteFile.WriteLine(Format(Now.Date, "dd-MMM-yyyy") & " " & hour & ":" & Now.Minute & ":" & Now.Second & ":" & Now.Millisecond & " " & AMPM & Environment.NewLine)
                StreamWriteFile.WriteLine("Funtion Called-: " & functionName & Environment.NewLine)
                StreamWriteFile.WriteLine("Action-: " & action & Environment.NewLine)
                StreamWriteFile.WriteLine("Position-" & position & Environment.NewLine)
                StreamWriteFile.WriteLine("---------------------------------------------------------" & Environment.NewLine & Environment.NewLine)
                StreamWriteFile.Close()
            End If
        Catch ex As Exception

        End Try
    End Sub

    Private Sub writeBytes(ByVal writer As BinaryWriter, ByVal content As String)
        Try
            Dim enc As New System.Text.UnicodeEncoding
            Dim dBytes As Byte() = enc.GetBytes(content)
            For i As Integer = 0 To dBytes.Length - 1
                dBytes(i) = 255 - dBytes(i)
            Next
            writer.Write(dBytes)
        Catch ex As Exception

        End Try
    End Sub

    Public Sub createFolder()
        Try
            Dim folderName As String = ErLogFolderPath
            Dim errorFileFolderName As String = folderName & "\ErrorLog\LogFile"
            Dim errorScreenShotFolderName As String = folderName & "\ErrorLog\ScreenShot"
            Dim commandsendFolderName As String = folderName & "\CommandSend"
            Dim moduleInitialisationFolderName As String = folderName & "\ModuleInit"
            Dim programlogFolderName As String = folderName & "\ProgramLog"
            Dim userlogFolderName As String = folderName & "\UserLog"
            Dim pointslogFolderName As String = folderName & "\PointsLog"
            UserLogFilePath = userlogFolderName
            ErrorLogFilePath = errorFileFolderName
            ErrorScrrenShotFilePath = errorScreenShotFolderName
            CommandSendFilePath = commandsendFolderName
            ModuleInitialisationPath = moduleInitialisationFolderName
            CreateFolderDir(folderName)
            CreateFolderDir(errorFileFolderName)
            CreateFolderDir(errorScreenShotFolderName)
            CreateFolderDir(commandsendFolderName)
            CreateFolderDir(moduleInitialisationFolderName)
            CreateFolderDir(programlogFolderName)
            CreateFolderDir(userlogFolderName)
            CreateFolderDir(pointslogFolderName)
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub

    Private Sub CreateFolderDir(ByVal Path As String)
        Try
            If Not Directory.Exists(Path) Then
                Directory.CreateDirectory(Path)
                Dim info As DirectoryInfo = New DirectoryInfo(Path)
                Dim security As DirectorySecurity = info.GetAccessControl()
                security.AddAccessRule(New FileSystemAccessRule("Everyone", FileSystemRights.FullControl, InheritanceFlags.ContainerInherit, PropagationFlags.None, AccessControlType.Allow))
                security.AddAccessRule(New FileSystemAccessRule("Everyone", FileSystemRights.FullControl, InheritanceFlags.ObjectInherit, PropagationFlags.None, AccessControlType.Allow))
                info.SetAccessControl(security)
            End If
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub

    Private Sub captureErrorScreen(ByVal errorCode As String)
        Try
            Dim hour As Integer = Now.Hour
            Dim AMPM As String = "AM"
            getHour(hour, AMPM)
            Dim bmpSS As Bitmap = New Bitmap(Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Height, PixelFormat.Format32bppArgb)
            Dim gfxSS As Graphics = Graphics.FromImage(bmpSS)
            Dim Si As Size = New Size(Screen.PrimaryScreen.Bounds.Width, Screen.PrimaryScreen.Bounds.Height)
            gfxSS.CopyFromScreen(0, 0, 0, 0, Si)
            bmpSS.Save(ErrorScrrenShotFilePath & "\" & errorCode & "_" & Format(Now.Date, "dd-MMM-yyyy") & "_" & hour & "_" & Now.Minute & AMPM & ".jpeg", ImageFormat.Jpeg)
        Catch ex As Exception

        End Try
    End Sub
End Class