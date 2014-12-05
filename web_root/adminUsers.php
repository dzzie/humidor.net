<?

/*
	Copyright David Zimmer <dzzie@yahoo.com>
	WebSite:  http://sandsprite.com
	All rights reserved, no portion of this code is authorized for sale or redistribution
*/
	
require_once('functions.php');

requireLogin();

include("style.inc");

$username  = maxlength(strip($_POST['username']),254);
$email  = maxlength(strip($_POST['email']),254);
$apikey = maxlength(strip($_POST['apikey']),254);
$img    = maxlength(strip($_POST['img']),254);

$uid    = (int)$_GET['uid'];
$delete = (int)$_POST['delete'];
$state  = (int)$_POST['state'];
$pid    = (int)$_GET['pid'];
$edit   = (int)$_POST['edit'];
$euid   = (int)$_POST['euid'];


ConnectDB();

if(strlen($username) > 0 && $edit==0){
	$sql = "Insert into humiusers(username,email,apikey,img) Values('$username','$email', '$apikey','$img')";	
	$msg = "Insert successful";
	if(!mysql_query($sql)) $msg = "Insert failed! ".mysql_error();
	echo $msg;
	$username='';$email='';$apikey='';$img='';
}else{
	
	if($delete==1 && $euid > 0){
		$sql = "Delete from humiusers where autoid=$euid";
		$msg = "Delete successful<br>"; 	
		if(!mysql_query($sql)) $msg = "Update failed! ".mysql_error();
		echo $msg;	
		$uid=0;$edit=0;
	}
	else if($edit==1 && $euid > 0){ 
		$sql = "Update humiusers set username='$username', email='$email', apikey='$apikey', img='$img' where autoid=$euid";
		$msg = "Update successful<br>"; 	
		if(!mysql_query($sql)) $msg = "Update failed! ".mysql_error();
		echo $msg;	
	}
	
	if($uid>0){ //starting an edit only from get link load post for edit
		$r = mysql_query("Select * from humiusers where autoid=$uid");
		$rr = mysql_fetch_assoc($r);
		$email = $rr['email'];
		$apikey = $rr['apikey'];
		$username = $rr['username'];
		$img = $rr['img'];
		$edit=1;
	} 
	 
}

?>
 

<form method=post id=frmMain action=adminUsers.php?uid=<?=$uid?>>
<input type=hidden name=edit value=<?=$edit?>>
<input type=hidden name=euid value=<?=$uid?>>
<input type=hidden name=delete value=0>
<table>

 
<tr><td valign=top>Select User to Edit: </td><td><?getUsers($uid,"ChangeUser()")?><br><br><br><br></td></tr>
<tr><td>Username:</td><td><input type=text name=username size=45 value='<?=$username?>'></td></tr>
<tr><td>email:</td><td><input type=text name=email size=45 value='<?=$email?>'></td></tr>
<tr><td>apikey:</td><td><input type=text name=apikey size=45 value='<?=$apikey?>'></td></tr>
<tr><td>Image:</td><td><input type=text name=img size=45 value='<?=$img?>'></td></tr>
<tr><td align=center colspan=2>
	<input class=btn type=submit value="<?=($edit==1 ? "Update":"Create")?> User">
	<?if($edit==1){?><input class=btn type=button value="Delete User" onclick='doDelete()'><?}?>
</td></tr>
</table>
</form>


<script>
function ChangeUser(){
	cbo=document.getElementById('users')
	uid=cbo[cbo.selectedIndex].value
	location = 'adminUsers.php?uid='+uid
}
function doDelete(){
	frm=document.getElementById('frmMain')
	frm.delete.value=1;
	frm.submit();
}
</script>

 
<?

function getUsers($seluid,$onselect=''){
	$r = mysql_query("Select * from humiusers");
	echo "<form><select name=users id=users onchange='$onselect'>";
	echo "<option value=0> </option>";
	while($rr = mysql_fetch_assoc($r)){
		$u = $rr['username'];
		$id = $rr['autoid'];
		$s = $seluid == $id ? "SELECTED" : "";
		echo "<option value=$id $s>$id: $u</option>";
	}
	echo "</select></form>";
}

?>





