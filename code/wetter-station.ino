//Librarys
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>
#include <BME280I2C.h>

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeMono12pt7b.h>

//BME688
#define BME_SDA D4
#define BME_SCL D5
Adafruit_BME680 bme68;

//BME280
BME280I2C bme28;

//E-Paper-Display
#define EPAPER_CS D1
#define EPAPER_DC D3
#define EPAPER_RST D0
#define EPAPER_BUSY D2 //D5
GxEPD2_BW<GxEPD2_290_BS, GxEPD2_290_BS::HEIGHT> display(GxEPD2_290_BS(/*CS=5*/ EPAPER_CS, /*DC=*/ EPAPER_DC, /*RES=*/ EPAPER_RST, /*BUSY=*/ EPAPER_BUSY)); // DEPG0290BS 128x296, SSD1680

// Globale variablen
float temp(NAN),     hum(NAN),     pres(NAN),     gas(NAN),     alt(NAN);
// float lastTemp(NAN), lastHum(NAN), lastPres(NAN), lastGas(NAN), lastAlt(NAN);
float temp68(NAN),   hum68(NAN),   pres68(NAN),   gas68(NAN),   alt68(NAN);
float temp28(NAN),   hum28(NAN),   pres28(NAN);

// Makro für Serial-Ausgaben
#define SERIAL_DEBUG_BEGIN()   do { if (Serial) { Serial.begin(9600); while (!Serial); } } while (0)
#define SERIAL_PRINT(...)      do { if (Serial) { Serial.print(__VA_ARGS__); } } while (0)
#define SERIAL_PRINTLN(...)    do { if (Serial) { Serial.println(__VA_ARGS__); } } while (0)

void setup() {
  delay(1000);
  SERIAL_DEBUG_BEGIN();

  Wire.begin();

  // BME688 initialisieren
  while (!bme68.begin()) {  // Standard I2C Adresse 0x76
    SERIAL_PRINTLN("Could not find BME688 sensor!");
    delay(1000);
  }

  // BME280 initialisieren
  while (!bme28.begin()){
    SERIAL_PRINTLN("Could not find BME280 sensor!");
    delay(1000);
  }

  // Setze die Sensorparameter für den BME688
  bme68.setTemperatureOversampling(BME680_OS_8X);
  bme68.setHumidityOversampling(BME680_OS_2X);
  bme68.setPressureOversampling(BME680_OS_4X);
  bme68.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme68.setGasHeater(320, 150); // 320°C für 150 ms

  // E-Paper-Display initialisieren
  display.init(9600, false /* serial Kommunikation */, 100, true /* Reset-Pin */);
  display.setRotation(1); // 0 oder 1 für Portrait, 2 oder 3 für Landscape
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMono12pt7b);
}

void loop() {
  readSensor688();
  readSensor280();
  calculateValues();
  printValues();
  displayValues();

  delay(600000);
}

void readSensor688(){
  if (! bme68.performReading()) {
    SERIAL_PRINTLN("Failed to perform reading on BME688");
    delay(200);
    return;
  } else {
    temp68 = bme68.temperature;
    hum68 = bme68.humidity;
    pres68 = (bme68.pressure / 100.0);
    gas68 = (bme68.gas_resistance / 1000.0);
    alt68 = (bme68.readAltitude(1013.25));
  }
}

void readSensor280(){
  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  try {
    bme28.read(pres28, temp28, hum28, tempUnit, presUnit);
  } catch(...) {
    SERIAL_PRINTLN("Failed to perform reading on BME280");
  }
}

void printValues() {
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

void calculateValues(){
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

  // Durchschnittsfeuchtigkeit berechnen
  if (!isnan(hum68) && !isnan(hum28)) {
    hum = (hum68 + hum28) / 2.0;
  } else if (!isnan(hum68)) {
    hum = hum68;
  } else if (!isnan(hum28)) {
    hum = hum28;
  } else {
    hum = NAN;
  }

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

void displayValues(){
  //display.setFullWindow();
  display.fillScreen(GxEPD_WHITE);

  display.setCursor(35, 20);
  display.print("Temp:  " + (String)bme68.temperature + "C°");

  display.setCursor(35, 40);
  if((bme68.pressure / 100.0) < 1000){
    display.print("Press:  " + (String)(bme68.pressure / 100.0) + "hPa");
  }else{
    display.print("Press: " + (String)(bme68.pressure / 100.0) + "hPa");
  }

  display.setCursor(35, 60);
  display.print("Hum:   " + (String)bme68.humidity + "%");

  display.setCursor(35, 80);
  display.print("Gas:   " + (String)(bme68.gas_resistance / 1000.0) + "KOhms");

  display.display();
}
