<?php

die('for dev work only..');

//dumping events from old db format into new humievents table...
//i used two different databases and transferred the records to the new table...
//out of 1647 events 1374 were power events...maybe we should just save the last power event to user table...or way to clear power events..

error_reporting(E_ALL);

//$SIMPLE_MODE = 1;
$dblink=0;
$dev_machine_webroot = "D:/_web_root/";

$default_limit = 3 * 24; //1 day (3 updates an hour x 24 hours)

/*---------- public config block -----------------------
  	you can include private machine overrides in private_db.php
	change $dev_machine_webroot so it knows when to use private db or not..*/
$DB_IP = '127.0.0.1';
$DB_USER = 'root';
$DB_PASS = '';
$DB_DB = 'wordpress'; //db to query from
$DB_TRANSFERT0 = 'test';


function ConnectDB($shutdown=0){
	global $dblink, $DB_IP, $DB_USER, $DB_PASS,$DB_DB;

	if($shutdown==0){
		if($dblink==0){
			$dblink = mysql_connect($DB_IP, $DB_USER, $DB_PASS)
		    or die('Please try back latter, Could not connect to Database Server'.mysql_error() );
			mysql_select_db($DB_DB) or die("Could not select database $DB_DB<br><br>".mysql_error());
		}
	}else{
		mysql_close($dblink);
		$dblink=0;
	}

}

ConnectDB();
$r = mysql_query("Select * from humidor where smoked >0 or watered >0 or powerevt > 0");
echo mysql_num_rows($r). ' records found<br><br><pre>';

mysql_select_db($DB_TRANSFERT0);

while($rr = mysql_fetch_assoc($r)){
	
	$watered = $rr['watered'];
	$powerevt = $rr['powerevt'];
	$clientid = $rr['clientid'];
	$smoked = $rr['smoked'];
	$tstamp = $rr['tstamp'];
	
	$sql2 = "insert into humievents(watered,powerevt,clientid,smoked,tstamp) values($watered,$powerevt,$clientid,$smoked,'$tstamp')\r\n";
	//echo $sql2;
	mysql_query($sql2);

}

echo "records transferred";



?>