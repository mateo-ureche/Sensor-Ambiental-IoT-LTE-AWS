#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
HardwareSerial sim(1);
TinyGsm modem(sim);

void setup() {
  Serial.begin(115200);
  delay(3000);
  sim.begin(115200, SERIAL_8N1, 17, 18);
  modem.restart();
  delay(5000);
  sim.println("AT+CGNSSPWR=1");
  delay(2000);
  while (sim.available()) Serial.write(sim.read());
  Serial.println("GPS encendido. Ve al exterior y espera 30-90 segundos...");
}

void loop() {
  sim.println("AT+CGNSSINFO");
  String resp = "";
  long t = millis();
  while (millis() - t < 3000)
    while (sim.available()) resp += (char)sim.read();
  Serial.println(resp);
  delay(5000);
}
