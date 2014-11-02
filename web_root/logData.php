<?php

include("functions.php");
    
$key  = $_GET['apikey'];
$temp = (int)$_GET['temp'];
$humi = (int)$_GET['humi'];
$watered = (int)$_GET['watered'];
$smoked = (int)$_GET['smoked'];
$powerevt = (int)$_GET['powerevt'];
$hw_failure = (int)$_GET['failure'];
$clientid   = (int)$_GET['clientid'];
$clear_alert = (int)$_GET['clear_alert'];
//$ip   = $_SERVER['REMOTE_ADDR'];

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
 
if($temp <= 0 || $humi <= 0)       die("Invalid data low");
if($temp >= 120 || $humi >= 100)   die("Invalid data high");

$sql = "insert into humidor(temp,humidity,watered,powerevt,clientid,smoked) values($temp,$humi,$watered,$powerevt,$clientid,$smoked)";

$sendEmail = 0;
if($temp < 60 || $temp > 75) $sendEmail = 1;
if($humi < 60 || $humi > 75) $sendEmail = 1;
if($hw_failure==1) $sendEmail = 1;

if($sendEmail == 1){
	$alert = "Temp: $temp\r\nHumidity: $humi\r\nDate: " . date('l jS \of F Y h:i:s A');
	if( $user['alertsent'] == 0 ){
		$subj = "Humidor " . ($hw_failure==1 ? " Hardware Failure" : "") . " Alert!";
		$sent = sendMail($ALERT_EMAIL, $subj, $alert );
		mysql_query("update humiusers set alertsent=1 where autoid=$clientid");
		echo "Alert Sent to $ALERT_EMAIL = $sent<br>";
	}
}		
 
if(!mysql_query($sql)){ echo "Error adding data to db"; }
 else{ echo "Record Added!"; }
 
ConnectDB(1);

?>

