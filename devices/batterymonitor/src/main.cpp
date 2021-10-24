#include <Arduino.h>
#include <wifi_connector.h>
#include <aws_iot.h>
#include <esp_adc_cal.h>

#include "wifi_credentials.h"
#include "aws_credentials.h"
#include "coordinate.h"
#include "straight.h"
#include "ota.h"

const int battery1Pin = 39;
const int battery2Pin = 35;

AwsIot awsIot = AwsIot(aws_cert_ca, aws_cert_crt, aws_cert_private, aws_iot_endpoint, device_name, aws_max_reconnect_tries);

double readVoltage(const int pin)
{
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return esp_adc_cal_raw_to_voltage(analogRead(pin), &adc_chars) / 1000.0;
}

double calculateVoltageSensor(double voltage)
{
  float R1 = 30000.0;
  float R2 = 7500.0;

  return voltage / (R2 / (R1 + R2));
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
  TelnetStream.print("Publish: ");
  TelnetStream.println(jsonBuffer);

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

  WiFi.setHostname("offgridpv-batterymonitor");
  WifiConnector wifiConnector = WifiConnector(ssid, password);
  wifiConnector.connect();
  if (wifiConnector.isConnected())
  {
    TelnetStream.print("Connected to Wifi. IP: ");
    Serial.println(WiFi.localIP());
  }

  if (awsIot.connect())
    TelnetStream.println("Connected to AwsIot.");

  setupOTA("batterymonitor", ssid.c_str(), password.c_str());
}

void loop()
{
  ArduinoOTA.handle();
  double battery1Voltage = calculateVoltageSensor(readVoltage(battery1Pin));
  double battery2Voltage = calculateVoltageSensor(readVoltage(battery2Pin));
  double battery1Capacity = calculateCapacity(battery1Voltage);
  double battery2Capacity = calculateCapacity(battery2Voltage);
  publishMessageToAws(battery1Voltage, battery2Voltage, battery1Capacity, battery2Capacity);
  delay(5000);
}