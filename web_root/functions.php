<?

error_reporting(E_ERROR  /*|  E_PARSE  | E_COMPILE_ERROR  */| E_COMPILE_WARNING);


$dblink=0;
$dev_machine_webroot = "D:/_web_root/";
$default_limit = 3 * 24; //1 day (3 updates an hour x 24 hours)
$alert_file = "./alert.txt"; //user must clear the alert file, so we dont spam them waiting to handle..

/*---------- public config block -----------------------
  	you can include private machine overrides in private_db.php
	change $dev_machine_webroot so it knows when to use private db or not..*/
$DB_IP = '127.0.0.1';
$DB_USER = 'root';
$DB_PASS = '';
$DB_DB = 'test';
$API_KEY = '123456';
$ALERT_EMAIL = 'DZZIE@YAHOO.COM';
$GRAPH_TITLE = "Daves Humidor";
//--------- end public comfig block --------------------

//date_default_timezone_set('America/New_York'); //php5 only
putenv("TZ=US/Eastern");

if( strcasecmp($_SERVER['DOCUMENT_ROOT'], $dev_machine_webroot)!=0){ //NOT dev machine
	if(file_exists("private_db.php")) include_once("private_db.php");
}
	
if(!function_exists('file_put_contents')) {
    function file_put_contents($filename, $data, $file_append = false) {
      $fp = fopen($filename, (!$file_append ? 'w+' : 'a+'));
        if(!$fp) {
          trigger_error('file_put_contents cannot write in file.', E_USER_ERROR);
          return;
        }
      fputs($fp, $data);
      fclose($fp);
    }
}

if(!function_exists('file_get_contents')) {
	function file_get_contents( $filename ){// get contents of a file into a string
		$handle = fopen($filename, "r");
		$contents = fread($handle, filesize($filename));
		fclose($handle);
		return $contents;
	} 
}
  
/*
session_start();

function requireLogin(){
	
	if(strlen($_SESSION['user'])==0){
		header('Location: http://www.goatse.info/');
		exit();
	}
	
}
*/

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

function sendMail($to,$subj,$body,$attachmentData='',$attachFileName=''){
	 		 
		 $message = $body;
		 $boundry = "=====================_" . md5(time()) . "==_"; 
         $headers = "From: Sandsprite Blogs <no-reply@sandsprite.com>\r\n".
			        "To:".$to."\r\n";
			                 
		 if(strlen($attachmentData)>0){	  
			  $data = chunk_split(base64_encode($attachmentData));            	          	  
			  
			  $headers.= "Mime-Version: 1.0\r\n".
	    				 "Content-Type: multipart/mixed; boundary=\"" . $boundry . "\"\r\n";
			  
	    	  $message=  "\r\n\r\n--"  . $boundry."\r\n".
	    				 "Content-Type: text/plain; charset=\"us-ascii\"\r\n\r\n".
	    				 $body."\r\n\r\n".
	    				 "--" . $boundry . "\r\n".
					     "Content-Type: application/octet-stream; name=\"" .$attachFileName. "\"\r\n".
					     "Content-Transfer-Encoding: base64\r\n".
					     "Content-Disposition: attachment; filename=\"" .$attachFileName. "\"\r\n\r\n".
					     $data."\r\n\r\n";
		}
	   		
        return mail($to,$subj,$message,$headers);                    
	                      
}

?>