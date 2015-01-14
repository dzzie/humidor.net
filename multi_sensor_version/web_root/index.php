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
	$force         = (int)$_GET['force'];
		
	$lastID        = 0;
	$cache_enabled = 1;
	$one_day = 2 * 24;
	
	if($force==1) $cache_enabled=0;
	
    if($limit < 1 || $limit > 13000) $limit = $one_day;
    if($offset < 1) $offset = 0;
    
    ConnectDB();
    
    $userr = mysql_query("select * from humiusers where autoid=$clientid");
    if(mysql_num_rows($userr)==0){ //list available users..
	    $r = mysql_query("select * from humiusers");
	    
		if(mysql_num_rows($r)==0) die("Log into <a href=admin.php>control panel</a> and create users.."); 
		  
		if(mysql_num_rows($r)==1){ 
			$rr = mysql_fetch_assoc($r); //only one, so just display them..
			$userr = mysql_query("select * from humiusers where autoid=".$rr['autoid']);
		}
		else{   
		    $report = "<b><br><br>Please select a user: <br><ul>\n";
		    while($rr = mysql_fetch_assoc($r)){
			     $report .= "<li><a href=index.php?id=".$rr['autoid'].">".$rr['username']."</a>\n";
		    }
		    die($report);
		}
    }
    
    $user = mysql_fetch_assoc($userr);
    $humi_img     = "./images/".$user['img'];
    $GRAPH_TITLE  = $user['username']." Humidor";
    $scnt         = (int)$user['scnt'];
	
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
    
	$report="
	<html>  
	    	<head>
	    	<title>Humidor Temperature/Humidity Log</title>
			<link rel='stylesheet' type='text/css' href='dropdown.css'>
		    <style type='text/css'>
			    .bb td, .bb th {
			     border-bottom: 1px solid black !important;
			    }
			    TABLE{ font-family: verdana; font-size:10px;}
			</style>
			<script src='Chart.js'></script>
			<script src='dropdown.js'></script>
			<script>
				Chart.defaults.global.animation = false;
				function doChange(limit){
					location = 'index.php?limit='+limit+'&offset=$offset&id=$clientid'
				}
	    		function wasWatered(){
		    		var key = prompt('Enter the apikey to proceed');
		    		if(key.length==0) return;
		    		window.open('logData.php?wasWatered=1&clientid=$clientid&apikey='+key, '', 'width=200, height=100');
	    		}
				function wasSmoked(){
		    		var key = prompt('Enter the apikey to proceed');
		    		if(key.length==0) return;
		    		window.open('logData.php?wasSmoked=1&clientid=$clientid&apikey='+key, '', 'width=200, height=100');
	    		}
			</script>
			</head>
	    	<center>
	    	<table border=1 bordercolor='#003300' cellspacing=0 cellpadding=6>
	    		<tr>
	    			<td rowspan=2 valign=top>
	    				<img src='$humi_img'>
	    				<div style='position:relative;top:10px;'>
						<table border=0>
						<tr>
							<td>
			    				<ul id='sddm'>
									<li><a href='#' onmouseover=\"mopen('m1')\" onmouseout='mclosetime()'>Menu</a>
										<div style='visibility:hidden' id='m1' onmouseover='mcancelclosetime()' onmouseout='mclosetime()'>
											<a href='#' onclick='wasWatered()'>Watered</a>
											<a href='#' onclick='wasSmoked()'>Smoked</a>
											<!--a href='#'>Add Note</a>
											<a href='#'>Clear db</a-->
										</div>
									</li>
								</ul>
							</td>
							<td valign=top>
								Interval:&nbsp;
			    				<select name=timeSpan id=timeSpan onchange='doChange(this.value)'>
				    				<option value=$one_day>Day</option>
				    				<option ".($limit==($one_day * 3) ? "SELECTED" : "")." value=".($one_day * 3).">3 day</option>
				    				<option ".($limit==($one_day * 7) ? "SELECTED" : "")." value=".($one_day * 7).">Week</option>
									<option ".($limit==($one_day * 14) ? "SELECTED" : "")." value=".($one_day * 14).">2 Week</option>
				    				<option ".($limit==($one_day * 30) ? "SELECTED" : "")." value=".($one_day * 30).">Month</option>
				    				<!--option ".($limit==($one_day * 90) ? "SELECTED" : "")." value=".($one_day * 90).">3 Month</option-->
				    				<!--option ".($limit==($one_day * 180) ? "SELECTED" : "")." value=".($one_day * 180).">6 Month</option-->
			    				</select>
							</td>
						</tr>
						</table>
	    				</div>
	    				<br>
	    			</td>
	    		</tr>
	    		<tr>
	    			<td align=left>
		";
		
		$js='';
		for($i=0;$i<$scnt;$i++){ //for each sensor 
			$report.=generate_report($i,$js)."<br><br>";
		}
		
		$report.="
				<hr>
				<table>
					<tr>
    					<td>Last Watered: </td>
    					<td>$lastWatered</td>
    				 
    					<td width=60 &nbsp; </td>
    					
    					<td width=120 align=left>Records: </td>
    					<td>$record_cnt</td>
    				</tr>
    				
    				<tr>
    					<td>Last Update: </td>
    					<td>$lastUpdate</td>
    				 
    					<td width=60 &nbsp; </td>
    					
    					<td>Last Reboot: </td>
    					<td>$lastPowerEvent</td>
    				</tr>
				</table>
				</td>
		    	</tr>
		    	<tr>
		    		<td colspan=2>
		    			<table>
						<tr>
		    				<td colspan=3 align=right>
								<!--
								<div style='position: relative; width: 0; height: 0;'>
									<!-- outter div is so doesnt take up any space in document flow -- >
									<div id='events_legendDiv' style='position:relative;top:20;back-ground-color:#e0e0e0;display:inline-block;white-space: nowrap;'>
											&bull; <font style='background-color:rgba(151,187,205,1)'>Watered</font><br>
											&bull; <font style='background-color:rgba(220,220,220,1)'>Smoked </font><br>
									</div>
								</div>
								-->
								<canvas id='event_canvas' height='50'></canvas>
								<br><br>
							</td>
		    			</tr>
						<tr>
		    				<td width=185> &nbsp; </td>
		    				<td width=320 valign=top>$waterTable</td>
		    				<td valign=top>$smokedTable</td>
		    			</tr>
					</table>
		    		</td>
		    	</tr>
		    </table>
		    </center>
		    <br><br>
	    ";

	$event_js = eventsGraph();
	echo $report.$js.$event_js;
	
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
	
	
function generate_report($sid,&$js){
		global $image_path,$clientid,$limit,$offset,$GRAPH_TITLE, $record_cnt,$one_day;
	
		//now we build the temp/humidity value arrays from the database
	    $r = mysql_query("select * from humidor where clientid=$clientid and sid=$sid order by autoid desc limit $limit offset $offset");
		
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
		
		$record_cnt = $i;
		$avgTemp = round($avgTemp / $record_cnt,1);
		$avgHumi = round($avgHumi / $record_cnt,1);
		$curh = $h[0];
		$curt = $t[0];
		
		//reverse because we ordered desc in sql..
		$t = array_reverse($t); 
	    $h = array_reverse($h);
	    $d = array_reverse($d);
	    $js .= generateGraph($sid, $t, $h, $d);  //byref out value return..	 
			    
		$links = "<table width=100%><tr><td>$d1</td><td align=center>";
		$links .= "<a href='index.php?limit=$limit&offset=".($offset+$limit)."&id=$clientid'>Previous</a>";
		if( ($offset-$limit) >= 0 ) $links .= " &nbsp; &nbsp; <a href='index.php?limit=$limit&offset=".($offset-$limit)."&id=$clientid'>Next</a>";
		$links .= "</td><td align=right>$d2</td></tr></table>";
	  
	    $report = " 
		    			<table>
							<tr>
								<td colspan=5>
									$links
									<div st-yle='width:30%'>
										<div>
											<canvas id='canvas_$sid' height='250' width='600'></canvas>
										</div>
									</div>
								</td>
							</tr>
		    				<tr class=bb>
		    					<td width=120 align=left><font style='font-size:20px;color:blue'>Humidity:</font></td>
		    					<td><font style='font-size:20px;color:blue'> &nbsp; $curh</font></td>
		    				
		    					<td width=60 &nbsp; </td>
		    					
		    					<td><font style='font-size:20px;color:blue'>Temperature:</font></td>
		    					<td><font style='font-size:20px;color:blue'> &nbsp; $curt</font></td>
		    				</tr>
		    				
		    				<tr>
		    					<td>Hum low/high</td>
		    					<td>$lowHumi &nbsp; / &nbsp; $highHumi</td>
		    				
		    					<td width=60 &nbsp; </td>
		    					
		    					<td>Temp low/high</td>
		    					<td>$lowTemp &nbsp; / &nbsp; $highTemp</td>
		    				</tr>
		    				
		    				<tr>
		    					<td>Avg Humidity: </td>
		    					<td>$avgHumi</td>
		    				
		    					<td width=60 &nbsp; </td>
		    					
		    					<td>Avg Temperature: </td>
		    					<td>$avgTemp</td>
		    				</tr>
		    				<!--tr><td></td><td></td></tr-->
		    			</table>";
	    
	    return $report;
}
    
