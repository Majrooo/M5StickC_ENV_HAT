#include <Arduino.h>
// Test for M5Stack ENV HAT, showing temperature and humidity from DHT12 sensor.
// there is also output from JSON to serial port


#include <M5StickC.h>
#include "DHT12.h"
#include <Wire.h> //The DHT12 uses I2C comunication.
#include <Chrono.h>
Chrono chronoDHT12(Chrono::SECONDS);
#define chronoDHT12_seconds 1
Chrono chrono250;

DHT12 dht12(0, 0x5c); // Preset scale CELSIUS and ID 0x5c.
#include <ArduinoJson.h>

uint8_t led_count = 10;

void drawBatteryInfo();
void readDHT12();

void setup()
{
  Serial.begin(115200);
  M5.begin();
  Wire.begin(0, 26);
  M5.Lcd.setRotation(3);
  M5.Axp.ScreenBreath(led_count);
  M5.Lcd.println("ENV Unit test...");
  M5.Lcd.setTextColor(WHITE, BLACK);
}

void loop()
{
  if (chronoDHT12.hasPassed(chronoDHT12_seconds))
  { // returns true if it passed defined millis since it was started
    readDHT12();
    chronoDHT12.restart(); // restart the crono so that it triggers again later
  }
  if (chrono250.hasPassed(250))
  { // returns true if it passed defined millis since it was started
    drawBatteryInfo();
    chrono250.restart(); // restart the crono so that it triggers again later
  }

  if (digitalRead(M5_BUTTON_HOME) == LOW)
  {
    led_count++;
    if (led_count >= 13)
      led_count = 7;
    while (digitalRead(M5_BUTTON_HOME) == LOW)
     M5.Axp.ScreenBreath(led_count);
  }
}
void drawBatteryInfo()
{
  M5.Lcd.setCursor(0, 0, 2);
  M5.Lcd.setTextSize(1);
  if ((M5.Axp.GetVapsData() * 1.4 / 1000) > 4.5)
  {
    M5.Lcd.setTextColor(RED, BLACK);
  }
  else
  {
    M5.Lcd.setTextColor(WHITE, BLACK);
  }

  M5.Lcd.printf("vbat : %.2fV\r\n", M5.Axp.GetVbatData() * 1.1 / 1000);
}

void readDHT12()
{
  // bool chk = dht12.read(); // true read is ok, false read problem
  float tmp = dht12.readTemperature();
  float hum = dht12.readHumidity();
  DynamicJsonDocument doc(200);
  doc["Temperature"] = tmp;
  doc["Humidity"] = hum;
  serializeJson(doc, Serial);
  Serial.println();
  M5.Lcd.setCursor(0, 15, 4);
  M5.Lcd.setTextColor(WHITE, BLACK);
  // float pressure = bme.readPressure();
  // Serial.printf("Temperatura: %2.2f*C  Humedad: %0.2f%%  Pressure: %0.2fPa\r\n", tmp, hum, pressure);
  // Serial.printf("Temperature: %2.2f°C  Humidity: %0.2f%%\r\n", tmp, hum);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPressure:%2.0fPa\r\n", tmp, hum, pressure);
  M5.Lcd.printf("T: %2.1f °C  \r\nH: %2.1f %%\r\n", tmp, hum);
}
