#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "LAPTOP-DI4IE39T 3857"
#define WLAN_PASS       "Ha656072"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "emmaRomero"
#define AIO_KEY         "aio_EqNo68BlxJ5eDXWQcp2OE56gBPrW"

/****************************** Feeds ***************************************/
#define Relay           2
#define lr              12
#define lg              13
#define lb              15

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe Light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/google");
Adafruit_MQTT_Subscribe intensity= Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/intensidad", MQTT_QOS_1);
Adafruit_MQTT_Subscribe LightRGB = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led_rgb");
Adafruit_MQTT_Subscribe intensityRGB= Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/intensidad_rgb", MQTT_QOS_1);
Adafruit_MQTT_Subscribe textColor = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/color");

/*************************** Sketch Code ************************************/

uint16_t ledBrightValue = 0;
String color="";
double lr2 = 0;
double lg2 = 0;
double lb2 = 0;


void setup() {
  Serial.begin(115200);
  pinMode(Relay, OUTPUT);
  pinMode(lr, OUTPUT);
  pinMode(lg, OUTPUT);
  pinMode(lb, OUTPUT);
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
  mqtt.subscribe(&Light1);
  mqtt.subscribe(&intensity);
  mqtt.subscribe(&LightRGB);
  mqtt.subscribe(&intensityRGB);
  mqtt.subscribe(&textColor);
}


void loop() {
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  
  while ((subscription = mqtt.readSubscription(200))) {

/***************************LED1 ************************************/
    if (subscription == &Light1) {
      Serial.println("LED 1");
      Serial.print(F("Got: "));
      Serial.println((char *)Light1.lastread);
      if(strcmp((char *)Light1.lastread, "ON") == 0){
        digitalWrite(Relay, HIGH);
      }
      else if(strcmp((char *)Light1.lastread, "OFF") == 0){
        digitalWrite(Relay, LOW);
      }
    }
    if(subscription==&intensity){
      Serial.println("LED 1 intensidad");
      Serial.println("Obteniendo valor de intensidad del LED");
      ledBrightValue = atoi((char *)intensity.lastread);
      Serial.println("Valor de intensidad");
      Serial.println(ledBrightValue*2.55);
      analogWrite(Relay,ledBrightValue*2.55);
    }
/***************************LED RGB************************************/

    if (subscription == &LightRGB) {
      Serial.println("LED 2");
      Serial.print(F("Got: "));
      Serial.println((char *)LightRGB.lastread);
      if(strcmp((char *)LightRGB.lastread, "ON") == 0){
        rgb_color(255, 255, 255);
      }
      else if(strcmp((char *)LightRGB.lastread, "OFF") == 0){
         rgb_color(0, 0, 0);
      }
    }
    
    if (subscription == &textColor) {
      Serial.println("Color");
      Serial.print(F("Got: "));
      Serial.println((char *)textColor.lastread);
      if(strcmp((char *)textColor.lastread, "rojo") == 0){
        rgb_color(255, 0, 0);
        color="rojo";
      }
      else if(strcmp((char *)textColor.lastread, "verde") == 0){
         rgb_color(0, 255, 0);
        color="verde";
      }
      else if(strcmp((char *)textColor.lastread, "azul") == 0){
         rgb_color(0, 0, 255);
        color="azul";
      }
      else if(strcmp((char *)textColor.lastread, "rosa") == 0){
        rgb_color(250, 50, 47);
        color="rosa";
      }
      else if(strcmp((char *)textColor.lastread, "morado") == 0){
         rgb_color(80,3,110);
        color="morado";
      }
      else if(strcmp((char *)textColor.lastread, "amarillo") == 0){
         rgb_color(255,255,0);
        color="amarillo";
      }
      else if(strcmp((char *)textColor.lastread, "naranja") == 0){
         rgb_color(255,69,0);
        color="naranja";
      }
      else if(strcmp((char *)textColor.lastread, "cian") == 0){
         rgb_color(0,150,200);
        color="cian";
      }
      else if(strcmp((char *)textColor.lastread, "Limón") == 0){
         rgb_color(50, 150, 15);
        color="Limón";
      }
      else if(strcmp((char *)textColor.lastread, "blanco") == 0){
         rgb_color(255,255,255);
        color="blanco";
      }
      else{
        rgb_color(0, 0, 0);
        color="";
      }
    }
    
    if(subscription==&intensityRGB){
      Serial.println("LED RGB intensidad");
      Serial.println("Obteniendo valor de intensidad del LED");
      ledBrightValue = atoi((char *)intensityRGB.lastread);
      Serial.println("Valor de intensidad");
      Serial.println(ledBrightValue*2.55);
      if(color == "rojo"){
        lr2=(ledBrightValue*2.55);
        analogWrite(lr,lr2);
      }
      else if(color == "verde"){
        lg2=(ledBrightValue*2.55);
        analogWrite(lg,lg2);
      }
      else if(color == "azul"){
        lb2=(ledBrightValue*2.55);
        analogWrite(lb, lb2);
      }
      else if(color == "rosa"){
        lr2=(ledBrightValue*2.55)+170;
        lb2=(ledBrightValue*2.55);
        analogWrite(lr, lr2);
        analogWrite(lb, lb2);
      }
      else if(color == "morado"){
        lb2=(ledBrightValue*2.55)+50;
        analogWrite(lb, lb2);
      }
      else if(color == "amarillo"){
        lr2=(ledBrightValue*2.55);
        lg2=(ledBrightValue*2.55);
        analogWrite(lr, lr2);
        analogWrite(lg, lg2);
      }
      else if(color == "naranja"){
        lr2=(ledBrightValue*2.55)+170;
        analogWrite(lr,lr2);
      }
      else if(color == "cian"){
        lg2=(ledBrightValue*2.55)+150;
        lb2=(ledBrightValue*2.55)+50;
        analogWrite(lg, lg2);
        analogWrite(lb, lb2);
      }
      else if(color == "Limón"){
        lg2=(ledBrightValue*2.55)+130;
        analogWrite(lg,lg2);
      }
      else if(color == "blanco"){
        lr2=(ledBrightValue*2.55);
        lg2=(ledBrightValue*2.55);
        lb2=(ledBrightValue*2.55);
        analogWrite(lr, lr2);
        analogWrite(lg, lg2);
        analogWrite(lb, lb2);
      }
    }
    }
  }

void rgb_color(int red, int green, int blue){
    analogWrite(lr, red);
    analogWrite(lg, green);
    analogWrite(lb, blue);
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
