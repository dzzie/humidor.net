VERSION 5.00
Object = "{648A5603-2C6E-101B-82B6-000000000014}#1.1#0"; "MSCOMM32.OCX"
Begin VB.Form Form1 
   Caption         =   "IoT Humidor Configuration Wizard - sandsprite.com"
   ClientHeight    =   5385
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7470
   LinkTopic       =   "Form1"
   ScaleHeight     =   5385
   ScaleWidth      =   7470
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton Command2 
      Caption         =   "Clear"
      Height          =   240
      Left            =   4635
      TabIndex        =   23
      Top             =   3645
      Width           =   1320
   End
   Begin VB.ListBox List1 
      Height          =   1425
      Left            =   135
      TabIndex        =   22
      Top             =   3915
      Width           =   7125
   End
   Begin VB.Frame Frame3 
      Caption         =   "Upload Server Settings"
      Height          =   1545
      Left            =   3735
      TabIndex        =   15
      Top             =   1215
      Width           =   3660
      Begin VB.TextBox txtUID 
         Height          =   285
         Left            =   810
         TabIndex        =   21
         Top             =   360
         Width           =   510
      End
      Begin VB.TextBox txtApiKey 
         Height          =   285
         Left            =   810
         TabIndex        =   19
         Top             =   720
         Width           =   1365
      End
      Begin VB.TextBox txtServer 
         Height          =   285
         Left            =   810
         TabIndex        =   16
         Top             =   1080
         Width           =   2220
      End
      Begin VB.Label Label8 
         Caption         =   "UserID"
         Height          =   240
         Left            =   270
         TabIndex        =   20
         Top             =   405
         Width           =   600
      End
      Begin VB.Label Label6 
         Caption         =   "ApiKey"
         Height          =   285
         Left            =   270
         TabIndex        =   18
         Top             =   720
         Width           =   510
      End
      Begin VB.Label lblVal 
         Caption         =   "Server "
         Height          =   285
         Left            =   270
         TabIndex        =   17
         Top             =   1125
         Width           =   1005
      End
   End
   Begin VB.Frame Frame2 
      Caption         =   "Wifi Settings"
      Height          =   1545
      Left            =   180
      TabIndex        =   8
      Top             =   1215
      Width           =   3435
      Begin VB.ComboBox cboEncrypt 
         Height          =   315
         Left            =   945
         TabIndex        =   14
         Top             =   1080
         Width           =   2310
      End
      Begin VB.TextBox txtPasswd 
         Height          =   330
         Left            =   945
         TabIndex        =   12
         Top             =   675
         Width           =   2310
      End
      Begin VB.TextBox txtSSID 
         Height          =   330
         Left            =   945
         TabIndex        =   9
         Top             =   270
         Width           =   2310
      End
      Begin VB.Label Label4 
         Caption         =   "Encryption"
         Height          =   240
         Left            =   90
         TabIndex        =   13
         Top             =   1170
         Width           =   780
      End
      Begin VB.Label Label3 
         Caption         =   "Password"
         Height          =   285
         Left            =   135
         TabIndex        =   11
         Top             =   720
         Width           =   825
      End
      Begin VB.Label Label1 
         Caption         =   "SSID"
         Height          =   285
         Left            =   450
         TabIndex        =   10
         Top             =   315
         Width           =   465
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Arduino COM Port Connection"
      Height          =   960
      Left            =   135
      TabIndex        =   4
      Top             =   135
      Width           =   7305
      Begin VB.ComboBox CboPort 
         Height          =   315
         Left            =   945
         TabIndex        =   5
         Top             =   360
         Width           =   2400
      End
      Begin VB.Label Label2 
         Caption         =   "Port"
         Height          =   285
         Left            =   495
         TabIndex        =   7
         Top             =   405
         Width           =   870
      End
      Begin VB.Label lblRefresh 
         Caption         =   "Refresh"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   -1  'True
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00FF0000&
         Height          =   195
         Left            =   3600
         TabIndex        =   6
         Top             =   450
         Width           =   780
      End
   End
   Begin VB.CommandButton Command1 
      Caption         =   "about"
      Height          =   375
      Left            =   4725
      TabIndex        =   1
      Top             =   2925
      Width           =   1185
   End
   Begin VB.CommandButton cmdConfigure 
      Caption         =   "configure"
      Height          =   375
      Left            =   6165
      TabIndex        =   0
      Top             =   2925
      Width           =   1185
   End
   Begin MSCommLib.MSComm MSComm1 
      Left            =   4005
      Top             =   2790
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      DTREnable       =   -1  'True
   End
   Begin VB.Label Label5 
      Caption         =   "received"
      Height          =   240
      Index           =   1
      Left            =   1035
      TabIndex        =   2
      Top             =   3600
      Width           =   690
   End
   Begin VB.Label Label7 
      Caption         =   "Debug Info:"
      Height          =   240
      Left            =   45
      TabIndex        =   3
      Top             =   3600
      Width           =   1230
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'Copyright David Zimmer <dzzie@yahoo.com>
'all rights reserved
'this software is free for personal use.
'If you would like to to use this commercially, please consider a paypal donation
'in respect for my time creating it.

'server, APIKEY, ssid, encrypt type, wifi pass, client id

'#define      WLAN_SEC_UNSEC (0)
'#define      WLAN_SEC_WEP   (1)
'#define      WLAN_SEC_WPA   (2)
'#define      WLAN_SEC_WPA2  (3)

'#define WLAN_SECURITY   WLAN_SEC_WEP     // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
'const char WLAN_PASS[] = {};
'
'#define WLAN_SECURITY   WLAN_SEC_UNSEC     // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
'
'#define WLAN_SECURITY   WLAN_SEC_WPA     // Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
'#define WLAN_PASS       "string"
    
