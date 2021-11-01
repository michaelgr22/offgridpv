#include <Arduino.h>
#include <wifi_connector.h>
#include <aws_iot.h>
#include <esp_adc_cal.h>

#include "wifi_credentials.h"
#include "aws_credentials.h"

AwsIot awsIot = AwsIot(aws_cert_ca, aws_cert_crt, aws_cert_private, aws_iot_endpoint, device_name, aws_max_reconnect_tries);
WifiConnector wifiConnector = WifiConnector(ssid, password);

double battery1Voltage = 0.0;
double battery2Voltage = 1.0;
double battery1Capacity = 10.0;
double battery2Capacity = 20.0;

void setupConnections()
{
  wifiConnector.connect();
  if (wifiConnector.isConnected())
    Serial.println("Connected to Wifi");

  if (awsIot.connect())
    Serial.println("Connected to AWS Iot");
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
  Serial.print("Publish: ");
  Serial.println(jsonBuffer);

  awsIot.publishMessage(json, aws_iot_topic);
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
    battery1Voltage += 0.1;
    battery2Voltage += 0.1;
    battery1Capacity += 2.0;
    battery2Capacity += 2.0;
    publishMessageToAws(battery1Voltage, battery2Voltage, battery1Capacity, battery2Capacity);
  }
  else
  {
    setupConnections();
  }

  delay(5000);
}