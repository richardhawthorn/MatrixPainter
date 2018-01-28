#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//make sure you rename and complete the connection_details.h file
//this defines wifi settings and hardware type
#include "connection_details.h"

#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

/************ Global State (you don't need to change this!) ******************/

// WiFiFlientSecure for SSL/TLS support
WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, CONNECTION_SERVER, CONNECTION_SERVERPORT, CONNECTION_USERNAME, CONNECTION_KEY);

/****************************** Feeds ***************************************/

// Setup a feed for subscribing to changes.
Adafruit_MQTT_Subscribe matrixReceive = Adafruit_MQTT_Subscribe(&mqtt, "/matrix/16x16");

/************************* Setup Neopixels **********************************/

Adafruit_NeoPixel strip = Adafruit_NeoPixel(WIDTH * HEIGHT, PIN, NEO_GRB + NEO_KHZ800);

/*************************** Sketch Code ************************************/

void MQTT_connect();


void setup() {

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  Serial.begin(115200);
  delay(10);

  Serial.println(F("MatrixPainter"));

  pinMode(TRIGGER_PIN, INPUT);

  //subscribe to incoming data
  mqtt.subscribe(&matrixReceive);

}

uint32_t x=0;


void wifiSetup(){

  if ( digitalRead(TRIGGER_PIN) == LOW ) {

    WiFiManager wifiManager;
    
    if (!wifiManager.startConfigPortal("MatrixPainter")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }

  }
  
}


void processColour(int pixel, String hex){

    int pixel_adjust = pixel;

    int r;
    int g;
    int b;
    
    if (hex.length() == 3){
      r = hex[0] - 48;
      g = hex[1] - 48;
      b = hex[2] - 48;
    } else {
      r = hex[0] - 48;
      g = hex[2] - 48;
      b = hex[4] - 48;
    }
    
    if (r > 9){ r = r - 7;}
    if (g > 9){ g = g - 7;}
    if (b > 9){ b = b - 7;}

    if (REVERSE_ODD || REVERSE_EVEN){
      int reverse = 0;
      if (REVERSE_EVEN){
        reverse = 1;
      }
      int rows = pixel/WIDTH;
      if(rows%2==reverse){
        int rem = pixel%WIDTH;
        pixel_adjust = ((rows+1)*WIDTH)-rem-1;
      }
    }

    strip.setPixelColor(pixel_adjust, strip.Color(r, g, b));
    
}

void loop() {

  wifiSetup();
  MQTT_connect();

  int char_int = 0;
  String hex = "";
  String received = "";
  int pixel = 0;

  int next = 0;
  int id = 0;
  int frame = 0;
  int message = 0;
  int total = 0;

  received.reserve(200);

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    
    if (subscription == &matrixReceive) {
   
      received = (char *)matrixReceive.lastread;
      received += ',';

      //do we have metadata included in the message
      if (received.substring(0, 1) == "m"){
        String metadata = received.substring(2, received.indexOf('|'));
        String item = "";

        next = metadata.indexOf(',');
        item = metadata.substring(0, next);
        id = item.toInt();
        metadata = metadata.substring(next + 1);

        next = metadata.indexOf(',');
        item = metadata.substring(0, next);
        frame = item.toInt();
        metadata = metadata.substring(next + 1);

        next = metadata.indexOf(',');
        item = metadata.substring(0, next);
        message = item.toInt();
        metadata = metadata.substring(next + 1);

        next = metadata.indexOf(',');
        item = metadata.substring(0, next);
        total = item.toInt();
        metadata = metadata.substring(next + 1);

        pixel = metadata.toInt() - 1;

        received = received.substring(received.indexOf('|') + 1);
        
      }

      Serial.println(received);

      for (int i = 0; i < received.length(); i++){
        
        if (received.charAt(i) == ','){
          
          processColour(pixel, hex);
          hex = "";
          pixel++;
        } else {
          hex += received.charAt(i);
        }
      }
      
    }

    strip.show();

  }

  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }

  }

  Serial.println("MQTT Connected!");
}
