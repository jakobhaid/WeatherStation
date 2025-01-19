# Wetterstation mit Arduino
<img src="../images/assembly/bild1.jpg" width="40%">   <img src="../images/assembly/bild2.jpg" width="40%">

Dieses Arduino-Projekt implementiert eine Wetterstation, die die Sensoren BME680 und BME280 verwendet, um verschiedene Umweltparameter wie Temperatur, Luftfeuchtigkeit, Luftdruck und Gaswiderstand zu messen. Die gemessenen Daten werden auf einem E-Paper-Display angezeigt.

## Komponenten

### Verwendete Hardware
| name | description | Image |
| --- | --- | --- |
| **BME688 Sensor**  | Ein Umweltsensor, der Temperatur, Luftfeuchtigkeit, Luftdruck und Gaswiderstand misst.  |<img src="../images/components/bme688.jpg" height="70px">|
| **BME280 Sensor**  | Ein weiterer Umweltsensor, der Temperatur, Luftfeuchtigkeit und Luftdruck misst.  |<img src="../images/components/bme280.jpg" height="70px">|
| **E-Paper Display**  | GxEPD2 Library zur Ansteuerung eines E-Paper-Displays mit SSD1680 Controller.  |<img src="../images/components/e-paper.jpg" height="70px">|
| **ESP32**  | Seeed Studio XIAO ESP32S3  |<img src="../images/components/esp32.jpg" height="70px">|
| **Battery Shield**  | Battery Shield für eine 18650 Battery.  |<img src="../images/components/battery-shield.jpg" height="70px">|


### Verwendete Bibliotheken
- **Wire.h**: Standard Arduino Bibliothek für die I2C-Kommunikation.
- **Adafruit_Sensor.h**: Adafruit Sensor Bibliothek für die Sensoransteuerung.
- **Adafruit_BME680.h**: Adafruit Bibliothek für den BME680 Sensor.
- **GxEPD2_BW.h**: GxEPD2 Bibliothek für die Ansteuerung von E-Paper-Displays.
- **Adafruit_GFX.h**: Adafruit Grafikbibliothek für die Anzeige auf dem E-Paper-Display.
- **BME280I2C.h**: Bibliothek für die I2C-Kommunikation mit dem BME280 Sensor.
- **Fonts/FreeMonoBold12pt7b.h**: Schriftart für die Anzeige auf dem E-Paper-Display.


### Installation
1. **Arduino IDE**: Stelle sicher, dass die Arduino Integrated Development Environment (IDE) installiert ist.
2. **Bibliotheken**: Installiere die oben aufgeführten Bibliotheken in der Arduino IDE über den Bibliotheksmanager.


## Schaltungsaufbau
<img src="../images/wire-diagram.png" width="30%">


## 3D-Druck für Gehäuse
Das Gehäuse für die Wetterstation kann über folgenden Link heruntergeladen werden: [Link zur 3D-Druck Datei](3dmodels).
<img src="../images/components/gehaeuse.png" width="50%">


## Lizenz
Dieses Projekt ist unter der [Creative Commons Attribution-NonCommercial 4.0 International License](https://creativecommons.org/licenses/by-nc/4.0/) lizenziert. Das bedeutet, dass du das Material teilen und bearbeiten kannst, solange du es nicht für kommerzielle Zwecke nutzt und mich als Urheber angibst.

---
Autor: [Instagram](https://www.instagram.com/j.kob.h/)   [Youtube](https://www.youtube.com/@Jkobh)