'if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) return false;
    
 'bool     connectToAP(const char *ssid, const char *key, uint8_t secmode, uint8_t attempts = 0);
    
Dim WithEvents serial As clsSerial
Attribute serial.VB_VarHelpID = -1

Sub LoadSettings(Optional saveit As Boolean = False)
    
    Dim tmp
    
    For Each f In Me.Controls
        If TypeName(f) = "TextBox" Then
            If saveit = True Then
                SaveSetting App.EXEName, "settings", f.Name, f.Text
            Else
                tmp = GetSetting(App.EXEName, "settings", f.Name)
                If Len(tmp) > 0 Then f.Text = tmp
            End If
        End If
    Next
  
    If saveit = True Then
        SaveSetting App.EXEName, "settings", "encrypt", cboEncrypt.ListIndex
    Else
        On Error Resume Next
        tmp = GetSetting(App.EXEName, "settings", "encrypt")
        tmp = CLng(tmp)
        cboEncrypt.ListIndex = tmp
    End If
            
End Sub

 

Private Sub CboPort_Click()
    Dim port As Long
    Dim sport As String
    
    On Error Resume Next
    
    sport = CboPort.Text
    a = InStr(sport, " ")
    If a > 0 Then
        sport = Mid(sport, 4, a - 4)
        port = CLng(sport)
    End If
    
    With MSComm1
        If .PortOpen Then .PortOpen = False
        .CommPort = port
        .PortOpen = True
    End With
    
    If Err.Number <> 0 Then
        MsgBox "can not set commport to " & port
    End If
    
End Sub

Function validate() As Boolean
    
    On Error Resume Next
    Dim uid As Long
    
    If Not MSComm1.PortOpen Then
        MsgBox "you must first select an open com Port from the list"
        Exit Function
    End If
    
    uid = CLng(txtUID)
    If uid = 0 Then
        MsgBox "UserID must be numeric and > 0"
        Exit Function
    End If

    For Each f In Me.Controls
        If TypeName(f) = "TextBox" And f.Name <> "txtPasswd" Then
            If Len(f.Text) = 0 Then
                MsgBox f.Name & " can not be blank."
                Exit Function
            End If
        End If
    Next
                
    validate = True

End Function

Private Sub cmdConfigure_Click()

    Dim ret() As String, x
    
    On Error Resume Next
    
    If Not validate() Then
        Exit Sub
    End If
    
    For Each f In Me.Controls
        If TypeName(f) = "TextBox" Then
           push ret, LCase(VBA.Right(f.Name, Len(f.Name) - 3)) & ":" & f.Text 'string the txt from txtName
        End If
    Next
    
    push ret, "enc:" & cboEncrypt.ListIndex
    List1.Clear
    
    For Each x In ret
        List1.AddItem "Send: " & x & "\n"
    Next
    
    'Clipboard.Clear
    'Clipboard.SetText Join(ret, vbLf)
    
    'example:
    '----------------
    'uid:1
    'apikey:apikey
    'server:server
    'passwd:pass
    'ssid:ssid
    'enc:0

    MSComm1.Output = Join(ret, vbLf)
    
    If Err.Number <> 0 Then
        Me.Caption = Err.Description
    Else
        Me.Caption = "Ok"
    End If
    
End Sub
 

Private Sub Command1_Click()

    MsgBox "Copyright David Zimmer <dzzie@yahoo.com>" & vbCrLf & _
            "All Rights Reserved" & vbCrLf & _
            "This software is free for personal use." & vbCrLf & vbCrLf, vbInformation
            
End Sub

Private Sub Command2_Click()
    List1.Clear
End Sub

Private Sub Form_Load()
    
    Me.Height = 3855 'hide debug recv list
    Set serial = New clsSerial
    serial.Configure MSComm1
    
    Dim x
    'cboEncrypt.listIndex = correct enum value
    enc = Array("None", "WEP", "WPA", "WPA2")
    For Each x In enc
        cboEncrypt.AddItem x
    Next
    cboEncrypt.ListIndex = 0
    
    LoadPorts
    LoadSettings

End Sub



Private Sub LoadPorts()
    Dim strComputer As String
    Dim objWMIService As Object
    Dim colItems As Object
    Dim objItem As Object
    
    On Error Resume Next
    
    strComputer = "."
    Set objWMIService = GetObject("winmgmts:\\" & strComputer & "\root\CIMV2")
    Set colItems = objWMIService.ExecQuery("SELECT * FROM Win32_SerialPort", , 48)
    
    For Each objItem In colItems
        CboPort.AddItem objItem.DeviceID & " " & objItem.Description
    Next
       
    Set objItem = Nothing
    Set colItems = Nothing
    Set objWMIService = Nothing
    
    CboPort.ListIndex = 0
    
End Sub

Private Sub Form_Unload(Cancel As Integer)
    txtSend = Empty
    txtrecv = Empty
    LoadSettings True
End Sub

Private Sub lblRefresh_Click()
    Screen.MousePointer = vbHourglass
    CboPort.Clear
    txtrecv = Empty
    txtSend = Empty
    LoadPorts
    Screen.MousePointer = vbDefault
End Sub

Private Sub serial_MessageReceived(msg As String)
    List1.AddItem Format(Now, "hh:nn.ss  -  ") & msg
End Sub



Sub push(ary, value) 'this modifies parent ary object
    On Error GoTo init
    x = UBound(ary) '<-throws Error If Not initalized
    ReDim Preserve ary(UBound(ary) + 1)
    ary(UBound(ary)) = value
    Exit Sub
init:     ReDim ary(0): ary(0) = value
End Sub

