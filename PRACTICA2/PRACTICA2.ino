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
#define AIO_KEY         "aio_JJuZ61h7nkbiu18kGo8KlMHbUWFM"

/************************* TEMPERATURE *********************************/

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2
#define ledPin 16

#define DHTTYPE    DHT11 
DHT_Unified dht(DHTPIN, DHTTYPE);

//tiempo de espera
unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;
unsigned long tiempo3 = 10;
unsigned long tiempoSegundos = 0;
double temperatura= 0.0;

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temp_value = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/TEMPERATURE", MQTT_QOS_1);
Adafruit_MQTT_Subscribe led_control= Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED_CONTROL", MQTT_QOS_1);

uint16_t ledBrightValue = 0;

void MQTT_connect();

/************************* IFTTT *********************************/
const char* resource1 = "/trigger/TemperaturaIFTTT/with/key/b0EEUKFFRwk8y5pG74tbua";
const char* resource2 = "/trigger/TemperaturaEmail/with/key/b0EEUKFFRwk8y5pG74tbua";
const char* server = "maker.ifttt.com";



void setup() {
  Serial.begin(115200);
  delay(10);
  
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
  //tiempo
  tiempo1=millis();
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
  tiempo2 = millis();
  if(tiempo2 > (tiempo1+1000)){  //Si ha pasado 1 segundo ejecuta el IF
    tiempo1 = millis(); //Actualiza el tiempo actual
    tiempoSegundos = tiempo1/1000;
    while((tiempo1/1000)==tiempo3){
      tiempo3=tiempo3+10;
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if(isnan(event.temperature)){
        Serial.print("Error al leer los datos");
      }
      else{
        Serial.print("Temperature");
        Serial.println("°C");
        if(!temp_value.publish(event.temperature)){
          Serial.println("Error de publicacion");
        }else{
          Serial.print(event.temperature);
          Serial.println("OK!");
          if(((event.temperature)>=25.0) && ((event.temperature)<=28.0)){
            makeIFTTTRequest1();
          }
          if((event.temperature)>=30.0){
            makeIFTTTRequest2();
          }
        }
      }
    }
  }
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
  
  Serial.println("\nclosing connection");
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
  
  Serial.println("\nclosing connection");
  client.stop();
}
