#include <DHT.h>
#define DHTPIN 21
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  delay(3000);
  dht.begin();
  Serial.println("DHT22 iniciado");
}

void loop() {
  delay(2000);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("Error: DHT22 no responde");
    return;
  }
  Serial.print("Temp: "); Serial.print(t);
  Serial.print(" C  Hum: "); Serial.print(h);
  Serial.println(" %");
}
