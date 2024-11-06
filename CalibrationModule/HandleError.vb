Public Class ErrorHandler
    ' Events
    Public Shared Event OnErrorHappen As HandleError_Generated
    ' Nested Types
    Public Delegate Sub HandleError_Generated(ByVal sender As Exception, ByVal ErrorCode As String)
    ' Methods
    Public Sub DisposeIt()
        MyBase.Finalize()
    End Sub
    Public Shared Sub RaiseError(ByVal sender As Exception, ByVal ErrorCode As String)
        Dim onError As HandleError_Generated = ErrorHandler.OnErrorHappenEvent
        'Raising the Error When event is raised
        If (Not onError Is Nothing) Then
            onError.Invoke(sender, ErrorCode)
        End If
    End Sub
End Class