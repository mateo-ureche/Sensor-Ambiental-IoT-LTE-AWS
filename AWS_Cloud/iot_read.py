import boto3, json
from boto3.dynamodb.conditions import Key
from decimal import Decimal

dynamo = boto3.resource("dynamodb", region_name="us-east-2")
table  = dynamo.Table("sensor_data")

class DecimalEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Decimal):
            return float(obj)
        return super().default(obj)

def lambda_handler(event, context):
    resp = table.query(
        KeyConditionExpression=Key("device_id").eq("esp32_sensor"),
        ScanIndexForward=False,
        Limit=50
    )
    items = list(reversed(resp["Items"]))

    return {
        "statusCode": 200,
        "headers": {
            "Access-Control-Allow-Origin": "*",
            "Content-Type": "application/json"
        },
        "body": json.dumps(items, cls=DecimalEncoder)
    }
