#include <Arduino.h>
#include <iostream>
#include <wifi_connector.h>
#include <aws_iot.h>
#include <string.h>
#include <esp_adc_cal.h>
#include <driver/adc.h>
#include <Adafruit_ADS1X15.h>

#include "wifi_credentials.h"
#include "aws_credentials.h"
#include "logging.h"
#include "ota.h"

#define ADS_I2C_ADDR 0x48

using std::string;

const string device = "currentmonitor";

const int INPIN = 33;
const int OUTPIN = 39;
const int VCCPIN = 35;

const double R1 = 990.0;
const double R2 = 988.0;

AwsIot awsIot = AwsIot(aws_cert_ca, aws_cert_crt, aws_cert_private, aws_iot_endpoint, device_name, aws_max_reconnect_tries);
WifiConnector wifiConnector = WifiConnector(ssid, password);
Logging logging = Logging("http://kuberneteshome:8081/");
Adafruit_ADS1115 ads;

void setupConnections()
{

  WiFi.setHostname("offgridpv-currentmonitor");

  wifiConnector.connect();
  if (wifiConnector.isConnected())
  {
    //logging.syncTime();
    //logging.sendLog(device, "Connected to WiFi successfull");

    awsIot.connect();

    //logging.sendLog(device, "Connected to Aws Iot successfull");

    setupOTA("currentmonitor", ssid.c_str(), password.c_str());
  }
  while (!ads.begin(ADS_I2C_ADDR))
  {
    Serial.println("Failed to initialize ADS.");
  }
  ads.setGain(GAIN_TWOTHIRDS);
}

double readVoltage(const int adcPin)
{
  Serial.println("Test");
  const float multiplier1 = 0.1875;
  const float multiplier2 = 3;

  int16_t val_0 = ads.readADC_SingleEnded(0);
  int16_t val_1 = ads.readADC_SingleEnded(1);
  //int16_t val_0 = ads.getValue();
  Serial.print("\tAnalog0: ");
  Serial.print(val_0);
  Serial.print('\t');
  Serial.println(val_0 * multiplier2, 3);

  Serial.print("\tAnalog0: ");
  Serial.print(val_1);
  Serial.print('\t');
  Serial.println(val_1 * multiplier1, 3);
}

double calculateCurrentFromVoltage(const double inVoltage, const double vccVoltage)
{
  const int sensitivity = 40;
  const double factor = sensitivity / 1000.0;

  const double quiescentOutputvoltage = 0.5;
  const double qov = quiescentOutputvoltage * vccVoltage;

  double voltage_qov = inVoltage - qov;
  //logging.sendLog(device, String(voltage_qov, 6).c_str());
  return voltage_qov / factor;
}

double calculateVccVoltage(const double r2Voltage)
{
  return r2Voltage * ((R1 + R2) / R2);
}

void publishMessageToAws(double vccVoltage, double generatedSensorVoltage, double generatedCurrent)
{
  StaticJsonDocument<512> json;
  JsonObject stateObj = json.createNestedObject("state");
  JsonObject reportedObj = stateObj.createNestedObject("reported");
  JsonObject generated = reportedObj.createNestedObject("generated");
  JsonObject consumed = reportedObj.createNestedObject("consumed");

  generated["vccVoltage"] = vccVoltage;
  generated["generatedSensorVoltage"] = generatedSensorVoltage;
  generated["generatedCurrent"] = generatedCurrent;

  char jsonBuffer[512];
  serializeJson(json, jsonBuffer);

  string published = awsIot.publish(json, aws_iot_topic);
  //logging.sendLog(device, "Published: " + published);
}

void setup()
{

  Serial.begin(9600);

  pinMode(INPIN, INPUT);
  pinMode(OUTPIN, INPUT);
  pinMode(VCCPIN, INPUT);

  setupConnections();
}

void loop()
{
  if (wifiConnector.isConnected())
  {
    ArduinoOTA.handle();
    readVoltage(2);

    //publishMessageToAws(vccVoltage, inVoltage, inCurrent);
  }
  else
  {
    setupConnections();
  }

  delay(5000);
}