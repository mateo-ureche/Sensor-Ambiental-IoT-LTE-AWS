# Development Stages

Esta carpeta contiene las diferentes etapas de validación realizadas durante el desarrollo del firmware del sistema IoT de monitoreo ambiental.

## Stage 01 – Serial Test

Verificación inicial del funcionamiento del ESP32-S3 mediante comunicación serial.

## Stage 02 – DHT22 Test

Validación de lectura de temperatura y humedad utilizando el sensor DHT22.

## Stage 03 – BMP280 Test

Verificación de adquisición de presión atmosférica y temperatura mediante el sensor BMP280.

## Stage 04 – RGB LED Test

Prueba de funcionamiento del LED RGB WS2812B integrado en la placa ESP32-S3-SIM7670G.

## Stage 05 – ILI9341 Display Test

Verificación de comunicación SPI y funcionamiento de la pantalla TFT ILI9341.

## Stage 06 – SIM7670G AT Test

Validación de comunicación serial con el módulo SIM7670G mediante comandos AT.

## Stage 07 – LTE Connection Test

Prueba de registro en red LTE y obtención de conectividad a Internet utilizando una SIM Claro Colombia.

## Stage 08 – HTTPS AWS Test

Verificación del envío de datos mediante HTTPS hacia la API desplegada en AWS.

## Stage 09 – GNSS Test

Obtención de coordenadas geográficas mediante el receptor GNSS integrado del SIM7670G.

## Stage 10 – SMS Alert Test

Prueba de envío de mensajes SMS automáticos para notificaciones y alertas remotas.
