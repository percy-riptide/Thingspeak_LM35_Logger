#include<stdlib.h>

// Your channel's thingspeak API key (change it to your API key)
String apiKey = "JYVRWE17MR5R5ZPI";

//SSID and password (change it to your SSID and Pass)
char ssid[] = "Percy";
char pass[] = "yolo@1590";

const int dataPin = A1;
int value;
float temp;
const int groundPin = A0;
const int powerPin = A2;
String cmd;
char x[]=">";
char y[]="Error";
  
void setup(){                
  // enable debug serial
  // sets the baud rate to 115200
  Serial.begin(115200);

  // set pinmode to output
  pinMode(groundPin, OUTPUT);
  pinMode(powerPin, OUTPUT);

  // make groundPin low(0V/GND) and powerPin high(5V)
  digitalWrite(groundPin, LOW);
  digitalWrite(powerPin, HIGH);

  // set reference voltage to 1.1V
  analogReference(INTERNAL);
  
  // connect esp8266 to access point
  Serial.println("AT+CWMODE=3"); // STA+AP mode 
  cmd = "AT+CWJAP=\"";
  cmd += ssid;
  cmd += "\",\"";
  cmd += pass;
  cmd += "\"";
  Serial.println(cmd);
  cmd = "";

  // reset esp8266
  Serial.println("AT+RST");

  //set cpimux
  Serial.println("AT+CIPMUX=0");
}

void loop(){
  // read the value from LM35.
  value=analogRead(dataPin);

  /*value range from 0-1023 (total of 1024)
   *analogReference is set to internal which means 1.1V
   *Each step in the analog reading is approximately equal to: 1.1/1024=0.001074218=1.0742 millivolts
   *LM35 reports 1 degree Celsius per 10 millivolts
   *This means for every (10/1.0742), we detect one degree change i.e., every 9.31
   *Hence dividing by 9.31 gets temperature readings in Celsius*/
  temp = value/9.31;
    
  // convert float value to string
  char buf[16];
  String strTemp = dtostrf(temp, 4, 1, buf);
  
  Serial.println(strTemp);
  
  // TCP connection
  cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "api.thingspeak.com";
  cmd += "\",80";
  Serial.println(cmd);
  cmd = "";
   
  if(Serial.find(y)){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET https://api.thingspeak.com/update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  Serial.println(cmd);
  cmd = "";

  if(Serial.find(x)){
    Serial.print(getStr);
  }
  else{
    Serial.println("AT+CIPCLOSE");
  }
  
  // thingspeak needs 15 sec delay between updates
  delay(15000);  
}
