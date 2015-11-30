<?
/*
	Copyright David Zimmer <dzzie@yahoo.com>
	WebSite:  http://sandsprite.com
	All rights reserved, no portion of this code is authorized for sale or redistribution
*/
	
include("functions.php");

$logout = (int)($_GET['logout']);
if($logout==1) $_SESSION['user']='';

$pass = strip($_POST['pass']);

if(strlen($pass)==0){
	outputForm();
	exit();
}

ConnectDB();
$ok = strcmp($pass, $WEBADMIN_PASS) == 0 ? 1 : 0;

if($ok){
	$_SESSION['user'] = "admin";
	header('Location: adminUsers.php');
	exit();
}
else{
	outputForm();
	echo "<br><br><br><center><h2><font color=red>Login Failed</h2></center></font>";
	exit();
}

function outputForm(){
 ?>
 	<body>
 	<? include("style.inc"); ?>
 	<script>
 		if(window.top.location != location.href) window.top.location=location
 	</script>
			<center>
			<img src="sandsprite2.jpg">
 			<br><br>
			<table width=260 border=1 bordercolor=#003300 cellspacing=0 cellpadding=6>

			<tr>
				<td width=300 align=center bgcolor="#003366"><font color=white>User Login</font></td></tr>
			<tr>
				<td align=center>
					<form method=post action=admin.php>
					<table width=200>
					<tr></tr><tr></tr>		
					<!--tr><td align=right><b>Username:</b></td><td><input type=text name=user value=admin></td></tr-->
					<tr><td align=right><b>Password:</b></td><td><input type=password name=pass></td></tr>
					<tr><td></td><td align=right><input class=btn type=submit value=Login></td></tr>
					</table>
					</form>
				</td>
			</tr>
		</table>
	</center>
 <?
}