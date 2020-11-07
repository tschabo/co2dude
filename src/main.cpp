
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
#include <limits>

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

const char CMD_SET_TIME[] = "set time ";
const char CMD_CALIBRATE[] = "calibrate";

// valid only for Types <= 16 bit
// returns the max of the type in case of failure
template <typename T>
T toUnsignedInteger(const char *begin, const char *end)
{
  if (end - begin > 5)
    return std::numeric_limits<T>::max();
  uint32_t result{};
  uint16_t multiplier{1};
  for (; end >= begin; end--, multiplier *= 10)
  {
    if (*end > '9' || *end < '0')
      return std::numeric_limits<T>::max();
    result += (*end - '0') * multiplier;
  }
  if (result < std::numeric_limits<T>::max())
    return result;
  return std::numeric_limits<T>::max();
}

void execCommand(const String &c)
{
  Serial.println(c);
  if (c.startsWith(CMD_SET_TIME))
  {
    if (c.length() != 28)
    {
      Serial.print("Syntax Error: ");
      Serial.println(c);
      Serial.println("format is YYYY/MM/DD HH/MM/SS");
      return;
    }

    const char *pYear = c.c_str() + 9;
    const char *pMonth = pYear + 5;
    const char *pDay = pMonth + 3;
    const char *pHour = pDay + 3;
    const char *pMinute = pHour + 3;
    const char *pSecond = pMinute + 3;

    DateTime dt(toUnsignedInteger<uint16_t>(pYear, pYear + 3),
                toUnsignedInteger<uint8_t>(pMonth, pMonth + 1),
                toUnsignedInteger<uint8_t>(pDay, pDay + 1),
                toUnsignedInteger<uint8_t>(pHour, pHour + 1),
                toUnsignedInteger<uint8_t>(pMinute, pMinute + 1),
                toUnsignedInteger<uint8_t>(pSecond, pSecond + 1));
    if(!dt.isValid())
    {
      Serial.println("timestamp not valid!");
      Serial.println(dt.toString("YYYY/MM/DD hh:mm:ss"));
      return;
    }
    rtc.adjust(dt);
    Serial.println("success");
    return;
  }
  else if(c == CMD_CALIBRATE)
  {
    co2Sensor.calibrate();
    return;
  }
  Serial.print("unknown conmmand: ");
  Serial.println(c);
}

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
  if (every10Seconds())
  {
    ppm = co2Sensor.getCo2();
    logger.log(String(ppm));
    Serial.println(ppm);
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
  if (Serial.available() > 0)
  {
    execCommand(Serial.readString());
  }
}
