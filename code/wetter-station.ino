//Librarys
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <BME280I2C.h>
#include <Fonts/FreeMonoBold12pt7b.h>

//BME688
Adafruit_BME680 bme68;

//BME280
BME280I2C::Settings settings(
  BME280::OSR_X2, BME280::OSR_X16, 
  BME280::OSR_X8, BME280::Mode_Normal, 
  BME280::StandbyTime_125ms, BME280::Filter_16, 
  BME280::SpiEnable_False, BME280I2C::I2CAddr_0x76);
BME280I2C bme28(settings);

//E-Paper-Display
#define EPAPER_CS D1
#define EPAPER_DC D3
#define EPAPER_RST D0
#define EPAPER_BUSY D2
GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(/*CS=5*/ EPAPER_CS, /*DC=*/ EPAPER_DC, /*RES=*/ EPAPER_RST, /*BUSY=*/ EPAPER_BUSY)); // DEPG0290BS 128x296, SSD1680

// Globale variablen
RTC_DATA_ATTR float temp,   hum,   pres,   gas,   alt;
RTC_DATA_ATTR float lastTemp, lastHum, lastPres, lastGas, lastAlt;
float temp68, hum68, pres68, gas68, alt68;
float temp28, hum28, pres28;

// Makro für Serial-Ausgaben
#define SERIAL_PRINT(...)      do { if (Serial) { Serial.print(__VA_ARGS__); } } while (0)
#define SERIAL_PRINTLN(...)    do { if (Serial) { Serial.println(__VA_ARGS__); } } while (0)

// Deep Sleep
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  60 * 15     // Time ESP32 will go to sleep (in seconds)
RTC_DATA_ATTR int bootCount = 0;
#define BUTTON_PIN_BITMASK 0x100   // 2^8 in hex

void setup () {
  try { Serial.begin(115200); } catch (...){}

  Wire.begin();

  // Initialize BME688
  try {bme68.begin();} 
  catch (...){ SERIAL_PRINTLN("Could not find BME688 sensor!"); }
  // Setze die Sensorparameter für den BME688
  bme68.setTemperatureOversampling(BME680_OS_16X);
  bme68.setHumidityOversampling(BME680_OS_1X);
  bme68.setPressureOversampling(BME680_OS_16X);
  bme68.setIIRFilterSize(BME680_FILTER_SIZE_127);
  bme68.setGasHeater(320, 150); // 320°C für 150 ms

  // Initialize BME280
  try {bme28.begin();} 
  catch (...){ SERIAL_PRINTLN("Could not find BME280 sensor!"); }

  // E-Paper-Display initialisieren
  display.init(115200, false /* serial Kommunikation */, 100, true /* Reset-Pin */);
  display.setRotation(3);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold12pt7b);
}

void loop () {
  readSensor688();
  readSensor280();
  calculateValues();
  printValues();
  updateEPD();

  // delay(10000); //debug mode
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0); //1 = High, 0 = Low

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void updateEPD () {
  //Generiere text
  String tempStr = "Temp:  " + String(temp) + "C     " + getTrendSymbol(temp, lastTemp);
  String presStr = (pres < 1000.0) ? "Pres: " + String(pres) + "hPa   " + getTrendSymbol(pres, lastPres) : "Pres:"  + String(pres) + "hPa   " + getTrendSymbol(pres, lastPres);
  String humStr = "Hum:   " + String(hum) + "%     " + getTrendSymbol(hum, lastHum);
  String gasStr = (gas < 10.0) ? "Gas:    " + String(gas) + "KOhms " + getTrendSymbol(gas, lastGas) : (gas < 100.0) ? "Gas:   " + String(gas) + "KOhms " + getTrendSymbol(gas, lastGas) : "Gas:  " + String(gas) + "KOhms " + getTrendSymbol(gas, lastGas);
  String altStr = (alt < 100.0) ? "Alt:   " + String(alt) + "m     " + getTrendSymbol(alt, lastAlt) : "Alt:  " + String(alt) + "m     " + getTrendSymbol(alt, lastAlt);

  String data[] = {tempStr, presStr, humStr, gasStr, altStr};
  int yPos[] = {30, 50, 70, 90, 110};

  display.fillScreen(GxEPD_WHITE);

  for (int i = 0; i < 5; i++) {
    display.setCursor(15, yPos[i]);
    display.print(data[i]);
  }
  
  if (bootCount % 48 == 0) { display.display(); } 
  else {display.display(true);}
  ++bootCount;
}

void readSensor688 () {
  if (! bme68.performReading()) {
    SERIAL_PRINTLN("Failed to perform reading on BME688");
    temp68 = hum68 = pres68 = gas68 = alt68 = NAN;
    return;
  }

  temp68 = bme68.temperature;
  hum68 = bme68.humidity;
  pres68 = bme68.pressure / 100.0;
  gas68 = bme68.gas_resistance / 1000.0;
  alt68 = bme68.readAltitude(1013.25);
}

void readSensor280 () {
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  try { bme28.read(pres28, temp28, hum28, tempUnit, presUnit); }
  catch (...) {
    SERIAL_PRINTLN("Failed to perform reading on BME280");
    temp28 = hum28 = pres28 = NAN;
  }
}

void printValues () {
  //print BME688
  SERIAL_PRINT("Sensor BME688: Temp = " + String(temp68) + " *C\t");
  SERIAL_PRINT("Pres = " + String(pres68) + " hPa\t");
  SERIAL_PRINT("Hum = " + String(hum68) + " %\t");
  SERIAL_PRINT("Gas = " + String(gas68) + " KOhms\t");
  SERIAL_PRINTLN("Alt = " + String(alt68) + " m");

  //print BME280
  SERIAL_PRINT("Sensor BME280: Temp = " + String(temp28) + " *C\t");
  SERIAL_PRINT("Pres = " + String(pres28) + " hPa\t");
  SERIAL_PRINT("Hum = " + String(hum28) + " %");

  SERIAL_PRINTLN();
}

void calculateValues () {
  // Store previous values
  lastTemp = temp, lastHum = hum, lastPres = pres, lastGas = gas, lastAlt = alt;

  // Durchschnittstemperatur berechnen
  temp = !isnan(temp68) ? (isnan(temp28) ? temp68 : (temp68 + temp28) / 2.0) : temp28;
  temp = temp - 6; // eigene kalibrierung

  // Durchschnittsfeuchtigkeit berechnen   deaktiviert
  // temp = !isnan(hum68) ? (isnan(hum28) ? hum68 : (hum68 + hum28) / 2.0) : hum28;
  hum = hum68;

  // Durchschnittsdruck berechnen
  pres = !isnan(pres68) ? (isnan(pres28) ? pres68 : (pres68 + pres28) / 2.0) : pres28;

  // Gaswert und Höhe sind nur vom BME688 verfügbar
  gas = gas68;
  alt = alt68;
}

String getTrendSymbol (float current, float previous) {
  if (isnan(previous)) return "";  // No previous value
  return (current > previous) ? "+" : ((current < previous) ? "-" : "=");
}