//show rolling stats for last 12 mos, out args by ref 
// month names csv, smoked events csv, water events csv 
function eventsfor_last12Months(&$d,&$s,&$w){

    global $clientid;
	
    $mname = array(0, 'Jan','Feb','Mar','Apr','May','June','July','Aug','Sept','Oct','Nov','Dec', 14);
	$y=0; 
	$year_index=0;
	$month = date("m"); //start month number
	
    for($i=12; $i>0; $i--){
		$mi = $month - $y; //month index
		
		if($mi <= 0){
			 $mi += 12; 
			 $year_index = 1; //we have crossed into previous year now...
		}
		
		//echo $mi. ",";
		$d[$i-1] = $mname[$mi];
		
		$r = mysql_query("SELECT count(autoid) as c FROM humidor WHERE MONTH(tstamp) = $mi AND YEAR(tstamp) = (YEAR(NOW()) - $year_index) and smoked=1 and clientid=$clientid");
		$rr = mysql_fetch_assoc($r);
		$s[$i-1] = $rr['c'];
		
		$r = mysql_query("SELECT count(autoid) as c FROM humidor WHERE MONTH(tstamp) = $mi AND YEAR(tstamp) = (YEAR(NOW()) - $year_index) and watered=1 and clientid=$clientid");
		$rr = mysql_fetch_assoc($r);
		$w[$i-1] = $rr['c'];
		
		$y+=1;
	}
	//echo var_dump($d);
	$d = arytocsv( $d, 1 );
	$s = arytocsv( $s );
	$w = arytocsv( $w );
}

