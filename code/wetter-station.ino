//Librarys
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <BME280I2C.h>

#include <Fonts/FreeMonoBold12pt7b.h>
// #include <Fonts/FreeMono12pt7b.h>
// #include <Fonts/FreeMono9pt7b.h>

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
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60 * 15     /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0;
#define BUTTON_PIN_BITMASK 0x100 // 2^8 in hex

void setup() {
  try {Serial.begin(9600);
  }catch(...){}

  Wire.begin();

  // Initialize BME688
  while (!bme68.begin()) { // Standard I2C Adresse 0x76
    SERIAL_PRINTLN("Could not find BME688 sensor!");
    delay(5000);
  }

  // Initialize BME280
  while (!bme28.begin()){ // Standard I2C Adresse 0x77
    SERIAL_PRINTLN("Could not find BME280 sensor!");
    delay(5000);
  }

  // Setze die Sensorparameter für den BME688
  bme68.setTemperatureOversampling(BME680_OS_2X);
  bme68.setHumidityOversampling(BME680_OS_1X);
  bme68.setPressureOversampling(BME680_OS_16X);
  bme68.setIIRFilterSize(BME680_FILTER_SIZE_127);
  bme68.setGasHeater(320, 150); // 320°C für 150 ms

  // E-Paper-Display initialisieren
  display.init(9600, false /* serial Kommunikation */, 100, true /* Reset-Pin */);
  display.setRotation(1); // 0 oder 1 für Portrait, 2 oder 3 für Landscape
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold12pt7b);
}

