/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

ESP8266WebServer server (80);

namespace AQUA {
enum class CONTROLLER_TYPE {
  FILTER_CONTROLLER = 1,
  LIGHT_CONTROLLER = 2,
  CO2_CONTROLLER = 3
};
};

void handleRoot() {
  server.send ( 200, "text/html", "usage:\n /LightControll\n /Co2Controll\n /FilterControll\n /HeaterControll\n" );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.send ( 404, "text/plain", message );
}

bool StateFinder()
{
  std::string cmd("state");
  for ( uint8_t i = 0; i < server.args(); i++ ) {
    std::string str = server.argName(i).c_str();
    if (str.compare(cmd) == 0) {
      std::string  val = server.arg(i).c_str();
      if (val.compare("1") == 0) {
        //Serial.println ( "state=1 found in URL" );
        return true;
      }
    }
  }
  return false;
}

int GenericController(AQUA::CONTROLLER_TYPE type)
{
  bool state = StateFinder();
  // Since Relay is active low device need to invert the state.
  bool onOffState = !state;
  
  switch (type) {
    case AQUA::CONTROLLER_TYPE::FILTER_CONTROLLER:
      //if (digitalRead(D1) != onOffState) {
        digitalWrite(D1, onOffState);
      //}
      break;
    case AQUA::CONTROLLER_TYPE::LIGHT_CONTROLLER:
      //if (digitalRead(D2) != onOffState) {
        digitalWrite(D2, onOffState);
      //}
      break;
    case AQUA::CONTROLLER_TYPE::CO2_CONTROLLER:
      //if (digitalRead(D3) != onOffState) {
        digitalWrite(D3, onOffState);
      //}
      break;
  }
  int val = 50;
  ProgressIndicator(val);
  return 0;
}

void ProgressIndicator(int val)
{
  for (int i = 0; i < 5; i++) {
    delay(val);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(val);                       // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  }
  //digitalWrite(LED_BUILTIN, HIGH);
}

void setup ( void ) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode ( D2, OUTPUT ); // White wire
  pinMode ( D3, OUTPUT ); // Brown wire
  pinMode ( D1, OUTPUT ); // Black wire

  digitalWrite ( D2, 0 );
  digitalWrite ( D3, 0 );
  digitalWrite ( D1, 0 );
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin ( 115200 );
  WiFiManager wifiManager;
  wifiManager.autoConnect("SantTest");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  if ( MDNS.begin ( "esp8266" ) ) {
    Serial.println ( "MDNS responder started" );
  }
  server.on ( "/", handleRoot );
  server.on ( "/LightControll", []() {
    GenericController(AQUA::CONTROLLER_TYPE::LIGHT_CONTROLLER);
    std::string str("Aquarium LED Light is ");
    str += digitalRead(D2) ? "ON" : "OFF";
    str += "\n";
    //Serial.println (str.c_str());
    server.send ( 200, "text/plain", str.c_str() );
  } );

  server.on ( "/Co2Controll", []() {
    GenericController(AQUA::CONTROLLER_TYPE::CO2_CONTROLLER);
    std::string str("Aquarium C02 system  is ");
    str += digitalRead(D3) ? "ON" : "OFF";
    str += "\n";
    //Serial.println (str.c_str());
    server.send ( 200, "text/plain", str.c_str() );
  } );
  
  server.on ( "/FilterControll", []() {
    GenericController(AQUA::CONTROLLER_TYPE::FILTER_CONTROLLER);
    std::string str("Aquarium Filter system is ");
    str += digitalRead(D1) ? "ON" : "OFF";
    str += "\n";
    //Serial.println (str.c_str());
    server.send ( 200, "text/plain", str.c_str() );
  } );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop ( void ) {
  server.handleClient();
}
