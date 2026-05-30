# AWS Cloud

Infraestructura cloud implementada utilizando servicios serverless de Amazon Web Services.

## Servicios utilizados

- Amazon API Gateway
- AWS Lambda
- Amazon DynamoDB
- AWS IoT Core

## Funciones Lambda

### iot_publish

Recibe los datos enviados por el ESP32 mediante HTTPS, publica la información en AWS IoT Core y almacena los registros en DynamoDB.

### iot_read

Consulta los registros almacenados en DynamoDB y devuelve los datos en formato JSON para su visualización en el dashboard web.

## Base de datos

Tabla utilizada:

sensor_data

Campos principales:

- temperatura
- humedad
- presion
- lat
- lon
- altitud
- timestamp