void loop() {
  readSensor688();
  readSensor280();
  calculateValues();
  printValues();

  if (bootCount % 10 == 0){
    displayValuesFull();
    ++bootCount;
  }else {
    displayValuesPartiell();
    ++bootCount;
  }

  // delay(10000);
  // esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0); //1 = High, 0 = Low
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void readSensor688 (){
  if (! bme68.performReading()) {
    SERIAL_PRINTLN("Failed to perform reading on BME688");
    delay(200);
    temp68 = hum68 = pres68 = gas68 = alt68 = NAN;
    return;
  } else {
    temp68 = bme68.temperature;
    hum68 = bme68.humidity;
    pres68 = (bme68.pressure / 100.0);
    gas68 = (bme68.gas_resistance / 1000.0);
    alt68 = (bme68.readAltitude(1013.25));
  }
}

void readSensor280 (){
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  try {
    bme28.read(pres28, temp28, hum28, tempUnit, presUnit);
  } catch(...) {
    SERIAL_PRINTLN("Failed to perform reading on BME280");
    temp28 = hum28 = pres28 = NAN;
  }
}

void printValues () {
  //print BME688
  SERIAL_PRINT("Sensor BME688: Temp = ");
  SERIAL_PRINT(temp68);
  SERIAL_PRINT(" *C\t");
  
  SERIAL_PRINT("Pres = ");
  SERIAL_PRINT(pres68);
  SERIAL_PRINT(" hPa\t");

  SERIAL_PRINT("Hum = ");
  SERIAL_PRINT(hum68);
  SERIAL_PRINT(" %\t");

  SERIAL_PRINT("Gas = ");
  SERIAL_PRINT(gas68);
  SERIAL_PRINT(" KOhms\t");

  SERIAL_PRINT("Alt = ");
  SERIAL_PRINT(alt68);
  SERIAL_PRINTLN(" m");

  //print BME280
  SERIAL_PRINT("Sensor BME280: Temp = ");
  SERIAL_PRINT(temp28);
  SERIAL_PRINT(" *C\t");
  
  SERIAL_PRINT("Pres = ");
  SERIAL_PRINT(pres28);
  SERIAL_PRINT(" hPa\t");

  SERIAL_PRINT("Hum = ");
  SERIAL_PRINT(hum28);
  SERIAL_PRINTLN(" %");

  SERIAL_PRINTLN();
}

void calculateValues (){
  // Store previous values
  lastTemp = temp;
  lastHum = hum;
  lastPres = pres;
  lastGas = gas;
  lastAlt = alt;

  // Durchschnittstemperatur berechnen
  if (!isnan(temp68) && !isnan(temp28)) {
    temp = (temp68 + temp28) / 2.0;
  } else if (!isnan(temp68)) {
    temp = temp68;
  } else if (!isnan(temp28)) {
    temp = temp28;
  } else {
    temp = NAN;
  }

  /* Durchschnittsfeuchtigkeit berechnen   deaktiviert
  if (!isnan(hum68) && !isnan(hum28)) {
    hum = (hum68 + hum28) / 2.0;
  } else if (!isnan(hum68)) {
    hum = hum68;
  } else if (!isnan(hum28)) {
    hum = hum28;
  } else {
    hum = NAN;
  }*/
  hum = hum68;

  // Durchschnittsdruck berechnen
  if (!isnan(pres68) && !isnan(pres28)) {
    pres = (pres68 + pres28) / 2.0;
  } else if (!isnan(pres68)) {
    pres = pres68;
  } else if (!isnan(pres28)) {
    pres = pres28;
  } else {
    pres = NAN;
  }

  // Gaswert und Höhe sind nur vom BME688 verfügbar
  gas = gas68;
  alt = alt68;
}

void displayValuesPartiell (){
  // Calculate the update areas
  display.fillScreen(GxEPD_WHITE);
  int16_t x1, y1;
  uint16_t w, h;

  String tempStr = "Temp:  " + String(temp) + "C    " + getTrendSymbol(temp, lastTemp);
  String presStr = "";
  if(pres < 1000.0){
    presStr = "Pres:  " + String(pres) + "hPa " + getTrendSymbol(pres, lastPres);
  }else{
    presStr = "Pres: " + String(pres) + "hPa " + getTrendSymbol(pres, lastPres);
  }
  String humStr = "Hum:   " + String(hum) + "%    " + getTrendSymbol(hum, lastHum);
  String gasStr = "Gas:   " + String(gas) + "KOhms" + getTrendSymbol(gas, lastGas);
  String altStr = "Alt:   " + String(alt) + "m   " + getTrendSymbol(alt, lastAlt);

  display.getTextBounds(tempStr, 35, 20, &x1, &y1, &w, &h);
  display.fillRect(x1, y1, w, h, GxEPD_WHITE);
  display.setCursor(35, 20);
  display.print(tempStr);

  display.getTextBounds(presStr, 35, 40, &x1, &y1, &w, &h);
  display.fillRect(x1, y1, w, h, GxEPD_WHITE);
  display.setCursor(35, 40);
  display.print(presStr);

  display.getTextBounds(humStr, 35, 60, &x1, &y1, &w, &h);
  display.fillRect(x1, y1, w, h, GxEPD_WHITE);
  display.setCursor(35, 60);
  display.print(humStr);

  display.getTextBounds(gasStr, 35, 80, &x1, &y1, &w, &h);
  display.fillRect(x1, y1, w, h, GxEPD_WHITE);
  display.setCursor(35, 80);
  display.print(gasStr);

  display.getTextBounds(altStr, 35, 100, &x1, &y1, &w, &h);
  display.fillRect(x1, y1, w, h, GxEPD_WHITE);
  display.setCursor(35, 100);
  display.print(altStr);

  // Update display with partial update
  display.display(true); // partial update
}

void displayValuesFull (){
  //display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);

  display.setCursor(35, 20);
  display.print("Temp:  " + String(temp) + "C    " + getTrendSymbol(temp, lastTemp));

  display.setCursor(35, 40);
  if(pres < 1000.0){
    display.print("Pres:  " + String(pres) + "hPa " + getTrendSymbol(pres, lastPres));
  }else{
    display.print("Pres: " + String(pres) + "hPa " + getTrendSymbol(pres, lastPres));
  }

  display.setCursor(35, 60);
  display.print("Hum:   " + String(hum) + "%    " + getTrendSymbol(hum, lastHum));

  display.setCursor(35, 80);
  display.print("Gas:   " + String(gas) + "KOhms" + getTrendSymbol(gas, lastGas));

  display.setCursor(35, 100);
  display.print("Alt:   " + String(alt) + "m   " + getTrendSymbol(alt, lastAlt));

  display.display();
}

String getTrendSymbol (float current, float previous) {
  if (isnan(previous)) return "";  // No previous value
  return (current > previous) ? "+" : ((current < previous) ? "-" : "=");
}
