# Wetterstation mit Arduino

![Wetterstation Preview](link-zu-deinem-bild.jpg)

Dieses Arduino-Projekt implementiert eine Wetterstation, die die Sensoren BME680 und BME280 verwendet, um verschiedene Umweltparameter wie Temperatur, Luftfeuchtigkeit, Luftdruck und Gaswiderstand zu messen. Die gemessenen Daten werden auf einem E-Paper-Display angezeigt.

## Funktionen und Komponenten

### Verwendete Hardware

- **BME680 Sensor**: Ein Umweltsensor, der Temperatur, Luftfeuchtigkeit, Luftdruck und Gaswiderstand misst.
- **BME280 Sensor**: Ein weiterer Umweltsensor, der Temperatur, Luftfeuchtigkeit und Luftdruck misst.
- **E-Paper-Display**: GxEPD2 Library zur Ansteuerung eines E-Paper-Displays mit SSD1680 Controller.
- **Arduino Board**: Verwendet wird ein Arduino Mikrocontroller Board, z.B. Arduino Uno oder vergleichbar.

### Verwendete Bibliotheken

- **Wire.h**: Standard Arduino Bibliothek für die I2C-Kommunikation.
- **Adafruit_Sensor.h**: Adafruit Sensor Bibliothek für die Sensoransteuerung.
- **Adafruit_BME680.h**: Adafruit Bibliothek für den BME680 Sensor.
- **Adafruit_BME280.h**: Adafruit Bibliothek für den BME280 Sensor.
- **GxEPD2_BW.h**: GxEPD2 Bibliothek für die Ansteuerung von E-Paper-Displays.
- **Adafruit_GFX.h**: Adafruit Grafikbibliothek für die Anzeige auf dem E-Paper-Display.
- **BME280I2C.h**: Bibliothek für die I2C-Kommunikation mit dem BME280 Sensor.
- **Fonts/FreeMonoBold9pt7b.h**: Schriftart für die Anzeige auf dem E-Paper-Display.

### Installation

1. **Arduino IDE**: Stelle sicher, dass die Arduino Integrated Development Environment (IDE) installiert ist.
2. **Bibliotheken**: Installiere die oben aufgeführten Bibliotheken in der Arduino IDE über den Bibliotheksmanager.

### Schaltungsaufbau

Verwende folgende Verdrahtungsanweisungen für den Aufbau der Wetterstation:

- **BME680 Sensor**:
  - SDA an D4
  - SCL an D5

- **BME280 Sensor**: 
  - Verbinde den Sensor entsprechend der I2C-Spezifikation.

- **E-Paper-Display**: 
  - CS an D1
  - DC an D3
  - RST an D0
  - BUSY an D2

### 3D-Druck für Gehäuse

Das Gehäuse für die Wetterstation kann über folgenden Link heruntergeladen werden: [Link zur 3D-Druck Datei](link-zur-3d-druck-datei.stl).

## Lizenz
Dieses Projekt ist unter der [Creative Commons Attribution-NonCommercial 4.0 International License](https://creativecommons.org/licenses/by-nc/4.0/) lizenziert. Das bedeutet, dass du das Material teilen und bearbeiten kannst, solange du es nicht für kommerzielle Zwecke nutzt und mich als Urheber angibst.

---
Autor: [Jakob](https://github.com/jakobhaid)
