#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
HardwareSerial sim(1);
TinyGsm modem(sim);
const char apn[]     = "internet.comcel.com.co";
const char api_url[] = "https://x8etdro2r9.execute-api.us-east-2.amazonaws.com/sensor";

void leerRespuesta(int ms = 5000) {
  long t = millis();
  while (millis() - t < ms)
    while (sim.available()) Serial.write(sim.read());
}

void enviar(const String& cmd, int ms = 5000) {
  Serial.println(">> " + cmd);
  sim.println(cmd);
  leerRespuesta(ms);
}

void setup() {
  Serial.begin(115200);
  delay(3000);
  sim.begin(115200, SERIAL_8N1, 17, 18);
  modem.restart();
  delay(8000);
  modem.gprsConnect(apn, "", "");
  Serial.println("LTE OK");
  enviar("AT+CSSLCFG=\"sslversion\",0,3");
  enviar("AT+CSSLCFG=\"authmode\",0,0");
  enviar("AT+CSSLCFG=\"ignorelocaltime\",0,1");

  String payload = "{\"temperatura\":28.5,\"humedad\":65.0,\"presion\":1010.0}";
  enviar("AT+HTTPINIT");
  enviar(String("AT+HTTPPARA=\"URL\",\"") + api_url + "\"");
  enviar("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  enviar("AT+HTTPPARA=\"SSLCFG\",0");
  sim.println("AT+HTTPDATA=" + String(payload.length()) + ",5000");
  String resp = ""; long t = millis();
  while (millis() - t < 5000) {
    while (sim.available()) {
      char c = sim.read(); Serial.write(c); resp += c;
      if (resp.endsWith("DOWNLOAD")) {
        delay(100); sim.print(payload); sim.write(26);
        leerRespuesta(3000); goto done;
      }
    }
  }
  done:
  enviar("AT+HTTPACTION=1", 15000);
  enviar("AT+HTTPTERM");
}
void loop() {}
