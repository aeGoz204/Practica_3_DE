/****************************************
 * Libraries
 ****************************************/
#include <UbidotsEsp32Mqtt.h>
#include "DHT.h"
#include <TFT_eSPI.h>
#include <SPI.h>

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBUS-Dcrq35CuJZHXkBgjCxZ84sUCGg8fjK";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "TIGO-357A";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "2NB112101810";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL2 = "Humedad"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL1 = "Temp";

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;
#define DATAPIN 27
#define DHTTYPE DHT11
#define uS_TO_S_FACTOR 1000000ULL 
#define TIME_TO_SLEEP  5  

Ubidots ubidots(UBIDOTS_TOKEN);
DHT dht (DATAPIN, DHTTYPE);
TFT_eSPI tft = TFT_eSPI();

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void wakeUpFuction (){
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  dht.begin();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)){
    Serial.print(F("Error Sensor"));
    return;
  }
  tft.setTextColor(TFT_WHITE);
  tft.drawString( String(h, 1), 5, 55 , 6);
  tft.drawString( String(t, 1), 5, 170, 6);
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  else
  {
    ubidots.add(VARIABLE_LABEL1, t); // Insert your variable Labels and the value to be sent
    ubidots.add(VARIABLE_LABEL2, h);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
  delay(1000);
  tft.setTextColor(TFT_BLACK);
  tft.drawString( String(h, 1), 5, 55 , 6);
  tft.drawString( String(t, 1), 5, 170, 6);
}

/****************************************
 * Main Functions
 ****************************************/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  int x = 22, y = 85;
  tft.init();
  tft.fillScreen(TFT_BLACK);
  delay(100);
  tft.drawString(F("Humedad:"), 5, 20,  4);
  tft.drawString(F("Temp.:"),   5, 135, 4);
  tft.drawString(F("%"),      100, 55 , 4);
  tft.drawString(F("`C"),     100, 170, 4);
  wakeUpFuction();
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.flush();
  esp_deep_sleep_start();
  
}

void loop() {
  // put your main code here, to run repeatedly:
}
