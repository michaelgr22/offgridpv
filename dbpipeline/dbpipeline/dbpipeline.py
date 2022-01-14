import sys

from credentials import AWS_ENDPOINT, AWS_PORT, MONGO_CONNECTION_STRING, PKCS12_PASSWORD


from awsiot import AwsIot
from mongodb import MongoDB
from power_measurements_collection import PowerMeasurementsCollection
from models.batterymonitor_measurement import BatterymonintorMeasurement
from models.currentmonitor_measurement import CurrentmonitorMeasurement


device = sys.argv[1]
PKCS12 = device + "_credentials.p12"

aws_iot = AwsIot(AWS_ENDPOINT, AWS_PORT, PKCS12, PKCS12_PASSWORD)
mongodb = MongoDB(MONGO_CONNECTION_STRING, 'offgridpv', 'power_measurements')
powermeasurements_collection = PowerMeasurementsCollection(mongodb, device)

shadow = aws_iot.get_thing_shadow(device)
print(shadow)

if device == 'batterymonitor':
    batterymonitor_measurement = BatterymonintorMeasurement.from_shadow(shadow)
    print(batterymonitor_measurement.to_dict())
    result = powermeasurements_collection.insert_measurement(
        batterymonitor_measurement)
    print(result)
elif device == 'currentmonitor':
    currentmonitor_measurement = CurrentmonitorMeasurement.from_shadow(
        shadow)
    print(currentmonitor_measurement.to_dict())
    result = powermeasurements_collection.insert_measurement(
        currentmonitor_measurement)
    print(result)
