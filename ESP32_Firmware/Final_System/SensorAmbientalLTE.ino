#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// ---------- Pantalla TFT ----------
#define TFT_CS    10
#define TFT_DC    11
#define TFT_RST    9
#define TFT_MOSI  13
#define TFT_SCLK  12
#define TFT_MISO  -1
#define TFT_LED    5

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);

#define C_BG     0x0002
#define C_CARD   0x00C9
#define C_ACCENT 0x07E8
#define C_ORANGE 0xFD00
#define C_BLUE   0x1C3F
#define C_DIM    0x294A
#define C_LINE   0x07C4
#define C_WHITE  0xFFFF

// ---------- LED RGB ----------
#define LED_PIN 38
#define LED_NUM 1
Adafruit_NeoPixel led(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

// ---------- DHT22 ----------
#define DHTPIN 21
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ---------- BMP280 ----------
#define I2C_SDA 1
#define I2C_SCL 42
Adafruit_BMP280 bmp;

// ---------- LTE ----------
HardwareSerial sim(1);
TinyGsm modem(sim);

const char apn[]     = "internet.comcel.com.co";
const char api_url[] = "https://x8etdro2r9.execute-api.us-east-2.amazonaws.com/sensor";
const char sms_num[] = "3023015100";

// ---------- Timers ----------
unsigned long ultimoEnvio    = 0;
unsigned long ultimoPantalla = 0;
unsigned long ledTimer       = 0;
unsigned long ultimoSMS_T    = 0;
unsigned long ultimoSMS_H    = 0;
unsigned long ultimoSMS_P    = 0;
bool          ledState       = false;
bool          bmp_ok         = false;

#define SMS_INTERVALO 600000UL

float prev_temp = -999, prev_hum = -999, prev_pres = -999;

// ---------- Struct GPS ----------
struct GPSData {
  float lat = 0, lon = 0, alt = 0;
  bool valido = false;
};

// ============================================================
//  LED
// ============================================================

void ledColor(uint8_t r, uint8_t g, uint8_t b) {
  led.setPixelColor(0, led.Color(r, g, b));
  led.show();
}
void ledApagar() { ledColor(0, 0, 0); }

void ledFlashBlanco() {
  ledColor(255, 255, 255);
  delay(80);
  ledApagar();
}

void ledEstado(float temp, float hum, float pres) {
  unsigned long ahora = millis();
  if (temp >= 35 || hum >= 85 || hum <= 20 || pres < 1000) {
    if (ahora - ledTimer >= 200) {
      ledTimer = ahora; ledState = !ledState;
      if (ledState) ledColor(255, 0, 0); else ledApagar();
    }
    return;
  }
  if (temp >= 28 || hum >= 70 || hum <= 30 || pres < 1005) {
    if (ahora - ledTimer >= 800) {
      ledTimer = ahora; ledState = !ledState;
      if (ledState) ledColor(255, 150, 0); else ledApagar();
    }
    return;
  }
  ledColor(0, 200, 0);
}

// ============================================================
//  PANTALLA
// ============================================================

void drawBar(int x, int y, int w, int h, float pct, uint16_t color) {
  tft.fillRoundRect(x, y, w, h, 2, C_DIM);
  int fill = constrain((int)(w * pct), 0, w);
  if (fill > 0) tft.fillRoundRect(x, y, fill, h, 2, color);
}

void dibujarEstatico() {
  tft.fillScreen(C_BG);
  tft.setTextColor(C_ACCENT);
  tft.setTextSize(1);
  tft.setCursor(8, 8);
  tft.print("PANEL DE CONTROL IoT");
  tft.fillCircle(203, 11, 3, C_ACCENT);
  tft.setCursor(210, 8);
  tft.print("SENSORES OK");
  tft.drawFastHLine(6, 20, 308, C_LINE);

  int xs[3] = {6, 112, 218};
  for (int i = 0; i < 3; i++)
    tft.fillRoundRect(xs[i], 26, 100, 100, 6, C_CARD);

  tft.setTextSize(1);
  tft.setTextColor(C_DIM, C_CARD);
  tft.setCursor(xs[0] + 10, 32); tft.print("TEMPERATURA");
  tft.setCursor(xs[1] + 20, 32); tft.print("PRESION");
  tft.setCursor(xs[2] + 18, 32); tft.print("HUMEDAD");
  tft.setCursor(xs[0] + 28, 110); tft.print("DHT22");
  tft.setCursor(xs[1] + 24, 110); tft.print("BMP280");
  tft.setCursor(xs[2] + 28, 110); tft.print("DHT22");

  tft.drawFastHLine(6, 138, 308, C_DIM);
  tft.setTextColor(C_DIM, C_BG);
  tft.setCursor(8,  146); tft.print("TEMP");
  tft.setCursor(8,  161); tft.print("PRES");
  tft.setCursor(8,  176); tft.print("HUM ");

  tft.drawFastHLine(6, 196, 308, C_DIM);
  tft.setTextColor(C_DIM, C_BG);
  tft.setCursor(8,   204); tft.print("Sensores");
  tft.setCursor(120, 204); tft.print("Intervalo");
  tft.setTextColor(C_WHITE, C_BG);
  tft.setCursor(8,   216); tft.print("DHT22+BMP280");
  tft.setCursor(120, 216); tft.print("5 s");
}

void actualizarTarjeta(int card_idx, const char* valor, const char* unidad, uint16_t color) {
  int xs[3] = {6, 112, 218};
  int cx = xs[card_idx] + 50;
  tft.fillRect(xs[card_idx] + 2, 44, 96, 62, C_CARD);
  tft.setTextSize(3);
  tft.setTextColor(color, C_CARD);
  int16_t x1, y1; uint16_t tw, th;
  tft.getTextBounds(valor, 0, 0, &x1, &y1, &tw, &th);
  tft.setCursor(cx - tw / 2, 52);
  tft.print(valor);
  tft.setTextSize(1);
  tft.setTextColor(color, C_CARD);
  tft.getTextBounds(unidad, 0, 0, &x1, &y1, &tw, &th);
  tft.setCursor(cx - tw / 2, 96);
  tft.print(unidad);
}

void actualizarBarras(float temp, float pres, float hum) {
  float pct_t = constrain(temp / 50.0, 0, 1);
  float pct_p = constrain((pres - 950.0) / 100.0, 0, 1);
  float pct_h = constrain(hum / 100.0, 0, 1);
  int bx = 48, bw = 230, bh = 5;
  drawBar(bx, 148, bw, bh, pct_t, C_ORANGE);
  drawBar(bx, 163, bw, bh, pct_p, C_BLUE);
  drawBar(bx, 178, bw, bh, pct_h, C_ACCENT);
}

void actualizarPantalla(float temp, float hum, float pres) {
  char buf[12];
  if (temp != prev_temp) {
    if (isnan(temp)) actualizarTarjeta(0, "ERR", "C", 0xF800);
    else { dtostrf(temp, 4, 1, buf); actualizarTarjeta(0, buf, "grados C", C_ORANGE); }
    prev_temp = temp;
  }
  if (pres != prev_pres) {
    if (isnan(pres)) actualizarTarjeta(1, "ERR", "hPa", 0xF800);
    else { dtostrf(pres, 6, 1, buf); actualizarTarjeta(1, buf, "hPa", C_BLUE); }
    prev_pres = pres;
  }
  if (hum != prev_hum) {
    if (isnan(hum)) actualizarTarjeta(2, "ERR", "%", 0xF800);
    else { dtostrf(hum, 4, 1, buf); actualizarTarjeta(2, buf, "%", C_ACCENT); }
    prev_hum = hum;
  }
  if (!isnan(temp) && !isnan(pres) && !isnan(hum))
    actualizarBarras(temp, pres, hum);
}

// ============================================================
//  HELPERS AT
// ============================================================

void leerRespuesta(int ms = 5000) {
  String resp = "";
  long t = millis();
  while (millis() - t < ms) {
    while (sim.available()) {
      char c = sim.read();
      Serial.write(c);
      resp += c;
      if (resp.endsWith("OK\r\n") || resp.endsWith("ERROR\r\n")) {
        delay(50);
        while (sim.available()) Serial.write(sim.read());
        return;
      }
    }
  }
}

void enviar(const String& cmd, int ms = 5000) {
  Serial.println("\n>> " + cmd);
  sim.println(cmd);
  leerRespuesta(ms);
}

// ============================================================
//  SMS
// ============================================================

void enviarSMS(const String& mensaje) {
  Serial.println("\n=== Enviando SMS ===");
  Serial.println(mensaje);
  sim.println("AT+CMGF=1");
  leerRespuesta(3000);
  String cmd = "AT+CMGS=\""; cmd += sms_num; cmd += "\"";
  sim.println(cmd);
  leerRespuesta(3000);
  sim.print(mensaje);
  sim.write(26);
  leerRespuesta(10000);
  Serial.println("=== SMS enviado ===");
}

void verificarAlertas(float temp, float hum, float pres) {
  unsigned long ahora = millis();
  if (temp >= 35 && (ultimoSMS_T == 0 || ahora - ultimoSMS_T >= SMS_INTERVALO)) {
    ultimoSMS_T = ahora;
    String msg = "ALERTA SENSOR: Temperatura alta ";
    msg += String(temp, 1); msg += "C | Hum: ";
    msg += String(hum, 1); msg += "% | Pres: ";
    msg += String(pres, 1); msg += " hPa";
    enviarSMS(msg);
  }
  if ((hum >= 85 || hum <= 20) && (ultimoSMS_H == 0 || ahora - ultimoSMS_H >= SMS_INTERVALO)) {
    ultimoSMS_H = ahora;
    String msg = "ALERTA SENSOR: Humedad fuera de rango ";
    msg += String(hum, 1); msg += "% | Temp: ";
    msg += String(temp, 1); msg += "C";
    enviarSMS(msg);
  }
  if (pres < 1000 && (ultimoSMS_P == 0 || ahora - ultimoSMS_P >= SMS_INTERVALO)) {
    ultimoSMS_P = ahora;
    String msg = "ALERTA SENSOR: Presion baja ";
    msg += String(pres, 1); msg += " hPa | Temp: ";
    msg += String(temp, 1); msg += "C";
    enviarSMS(msg);
  }
}

// ============================================================
//  GPS
// ============================================================

GPSData leerGPS() {
  GPSData gps;
  sim.println("AT+CGNSSINFO");
  String resp = "";
  long t = millis();
  while (millis() - t < 3000)
    while (sim.available()) resp += (char)sim.read();

  int idx = resp.indexOf("+CGNSSINFO:");
  if (idx == -1) return gps;
  String data = resp.substring(idx + 12);
  if (data.startsWith(",,,")) return gps;

  int campos[14]; campos[0] = 0; int campo = 1;
  for (int i = 0; i < (int)data.length() && campo < 14; i++)
    if (data[i] == ',') campos[campo++] = i + 1;

  String latStr = data.substring(campos[5],  data.indexOf(',', campos[5]));
  String latDir = data.substring(campos[6],  data.indexOf(',', campos[6]));
  String lonStr = data.substring(campos[7],  data.indexOf(',', campos[7]));
  String lonDir = data.substring(campos[8],  data.indexOf(',', campos[8]));
  String altStr = data.substring(campos[11], data.indexOf(',', campos[11]));

  if (latStr.length() < 3) return gps;
  gps.lat = latStr.toFloat();
  gps.lon = lonStr.toFloat();
  gps.alt = altStr.toFloat();
  if (latDir == "S") gps.lat = -gps.lat;
  if (lonDir == "W") gps.lon = -gps.lon;
  gps.valido = true;
  return gps;
}

// ============================================================
//  ENVIAR A AWS
// ============================================================

void enviarAWS(const String& payload) {
  Serial.println("\n--- Enviando a AWS ---");
  Serial.println(payload);
  ledFlashBlanco();

  enviar("AT+HTTPINIT");
  enviar(String("AT+HTTPPARA=\"URL\",\"") + api_url + "\"");
  enviar("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  enviar("AT+HTTPPARA=\"SSLCFG\",0");

  int payloadLen = payload.length();
  sim.println("AT+HTTPDATA=" + String(payloadLen) + ",5000");
  String resp = ""; long t = millis(); bool enviado = false;
  while (millis() - t < 5000) {
    while (sim.available()) {
      char c = sim.read(); Serial.write(c); resp += c;
      if (resp.endsWith("DOWNLOAD")) {
        delay(100); sim.print(payload);
        Serial.println("\n[payload enviado]");
        leerRespuesta(3000); enviado = true; goto done;
      }
    }
  }
  done:
  if (!enviado) { enviar("AT+HTTPTERM"); return; }
  enviar("AT+HTTPACTION=1", 15000);
  leerRespuesta(3000);
  enviar("AT+HTTPTERM");
}

// ============================================================
//  SETUP
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 1. BMP280 PRIMERO antes de la pantalla
  Wire.begin(I2C_SDA, I2C_SCL);
  bmp_ok = bmp.begin(0x76);
  if (!bmp_ok) bmp_ok = bmp.begin(0x77);
  if (bmp_ok) {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_500);
    Serial.println("BMP280 OK");
  } else {
    Serial.println("BMP280 no encontrado");
  }

  // 2. Pantalla después del BMP280
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  tft.begin(24000000);
  tft.setRotation(1);
  dibujarEstatico();

  // 3. LED
  led.begin();
  led.setBrightness(80);
  ledColor(0, 0, 255);

  // 4. DHT22
  dht.begin();
  delay(2000);

  // 5. LTE
  sim.begin(115200, SERIAL_8N1, 17, 18);
  Serial.println("Iniciando modem...");
  modem.restart();
  delay(8000);

  Serial.println("Conectando LTE...");
  if (!modem.gprsConnect(apn, "", "")) {
    Serial.println("[ERROR] Fallo LTE");
    ledColor(255, 0, 0);
    while (1);
  }
  Serial.println("LTE OK");

  enviar("AT+CSSLCFG=\"sslversion\",0,3");
  enviar("AT+CSSLCFG=\"authmode\",0,0");
  enviar("AT+CSSLCFG=\"ignorelocaltime\",0,1");
  enviar("AT+CGNSSPWR=1");
  delay(2000);

  // 6. Redibujar pantalla después del LTE
  dibujarEstatico();

  Serial.println("Listo!");
  ledApagar();
}

