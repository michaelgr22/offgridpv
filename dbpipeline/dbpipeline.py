import sys

from credentials import AWS_ENDPOINT, AWS_PORT, MONGO_CONNECTION_STRING, PKCS12_PASSWORD

from awsiot import AwsIot
from mongodb import MongoDB
from power_measurements_collection import PowerMeasurementsCollection
from models.batterymonitor_measurement import BatterymonintorMeasurement


aws_iot = AwsIot(AWS_ENDPOINT, AWS_PORT, PKCS12_PASSWORD)
mongodb = MongoDB(MONGO_CONNECTION_STRING, 'offgridpv', 'power_measurements')
powermeasurements_collection = PowerMeasurementsCollection(mongodb)

device = sys.argv[1]


batterymonintor_shadow = aws_iot.get_thing_shadow(device)
print(batterymonintor_shadow)
batterymonitor_measurement = BatterymonintorMeasurement.from_shadow(
    batterymonintor_shadow)
print(batterymonitor_measurement.to_dict())
result = powermeasurements_collection.insert_measurement(
    batterymonitor_measurement)
print(result)
