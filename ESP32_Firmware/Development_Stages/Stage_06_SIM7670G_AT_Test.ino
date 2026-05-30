#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
HardwareSerial sim(1);
TinyGsm modem(sim);

void setup() {
  Serial.begin(115200);
  delay(3000);
  sim.begin(115200, SERIAL_8N1, 17, 18);
  Serial.println("Enviando AT...");
  sim.println("AT");
  delay(1000);
  while (sim.available()) Serial.write(sim.read());
  sim.println("ATI");
  delay(1000);
  while (sim.available()) Serial.write(sim.read());
  sim.println("AT+CSQ");
  delay(1000);
  while (sim.available()) Serial.write(sim.read());
}
void loop() {}
