<?php

include("functions.php");
    
$key  = $_GET['apikey'];
$temp = (int)$_GET['temp'];
$humi = (int)$_GET['humi'];
$watered = (int)$_GET['watered'];
$clear_alert = (int)$_GET['clear_alert'];
//$ip   = $_SERVER['REMOTE_ADDR'];

if( strcmp($key, $API_KEY) !== 0 ) die("Invalid api key!");
if($temp <= 0 || $humi <= 0)       die("Invalid data low");
if($temp >= 120 || $humi >= 100)   die("Invalid data high");

if($clear_alert==1 && file_exists($alert_file)) unlink($alert_file);

$sql = "insert into humidor(temp,humidity,watered) values($temp,$humi,$watered)";
	
$sendEmail = 0;
if($temp < 60 || $temp > 75) $sendEmail = 1;
if($humi < 60 || $humi > 75) $sendEmail = 1;

if($sendEmail == 1){
	$alert = "Temp: $temp\r\nHumidity: $humi\r\nDate: " . date('l jS \of F Y h:i:s A');
	if( !file_exists($alert_file) ){
		sendMail($ALERT_EMAIL, "Humidor Alert!", $alert );
		file_put_contents($alert_file, $alert);
	}
}		
 
ConnectDB();

if(!mysql_query($sql)){ echo "Error adding data to db"; }
 else{ echo "Ok"; }
 
ConnectDB(1);

?>

