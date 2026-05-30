import boto3, json, time
from decimal import Decimal

iot    = boto3.client("iot-data", region_name="us-east-2")
dynamo = boto3.resource("dynamodb", region_name="us-east-2")
table  = dynamo.Table("sensor_data")

def lambda_handler(event, context):
    body = json.loads(event.get("body", "{}"))

    iot.publish(topic="esp32/test", qos=1, payload=json.dumps(body))

    item = {
        "device_id":   "esp32_sensor",
        "timestamp":   int(time.time() * 1000),
        "temperatura": Decimal(str(body.get("temperatura", 0))),
        "humedad":     Decimal(str(body.get("humedad", 0))),
        "presion":     Decimal(str(body.get("presion", 0))),
    }

    # Guardar GPS solo si tiene señal
    if "lat" in body and body["lat"] != 0:
        item["lat"] = Decimal(str(body["lat"]))
        item["lon"] = Decimal(str(body["lon"]))
        item["altitud"] = Decimal(str(body.get("altitud", 0)))

    table.put_item(Item=item)

    return {"statusCode": 200, "body": "ok"}
