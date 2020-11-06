
// Hardware setup ...

#include "configuration.h"

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <RTClib.h>
#include <Wire.h>
#include <SSD1306Wire.h>
#include "TheLogger.h"

#include "utils.h"
#include "CO2Sensor/ICo2Sensor.h"
#include "LedStrip/LedStrip.h"

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
RTC_DS1307 rtc;
TheLogger logger;

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  while (!Serial)
    ;
  co2Sensor.begin();
  display.init();
  display.flipScreenVertically();
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }
  logger.begin(SD_SCK_MHZ(50));
  Serial.println("Heating");
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 14, "Heating");
  display.display();

  ledStrip.begin();
  auto checkSecond = CheckTimeSpanPassed(1000, false);
  while (!co2Sensor.isReady())
  {
    if (checkSecond())
      Serial.print(".");
    ledStrip.go(LedStrip::heating);
    delay(20); // only here ... nowhere else
  }
  Serial.println("Start");
}

char g_currentTimestamp[20]{}; // YYYY/MM/DD HH:MM:SS

DateTime now;
uint16_t ppm{};

void loop()
{
  static auto everySecond = CheckTimeSpanPassed(1000, true);
  static auto every10Seconds = CheckTimeSpanPassed(10000, true);
  static auto every20Millis = CheckTimeSpanPassed(20, false);
  
  if (every20Millis())
  {
    ledStrip.setPpm(ppm);
    ledStrip.go();
  }
  if (everySecond())
  {
    if (ppm == 0)
      return; // initial state ... wait for the first read;
    now = rtc.now();
    snprintf(g_currentTimestamp, sizeof(g_currentTimestamp), "%d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 2, g_currentTimestamp);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 14, String(ppm) + " ppm");
    display.display();
  }
  if (every10Seconds())
  {
    ppm = co2Sensor.getCo2();
    logger.log(String(ppm));
    Serial.println(ppm);
  }
}
