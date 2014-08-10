# MySQL-Front Dump 2.5
#
# Host: localhost   Database: test
# --------------------------------------------------------
# Server version 5.1.36-community-log


#
# Table structure for table 'humidor'
#

DROP TABLE IF EXISTS humidor;
CREATE TABLE IF NOT EXISTS humidor (
  autoid int(1) unsigned NOT NULL auto_increment,
  temp int(1) unsigned zerofill DEFAULT '0' ,
  humidity int(1) unsigned zerofill DEFAULT '0' ,
  tstamp timestamp NOT NULL DEFAULT 'CURRENT_TIMESTAMP' on update CURRENT_TIMESTAMP,
  watered tinyint(1) unsigned zerofill DEFAULT '0' ,
  powerevt tinyint(1) unsigned DEFAULT '0' ,
  clientid tinyint(1) unsigned DEFAULT '0' ,
  PRIMARY KEY (autoid),
  UNIQUE KEY autoid (autoid),
   KEY autoid_2 (autoid)
);



#
# Table structure for table 'humiusers'
#

DROP TABLE IF EXISTS humiusers;
CREATE TABLE IF NOT EXISTS humiusers (
  autoid tinyint(3) unsigned NOT NULL auto_increment,
  username char(255) NOT NULL ,
  img char(255) ,
  apikey char(255) ,
  email char(255) ,
  alertsent tinyint(3) unsigned ,
  lastid tinyint(3) unsigned ,
  PRIMARY KEY (autoid),
  UNIQUE KEY autoid (autoid),
   KEY autoid_2 (autoid)
);