// ============================================================
//  LOOP
// ============================================================

void loop() {
  float humedad     = dht.readHumidity();
  float temperatura = dht.readTemperature();
  float presion     = bmp_ok ? bmp.readPressure() / 100.0F : NAN;

  if (!isnan(humedad) && !isnan(temperatura))
    ledEstado(temperatura, humedad, presion);

  if (millis() - ultimoPantalla > 2000) {
    ultimoPantalla = millis();
    actualizarPantalla(temperatura, humedad, presion);
  }

  if (millis() - ultimoEnvio > 5000) {
    ultimoEnvio = millis();

    if (isnan(humedad) || isnan(temperatura)) {
      Serial.println("[ERROR] DHT22 sin lectura");
      return;
    }

    GPSData gps = leerGPS();

    String payload = "{";
    payload += "\"temperatura\":" + String(temperatura, 2);
    payload += ",\"humedad\":"    + String(humedad, 2);
    payload += ",\"presion\":"    + String(presion, 2);
    if (gps.valido) {
      payload += ",\"lat\":"     + String(gps.lat, 6);
      payload += ",\"lon\":"     + String(gps.lon, 6);
      payload += ",\"altitud\":" + String(gps.alt, 1);
    }
    payload += "}";

    enviarAWS(payload);
    verificarAlertas(temperatura, humedad, presion);
    Serial.println("------------------");
  }
}
