
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
#include "Co2Sensor.h"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
RTC_DS1307 rtc;
TheLogger logger;
MHZ19 mhz19;

auto blinkState = false;
uint32_t translate(uint16_t ppm)
{

  if (ppm < 700u)
  {
    if (ppm <= 400u)
      ppm = 400u;
    return Adafruit_NeoPixel::Color(0, 0xFF, 0);
  }
  else if (ppm < 1000u)
  {
    return Adafruit_NeoPixel::Color(0xFF, 0xFF, 0);
  }
  else if (ppm < 2000u)
  {
    return Adafruit_NeoPixel::Color(0xFF, 0, 0);
  }
  if (blinkState)
    return Adafruit_NeoPixel::Color(0xFF, 0, 0);
  return Adafruit_NeoPixel::Color(0, 0, 0);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);  
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
  CheckTimeSpanPassed blah(10000);
  CheckTimeSpanPassed checkSecond(1000);
  uint8_t col{1};
  bool direction{};
  strip.begin();
  strip.setBrightness(20);
  while (!co2Sensor.isReady())
  {
    if (checkSecond())
      Serial.print(".");
    if (col == 150 || col == 0)
      direction = !direction;
    strip.setPixelColor(0, Adafruit_NeoPixel::Color(0, 0, col));
    strip.show();
    direction ? col-- : col++;
    delay(10);
  }
  Serial.println("Start");
}

char g_currentTimestamp[20]{}; // YYYY/MM/DD HH:MM:SS

DateTime now;
uint16_t ppm{};

void loop()
{
  static CheckTimeSpanPassed everySecond(1000, true);
  static CheckTimeSpanPassed every10Seconds(10000,true);
  static CheckTimeSpanPassed every500Millis(500);

  if (every500Millis())
  {
    blinkState = !blinkState;
    strip.setPixelColor(0, translate(ppm));
    strip.show();
  }
  if (everySecond())
  {
    if(ppm == 0)
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
