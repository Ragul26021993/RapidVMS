Imports System.Data.OleDb
Imports System.Management
Imports System.ServiceProcess
Imports System.IO


Public Class LicensingDLL

    Dim DBConnectionString As String
    Dim DBConnection As OleDbConnection
    Dim LogFilePath As String
    Dim LogFileName As String = "RapidLogFile.txt"
    Dim ServicePath As String = "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\services\RapidIService"

    Public Enum LicenseType
        No
        Limited
        Full
    End Enum

    Public Enum ErrorType
        NoError
        ServiceNotFound
        ServiceDisabled
        LogFileNotFound
        LogFileCorrupted
    End Enum

#Region "Constructor"

    Sub New(ByVal DBConnectionStr As String, ByVal LogFilePath As String)
        DBConnectionString = DBConnectionStr
        DBConnection = New OleDbConnection(DBConnectionString)
        Me.LogFilePath = LogFilePath
    End Sub

#End Region

#Region "Public functions that shall be used by the program using the dll"

    ''This function checks the type of license that has been given to the user and returns this type
    Public Function CheckLicense() As LicenseType
        Try
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from FeatureLicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)

            ''Check the 2nd column(with index 1) of the 1st(and the only)row in the table and return value appropriately
            If LicenseInfoTable.Rows(0)("LicenseType") = "No" Then
                Return LicenseType.No
            ElseIf LicenseInfoTable.Rows(0)("LicenseType") = "Limited" Then
                Return LicenseType.Limited
            ElseIf LicenseInfoTable.Rows(0)("LicenseType") = "Full" Then
                Return LicenseType.Full
            End If
        Catch ex As Exception
            MsgBox("Error LIC01: A settings file is not accessible. Please check if the file exists or if the user has proper permissions." + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
            Return LicenseType.No
        End Try
    End Function

    ''This function gets the installation key that has already been created and saved in the database
    ''If its not in the database, then it creates a new key and saves it in the database for future ref
    Public Function GetInstallationKey() As String
        Try
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable
            Dim InstallationKey As String

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)

            ''If key has already been generated
            If LicenseInfoTable.Rows(0)("InstallationKeyGenerated") Then
                ''Then retrieve the key that is stored in the database and return
                Return LicenseInfoTable.Rows(0)("InstallationKey")
            Else
                ''Else generate the installation key
                InstallationKey = GenerateInstallationKey()
                ''Set the flag in the table as true indicating the key has been generated
                LicenseInfoTable.Rows(0)("InstallationKeyGenerated") = True
                ''save the newely generated key in the table
                LicenseInfoTable.Rows(0)("InstallationKey") = InstallationKey
                ''save the current date in the table
                LicenseInfoTable.Rows(0)("DateInstalled") = DateTime.Now
                ''update the database with the edited table
                LicenseInfoDBAdaptor.Update(LicenseInfoTable)
                ''return key
                Return InstallationKey
            End If
        Catch ex As Exception
            MsgBox("Error LIC02: A settings file is not accessible. Please check if the file exists or if the user has proper permissions." + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
            Return ""
        End Try
    End Function

    ''This function checks if an entered key is either limited or full license key
    ''If the key is any one of them, it returns true, else it returns false indicating an invalid key
    Public Function CheckKey(ByVal TheKeyToBeChecked As String) As Boolean
        Try
            ''Remove all the spaces that are present in the string
            TheKeyToBeChecked = TheKeyToBeChecked.Replace(" ", "")
            ''Check if the string length is 36
            If TheKeyToBeChecked.Length Mod 6 <> 0 Then
                Return False    ''if not, its not a valid key
            End If

            If DecryptKey(TheKeyToBeChecked) Then
                ''Create the log file(if it already exists, then refresh it)
                ''Add first entry to log file
                Dim LogFileWriter As StreamWriter = New StreamWriter(LogFilePath & "\" & LogFileName, False)
                'Dim DateInString As String = DateTime.Now.Date.ToShortDateString
                Dim DateInString As String = DateTime.Now.Day.ToString & "/" & _
                                             DateTime.Now.Month.ToString & "/" & _
                                             DateTime.Now.Year.ToString
                Dim EncrytedDateValue As String = ""

                For Each C As Char In DateInString
                    EncrytedDateValue &= Chr(Asc(C) + 11)
                Next

                LogFileWriter.WriteLine(EncrytedDateValue)
                LogFileWriter.Close()

                Return True
            Else
                Return False
            End If
        Catch ex As Exception
            MsgBox("Error LIC03: A settings file seems to be missing or corrupted" + "\n" + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
            Return False
        End Try
    End Function

    ''This function is used to determine the number of days remaining for the limited license to expire
    ''It gives 0 if the license has expired.
    Public Function NoOfDaysOfValidityRemaining() As Integer
        Try
            Dim LimitedLicenseDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from FeatureLicenseInfo", DBConnection))
            Dim LimitedLicenseDBCommandBuilder = New OleDbCommandBuilder(LimitedLicenseDBAdaptor)
            Dim LimitedLicenseTable = New DataTable

            ''Get the LimitedLicenseInfo table from the database
            LimitedLicenseDBAdaptor.Fill(LimitedLicenseTable)

            Dim ValidDays As TimeSpan = New TimeSpan(Convert.ToInt16(LimitedLicenseTable.Rows(0)("NoOfDaysValid")), 0, 0, 0)
            Dim ExpiryDate As DateTime = Convert.ToDateTime(LimitedLicenseTable.Rows(0)("DateEntered")) + ValidDays
            Dim ValidityRemaining As Integer

            If ExpiryDate < DateTime.Now Then
                ValidityRemaining = 0
            Else
                ValidityRemaining = (ExpiryDate - DateTime.Now).Days
            End If
            Return ValidityRemaining
        Catch ex As Exception
            MsgBox("Error LIC04: A settings file seems to be missing or corrupted" + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
            Return 0
        End Try
    End Function

    ''To check for any possible errprs or mismatches
    ''Errors types are defined in the ErrorType enum
    Public Function CheckForError() As ErrorType
        Dim ErIndx As Integer
        Try
            ''If log file not found, then return the error
            If Not System.IO.File.Exists(LogFilePath & "\" & LogFileName) Then
                Return ErrorType.LogFileNotFound
            End If

            ''Get all the services in the current machine
            Dim AllController() As ServiceController = ServiceController.GetServices()
            Dim ServiceFound As Boolean = False
            For Each TmpController As ServiceController In AllController
                If TmpController.ServiceName = "RapidIService" Then
                    ServiceFound = True
                    Exit For
                End If
            Next
            If Not ServiceFound Then
                ''This means service by the name "RapidIService" was not found, return error
                Return ErrorType.ServiceNotFound
            End If

            ''Check if service is disabled
            Dim valueName As String = "Start"
            If CInt(My.Computer.Registry.GetValue(ServicePath, valueName, 0)) = 4 Then
                ''Value 4 implies disabled
                Return ErrorType.ServiceDisabled
            End If

            ''Now get all the values from the log file
            Dim LogFileContent As ArrayList = New ArrayList
            Dim TempEncryptedString, TempDecryptedString As String
            Dim LogFileReader = New StreamReader(LogFilePath & "\" & LogFileName)

            While Not LogFileReader.EndOfStream
                ErIndx += 1
                TempEncryptedString = LogFileReader.ReadLine
                TempEncryptedString = TempEncryptedString.Replace(" ", "") ''Remove spaces
                TempEncryptedString = TempEncryptedString.Replace(" ", "") ''Remove tabs
                If TempEncryptedString = "" Then Continue While
                TempDecryptedString = ""
                For Each C As Char In TempEncryptedString
                    If Asc(C) = 0 Then Continue For
                    ''decrypt it
                    TempDecryptedString &= Chr(Asc(C) - 11)
                Next
                ''Convert it into date format and put it into the list
                Dim DtComponents() As String = TempDecryptedString.Split("/")
                LogFileContent.Add(New Date(CInt(DtComponents(2)), CInt(DtComponents(1)), CInt(DtComponents(0))))
            End While
            LogFileReader.Close()

            ''If the last entry in the log file is > the present time, then there is something fishy, return error 
            If LogFileContent(LogFileContent.Count - 1) > DateTime.Now.Date Then
                Return ErrorType.LogFileCorrupted
            End If

            ''if any entry in the log file is less than its previous value, then something is wrong with the file
            If LogFileContent.Count > 1 Then
                For i As Integer = 1 To LogFileContent.Count - 1
                    If LogFileContent(i) < LogFileContent(i - 1) Then
                        ''return error as ith value is lesser than its previous value
                        Return ErrorType.LogFileCorrupted
                    End If
                Next
            End If

            Return ErrorType.NoError
        Catch ex As Exception
            MsgBox("Error LIC04: A settings file seems to be missing or corrupted" + ErIndx.ToString + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
            Return ErrorType.ServiceNotFound
        End Try
    End Function

    ''Function to check if any feature has limited license.
    ''This helps in deciding to check for error
    Public Function FeatureWithLimitedLicensePresent() As Boolean
        Try
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from FeatureLicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)

            For i = 0 To LicenseInfoTable.Rows.Count - 1
                If LicenseInfoTable.Rows(i)("LicenseType") = "Limited" Then
                    Return True
                End If
            Next
            Return False
        Catch ex As Exception
            MsgBox("Error LIC05: A settings file is not accessible. Please check if the file exists or if the user has proper permissions." + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
            Return False
        End Try
    End Function

    ''Must be called at the start of module to do new log file entry
    Public Sub DoNewLogFileEntry()
        Try
            ''Append new log file entry
            Dim LogFileWriter As StreamWriter = New StreamWriter(LogFilePath & "\" & LogFileName, True)
            Dim DateInString As String = DateTime.Now.Day.ToString & "/" & _
                                         DateTime.Now.Month.ToString & "/" & _
                                         DateTime.Now.Year.ToString
            Dim EncrytedDateValue As String = ""

            For Each C As Char In DateInString
                EncrytedDateValue &= Chr(Asc(C) + 11)
            Next

            LogFileWriter.WriteLine(EncrytedDateValue)
            LogFileWriter.Close()
        Catch ex As Exception
            MsgBox("Error LIC06: Not able to access log file. Pease check if the user has proper permissions." + vbNewLine + ex.Message + vbNewLine + ex.StackTrace, MsgBoxStyle.OkOnly)
        End Try
    End Sub

#End Region

#Region "Private functions that is used within the class"

    ''This function generates a new installation key
    Private Function GenerateInstallationKey() As String
        Try
            Dim mac As String = GetMACAddress()
            Dim DateNow As DateTime = DateTime.Now
            Dim SecCount As Integer = DateNow.Second
            Dim GeneratedKey As String = ""
            Dim GenKeyArray(35), MacArray(11), DateArray(5), DecryptKeyArray(5), JunkArray(11), i As Integer

            SecCount = SecCount \ 10 + SecCount Mod 10 ''This value is used as the key for encryption

            DecryptKeyArray(0) = SecCount
            ''for each of the 5 blocks of installation key, a different key of encryption is derived from the 1st key
            For i = 1 To 5
                DecryptKeyArray(i) = (DecryptKeyArray(i - 1) * DecryptKeyArray(0)) Mod 15
            Next

            ''Jumble the mac address
            For i = 0 To 5
                MacArray(i) = HexToInt(mac(2 * i))
                MacArray(i + 6) = HexToInt(mac(2 * i + 1))
            Next

            ''Get the present date into the array
            DateArray(0) = DateNow.Day \ 10
            DateArray(1) = DateNow.Day Mod 10
            DateArray(2) = DateNow.Month \ 10
            DateArray(3) = DateNow.Month Mod 10
            DateArray(4) = (DateNow.Year Mod 2000) \ 10
            DateArray(5) = (DateNow.Year Mod 2000) Mod 10

            ''generate 12 junk values, 2 for each of the 6 blocks
            For i = 0 To 5
                JunkArray(2 * i) = (DecryptKeyArray(i) + DateArray(i) + MacArray(2 * i) + MacArray(2 * i + 1)) Mod 36
                JunkArray(2 * i + 1) = (DecryptKeyArray(i) ^ 2 + DateArray(i) + MacArray(2 * i) + MacArray(2 * i + 1)) Mod 36
            Next

            ''Put all the values together
            For i = 0 To 5
                GenKeyArray(6 * i) = JunkArray(2 * i)
                GenKeyArray(6 * i + 1) = MacArray(2 * i) + DecryptKeyArray(i)
                GenKeyArray(6 * i + 2) = MacArray(2 * i + 1) + DecryptKeyArray(i)
                GenKeyArray(6 * i + 3) = DecryptKeyArray(i)
                GenKeyArray(6 * i + 4) = DateArray(i) + DecryptKeyArray(i)
                GenKeyArray(6 * i + 5) = JunkArray(2 * i + 1)
            Next

            ''Convert the integer array into a string(the installation key)
            For i = 0 To 35
                GeneratedKey &= IntToHexChar(GenKeyArray(i))
            Next

            Return GeneratedKey
        Catch ex As Exception
            MsgBox("Error LIC07" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return ""
        End Try
    End Function

    ''This function checks if a key is valid full key or not
    ''If it is, then it returns true, else false
    Private Function ValidFullKey(ByVal TheKeyToBeChecked As String) As Boolean
        Try
            Dim MACID(11), DateArray(5) As Integer
            Dim MACAddress As String = ""
            Dim InstalDate As DateTime
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)

            For i As Integer = 0 To 5
                MACID(2 * i) = HexToInt(TheKeyToBeChecked(6 * i + 2)) - HexToInt(TheKeyToBeChecked(6 * i + 5))
                MACID(2 * i + 1) = HexToInt(TheKeyToBeChecked(6 * i + 3)) - HexToInt(TheKeyToBeChecked(6 * i + 5))
                DateArray(i) = HexToInt(TheKeyToBeChecked(6 * i + 1)) - HexToInt(TheKeyToBeChecked(6 * i + 5))
            Next

            For i As Integer = 0 To 5
                MACAddress &= IntToHexChar(MACID(2 * i))
            Next
            For i As Integer = 0 To 5
                MACAddress &= IntToHexChar(MACID(2 * i + 1))
            Next

            Try
                InstalDate = New DateTime(2000 + DateArray(4) * 10 + DateArray(5), DateArray(2) * 10 + DateArray(3), DateArray(0) * 10 + DateArray(1))
            Catch
                Return False
            End Try

            If Not CheckMACAddress(MACAddress) Then
                Return False
            End If

            If InstalDate.Date <> Convert.ToDateTime(LicenseInfoTable.Rows(0)(4).ToString).Date Then
                Return False
            End If

            Return True
        Catch ex As Exception
            MsgBox("Error LIC08" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return False
        End Try
    End Function

    ''This function checks if a key is valid limited key or not
    ''If it is, then it fills the NoOfDays field and returns true, else false
    Private Function ValidLimitedKey(ByVal TheKeyToBeChecked As String, ByRef NoOfDays As Integer) As Boolean
        Try
            Dim MACID(11), DateArray(5) As Integer
            Dim MACAddress As String = ""
            Dim InstalDate As DateTime
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)

            For i As Integer = 0 To 5
                MACID(2 * i) = HexToInt(TheKeyToBeChecked(6 * i + 2)) - HexToInt(TheKeyToBeChecked(6 * i + 5))
                MACID(2 * i + 1) = HexToInt(TheKeyToBeChecked(6 * i + 3)) - HexToInt(TheKeyToBeChecked(6 * i + 5))
                DateArray(i) = HexToInt(TheKeyToBeChecked(6 * i + 1)) - HexToInt(TheKeyToBeChecked(6 * i + 5))
            Next

            For i As Integer = 0 To 5
                MACAddress &= IntToHexChar(MACID(2 * i))
                MACAddress &= IntToHexChar(MACID(11 - 2 * i))
            Next

            Try
                InstalDate = New DateTime(2000 + DateArray(4) * 10 + DateArray(5), DateArray(2) * 10 + DateArray(3), DateArray(0) * 10 + DateArray(1))
            Catch ex As Exception
                Return False
            End Try

            If Not CheckMACAddress(MACAddress) Then
                Return False
            End If

            If InstalDate.Date <> Convert.ToDateTime(LicenseInfoTable.Rows(0)(4)).Date Then
                Return False
            End If

            If LimitedKeyAlreadyEntered(TheKeyToBeChecked) Then
                Return False
            End If

            If HexToInt(TheKeyToBeChecked(4)) - HexToInt(TheKeyToBeChecked(5)) = 1 Then
                NoOfDays = HexToInt(TheKeyToBeChecked(5 * 6 + 4)) - HexToInt(TheKeyToBeChecked(5 * 6 + 5))
                If NoOfDays < 0 Then Return False
            ElseIf HexToInt(TheKeyToBeChecked(4)) - HexToInt(TheKeyToBeChecked(5)) = 2 Then
                NoOfDays = HexToInt(TheKeyToBeChecked(4 * 6 + 4)) - HexToInt(TheKeyToBeChecked(4 * 6 + 5))
                If NoOfDays < 0 Then Return False
                NoOfDays *= 10
                NoOfDays += HexToInt(TheKeyToBeChecked(5 * 6 + 4)) - HexToInt(TheKeyToBeChecked(5 * 6 + 5))
                If NoOfDays < 0 Then Return False
            ElseIf HexToInt(TheKeyToBeChecked(4)) - HexToInt(TheKeyToBeChecked(5)) = 3 Then
                NoOfDays = HexToInt(TheKeyToBeChecked(3 * 6 + 4)) - HexToInt(TheKeyToBeChecked(3 * 6 + 5))
                If NoOfDays < 0 Then Return False
                NoOfDays *= 10
                NoOfDays += HexToInt(TheKeyToBeChecked(4 * 6 + 4)) - HexToInt(TheKeyToBeChecked(4 * 6 + 5))
                If NoOfDays < 0 Then Return False
                NoOfDays *= 10
                NoOfDays += HexToInt(TheKeyToBeChecked(5 * 6 + 4)) - HexToInt(TheKeyToBeChecked(5 * 6 + 5))
                If NoOfDays < 0 Then Return False
            End If

            Return True
        Catch ex As Exception
            MsgBox("Error LIC09" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return False
        End Try
    End Function

    ''This function updates the database with the full key info
    Private Sub UpdateDBWithFullKey(ByVal TheFullKey As String)
        Try
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)

            LicenseInfoTable.Rows(0)(1) = "Full"
            LicenseInfoTable.Rows(0)(5) = TheFullKey
            ''Update the changes made in the table to the database
            LicenseInfoDBAdaptor.Update(LicenseInfoTable)
        Catch ex As Exception
            MsgBox("Error LIC10: Not able to update the settings file. Please check if the file exists or if the user has proper permissions." + "\n" + ex.Message, MsgBoxStyle.OkOnly)
        End Try
    End Sub

    ''This function updates the database with the Limited key info
    Private Sub UpdateDBWithLimitedKey(ByVal TheLimitedKey As String, ByVal NoOfDays As Integer)
        Try
            Dim LicenseInfoDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseInfo", DBConnection))
            Dim LicenseInfoDBCommandBuilder = New OleDbCommandBuilder(LicenseInfoDBAdaptor)
            Dim LicenseInfoTable = New DataTable

            ''Get the LicenseInfo table from the database
            LicenseInfoDBAdaptor.Fill(LicenseInfoTable)
            ''Set the license type to limited
            LicenseInfoTable.Rows(0)(1) = "Limited"
            ''Update the changes made in the table to the database
            LicenseInfoDBAdaptor.Update(LicenseInfoTable)

            ''To update the Limited license table
            Dim LimitedLicenseDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LimitedLicenseInfo", DBConnection))
            Dim LimitedLicenseDBCommandBuilder = New OleDbCommandBuilder(LimitedLicenseDBAdaptor)
            Dim LimitedLicenseTable = New DataTable

            ''Get the LimitedLicenseInfo table from the database
            LimitedLicenseDBAdaptor.Fill(LimitedLicenseTable)

            ''Create a new row
            Dim DRow As DataRow = LimitedLicenseTable.NewRow
            ''1st item as the limited key itself
            DRow.Item(1) = TheLimitedKey
            ''2nd item as the date on which it was entered
            DRow.Item(2) = DateTime.Now.Date
            ''3rd item as the number of days it is valid
            DRow.Item(3) = NoOfDays
            ''Add the row to the table
            LimitedLicenseTable.Rows.Add(DRow)

            ''Update database with the changes made to the table
            LimitedLicenseDBAdaptor.Update(LimitedLicenseTable)
        Catch ex As Exception
            MsgBox("Error LIC11: Not able to update the settings file. Please check if the file exists or if the user has proper permissions." + "\n" + ex.Message, MsgBoxStyle.OkOnly)
        End Try
    End Sub

    ''Check if the limited license key has already been entered before
    Private Function LimitedKeyAlreadyEntered(ByVal TheLimitedKey As String) As Boolean
        Try
            Dim LimitedLicenseDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LimitedLicenseInfo", DBConnection))
            Dim LimitedLicenseDBCommandBuilder = New OleDbCommandBuilder(LimitedLicenseDBAdaptor)
            Dim LimitedLicenseTable = New DataTable

            ''Get the LimitedLicenseInfo table from the database
            LimitedLicenseDBAdaptor.Fill(LimitedLicenseTable)

            If LimitedLicenseTable.Rows.Count > 0 Then
                For i As Integer = 0 To LimitedLicenseTable.Rows.Count - 1
                    Dim TempKey As String = LimitedLicenseTable.Rows(i)(1)
                    If TempKey(5) = TheLimitedKey(5) Or TempKey(6 * 3 + 5) = TheLimitedKey(6 * 3 + 5) Then
                        Return True
                    End If
                Next
            End If

            Return False
        Catch ex As Exception
            MsgBox("Error LIC12: Not able to access the settings file. Please check if the file exists or if the user has proper permissions." + "\n" + ex.Message, MsgBoxStyle.OkOnly)
        End Try
    End Function

    ''This function is used to get the MAC address of the machine in string format
    Private Function GetMACAddress() As String
        Try
            ''The following three variables are needed to access the registry to retrieve the MAC address
            Dim mc As ManagementClass = New ManagementClass("Win32_NetworkAdapterConfiguration")
            Dim moc As ManagementObjectCollection = mc.GetInstances()
            Dim cnt As Integer = moc.Count

            Dim MACAddress As String = ""   ''string to store the retrieved MAC address

            For Each mo As ManagementObject In moc
                If MACAddress = String.Empty Then

                    Try
                        MACAddress = mo("MACAddress").ToString()
                        Exit For ''only return MAC Address from first card
                    Catch ex As Exception

                    End Try
                End If
                mo.Dispose()
            Next

            MACAddress = MACAddress.Replace(":", "")
            MACAddress = MACAddress.ToUpper

            Return MACAddress

        Catch ex As Exception
            MsgBox("Error LIC13" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return ""
        End Try
    End Function

    ''This function is used to check the MAC address of the machine in string format
    Private Function CheckMACAddress(ByVal MacAdd As String) As Boolean
        Try
            ''The following three variables are needed to access the registry to retrieve the MAC address
            Dim mc As ManagementClass = New ManagementClass("Win32_NetworkAdapterConfiguration")
            Dim moc As ManagementObjectCollection = mc.GetInstances()
            Dim cnt As Integer = moc.Count
            Dim Found = False

            Dim MACAddress As String = ""   ''string to store the retrieved MAC address

            For Each mo As ManagementObject In moc
                Try
                    MACAddress = mo("MACAddress").ToString()
                Catch ex As Exception

                End Try
                MACAddress = MACAddress.Replace(":", "")
                MACAddress = MACAddress.ToUpper
                mo.Dispose()
                If MacAdd = MACAddress Then
                    Found = True
                    Exit For
                End If
            Next

            If Found Then
                Return True
            Else
                Return False
            End If

        Catch ex As Exception
            MsgBox("Error LIC14" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return False
        End Try
    End Function

    ''This function is used to convert characters starting from '0' till 'Z' to integers starting from 0 till 35 respectively
    Private Function HexToInt(ByVal HexVal As Char) As Integer
        Try
            Select Case HexVal
                Case "1"
                    Return 1
                Case "2"
                    Return 2
                Case "3"
                    Return 3
                Case "4"
                    Return 4
                Case "5"
                    Return 5
                Case "6"
                    Return 6
                Case "7"
                    Return 7
                Case "8"
                    Return 8
                Case "9"
                    Return 9
                Case "A"
                    Return 10
                Case "B"
                    Return 11
                Case "C"
                    Return 12
                Case "D"
                    Return 13
                Case "E"
                    Return 14
                Case "F"
                    Return 15
                Case "G"
                    Return 16
                Case "H"
                    Return 17
                Case "I"
                    Return 18
                Case "J"
                    Return 19
                Case "K"
                    Return 20
                Case "L"
                    Return 21
                Case "M"
                    Return 22
                Case "N"
                    Return 23
                Case "O"
                    Return 24
                Case "P"
                    Return 25
                Case "Q"
                    Return 26
                Case "R"
                    Return 27
                Case "S"
                    Return 28
                Case "T"
                    Return 29
                Case "U"
                    Return 30
                Case "V"
                    Return 31
                Case "W"
                    Return 32
                Case "X"
                    Return 33
                Case "Y"
                    Return 34
                Case "Z"
                    Return 35
                Case "0"
                    Return 0
                Case Else
                    Return 0

            End Select
        Catch ex As Exception
            MsgBox("Error LIC15" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return 0
        End Try
    End Function

    ''This function is used to convert integers starting from 0 till 35 to characters starting from '0' till 'Z' respectively
    Private Function IntToHexChar(ByVal IntVal As Integer) As Char
        Try
            Dim Array() As Char = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"}
            If IntVal < 0 Or IntVal >= Array.Length Then Return "0"
            Return Array(IntVal)
        Catch ex As Exception
            MsgBox("Error LIC16" + "\n" + ex.Message, MsgBoxStyle.OkOnly)
            Return "0"
        End Try
    End Function

    Private Function AppendCRC(ByVal CommandStr As String) As String
        Dim FinalCRCStr As String = ""

        If CommandStr.Length Mod 2 <> 0 Then
            CommandStr += "0"
        End If
        Dim n As Integer = CommandStr.Length
        Dim ByteArray(n + 1) As Byte

        For i As Integer = 0 To n - 1
            'ByteArray(i) = Convert.ToInt16(CommandStr.Substring(i * 2, 2), 16)
            ByteArray(i) = Asc(CommandStr(i))
        Next
        ByteArray(n) = 0
        ByteArray(n + 1) = 0
        Dim CRCValue As UInt16 = Get16bitCRC(ByteArray, n)
        Dim CRCStr As String = Hex(CRCValue).ToUpper()

        For i As Integer = CRCStr.Length To 3
            CRCStr = "0" + CRCStr
        Next
        FinalCRCStr = CommandStr + CRCStr
        Return FinalCRCStr
    End Function

    Private Function Get16bitCRC(ByVal ptr As Byte(), ByVal length As Integer) As UInt16
        Dim InitVal1 As UInt16 = ptr(0)
        Dim InitVal2 As UInt16 = ptr(1)
        Dim CRCValue As UInt16 = (InitVal1 << 8) Or InitVal2
        For i As Integer = 0 To length * 8 - 1
            Dim NextBit As UInt16
            If (ptr(Math.Floor(i / 8) + 2) And (1 << (7 - (i Mod 8)))) > 0 Then
                NextBit = 1
            Else
                NextBit = 0
            End If

            If (CRCValue And &H8000) Then
                CRCValue = ((CRCValue << 1) + NextBit) Xor &H8005
            Else
                CRCValue = (CRCValue << 1) + NextBit
            End If
        Next
        Return CRCValue
    End Function

    Private Function DecryptKey(ByVal TheKeyToBeChecked As String) As Boolean
        Dim LimitedLicenseDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseList", DBConnection))
        Dim LimitedLicenseDBCommandBuilder = New OleDbCommandBuilder(LimitedLicenseDBAdaptor)
        Dim LimitedLicenseTable = New DataTable
        ''Get the LimitedLicenseInfo table from the database
        LimitedLicenseDBAdaptor.Fill(LimitedLicenseTable)
        For i = 0 To LimitedLicenseTable.Rows.Count - 1
            If TheKeyToBeChecked = LimitedLicenseTable.Rows(i)("LicenseKey").ToString() Then
                Return False
            End If
        Next
        Dim LimitRow = LimitedLicenseTable.NewRow()
        LimitRow("LicenseKey") = TheKeyToBeChecked
        LimitRow("DateEntered") = DateTime.Now
        LimitedLicenseTable.Rows.Add(LimitRow)
        LimitedLicenseDBAdaptor.Update(LimitedLicenseTable)

        LimitedLicenseDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from FeatureLicenseInfo", DBConnection))
        LimitedLicenseDBCommandBuilder = New OleDbCommandBuilder(LimitedLicenseDBAdaptor)
        LimitedLicenseTable = New DataTable
        ''Get the LimitedLicenseInfo table from the database
        LimitedLicenseDBAdaptor.Fill(LimitedLicenseTable)

        Dim CRCCheck = AppendCRC(TheKeyToBeChecked.Substring(0, TheKeyToBeChecked.Length - 4))
        If TheKeyToBeChecked <> CRCCheck Then
            Return False
        End If

        Dim DecrytionKey As Integer = HexToInt(TheKeyToBeChecked(TheKeyToBeChecked.Length - 6))
        Dim NumberOfFeatures As Integer = HexToInt(TheKeyToBeChecked(TheKeyToBeChecked.Length - 5)) - DecrytionKey
        Dim MaxNumberOfFeatures As Integer = (TheKeyToBeChecked.Length - 18) / 2
        Dim MinimumFeatures = 0
        If NumberOfFeatures < LimitedLicenseTable.Rows.Count Then
            MinimumFeatures = NumberOfFeatures
        Else
            MinimumFeatures = LimitedLicenseTable.Rows.Count
        End If

        Dim MacAddress As String = ""
        For i = 0 To 11
            MacAddress &= IntToHexChar(HexToInt(TheKeyToBeChecked(TheKeyToBeChecked.Length - 18 + i)) - DecrytionKey)
        Next
        If Not CheckMACAddress(MacAddress) Then
            Return False
        End If

        Dim UnorderedFeatures = TheKeyToBeChecked.Substring(0, TheKeyToBeChecked.Length - 18)
        Dim Features As String = ""
        Dim NoOfBlocks As Integer = UnorderedFeatures.Length / 6
        For i = 0 To MaxNumberOfFeatures
            Features &= UnorderedFeatures.Substring(UnorderedFeatures.Length - (6 * (i Mod NoOfBlocks + 1)) + (2 - Math.Floor(i / NoOfBlocks)) * 2, 2)
        Next

        For i = 0 To MinimumFeatures - 1
            Dim TmpFeature = Features.Substring(2 * i, 2)
            If TmpFeature(1) = "U" Then
                LimitedLicenseTable.Rows(i)("LicenseType") = "Full"
            ElseIf TmpFeature(1) = "L" Then
                LimitedLicenseTable.Rows(i)("LicenseType") = "No"
            Else
                LimitedLicenseTable.Rows(i)("LicenseType") = "Limited"
                LimitedLicenseTable.Rows(i)("DateEntered") = DateTime.Now
                LimitedLicenseTable.Rows(i)("NoOfDaysValid") = HexToInt(TmpFeature(0)) * 16 + HexToInt(TmpFeature(1))
            End If
        Next
        LimitedLicenseDBAdaptor.Update(LimitedLicenseTable)

        LimitedLicenseDBAdaptor = New OleDbDataAdapter(New OleDbCommand("Select * from LicenseInfo", DBConnection))
        LimitedLicenseDBCommandBuilder = New OleDbCommandBuilder(LimitedLicenseDBAdaptor)
        LimitedLicenseTable = New DataTable
        ''Get the LimitedLicenseInfo table from the database
        LimitedLicenseDBAdaptor.Fill(LimitedLicenseTable)
        LimitedLicenseTable.Rows(0)("LicenseEntered") = True
        LimitedLicenseDBAdaptor.Update(LimitedLicenseTable)

        Return True
    End Function

#End Region

End Class
