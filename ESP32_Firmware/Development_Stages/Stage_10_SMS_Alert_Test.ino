HardwareSerial sim(1);

void setup() {
  Serial.begin(115200);
  delay(3000);
  sim.begin(115200, SERIAL_8N1, 17, 18);
  delay(3000);
  sim.println("AT+CMGF=1");
  delay(2000);
  while (sim.available()) Serial.write(sim.read());
  sim.println("AT+CMGS=\"3023015100\"");
  delay(2000);
  while (sim.available()) Serial.write(sim.read());
  sim.print("ALERTA SENSOR: Temperatura alta 36.2C | Hum: 60%");
  sim.write(26);
  delay(10000);
  while (sim.available()) Serial.write(sim.read());
  Serial.println("SMS enviado");
}
void loop() {}
