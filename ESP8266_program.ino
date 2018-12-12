/*
    Project created to read 6 values received by serial:
    * An URL where will be load the data
    * Temperature
    * Humedity
    * Heat index
    * PM10 particles
    * PM2.5 particles
    
    This sketch must be loaded to ESP8266 module
*/

// Librery for JSON
#include <ArduinoJson.h>

// Libreries for ESP2866 WiFi module
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>


#define USE_SERIAL Serial

String host="";
String temp="";
String hum="";
String inh="";
String pm10="";
String pm25="";


ESP8266WiFiMulti WiFiMulti;
void setup() {
    // Init Serial speed
    USE_SERIAL.begin(9600);
    USE_SERIAL.setTimeout(5);
   
    
    // Add network to WiFiMulti
      WiFiMulti.addAP("SSID","Password");
}

void conectar(){
  if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        USE_SERIAL.print("[HTTP] begin...\n");
        // Configure traged server and url
        http.begin("http://"+host); //HTTP
        // Adding headers
        http.addHeader("Authorization", "");
        http.addHeader("Content-Type", "application/json");
        
        USE_SERIAL.print("[HTTP] POST...\n");
        
        //Declare JSON
        StaticJsonBuffer<200> jsonBuffer;
        char json[256];
        JsonObject& root = jsonBuffer.createObject();

        // Reading temperature and adding to JSON
        root["temperature"] = temp.toFloat();
        root["humedad"]=hum.toFloat();
        root["indice"]=inh.toFloat();
        root["pm10"]=pm10.toInt();
        root["pm25"]=pm25.toInt();
        root.printTo(json, sizeof(json));
        Serial.print(json);
 
        // Start connection, adding json and send HTTP header
        int httpCode = http.POST(json);
        
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] POST... code: %d\n", httpCode);

            // File found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        // Ending conection
        http.end();
    }
    delay(10000);
}

int comas[5];

void loop(){
  String result="";
  if(USE_SERIAL.available() > 0){
    result = USE_SERIAL.readStringUntil('\r\n');
    USE_SERIAL.println(result);
      int cm=0;
      for(int i=0; i<result.length(); i++){
        if(result.charAt(i)==','){
          comas[cm]=i;
          cm++;
        }
      }
      if(cm==5){
        host=result.substring(0,comas[0]);
        temp=result.substring(comas[0]+1,comas[1]);
        hum=result.substring(comas[1]+1,comas[2]);
        inh=result.substring(comas[2]+1,comas[3]);
        pm10=result.substring(comas[3]+1,comas[4]);
        pm25=result.substring(comas[4]+1,result.length()-1);
        conectar();
      }else{
        USE_SERIAL.println("Parametros incompletos");
       }
      }
}
