<?php

/*
	Copyright David Zimmer <dzzie@yahoo.com>
	WebSite:  http://sandsprite.com
	All rights reserved, no portion of this code is authorized for sale or redistribution
*/
	
include("functions.php");

//ex: http://localhost/humidor/logData.php?clientid=6&watered=1&humi=99&temp=44&apikey=test
    
$key  = $_GET['apikey'];
$temp = (int)$_GET['temp'];
$humi = (int)$_GET['humi'];
$watered = (int)$_GET['watered'];
$smoked = (int)$_GET['smoked'];
$powerevt = (int)$_GET['powerevt'];
$hw_failure = (int)$_GET['failure'];
$clientid   = (int)$_GET['clientid'];
$clear_alert = (int)$_GET['clear_alert'];
$clear_data  = (int)$_GET['clear_data'];

//$ip   = $_SERVER['REMOTE_ADDR'];

$wasWatered = (int)$_GET['wasWatered'];
$wasSmoked = (int)$_GET['wasSmoked'];

ConnectDB();

$userr = mysqli_query($dblink,"select * from humiusers where autoid=$clientid");
if(mysqli_num_rows($userr)==0) die('Bad userid');
$user = mysqli_fetch_assoc($userr);
    
$API_KEY = $user['apikey'];
$ALERT_EMAIL = $user['email'];

if( strcmp($key, $API_KEY) !== 0 ) die("Invalid api key!");

//----------  [these are from manual triggers in webui] ----------------------------
if($clear_alert==1){
	 if($user['alertsent']==1){
	 	mysqli_query($dblink,"update humiusers set alertsent=0 where autoid=$clientid");
	 	die("Alert Cleared");
 	}
	die("No Alert Set");  
 }
 
 if($clear_data==1){
 	/* keep last n records?
	DELETE FROM 'table' WHERE id NOT IN (
	  SELECT id FROM (
	    SELECT id
	    FROM `table`
	    ORDER BY id DESC
	    LIMIT 42 -- keep this many records
	  ) foo
	);
	*/
 	 mysqli_query($dblink,"delete from humidor where clientid=$clientid");
	 //i will hang onto these for now...maybe seperate table since they are the 1400/1700 events in whole table?
	 //mysqli_query("delete from humievents where clientid=$clientid and powerevt>0 and smoked=0 and watered=0");
	 die("Cleared records");	 
 }
 
 if($wasWatered==1){
 	 mysqli_query($dblink,"insert into humievents(watered,clientid) values(1,$clientid)");
	 die("Updated record");	 
 }
 
 if($wasWatered==1){
 	 mysqli_query($dblink,"insert into humievents(watered,clientid) values(1,$clientid)");
	 die("Updated record");	 
 }
 
 if($wasSmoked==1){
 	 mysqli_query($dblink,"insert into humievents(smoked,clientid) values(1,$clientid)");
	 die("Updated record");
 }


 
if($temp <= 0 || $humi <= 0)       die("Invalid data low");
if($temp >= 120 || $humi >= 100)   die("Invalid data high");

$sql = "insert into humidor(temp,humidity,clientid) values($temp,$humi,$clientid)";
$sql2 = "insert into humievents(watered,powerevt,clientid,smoked) values($watered,$powerevt,$clientid,$smoked)";

$sendEmail = 0;
if($temp < 60 || $temp > 75) $sendEmail = 1;
if($humi < 60 || $humi > 75) $sendEmail = 1;
if($hw_failure==1) $sendEmail = 1;

if($sendEmail == 1){
	$alert = "Temp: $temp\r\nHumidity: $humi\r\nDate: " . date('l jS \of F Y h:i:s A');
	if( $user['alertsent'] == 0 ){
		$subj = "Humidor " . ($hw_failure==1 ? " Hardware Failure" : "") . " Alert!";
		$sent = sendMail($ALERT_EMAIL, $subj, $alert );
		mysqli_query($dblink,"update humiusers set alertsent=1 where autoid=$clientid");
		echo "Alert Sent to $ALERT_EMAIL = $sent<br>";
	}
}		
 
if(!mysqli_query($dblink,$sql)){ echo "Error adding data to db"; }
 else{ echo "Data Record Added!"; }
 
if($watered > 0 || $powerevt > 0 || $smoked > 0){
	if(!mysqli_query($dblink,$sql2)){ echo "Error adding event to db"; }
 		else{ echo "Event Record Added!"; }
}

ConnectDB(1);

?>

