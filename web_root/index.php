<?php  
    include("functions.php");
    
    $limit  = (int)$_GET['limit'];
	$offset = (int)$_GET['offset'];
	$image_path = "humidor.png";
	$last_report = "lastReport.html";
	$last_graph_id = './lastGraph.txt';
	$humi_img = "daves_humi.jpg";
	$lastID = 0;
	$cache_enabled = 0;
	
    if($limit < 1 || $limit > 1000) $limit = $default_limit;
    if($offset < 1) $offset = 0;
    
    ConnectDB();
    
    /* limit graph re-generation to only when new data available (unless special req)*/
    $isStdReport = ($limit == $default_limit && $offset==0) ? 1 : 0;
    if($isStdReport == 0) $image_path = "historical.png";
    
    if($isStdReport && $cache_enabled){ 
	    $r = mysql_query("select autoid from humidor order by autoid desc limit 1");
	    $rr = mysql_fetch_assoc($r);
	    $lastID = $rr['autoid'];
	    $lastGraphID = (int)file_get_contents($last_graph_id);
	    if($lastID == $lastGraphID && file_exists($image_path) && file_exists($last_report)){
		 	   die( '<html><head><title>Cached results</title></head>'.file_get_contents($last_report) );
	    }
	    file_put_contents($last_graph_id,$lastID); 
	}
    
    //get last watered time..
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where watered > 0 order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastWatered = date("g:i a - D m.d.y",$rr['int_tstamp']);
    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor where powerevt > 0 order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastPowerEvent = date("g:i a - D m.d.y",$rr['int_tstamp']);
    
    $r = mysql_query("SELECT UNIX_TIMESTAMP(tstamp) as int_tstamp from humidor order by autoid desc limit 1");
    $rr = mysql_fetch_assoc($r);
    $lastUpdate = date("g:i a - D m.d.y",$rr['int_tstamp']);
    
    //now we build the temp/humidity value arrays from the database
    $r = mysql_query("select * from humidor order by autoid desc limit $limit offset $offset");
	
    $i=0;
    $lowest = 200;
    $highest = 0;
    $avgTemp=0;
    $avgHumi=0;
    $records = 0;
    
	while($rr = mysql_fetch_assoc($r)){
		
		$h[$i] = $rr['humidity'];
		$t[$i] = $rr['temp'];
		
		$avgTemp += $t[$i];
		$avgHumi += $h[$i];
		
		if($h[$i] < $lowest) $lowest = $h[$i];
		if($t[$i] < $lowest) $lowest = $t[$i];
		if($h[$i] > $highest) $highest = $h[$i];
		if($t[$i] > $highest) $highest = $t[$i];
		
		$i++;
	}
	
	$record_cnt = $i;
	$avgTemp = round($avgTemp / $record_cnt,1);
	$avgHumi = round($avgHumi / $record_cnt,1);
	
	//reverse because we ordered desc in sql..
	$t = array_reverse($t); 
    $h = array_reverse($h);
    generateGraph($image_path);  	 
	 
    $report = "   
    	<center>
    	<table border=1 bordercolor=#003300 cellspacing=0 cellpadding=6>
    		<tr>
    			<td rowspan=2 valign=top><img src='$humi_img'></td>
    			<td><img src='$image_path'></td>
    		</tr>
    		<tr>
    			<td align=left>
	    			<table>
	    				<tr><td width=120 align=left>Records: </td><td>$record_cnt</td></tr>
	    				<tr><td>Avg Temperature: </td><td>$avgTemp</td></tr>
	    				<tr><td>Avg Humidity: </td><td>$avgHumi</td></tr>
	    				<tr><td>Last Watered: </td><td>$lastWatered</td></tr>
	    				<tr><td>Last Update: </td><td>$lastUpdate</td></tr>
	    				<tr><td>Last Reboot: </td><td>$lastPowerEvent</td></tr>
	    				<!--tr><td></td><td></td></tr-->
	    			</table>
	    		</td>
	    	</tr>
	    </table>
    ";

    echo $report;
    if($isStdReport) file_put_contents($last_report, $report);
     
 
     
function generateGraph($output_file){

	global $t, $h, $lowest, $highest, $GRAPH_TITLE;
	
	//now we generate the graph 
     include("pChart/pData.class");  
     include("pChart/pChart.class");  
      
     // Dataset definition   
     $DataSet = new pData;  
     //$DataSet->AddPoint(array(1,4,3,4,3,3,2,1,0,7,4,3,2,3,3,5,1,0),"Temperature");  
     //$DataSet->AddPoint(array(1,4,2,6,2,3,0,1,5,1,2,4,5,2,1,0,6,4),"Humidity");  
     $DataSet->AddPoint($t,"Temperature");
     $DataSet->AddPoint($h,"Humidity"); 
     $DataSet->AddAllSeries();  
     $DataSet->SetAbsciseLabelSerie();  
     $DataSet->SetSerieName("Temperature","Temperature");  
     $DataSet->SetSerieName("Humidity","Humidity");  
      
     // Initialise the graph  
     $Test = new pChart(700,230);  
     $Test->setFixedScale($lowest-5, $highest+5);  
     $Test->setFontProperties("Fonts/tahoma.ttf",8);  
     $Test->setGraphArea(50,30,585,200);  
     $Test->drawFilledRoundedRectangle(7,7,693,223,5,240,240,240);  
     $Test->drawRoundedRectangle(5,5,695,225,5,230,230,230);  
     $Test->drawGraphArea(255,255,255,TRUE);  
     $Test->drawScale($DataSet->GetData(),$DataSet->GetDataDescription(),SCALE_NORMAL,150,150,150,TRUE,0,2);     
     $Test->drawGrid(4,TRUE,230,230,230,50);  
      
     // Draw the 0 line  
     $Test->setFontProperties("Fonts/tahoma.ttf",6);  
     $Test->drawTreshold(0,143,55,72,TRUE,TRUE);  
      
     // Draw the cubic curve graph  
     $Test->drawCubicCurve($DataSet->GetData(),$DataSet->GetDataDescription());  
      
     // Finish the graph  
     $Test->setFontProperties("Fonts/tahoma.ttf",8);  
     $Test->drawLegend(600,30,$DataSet->GetDataDescription(),255,255,255);  
     $Test->setFontProperties("Fonts/tahoma.ttf",10);  
     $Test->drawTitle(50,22,$GRAPH_TITLE . date(' - m.d.y') ,50,50,50,585);  
     
     $Test->Render($output_file);
	
}        
    
 
?>