function arytocsv($ary, $isStr = 0){
	$r='';
	for($i=0; $i < count($ary); $i++){
		if($isStr == 1) $r.= "'".$ary[$i]."'"; else $r.= $ary[$i];
		if($i != count($ary)-1) $r.= ", ";
	}
	return $r;
} 
 
function eventsGraph(){

    $smoke_data = ''; $month_names=''; 	$water_data='';
	eventsfor_last12Months($month_names, $smoke_data, $water_data);
	
	$r = "
		<script>
			  
			var smokeChartData = {
				labels : [$month_names],
				datasets : [
					{
						label: 'Watered',
						fillColor : 'rgba(151,187,205,0.5)',
						strokeColor : 'rgba(151,187,205,0.8)',
						highlightFill : 'rgba(151,187,205,0.75)',
						highlightStroke : 'rgba(151,187,205,1)',
						data : [$water_data]
					},
					{
						label: 'Smoked',
						fillColor : 'rgba(220,220,220,0.5)',
						strokeColor : 'rgba(220,220,220,0.8)',
						highlightFill: 'rgba(220,220,220,0.75)',
						highlightStroke: 'rgba(220,220,220,1)',
						data : [$smoke_data]
					}
				]
			}
			
			var ctx = document.getElementById('event_canvas').getContext('2d');
			window.myBar = new Chart(ctx).Bar(smokeChartData, {
				responsive : true
			});
			
			/*window.myBar.options.legendTemplate = 
				'<span style=\"background-color:#ccffcc;\"><ul>'
                  +'<% for (var i=0; i<datasets.length; i++) { %>'
                    +'<li>'
                    +'<font xx style=\"width:10;background-color:<%=datasets[i].fillColor%>\">'
                    +'<% if (datasets[i].label) { %><%= datasets[i].label %><% } %>'
                  +'</font></li>'
                +'<% } %>'
              +'</ul></span>'
			
			document.getElementById('events_legendDiv').innerHTML = window.myBar.generateLegend() ;
			*/
			
		</script>
		";
			
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