<?php

include("functions.php");
    
$sql = "insert into humidor(temp,humidity,watered,powerevt,clientid,smoked,sid) values(66,67,0,0,6,0,0)";
 
ConnectDB();
for($i=0; $i< 1000; $i++) mysql_query($sql);
ConnectDB(1);

?>
