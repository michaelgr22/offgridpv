#include <Arduino.h>
#include <wifi_connector.h>
#include <aws_iot.h>
#include <esp_adc_cal.h>
#include <string>

#include "wifi_credentials.h"
#include "aws_credentials.h"
#include "coordinate.h"
#include "straight.h"
#include "ota.h"
#include "logging.h"

using std::string;

const string device = "batterymonitor";

const float R1 = 125000.0;
const float R2R4 = 100000.0;
const float R3 = 25000.0;
const float R5 = 12000.0;

const int R3PIN = 33;
const int R5PIN = 39;

AwsIot awsIot = AwsIot(aws_cert_ca, aws_cert_crt, aws_cert_private, aws_iot_endpoint, device_name, aws_max_reconnect_tries);
WifiConnector wifiConnector = WifiConnector(ssid, password);
Logging logging = Logging("http://kuberneteshome:8081/");

void setupConnections()
{

  WiFi.setHostname("offgridpv-batterymonitor");

  wifiConnector.connect();
  if (wifiConnector.isConnected())
  {
    logging.syncTime();
    logging.sendLog(device, "Connected to WiFi successfull");

    if (awsIot.connect())
      logging.sendLog(device, "Connected to Aws Iot successfull");

    setupOTA("batterymonitor", ssid.c_str(), password.c_str());
  }
}

double readVoltage(const int pin)
{
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  double voltage = esp_adc_cal_raw_to_voltage(analogRead(pin), &adc_chars) / 1000.0;
  return voltage;
}

double calculateBattery2Voltage(double r3voltage)
{
  return r3voltage * ((R2R4 + R3) / R3);
}

double calculateBattery1Voltage(double r5Voltage, double battery2Voltage)
{
  double battery1and2Voltage = r5Voltage * ((R5 + R2R4) / R5);
  return battery1and2Voltage - battery2Voltage;
}

void publishMessageToAws(double battery1Voltage, double battery2Voltage, double battery1Capacity, double battery2Capacity)
{
  StaticJsonDocument<512> json;
  JsonObject stateObj = json.createNestedObject("state");
  JsonObject reportedObj = stateObj.createNestedObject("reported");
  JsonObject battery1 = reportedObj.createNestedObject("battery1");
  JsonObject battery2 = reportedObj.createNestedObject("battery2");

  battery1["voltage"] = battery1Voltage;
  battery1["capacity"] = battery1Capacity;
  battery2["voltage"] = battery2Voltage;
  battery2["capacity"] = battery2Capacity;

  char jsonBuffer[512];
  serializeJson(json, jsonBuffer);

  logging.sendLog(device, "Publish Message to Aws IoT");
  logging.sendLog(device, jsonBuffer);

  awsIot.publishMessage(json, aws_iot_topic);
}

double calculateCapacity(double voltage)
{

  if (voltage >= 13.6)
    return 100.0;
  if (voltage <= 10.0)
    return 0.0;

  Coordinate coordinates[11] = {Coordinate(10, 0), Coordinate(12, 9), Coordinate(12.5, 14), Coordinate(12.8, 17),
                                Coordinate(12.9, 20), Coordinate(13, 30), Coordinate(13.1, 40), Coordinate(13.2, 70), Coordinate(13.3, 90),
                                Coordinate(13.4, 99), Coordinate(13.6, 100)};

  int i = 0;
  while (voltage > coordinates[i].x)
  {
    i++;
  }

  Straight straight = Straight(coordinates[i - 1], coordinates[i]);

  return straight.m * voltage + straight.b;
}

void setup()
{

  Serial.begin(9600);

  setupConnections();
}

void loop()
{
  if (wifiConnector.isConnected())
  {
    ArduinoOTA.handle();
    logging.sendLog(device, "Start Voltage Reading");
    double r3Voltage = readVoltage(R3PIN);
    double r5Voltage = readVoltage(R5PIN);

    double battery2Voltage = calculateBattery2Voltage(r3Voltage);
    double battery1Voltage = calculateBattery1Voltage(r5Voltage, battery2Voltage);

    double battery1Capacity = calculateCapacity(battery1Voltage);
    double battery2Capacity = calculateCapacity(battery2Voltage);

    publishMessageToAws(battery1Voltage, battery2Voltage, battery1Capacity, battery2Capacity);
  }
  else
  {
    setupConnections();
  }

  delay(5000);
}