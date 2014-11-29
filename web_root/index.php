<?php  
    include("functions.php");
    
    $limit         = (int)$_GET['limit'];
	$offset        = (int)$_GET['offset'];
	$clientid      = (int)$_GET['id'];
		
	$lastID        = 0;
	
    if($limit < 1 || $limit > 13000) $limit = $default_limit;
    if($offset < 1) $offset = 0;
    
    ConnectDB();
    
    $userr = mysql_query("select * from humiusers where autoid=$clientid");
    if(mysql_num_rows($userr)==0){ //list available users..
	    $r = mysql_query("select * from humiusers");
	    if(mysql_num_rows($r)==0) die("Log into <a href=admin.php>control panel</a> and create users..");    
	    $report = "<b><br><br>Please select a user: <br><ul>\n";
	    while($rr = mysql_fetch_assoc($r)){
		     $report .= "<li><a href=index.php?id=".$rr['autoid'].">".$rr['username']."</a>\n";
	    }
	    die($report);
    }
    
    $user = mysql_fetch_assoc($userr);
    $humi_img     = "./images/".$user['img'];
    $GRAPH_TITLE  = $user['username']." Humidor";
        
    //get last watered time..
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where watered > 0 and clientid=$clientid order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastWatered = date("g:i a - D m.d.y",$rr['int_tstamp']);
    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where watered > 0 and clientid=$clientid order by autoid desc limit 10");
    $cnt = mysql_num_rows($r);
    if($cnt > 0){
	    $waterTable = "<b>Last $cnt Waterings:</b><ul>\r\n";
	    while($rr = mysql_fetch_assoc($r)){
		    $waterTable.="<li>".date("g:i a - D m.d.y",$rr['int_tstamp'])."\r\n";
	    }
	    $waterTable.="</ul>\r\n";
	}
	
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where smoked > 0 and clientid=$clientid order by autoid desc limit 20");
    $cnt = mysql_num_rows($r);
    if($cnt > 0){
	    $smokedTable = "<b>Last $cnt Smokes:</b><ul>\r\n";
	    while($rr = mysql_fetch_assoc($r)){
		    $smokedTable.="<li>".date("g:i a - D m.d.y",$rr['int_tstamp'])."\r\n";
	    }
	    $smokedTable.="</ul>\r\n";
	}
	    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where powerevt > 0 and clientid=$clientid order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastPowerEvent = date("g:i a - D m.d.y",$rr['int_tstamp']);
    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where clientid=$clientid order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastUpdate = date("g:i a - D m.d.y",$rr['int_tstamp']);
    
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
	
	while($rr = mysql_fetch_assoc($r)){
		
		if( ($i+1) == mysql_num_rows($r)) $d1 = date("D g:i a - m.d.y",$rr['int_tstamp']); //start date (will end at last)
		if($d2 == '') $d2 = date("D g:i a - m.d.y",$rr['int_tstamp']); //end date (first)
		
		$h[$i] = $rr['humidity'];
		$t[$i] = $rr['temp'];
		if($i==0 || $i % $modo == 0 ) $d[$i] = date("D g:i a - m.d.y",$rr['int_tstamp']); else $d[$i] = '';
		
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
	
	//reverse because we ordered desc in sql to get last entries
	$t = array_reverse($t); 
    $h = array_reverse($h);
	$d = array_reverse($d);
    $js = generateGraph(0, $t, $h, $d);  	 
	
    $one_day = 2 * 24;
    
	$links = "<table width=100%><tr><td>$d1</td><td align=center>";
	$links .= "<a href='index.php?limit=$limit&offset=".($offset+$limit)."&id=$clientid'>Previous</a>";
	if( ($offset-$limit) >= 0 ) $links .= " &nbsp; &nbsp; <a href='index.php?limit=$limit&offset=".($offset-$limit)."&id=$clientid'>Next</a>";
	$links .= "</td><td align=right>$d2</td></tr></table>";
	
    $report = " 
    	<html>  
    	<head>
    	<title>Humidor Temperature/Humidity Log</title>
	    <style type='text/css'>
		    .bb td, .bb th {
		     border-bottom: 1px solid black !important;
		    }
		    TABLE{ font-family: verdana; font-size:10px;}
		</style>
		<script src='Chart.js'></script>
		<script>
			Chart.defaults.global.animation = false;
			function doChange(limit){
				location = 'index.php?limit='+limit+'&offset=$offset&id=$clientid'
			}
		</script>
		</head>
    	<center>
    	<table border=1 bordercolor='#003300' cellspacing=0 cellpadding=6>
    		<tr>
    			<td rowspan=2 valign=top>
    				<img src='$humi_img'>
    				<div style='position:relative;top:10px;right:-20px'>
    				Interval: &nbsp; &nbsp;
    				<select name=timeSpan id=timeSpan onchange='doChange(this.value)'>
	    				<option value=$one_day>Day</option>
	    				<option ".($limit==($one_day * 3) ? "SELECTED" : "")." value=".($one_day * 3).">3 day</option>
	    				<option ".($limit==($one_day * 7) ? "SELECTED" : "")." value=".($one_day * 7).">Week</option>
	    				<option ".($limit==($one_day * 30) ? "SELECTED" : "")." value=".($one_day * 30).">Month</option>
	    				<!--option ".($limit==($one_day * 90) ? "SELECTED" : "")." value=".($one_day * 90).">3 Month</option-->
	    				<!--option ".($limit==($one_day * 3) ? "SELECTED" : "")." value=".($one_day * 180).">6 Month</option-->
    				</select>
    				</div>
    				<br>
    			</td>
    			<td>
					$links
					<div st-yle='width:30%'>
						<div>
							<canvas id='canvas_0' height='450' width='600'></canvas>
						</div>
					</div>
				</td>
    		</tr>
    		<tr>
    			<td align=left>
	    			<table>
	    				<tr class=bb>
	    					<td width=120 align=left><font style='font-size:20px;color:blue'>Humidity:</font></td>
	    					<td><font style='font-size:20px;color:blue'> &nbsp; $h[0]</font></td>
	    					<td width=60 style='border-bottom: 0px solid white !important;'> &nbsp; </td>  					
	    					<td><font style='font-size:20px;color:blue'>Temperature:</font></td>
	    					<td><font style='font-size:20px;color:blue'> &nbsp; $t[0]</font></td>
	    				</tr>
	    				
	    				<tr>
	    					<td>Hum low/high</td>
	    					<td>$lowHumi &nbsp; / &nbsp; $highHumi</td>
	    					<td width=60> &nbsp; </td>
	    					<td>Temp low/high</td>
	    					<td>$lowTemp &nbsp; / &nbsp; $highTemp</td>
	    				</tr>
	    				
	    				<tr>
	    					<td>Avg Humidity: </td>
	    					<td>$avgHumi</td>
	    					<td width=60> &nbsp; </td>
	    					<td>Avg Temperature: </td>
	    					<td>$avgTemp</td>
	    				</tr>
	    				
	    				<tr>
	    					<td>Last Watered: </td>
	    					<td>$lastWatered</td>
	    					<td width=60> &nbsp; </td>
	    					<td width=120 align=left>Records: </td>
	    					<td>$record_cnt</td>
	    				</tr>
	    				
	    				<tr>
	    					<td>Last Update: </td>
	    					<td>$lastUpdate</td>
	    					<td width=60> &nbsp; </td>
	    					<td>Last Reboot: </td>
	    					<td>$lastPowerEvent</td>
	    				</tr>
	    				<!--tr><td></td><td></td></tr-->
	    			</table>
	    		</td>
	    	</tr>
	    	<tr>
	    		<td colspan=2>
	    			<table><tr>
	    				<td width=185> &nbsp; </td>
	    				<td width=320>$waterTable</td>
	    				<td>$smokedTable</td>
	    			</tr></table>
	    		</td>
	    	</tr>
	    </table>
	    </center>
	    <br><br>
    ";
    
    echo $report.$js;
       
    if( $user['alertsent']==1 ){ //we dont want the alert to get cached..
	    echo "
	    	<script>
	    		function clear_alert(){
		    		var key = prompt('Enter the apikey to clear the alert:');
		    		if(key.length==0) return;
		    		window.open('logData.php?clear_alert=1&clientid=$clientid&apikey='+key, '', 'width=200, height=100');
	    		}
	    	</script>
	    	<center><font color=red size=+4><u><a onmouseover='this.style.cursor=\"pointer\"' onclick='clear_alert()'>Clear Alert</a></u></font></center>	    
	    ";
    }
     
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

			window.onload = function(){
				var ctx = document.getElementById('canvas_$index').getContext('2d');
				window.myLine = new Chart(ctx).Line(lineChartData_$index, {
					responsive: true, pointHitDetectionRadius: 1, datasetStrokeWidth: 1,
					pointDotRadius: 1, pointDot: false 
				});
			}
	</script>";
	
	return $r;
	
}        
    
 
?>