#include <Arduino.h>
#include <iostream>
#include <wifi_connector.h>
#include <aws_iot.h>
#include <string.h>
#include <esp_adc_cal.h>
#include <driver/adc.h>

#include "wifi_credentials.h"
#include "aws_credentials.h"
#include "logging.h"
#include "ota.h"

using std::string;

const string device = "currentmonitor";
const int vref = 1121;
const int measured_offset = 35;

const int INPIN = 33;
const int OUTPIN = 39;
const int VCCPIN = 35;

const double R1 = 990.0;
const double R2 = 988.0;

AwsIot awsIot = AwsIot(aws_cert_ca, aws_cert_crt, aws_cert_private, aws_iot_endpoint, device_name, aws_max_reconnect_tries);
WifiConnector wifiConnector = WifiConnector(ssid, password);
Logging logging = Logging("http://kuberneteshome:8081/");

void setupConnections()
{

  WiFi.setHostname("offgridpv-currentmonitor");

  wifiConnector.connect();
  if (wifiConnector.isConnected())
  {
    logging.syncTime();
    logging.sendLog(device, "Connected to WiFi successfull");

    if (awsIot.connect())
      logging.sendLog(device, "Connected to Aws Iot successfull");

    setupOTA("currentmonitor", ssid.c_str(), password.c_str());
  }
}

double readVoltage(const int pin)
{
  const int numberOfSamples = 32;
  double sum = 0;

  esp_adc_cal_characteristics_t *adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, vref, adc_chars);

  for (int i = 0; i < numberOfSamples; i++)
    sum += (esp_adc_cal_raw_to_voltage(analogRead(pin), adc_chars) + measured_offset);
  return (sum / numberOfSamples) / 1000.0;
}

double calculateCurrentFromVoltage(const double inVoltage, const double vccVoltage)
{
  const int sensitivity = 40;
  const double factor = sensitivity / 1000.0;

  const double quiescentOutputvoltage = 0.5;
  const double qov = quiescentOutputvoltage * vccVoltage;

  double voltage_qov = inVoltage - qov + 0.03;
  logging.sendLog(device, String(voltage_qov, 6).c_str());
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
  logging.sendLog(device, "Published: " + published);
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
    logging.sendLog(device, "Start Voltage Reading");
    double r2Voltage = readVoltage(VCCPIN);
    logging.sendLog(device, "R2 Voltage");
    logging.sendLog(device, String(r2Voltage, 6).c_str());
    double vccVoltage = calculateVccVoltage(r2Voltage);
    logging.sendLog(device, "Vcc Voltage");
    logging.sendLog(device, String(vccVoltage, 6).c_str());
    double inVoltage = readVoltage(INPIN);
    logging.sendLog(device, "inVoltage");
    logging.sendLog(device, String(inVoltage, 6).c_str());
    double inCurrent = calculateCurrentFromVoltage(inVoltage, vccVoltage);
    logging.sendLog(device, "inCurrent");
    logging.sendLog(device, String(inCurrent, 6).c_str());

    publishMessageToAws(vccVoltage, inVoltage, inCurrent);
  }
  else
  {
    setupConnections();
  }

  delay(5000);
}