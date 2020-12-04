#include <SoftwareSerial.h>

// Access point SSID and Password (change it with your SSID and Password)
char ssid[] = "Percy";
char pass[] = "yolo@1590";

// Your channel's thingspeak API key (change it to your API key)
String apiKey = "JYVRWE17MR5R5ZPI";

int lm35Pin = A0;

// connect pin 2 to TX of Esp and pin 3 to RX of Esp
SoftwareSerial esp8266(2,3);

void setup(){                
  // enable debug serial
  // sets the baud rate to 9600
  Serial.begin(9600);
   
  // enable software serial
  // sets the baud rate to 9600
  esp8266.begin(9600);

  // connect esp8266 to wifi
  esp8266.println("AT+CWMODE=3"); // STA+AP mode 
  String conn = "AT+CWJAP=\"";
  conn += ssid;
  conn += "\",\"";
  conn += pass;
  conn += "\"";
  esp8266.println(conn);
  
  // reset ESP8266
  esp8266.println("AT+RST");
}

void loop(){
  int val = 0;
  char x[]=">";
  char y[]="Error";
  // read the value from LM35.
  val=analogRead(lm35Pin);
  
  // convert to actual temperature reading
  // analog temp values range from 0-1023
  // LM35 outputs 10mV/degree C. ie, 1 Volt => 100 degrees C
  // So Temp = (avg_val/1023)*5 Volts * 100 degrees/Volt
  // which is val*(500/1023) => val*0.489
  float temp = val*0.489;
  
  // convert to string
  char buf[16];
  String strTemp = dtostrf(temp, 4, 1, buf);
  
  Serial.println(strTemp);
  
  // TCP connection
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  // AT+CIPSTART=TCP,api.thingspeak.com,80 (TCP is connection type, api.thingspeak.com is same as the IP written above(184.106.153.149) and 80 is the port (CIPMUX=0 type connection))
  esp8266.println(cmd);
   
  if(esp8266.find(y)){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // prepare GET string
  String getStr = "GET /update?api_key=";
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(strTemp);
  getStr += "\r\n\r\n";

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  esp8266.println(cmd);

  if(esp8266.find(x)){
    esp8266.print(getStr);
  }
  else{
    esp8266.println("AT+CIPCLOSE");
  }
  
  // thingspeak needs 15 sec delay between updates
  delay(5000);  
}
