# Monitoring my offgrid Photovoltaik System
To automate the workloads of my pv system I use different microcontrollers and sensors. Automation includes monitoring the healthiness and SOC of the batteries. Furthermore the current power consumption should be calculated. Therefore the current flowing in and out the batteries and the voltage is measured.

![structure](https://i.ibb.co/QbVm8Ct/strucutre.png)

## Sensors:

 - [ACS758](https://www.digikey.de/de/datasheets/allegromicrosystemsllc/allegro-microsystems-llcacs758datasheetashx)-> current measurement 
  -  [ADS1115](https://www.ti.com/lit/ds/symlink/ads1114.pdf?ts=1648959763893&ref_url=https%253A%252F%252Fwww.google.com%252F)-> external ADC to measure output voltage of ACS758 
  -  [ESP32](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)-> internal ADC to measure voltage of batteries

## Infrastructure:
![infrastructure](https://i.ibb.co/tmsxzjy/infrastructure.png)
