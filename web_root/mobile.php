<?php  

	/*
	Copyright David Zimmer <dzzie@yahoo.com>
	WebSite:  http://sandsprite.com
	All rights reserved, no portion of this code is authorized for sale or redistribution
	*/

    include("functions.php");
    
    $limit         = (int)$_GET['limit'];
	$offset        = (int)$_GET['offset'];
	$clientid      = (int)$_GET['id'];
	$page          = (int)$_GET['page'];
	
	$lastID        = 0;
	$one_day = 2 * 24; //30 minute intervals..
	
    if($limit < 1 || $limit > 13000) $limit = $one_day;
    if($offset < 1) $offset = 0;
    
    ConnectDB();
    
    $userr = mysql_query("select * from humiusers where autoid=$clientid");
    if(mysql_num_rows($userr)==0){ //list available users..
	    die("Invalid userid: " . $id);
    }
    
    $user = mysql_fetch_assoc($userr);
	    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where powerevt > 0 and clientid=$clientid order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastPowerEvent = date("m.d.y - g:i a",$rr['int_tstamp']);
    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where clientid=$clientid order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastUpdate = date("m.d.y - g:i a",$rr['int_tstamp']);
	    
    if($page==0 || $page==3){
	    
	    	$clearAlert = '';
	
			if($user['alertsent'] == 1){
				//$clearAlert = "<a href='#' onclick='clear_alert()'><font color=red>Clear Alert</font></a>";
			} 
	
	 	   $r  = mysql_query("SELECT * from humidor where clientid=$clientid order by autoid desc limit 1");
    	   $rr = mysql_fetch_assoc($r);
    	   $h  = $rr['humidity'];
		   $t  = $rr['temp'];
		   
		   $tcolor = 'white';
		   $hcolor = 'white';
		   
		   if($t < 64 || $t > 75) $tcolor = 'red';
		   if($h < 64 || $h > 75) $hcolor = 'red';

		   if($page==0){
		       $report = "
		       			 <html>
			       			 <head>
			       			 </head>
			       			 <body bgcolor=black>
				       			 <br>
				       			 <font style='font-family: Segoe WP; font-size:80px; color: gray'>
				       			 Humi: <font color=$hcolor> $h </font><br>
				       			 Temp: <font color=$tcolor> $t </font><br>
				       			 <br>
				       			 Updated:<br>
				       			 <font color=white style='position:relative;left:100px;font-size:60px;'>$lastUpdate</font>
				       			 <br><br>
				       			 Rebooted: <br>
				       			 <font color=white style='position:relative;left:100px;font-size:60px;'>$lastPowerEvent</font>
				       			 <br><br>
				       			 </font>
			       			 </body>
		       			 </html>
		       			";
       		}else{
	       		//page 3 is for live tile
	       		$report = "Humi: $h\n".
	       				  "Temp: $t\n\n".
				       	  "Updated:\n$lastUpdate";
       		}
	       			
	      die($report);
    }
    
    //now we build the temp/humidity value arrays from the database
    $r = mysql_query("select UNIX_TIMESTAMP(tstamp) as int_tstamp, humidity,temp from humidor where clientid=$clientid order by autoid desc limit $limit offset $offset");
	
    $i=0;
    $lowest  = 200;
    $highest = 0;
    $avgTemp = 0;
    $avgHumi = 0;
    $records = 0;
    
    $highTemp= 0;
    $highHumi= 0;
    $lowTemp = 200;
    $lowHumi = 200;
    
	$modo = $limit / 20;
	$d2 = '';
	$curday = -1;
	
	while($rr = mysql_fetch_assoc($r)){
		
		if( $i == (mysql_num_rows($r)-1) ) $d1 = date("D g:i a - m.d.y",$rr['int_tstamp']); //start date (will end at last)
		if($d2 == '') $d2 = date("D g:i a - m.d.y",$rr['int_tstamp']); //end date (first)
		
		$h[$i] = $rr['humidity'];
		$t[$i] = $rr['temp'];
		
		if($i==0 || $i % $modo == 0 ){
			if($limit==$one_day)
				$d[$i] = date("g:i a",$rr['int_tstamp']);
			else{
				if($curday == -1 || date( "w", $rr['int_tstamp']) != $curday){
					$curday = date( "w", $rr['int_tstamp']);
			 		$d[$i] = date("m.d",$rr['int_tstamp']);
				}else{
					$d[$i] = date("g:i a",$rr['int_tstamp']);
				}
			}
		}	 
		 else $d[$i] = '';
		
		$avgTemp += $t[$i];
		$avgHumi += $h[$i];
		
		if($h[$i] < $lowHumi) $lowHumi = $h[$i];
		if($t[$i] < $lowTemp) $lowTemp = $t[$i];
		if($h[$i] > $highHumi ) $highHumi = $h[$i];
		if($t[$i] > $highTemp)  $highTemp = $t[$i];
		
		if($h[$i] < $lowest) $lowest = $h[$i];
		if($t[$i] < $lowest) $lowest = $t[$i];
		if($h[$i] > $highest) $highest = $h[$i];
		if($t[$i] > $highest) $highest = $t[$i];
		
		$i++;
	}
	
	//$d1 = date("D g:i a - m.d.y",$rr['int_tstamp']); //start date
	$record_cnt = $i;
	$avgTemp = round($avgTemp / $record_cnt,1);
	$avgHumi = round($avgHumi / $record_cnt,1);
	$curh = $h[0];
	$curt = $t[0];
	
	//reverse because we ordered desc in sql to get last entries
	$t = array_reverse($t); 
    $h = array_reverse($h);
	$d = array_reverse($d);
    $js = generateGraph(0, $t, $h, $d);  	 
    
    $interval = "<select name=timeSpan id=timeSpan onchange='doChange(this.value)'>
    				<option value=$one_day>Day</option>
    				<option ".($limit==($one_day * 3) ? "SELECTED" : "")." value=".($one_day * 3).">3 day</option>
    				<option ".($limit==($one_day * 7) ? "SELECTED" : "")." value=".($one_day * 7).">Week</option>
					<option ".($limit==($one_day * 14) ? "SELECTED" : "")." value=".($one_day * 14).">2 Week</option>
    				<option ".($limit==($one_day * 30) ? "SELECTED" : "")." value=".($one_day * 30).">Month</option>
    				<!--option ".($limit==($one_day * 90) ? "SELECTED" : "")." value=".($one_day * 90).">3 Month</option-->
    				<!--option ".($limit==($one_day * 180) ? "SELECTED" : "")." value=".($one_day * 180).">6 Month</option-->
				</select>";
						
	$links = "<table width=100%><tr><td width=200 align=left>$d1</td><td>$interval</td><td align=center>";
	$links .= "<a href='mobile.php?limit=$limit&offset=".($offset+$limit)."&id=$clientid&page=2'><font color=white>Previous</font></a>";
	if( ($offset-$limit) >= 0 ) $links .= " &nbsp; &nbsp; <a href='index.php?limit=$limit&offset=".($offset-$limit)."&id=$clientid'><font color=white>Next</font></a>";
	$links .= "<td width=200>&nbsp;</td></td><td align=right>$d2</td></tr></table>";
	
	$mcolor = "#898989";

    $report = " 
    	<html>  
    	<head>
    	<title>Humidor Temperature/Humidity Log</title>
		<link rel='stylesheet' type='text/css' href='dropdown.css'>
	    <style type='text/css'>
		    .bb td, .bb th {
		     border-bottom: 1px solid black !important;
		    }
		    TABLE{ font-family: verdana; font-size:10px; color: white}
		</style>
		<script src='Chart.js'></script>
		<script src='dropdown.js'></script>
		<script>
			Chart.defaults.global.animation = false;
			function doChange(limit){
				location = 'mobile.php?limit='+limit+'&offset=$offset&id=$clientid&page=2'
			}	
		</script>
		</head>
		<body bgcolor=black>
    	<center>
			<div style='width:100%; height:100%'>
				$links
				<canvas id='canvas_0'></canvas> 
			</div>
	    </center>
	    <br><br>
    ";
    
    echo $report.$js.$event_js;
       




