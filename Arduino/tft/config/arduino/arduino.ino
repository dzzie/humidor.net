  

//make sure all serial responses end with \n ! 

  void setup()
  {
        Serial.begin(9600);
		Serial.print("welcome\n");
 }
     
 void loop()
 {

      String nom;
	  String val;

      if (Serial.available())
      {     
		  Serial.print("avail!\n");
		  while( Serial.available() ){
			  nom = Serial.readStringUntil(':');
			  val = Serial.readStringUntil('\n');	
			  Serial.print(nom + val+"\n");
		  }
	  }else{
		  Serial.print("nada\n");
	  }

	  delay(2000);
      Serial.print("next\n");

 }
 
 
 
 