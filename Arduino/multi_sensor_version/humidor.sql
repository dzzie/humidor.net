# MySQL-Front Dump 2.5
#
# Host: localhost   Database: test
# --------------------------------------------------------
# Server version 5.1.36-community-log


#
# Table structure for table 'humidor'
#

CREATE TABLE IF NOT EXISTS humidor (
  autoid int(1) unsigned NOT NULL auto_increment,
  temp int(1) unsigned zerofill DEFAULT '0' ,
  humidity int(1) unsigned zerofill DEFAULT '0' ,
  tstamp timestamp NOT NULL DEFAULT 'CURRENT_TIMESTAMP' on update CURRENT_TIMESTAMP,
  watered tinyint(1) unsigned zerofill DEFAULT '0' ,
  powerevt tinyint(1) unsigned DEFAULT '0' ,
  clientid tinyint(1) unsigned DEFAULT '0' ,
  smoked tinyint(3) unsigned DEFAULT '0' ,
  sid tinyint(1) unsigned DEFAULT '1' ,
  PRIMARY KEY (autoid),
  UNIQUE KEY autoid (autoid),
   KEY autoid_2 (autoid)
);



#
# Dumping data for table 'humidor'
#

INSERT INTO humidor VALUES("2473", "68", "69", "2014-11-22 01:52:24", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2474", "66", "69", "2014-11-23 02:53:56", "0", "1", "6", "0", "1");
INSERT INTO humidor VALUES("2475", "67", "70", "2014-11-24 03:54:07", "1", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2476", "66", "69", "2014-11-25 04:54:02", "0", "0", "6", "1", "1");
INSERT INTO humidor VALUES("2477", "67", "69", "2014-11-26 05:50:06", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2478", "66", "71", "2014-11-27 06:52:51", "0", "0", "6", "0", "1");
INSERT INTO humidor VALUES("2479", "67", "69", "2014-11-28 07:50:06", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2480", "66", "69", "2014-11-29 08:51:29", "0", "0", "6", "0", "1");
INSERT INTO humidor VALUES("2481", "66", "69", "2014-11-30 09:54:09", "1", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2482", "66", "68", "2014-11-27 04:54:03", "0", "0", "6", "1", "1");
INSERT INTO humidor VALUES("2483", "67", "69", "2014-11-27 04:50:06", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2484", "66", "69", "2014-11-27 04:51:29", "0", "0", "6", "0", "1");
INSERT INTO humidor VALUES("2485", "67", "67", "2014-11-27 04:52:58", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2486", "66", "69", "2014-11-27 04:51:29", "0", "0", "6", "0", "1");
INSERT INTO humidor VALUES("2487", "68", "66", "2014-11-27 04:54:06", "0", "0", "6", "1", "0");
INSERT INTO humidor VALUES("2488", "66", "69", "2014-11-27 04:51:29", "0", "0", "6", "0", "1");
INSERT INTO humidor VALUES("2489", "67", "67", "2014-11-27 04:53:13", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2490", "66", "69", "2014-11-27 04:54:12", "1", "0", "6", "0", "1");
INSERT INTO humidor VALUES("2491", "69", "68", "2014-11-27 04:53:17", "0", "0", "6", "0", "0");
INSERT INTO humidor VALUES("2492", "66", "69", "2014-11-27 04:51:29", "0", "0", "6", "0", "1");


#
# Table structure for table 'humiusers'
#

CREATE TABLE IF NOT EXISTS humiusers (
  autoid tinyint(3) unsigned NOT NULL auto_increment,
  username char(255) NOT NULL ,
  img char(255) ,
  apikey char(255) ,
  email char(255) ,
  alertsent tinyint(3) unsigned ,
  lastid tinyint(3) unsigned ,
  scnt tinyint(1) unsigned ,
  PRIMARY KEY (autoid),
  UNIQUE KEY autoid (autoid),
   KEY autoid_2 (autoid)
);



#
# Dumping data for table 'humiusers'
#

INSERT INTO humiusers VALUES("6", "Dave", "daves_humi.jpg", "password", "dzzie@yahoo.com", "0", NULL, "2");
