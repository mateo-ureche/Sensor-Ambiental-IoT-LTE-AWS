#include <Wire.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Wire.begin(1, 2);
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 no encontrado");
    while (1);
  }
  Serial.println("BMP280 OK");
}

void loop() {
  float p = bmp.readPressure() / 100.0F;
  float t = bmp.readTemperature();
  Serial.print("Presion: "); Serial.print(p);
  Serial.print(" hPa  Temp: "); Serial.print(t);
  Serial.println(" C");
  delay(2000);
}
