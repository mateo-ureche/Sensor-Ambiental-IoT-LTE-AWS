void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("ESP32-S3 funcionando correctamente");
}

void loop() {
  Serial.println("Latido: " + String(millis()) + " ms");
  delay(1000);
}
