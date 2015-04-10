<?php

/*
	Copyright David Zimmer <dzzie@yahoo.com>
	WebSite:  http://sandsprite.com
	All rights reserved, no portion of this code is authorized for sale or redistribution
*/
	
include("functions.php");
    
$key  = $_GET['apikey'];
$cnt  = (int)$_GET['cnt'];

if($cnt==0){
	$cnt=1;
	$temp[$i] = (int)$_GET['temp'];
	$humi[$i] = (int)$_GET['humi'];
}else{
	if($cnt > 10 || $cnt < 1) die("Bad Sensor Count");

	for($i=0; $i < $cnt; $i++){
		$temp[$i] = (int)$_GET['temp'.$i];
		$humi[$i] = (int)$_GET['humi'.$i];
	}
}
	
$watered = (int)$_GET['watered'];
$smoked = (int)$_GET['smoked'];
$powerevt = (int)$_GET['powerevt'];
$hw_failure = (int)$_GET['failure'];
$clientid   = (int)$_GET['clientid'];
$clear_alert = (int)$_GET['clear_alert'];
//$ip   = $_SERVER['REMOTE_ADDR'];

$wasWatered = (int)$_GET['wasWatered'];
$wasSmoked = (int)$_GET['wasSmoked'];

ConnectDB();

$userr = mysql_query("select * from humiusers where autoid=$clientid");
if(mysql_num_rows($userr)==0) die('Bad userid');
$user = mysql_fetch_assoc($userr);
    
$API_KEY = $user['apikey'];
$ALERT_EMAIL = $user['email'];

if( strcmp($key, $API_KEY) !== 0 ) die("Invalid api key!");

if($clear_alert==1 && $user['alertsent']==1){
	 mysql_query("update humiusers set alertsent=0 where autoid=$clientid");
	 die("Alert Cleared");
 }
 
if($wasWatered==1){
	 $r = mysql_query("select * from humidor where clientid=$clientid order by autoid desc limit 1");
	 $rr = mysql_fetch_assoc($r);
	 if($rr){
	 	 mysql_query("update humidor set watered=1 where autoid=" . $rr['autoid']);
		 die("Updated record " . $rr['autoid']);
	 }
	 die("Error No Record");
 }
 
 if($wasSmoked==1){
	 $r = mysql_query("select * from humidor where clientid=$clientid order by autoid desc limit 1");
	 $rr = mysql_fetch_assoc($r);
	 if($rr){
	 	 mysql_query("update humidor set smoked=1 where autoid=" . $rr['autoid']);
		 die("Updated record " . $rr['autoid']);
	 }
	 die("Error No Record");
 }
 
 
if($clear_alert==1 && $user['alertsent']==1){
	 mysql_query("update humiusers set alertsent=0 where autoid=$clientid");
	 die("Alert Cleared");
 }
 
//if( $user['scnt'] != $cnt ) die("Scnt != cnt");
 
$hadErr = 0;  

for($i=0; $i < $cnt; $i++){ 
	if($temp[$i] <= 0 || $humi[$i] <= 0)       die("Invalid data low $i");
	if($temp[$i] >= 120 || $humi[$i] >= 100)   die("Invalid data high $i");

	$sendEmail = 0;
	if($temp[$i] < 60 || $temp[$i] > 75) $sendEmail = 1;
	if($humi[$i] < 60 || $humi[$i] > 75) $sendEmail = 1;
	if($hw_failure==1) $sendEmail = 1;

	if($sendEmail == 1){
		$alert = "Sensor: $i\r\nTemp: ".$temp[$i]."\r\nHumidity: ".$humi[$i]."\r\nDate: " . date('l jS \of F Y h:i:s A');
		if( $user['alertsent'] == 0 ){
			$subj = "Humidor " . ($hw_failure==1 ? " Hardware Failure" : "") . " Alert!";
			$sent = sendMail($ALERT_EMAIL, $subj, $alert );
			mysql_query("update humiusers set alertsent=1 where autoid=$clientid");
			echo "Alert Sent to $ALERT_EMAIL = $sent<br>";
		}
	}		

	$sql = "insert into humidor(temp,humidity,watered,powerevt,clientid,smoked,sid) values(".$temp[$i].",".$humi[$i].",$watered,$powerevt,$clientid,$smoked, $i)";
 	if(!mysql_query($sql)) $hadErr = 1; 
}
	
if(	$hadErr == 1){ echo "Error adding data to db"; }
 else{ echo "Record Added!"; }
 
ConnectDB(1);

?>

