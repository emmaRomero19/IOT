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
#define AIO_KEY         "aio_sSJy95pDMDi9eBVUgjh05HVLLP4y"

/************************* TEMPERATURE *********************************/

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define ledPin 16

#define DHTTYPE    DHT11 
DHT_Unified dht(DHTPIN, DHTTYPE);

//tiempo de espera
unsigned long tiempo1 = 10000;
unsigned long tiempo2 = 0;

double temperatura= 0.0;

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temp_value = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/TEMPERATURE", MQTT_QOS_1);
Adafruit_MQTT_Publish hum_value = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/HUMIDITY", MQTT_QOS_1);
Adafruit_MQTT_Subscribe led_control= Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED_CONTROL", MQTT_QOS_1);

uint16_t ledBrightValue = 0;

void MQTT_connect();

/************************* IFTTT *********************************/
const char* resource1 = "/trigger/TemperaturaIFTTT/with/key/b0EEUKFFRwk8y5pG74tbua";
const char* resource2 = "/trigger/TemperaturaEmail/with/key/b0EEUKFFRwk8y5pG74tbua";
const char* resource3 = "/trigger/HumidityEmail/with/key/b0EEUKFFRwk8y5pG74tbua";
const char* server = "maker.ifttt.com";


/************************* COLOR PICKER CONNECTION *********************************/
#include "AdafruitIO_WiFi.h"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE) ||         \
    defined(ADAFRUIT_PYPORTAL)
// Configure the pins used for the ESP32 connection
#if !defined(SPIWIFI_SS) // if the wifi definition isnt in the board variant
// Don't change the names of these #define's! they match the variant ones
#define SPIWIFI SPI
#define SPIWIFI_SS 10 // Chip select pin
#define NINA_ACK 9    // a.k.a BUSY or READY pin
#define NINA_RESETN 6 // Reset pin
#define NINA_GPIO0 -1 // Not connected
#endif
AdafruitIO_WiFi io(AIO_USERNAME, AIO_KEY, WLAN_SSID, WLAN_PASS, SPIWIFI_SS,
                   NINA_ACK, NINA_RESETN, NINA_GPIO0, &SPIWIFI);
#else
AdafruitIO_WiFi io(AIO_USERNAME, AIO_KEY, WLAN_SSID, WLAN_PASS);
#endif

/************************* COLOR PICKER LEDS *********************************/

#define RED_PIN   14
#define GREEN_PIN 12
#define BLUE_PIN  13
AdafruitIO_Feed *color = io.feed("Color_RGB");



void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT subscription
  mqtt.subscribe(&led_control);

  // Initialize device.
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  
  //CONECT TO IO.ADAFRUIT
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  color->onMessage(handleMessage);

}

void loop() {

  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while(subscription = mqtt.readSubscription(200))
  
  if(subscription==&led_control){
    Serial.println("Obteniendo valor de intensidad del LED");
    ledBrightValue = atoi((char *)led_control.lastread);
    Serial.println("valor de intensidad");
    Serial.println(ledBrightValue*2.55);
    analogWrite(ledPin,ledBrightValue*2.55);
  }
  
  //temperature on adafruit
  if((millis() - tiempo2) >= tiempo1){
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if(isnan(event.temperature)){
      Serial.print("Error al leer los datos");
      tiempo2=millis();
    }
    else{
      Serial.print("Temperature");
      Serial.println("°C");
      
      if(!temp_value.publish(event.temperature)){
        Serial.println("Error de publicacion");
      }else{
        Serial.print(event.temperature);
        Serial.println("OK!");
        if(((event.temperature)>=25.0) && ((event.temperature)<=26.0)){
          makeIFTTTRequest1();
        }
        if((event.temperature)>=30.0){
          makeIFTTTRequest2();
        }
      }
      dht.humidity().getEvent(&event);
      if(isnan(event.relative_humidity)){
        Serial.print("Error al leer los datos");
        tiempo2=millis();
      }
      else{
        Serial.print("Humidity");
        Serial.println("%");
        if(!hum_value.publish(event.relative_humidity)){
        Serial.println("Error de publicacion");
        }else{
          Serial.print(event.relative_humidity);
          Serial.println("OK!");
          if((event.relative_humidity)>=45.0){
            makeIFTTTRequest3();
          }
        }
      }
      tiempo2=millis();
    }
  }
  io.run();
}

void handleMessage(AdafruitIO_Data *data) {

  // print RGB values and hex value
  Serial.println("Received:");
  int rojo = (data->toRed());
  int verde = (data->toGreen());
  int azul = (data->toBlue());
  Serial.print(rojo);
  Serial.print(",");
  Serial.print(verde);
  Serial.print(",");
  Serial.print(azul);
  rgb_color(rojo,verde,azul);
}

void rgb_color(int red, int green, int blue){
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
  }

// Establish a Wi-Fi connection with your router
void initWifi() {
  Serial.print("Connecting to: "); 
  Serial.print(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);  

  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED  && (timeout-- > 0)) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect, going back to sleep");
  }

  Serial.print("WiFi connected in: "); 
  Serial.print(millis());
  Serial.print(", IP address: "); 
  Serial.println(WiFi.localIP());
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

void makeIFTTTRequest1() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource1);
  client.print(String("GET ") + resource1 + 
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" + 
                  "Connection: close\r\n\r\n");
                  
                  
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
     Serial.println("No response, going back to sleep");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("TEMPERATURA NOTIFICACION ENVIADA");
  client.stop();
}
void makeIFTTTRequest2() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource2);
  client.print(String("GET ") + resource2 + 
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" + 
                  "Connection: close\r\n\r\n");
                  
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
     Serial.println("No response, going back to sleep");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("TEMPERATURA EMAIL ENVIADO");
  client.stop();
}
void makeIFTTTRequest3() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");}
  Serial.println();
  if(!!!client.connected()) {
     Serial.println("Failed to connect, going back to sleep");}

  Serial.print("Request resource: "); 
  Serial.println(resource3);
  client.print(String("GET ") + resource3 + 
                  " HTTP/1.1\r\n" +
                  "Host: " + server + "\r\n" + 
                  "Connection: close\r\n\r\n");
                  
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);}
  if(!!!client.available()) {
     Serial.println("No response, going back to sleep");}
  while(client.available()){
    Serial.write(client.read());
  }
  Serial.println("HUMEDAD EMAIL ENVIADO");
  client.stop();
}
