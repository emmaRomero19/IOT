#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "LAPTOP-DI4IE39T 3857"
#define WLAN_PASS       "Ha656072"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "emmaRomero" //(see https://accounts.adafruit.com)..."
#define AIO_KEY         "aio_dqrp281N7lwXByc7a1XX8wqZgMk6"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** LED SETUP ***************************************/
int ledPin = 2; //GPIO 2 of ESP8266
int ledPin2 = 4; //GPIO 4 of ESP8266
int ledPin3 = 5; //GPIO 5 of ESP8266

/****************************** Feeds ***************************************/
// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/PRACTICE1_LED", MQTT_QOS_1);
Adafruit_MQTT_Subscribe onoffbutton2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/BOTON2", MQTT_QOS_1);
Adafruit_MQTT_Subscribe onoffbutton3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/BOTON 3", MQTT_QOS_1);
Adafruit_MQTT_Subscribe onoffbutton4 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/BOTONES", MQTT_QOS_1);

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  //LED SETUP
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin,LOW);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2,LOW);
  pinMode(ledPin3, OUTPUT);
  digitalWrite(ledPin3,LOW);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&onoffbutton1);
  mqtt.subscribe(&onoffbutton2);
  mqtt.subscribe(&onoffbutton3);
  mqtt.subscribe(&onoffbutton4);
  
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoffbutton1 || subscription == &onoffbutton2 || subscription == &onoffbutton3 || subscription == &onoffbutton4) {
      Serial.println(F("Got: "));
      if (subscription == &onoffbutton1){
        Serial.println((char *)onoffbutton1.lastread);
      }
      if (subscription == &onoffbutton2){
        Serial.println((char *)onoffbutton2.lastread);
      }
      if (subscription == &onoffbutton3){
        Serial.println((char *)onoffbutton3.lastread);
      }
      if (subscription == &onoffbutton4){
        Serial.println((char *)onoffbutton4.lastread);
      }
    }
    if (strcmp((char *)onoffbutton1.lastread, "1ON") == 0){
      digitalWrite(ledPin, HIGH);
    }
    if(strcmp((char *)onoffbutton1.lastread, "1OFF") == 0){
      digitalWrite(ledPin, LOW);
    }
    if (strcmp((char *)onoffbutton2.lastread, "2ON") == 0){
      digitalWrite(ledPin2, HIGH);
    }
    if(strcmp((char *)onoffbutton2.lastread, "2OFF") == 0){
      digitalWrite(ledPin2, LOW);
    }
    if (strcmp((char *)onoffbutton3.lastread, "3ON") == 0){
      digitalWrite(ledPin3, HIGH);
    }
    if(strcmp((char *)onoffbutton3.lastread, "3OFF") == 0){
      digitalWrite(ledPin3, LOW);
    }
    if (strcmp((char *)onoffbutton4.lastread, "ON") == 0){
      digitalWrite(ledPin, HIGH);
      digitalWrite(ledPin2, HIGH);
      digitalWrite(ledPin3, HIGH);
    }
    if(strcmp((char *)onoffbutton4.lastread, "OFF") == 0){
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, LOW);
      digitalWrite(ledPin3, LOW);
    }
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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