function arytocsv($ary, $isStr = 0){
	$r='';
	for($i=0; $i < count($ary); $i++){
		if($isStr == 1) $r.= "'".$ary[$i]."'"; else $r.= $ary[$i];
		if($i != count($ary)-1) $r.= ", ";
	}
	return $r;
} 

function generateGraph($index, $t, $h, $d){

	$temps = arytocsv($t);
	$humis = arytocsv($h);
	$dates = arytocsv($d,1);
	
	$r = "
	<script>
	
			var lineChartData_$index = {
				labels : [$dates],
				datasets : [
					{
						label: 'Temps',
						fillColor : 'rgba(255,255,255,0.2)',
						strokeColor : 'red',
						pointColor : 'red',
						pointStrokeColor : '#fff',
						pointHighlightFill : '#fff',
						pointHighlightStroke : 'rgba(220,220,220,1)',
						data : [$temps]
					},
					{
						label: 'Humis',
						fillColor : 'rgba(255,255,255,0.2)',
						strokeColor : 'green',
						pointColor : 'green',
						pointStrokeColor : '#fff',
						pointHighlightFill : '#fff',
						pointHighlightStroke : 'rgba(151,187,205,1)',
						data : [$humis]
					}
				]

			}
			
			var ctx = document.getElementById('canvas_$index').getContext('2d');
			window.myLine_$index = new Chart(ctx).Line(lineChartData_$index, {
				responsive: true, pointHitDetectionRadius: 1, datasetStrokeWidth: 1,
				pointDotRadius: 1, pointDot: false 
			});

	</script>
	";
	
	return $r;
	
}        
    
 
?>