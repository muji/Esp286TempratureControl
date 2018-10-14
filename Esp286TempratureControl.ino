/* Create a WiFi access point and provide a web server on it so show temperature. 
   Originally published by markingle on http://www.esp8266.com/viewtopic.php?p=47535
   Refactored and enhanced for Hackster.io by: M. Ray Burnette 20160620
   Arduino 1.6.9 on Linux Mint 64-bit version 17.3 compiled: 20160706 by Ray Burnette
    Sketch uses 284,865 bytes (27%) of program storage space. Maximum is 1,044,464 bytes.
    Global variables use 38,116 bytes (46%) of dynamic memory, leaving 43,836 bytes for local variables. Maximum is 81,920 bytes.
    192.168.244.100
*/

#include <FS.h>
#include <WebSocketsServer.h>
#include "HelperFunctions.h"
#include "max6675.h"

bool bl_BurnerState=false;






/*

 NodeMCU LoLin V3  MAX6675 Modul
3.3V  VCC
GND GND
D5  SCK
D6  SO
D7  CS

 
 */


#include "max6675.h"



int thermoCLK_A = 14;   ///D5--->sck
int thermoDO_A  = 12;   ///D6--->so
int thermoCS_A  = 13;   ///D7--->cs


int thermoCLK_B  = 16;  ///D0--->sck
int thermoDO_B =   5;   ///D1--->so
int thermoCS_B  =  4;   ///D2--->cs





MAX6675 thermocouple_A(thermoCLK_A, thermoCS_A, thermoDO_A);

MAX6675 thermocouple_B(thermoCLK_B, thermoCS_B, thermoDO_B);






///////////////////////*****************---------------Start Of Setup-----------------******************//////////////////////
void setup() {
  Serial.begin(115200);
  delay(1000);
  SPIFFS.begin();
  Serial.println(); Serial.print("Configuring access point...");
  setupWiFi();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: "); Serial.println(myIP);

  server.on("/", HTTP_GET, []() {
    handleFileRead("/");
  });

  server.onNotFound([]() {                          // Handle when user requests a file that does not exist
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });

  webSocket.begin();                                // start webSocket server
  webSocket.onEvent(webSocketEvent);                // callback function

  server.begin();
  Serial.println("HTTP server started");
  
}


///////////////////////*****************---------------End Of Setup-----------------******************//////////////////////



 int tempNow;


 
 int tempIgnitBurner;


 ///////////////////////*****************---------------Start Of loop-----------------******************//////////////////////

void loop() {
    static unsigned long l = 0;                     // only initialized once
    unsigned long t;                                // local var: type declaration at compile time

   
    t = millis();

    if((t - l) > 3000) {   
      
       //tempNow=thermocouple_A.readCelsius();
       tempNow=thermocouple_A.readFahrenheit();
    tempIgnitBurner=tempNow-TempSteps;
   
    
   
         for (int n : socketNumber) {
                                          
                                          

                                     webSocket.sendTXT(n, "wpMeter,Arduino,  " + String(tempNow) );
                                   

                             }
                                 
           


   
      
                       // Make Gas off  // 
                         if(intMaxSetTemp==tempNow)
                         {
                           shutdownGas();
                         }

                         // Make Gas On
                          
                         if(tempNow<= tempIgnitBurner)
                         {
                           makeGasOn();
                         }
                         
       
         
        
        l = t;                                    

         Serial.println("\n :Temprature F Now on Probe _A = " + String(thermocouple_A.readFahrenheit() ) );
         Serial.println("\n :Temprature F Now on Probe _B = " + String(thermocouple_B.readFahrenheit() ) );

         Serial.println("\n :Temprature C Now on Probe _A = " + String(thermocouple_A.readCelsius())  );
         Serial.println("\n :Temprature C Now on Probe _B = " + String(thermocouple_B.readCelsius() ) );

        
        {
        if(bl_BurnerState)
        
        Serial.print("Gas is On Now");
        else
        Serial.print("Gas is On Off");
        }
        
       
    }

    server.handleClient();
    webSocket.loop();
}

///////////////////////*****************---------------End Of loop-----------------******************//////////////////////





////////////////////make selonoid valve off close

int shutdownGas()
{
  int ret=-1;
Serial.print( "Gas is Shutdown" );
ret=1;
bl_BurnerState=false;
return ret;

  
}


int makeGasOn()
{
  int ret=-1;
Serial.print( "Gas is is On Now" );
ret=1;
bl_BurnerState=true;
return ret;

  
}
