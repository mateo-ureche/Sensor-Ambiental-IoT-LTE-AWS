#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
HardwareSerial sim(1);
TinyGsm modem(sim);
const char apn[] = "internet.comcel.com.co";

void setup() {
  Serial.begin(115200);
  delay(3000);
  sim.begin(115200, SERIAL_8N1, 17, 18);
  Serial.println("Reiniciando modem...");
  modem.restart();
  delay(8000);
  Serial.println("Conectando LTE...");
  if (modem.gprsConnect(apn, "", "")) {
    Serial.println("LTE OK");
    Serial.print("IP: ");
    Serial.println(modem.getLocalIP());
  } else {
    Serial.println("Fallo LTE");
  }
}
void loop() {}
