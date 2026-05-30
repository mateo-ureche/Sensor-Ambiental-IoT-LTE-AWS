#include <Wire.h>
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Wire.begin(1, 42);
  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 no encontrado");
    while(1);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  Serial.println("BMP280 OK");
}

void loop() {
  delay(2000);
  Serial.printf("Presion: %.2f hPa  Temp: %.2f C\n",
    bmp.readPressure() / 100.0F,
    bmp.readTemperature());
}
