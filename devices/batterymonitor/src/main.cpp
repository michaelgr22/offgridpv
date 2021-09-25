#include <Arduino.h>
#include <wifi_connector.h>
#include <aws_iot.h>

#include "wifi_credentials.h"
#include "aws_credentials.h"

void setup()
{
  Serial.begin(9600);
  WifiConnector wifiConnector = WifiConnector(ssid, password);
  wifiConnector.connect();
  AwsIot awsIot = AwsIot(aws_cert_ca, aws_cert_crt, aws_cert_private, aws_iot_endpoint, device_name, aws_max_reconnect_tries);
  awsIot.connect();

  StaticJsonDocument<512> json;
  JsonObject stateObj = json.createNestedObject("state");
  JsonObject reportedObj = stateObj.createNestedObject("reported");
  reportedObj["voltage"] = 13.0;

  awsIot.publishMessage(json, aws_iot_topic);
}

void loop()
{
  // put your main code here, to run repeatedly:
}