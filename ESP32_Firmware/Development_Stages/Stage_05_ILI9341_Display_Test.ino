#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#define TFT_CS 10
#define TFT_DC 11
#define TFT_RST 9
#define TFT_MOSI 13
#define TFT_SCLK 12
#define TFT_LED 5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, -1);

void setup() {
  Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  tft.begin(24000000);
  tft.setRotation(1);
  tft.fillScreen(0xF800); delay(500);
  tft.fillScreen(0x07E0); delay(500);
  tft.fillScreen(0x001F); delay(500);
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(3);
  tft.setCursor(40, 100);
  tft.print("PANTALLA OK");
  Serial.println("Pantalla OK");
}
void loop() {}
