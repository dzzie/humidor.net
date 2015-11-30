VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "clsSerial"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Dim WithEvents oCom As MSComm
Attribute oCom.VB_VarHelpID = -1
Public Event MessageReceived(msg As String)

Private buf As String

Sub Configure(com As MSComm)
    
    Set oCom = com
    
    With oCom
        If .PortOpen Then .PortOpen = False
        .Settings = "9600,N,8,1"
        .DTREnable = False
        .RTSEnable = False
        .RThreshold = 1
        .SThreshold = 0
        .InBufferSize = 1
    End With


End Sub

Private Sub Class_Terminate()
    Set oCom = Nothing
End Sub

'Multiple messages can all be received at once
'and potentially partial messages as well, so if there were multiple messages,
'but the last one was not complete this should still handle even that scenario

Private Sub oCom_OnComm()
    Dim tmp() As String
    Dim notLast As Boolean
    Dim iput As String
    
    On Error Resume Next
    
    Select Case oCom.CommEvent
        Case comEvReceive
            iput = oCom.Input
            buf = buf & iput
            If CountOccurances(buf, vbLf) > 0 Then
                If Right(buf, 1) <> vbLf Then notLast = True
                tmp = Split(buf, vbLf)
                For i = 0 To UBound(tmp)
                    If notLast And i = UBound(tmp) Then
                        buf = Mid(buf, InStrRev(buf, vbLf))
                    Else
                        If Len(tmp(i)) > 0 Then
                            RaiseEvent MessageReceived(tmp(i))
                            tmp(i) = Empty       'remove this entry from buffer..may be more
                            buf = Join(tmp, vbLf)
                        End If
                    End If
                Next
            End If
    End Select
  
End Sub



Function CountOccurances(it, find) As Integer
    Dim tmp() As String
    If InStr(1, it, find, vbTextCompare) < 1 Then CountOccurances = 0: Exit Function
    tmp = Split(it, find, , vbTextCompare)
    CountOccurances = UBound(tmp)
End Function

