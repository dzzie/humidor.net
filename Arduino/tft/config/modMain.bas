Attribute VB_Name = "modMain"
Function FileExists(path) As Boolean
  If Dir(path, vbHidden Or vbNormal Or vbReadOnly Or vbSystem) <> "" Then FileExists = True _
  Else FileExists = False
End Function


'This will handle the installation of the OCX control if it is not already found on the system
'this runs from a module before the main form is loaded because the main form depends on the OCX.
'I did not want to use a batch file because there is a strong chance that newer versions of Windows
'which are 64-bit would try to execute it with a 64-bit cmd.exe which would then screw up regsvr32 call.

Sub Main()
    
    Dim ocx As String
    Dim src As String
    
    On Error Resume Next
    
    ocx = "c:\windows\system32\mscomm32.ocx"
    src = App.path & "\mscomm32.ocx"
    
    If Not FileExists(ocx) Then
        If Not FileExists(src) Then
           MsgBox "serial port OCX not found and not installed?"
           End
        Else
            FileCopy src, ocx
            Shell "regsvr32 " & ocx
            If Err.Number <> 0 Then
                MsgBox "failed to register OCX control? you may have to run with administrator privileges", vbInformation
            End If
        End If
    End If
    
    Form1.Visible = True
    
End Sub

