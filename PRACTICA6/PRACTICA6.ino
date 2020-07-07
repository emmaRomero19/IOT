#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define WLAN_SSID       "LAPTOP-DI4IE39T 3857"
#define WLAN_PASS       "Ha656072"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "emmaRomero"
#define AIO_KEY         "aio_tCmP005J8b4sbnM7r4kbwGJaEDxU"

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
//Server for susbscribe to changes
Adafruit_MQTT_Subscribe calendario= Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/calendar", MQTT_QOS_1);

//RGB
#define REDPin   5
#define GREENPin 4
#define BLUEPin  2

int even1=0;
int even2=0;
int even3=0;

int e1IH=0;
int e1IM=0;
int e1FH=0;
int e1FM=0;

int e2IH=0;
int e2IM=0;
int e2FH=0;
int e2FM=0;

int e3IH=0;
int e3IM=0;
int e3FH=0;
int e3FM=0;

const long utcOffsetInSeconds = -18000;
char daysOfTheWeek[7][12] = {"Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup()
{
  Serial.begin(115200);
  pinMode(REDPin, OUTPUT);
  pinMode(GREENPin, OUTPUT);
  pinMode(BLUEPin, OUTPUT);
  Serial.println("Conected to: ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Conected");
  Serial.println("IP Address");
  Serial.println(WiFi.localIP());

  //SETUP Connection to subscription
  mqtt.subscribe(&calendario);
  timeClient.begin();
}

void loop()
{
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription1;
  while (subscription1 = mqtt.readSubscription(1000))
  Serial.println("");
  Serial.write("");
  Serial.println(F("Obtenido: "));
  String Date = (char *)calendario.lastread;
  Serial.println(Date);
  Serial.println("");

  //INICIO DEL EVENTO
  Serial.println("Inicio del evento");
  Serial.println("Fecha: ");
  String Iyear=(Date.substring(0, 4));
  String Imon=(Date.substring(5, 7)); 
  String Iday = (Date.substring(8, 11));
  int Idia = Iday.toInt();
  Serial.print(Iyear+"-"+Imon+"-"+Idia);

  Serial.println("Hora: ");
  Serial.print(Date.substring(11, 19));
  String ihours = (Date.substring(11, 13));
  int IHours = ihours.toInt();
  String iminutes = (Date.substring(14, 16));
  int IMinutes = iminutes.toInt();
  
  Serial.println("");

  //FIN DEL EVENTO
  Serial.println("Fin del evento");
  Serial.println("Fecha: ");
  String Fyear=(Date.substring(25, 29));
  String Fmon=(Date.substring(30, 32));
  String Fday = (Date.substring(33, 35));
  int Fdia = Fday.toInt();
  Serial.print(Fyear+"-"+Fmon+"-"+Fdia);
  
  Serial.println("Hora: ");
  Serial.println(Date.substring(36, 44));
  String fhours = (Date.substring(36, 38));
  int FHours = fhours.toInt();
  String fminutes = (Date.substring(39, 41));
  int FMinutes = fminutes.toInt();
  
  Serial.println("");
  Serial.println("Hora actual:");
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  int Hours = (timeClient.getHours());
  Serial.print(Hours);
  Serial.print(":");
  int Minutes = (timeClient.getMinutes());
  Serial.print(Minutes);
  Serial.print(":");
  int seg = (timeClient.getSeconds());
  Serial.print(seg);
  Serial.println("");
  
  
  if(even1==0){
    e1IH=IHours;
    e1IM=IMinutes;
    e1FH=FHours;
    e1FM=FMinutes;
    Serial.println("evento1");
    if (Hours >= e1IH && Hours <= e1FH && Minutes >= e1IM && Minutes <= e1FM){
      even1=1;
      int RED = 250;
      int GREEN = 0;
      int BLUE = 0;
      analogWrite(REDPin, RED);
      analogWrite(GREENPin, GREEN);
      analogWrite(BLUEPin, BLUE);
      Serial.println("ON Event1");
    }
    if(e2IH==e1IH && e2IM==e1IM && e2FH==e1FH && e2FM==e1FM){
      int RED = 0;
      int GREEN = 250;
      int BLUE = 0;
      analogWrite(REDPin, RED);
      analogWrite(GREENPin, GREEN);
      analogWrite(BLUEPin, BLUE);
      Serial.println("ON Event 2");
      even2=1;
    }
  }  
  if (Minutes > e1FM || Hours > e1FH ){
    int RED = 0;
    int GREEN = 0;
    int BLUE = 0;
    analogWrite(REDPin, RED);
    analogWrite(GREENPin, GREEN);
    analogWrite(BLUEPin, BLUE);
    Serial.println("OFF Event 1");
    e1IH=0;
    e1IM=0;
    e1FH=0;
    e1FM=0;
    even1=0;
    }
  if(even2==0){
    e2IH=IHours;
    e2IM=IMinutes;
    e2FH=FHours;
    e2FM=FMinutes;
    if(e2IH!=e1IH && e2IM!=e1IM && e2FH!=e1FH && e2FM!=e1FM){
      Serial.println("evento2");
      if (Hours >= e2IH && Hours <= e2FH){
        if (Minutes >= e2IM && Minutes <= e2FM){
          int RED = 0;
          int GREEN = 250;
          int BLUE = 0;
          analogWrite(REDPin, RED);
          analogWrite(GREENPin, GREEN);
          analogWrite(BLUEPin, BLUE);
          Serial.println("ON Event 2");
          even2=1;
        }
      }
    }
  }
  if (Minutes > e2FM || Hours > e2FH){
    int RED = 0;
    int GREEN = 0;
    int BLUE = 0;
    analogWrite(REDPin, RED);
    analogWrite(GREENPin, GREEN);
    analogWrite(BLUEPin, BLUE);
    Serial.println("OFF Event 2");
    e2IH=0;
    e2IM=0;
    e2FH=0;
    e2FM=0;
    even2=0;
    }
    Serial.println("Eventos");
  Serial.println(even1);
  Serial.println(even2);
  Serial.println(even3);
  Serial.println("e1");
  Serial.println(e1IH);
  Serial.println(e1IM);
  Serial.println(e1FH);
  Serial.println(e1FM);
  Serial.println("e2");
  Serial.println(e2IH);
  Serial.println(e2IM);
  Serial.println(e2FH);
  Serial.println(e2FM);
}

void MQTT_connect()
{
  int8_t ret;
  // STOP IF ALREADY CONNECTED.
  if (mqtt.connected())
  {
    return;
  }
  Serial.println("Conectando a MQTT... Un momento");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Reintentando conexion espere 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    Serial.println("Retry " + retries);
    if (retries == 0)
    {
      while (1)
        ;
    }
  }
  Serial.println("MQTT Conectado");
}
