#include <Adafruit_NeoPixel.h>
#define LED_PIN 38
#define LED_NUM 1
Adafruit_NeoPixel led(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  led.begin();
  led.setBrightness(80);
  Serial.println("LED RGB listo");
}

void loop() {
  Serial.println("Rojo");
  led.setPixelColor(0, led.Color(255, 0, 0));   led.show(); delay(800);
  Serial.println("Verde");
  led.setPixelColor(0, led.Color(0, 200, 0));   led.show(); delay(800);
  Serial.println("Azul");
  led.setPixelColor(0, led.Color(0, 0, 255));   led.show(); delay(800);
  Serial.println("Amarillo");
  led.setPixelColor(0, led.Color(255, 150, 0)); led.show(); delay(800);
  Serial.println("Apagado");
  led.setPixelColor(0, led.Color(0, 0, 0));     led.show(); delay(400);
}
