#include "esphome.h"

// https://wiki.seeedstudio.com/Grove-Water-Level-Sensor/
unsigned char low_data[8] = {0};
unsigned char high_data[12] = {0};

int sensorvalue_min = 250;
int sensorvalue_max = 255;

#define NO_TOUCH            0xFE
#define THRESHOLD           100
#define ATTINY1_HIGH_ADDR   0x78
#define ATTINY2_LOW_ADDR    0x77

class WaterLevelSensor : public PollingComponent, public Sensor {
 public:
  WaterLevelSensor() : PollingComponent(15000) {} 

  void setup() override {
    // Initialize the device here. Usually Wire.begin() will be called in here,
    // though that call is unnecessary if you have an 'i2c:' entry in your config

    ESP_LOGV("custom", "in-setup");
    Wire.begin();
  }
  void update() override {
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;

    getLow8SectionValue();
    getHigh12SectionValue();
 
    ESP_LOGV("custom", "low 8 sections value = %d.%d.%d.%d.%d.%d.%d.%d", low_data[0], low_data[1], low_data[2], low_data[3], low_data[4], low_data[5], low_data[6], low_data[7]);
 
    ESP_LOGV("custom", "high 12 sections value = %d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d", high_data[0], high_data[1], high_data[2], high_data[3], high_data[4], high_data[5], high_data[6], high_data[7], high_data[8], high_data[9], high_data[10], high_data[11]);
 
    for (int i = 0 ; i < 8; i++) {
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;
      }
    }

    for (int i = 0 ; i < 12; i++) {
      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t)1 << (8 + i);
      }
    }
 
    while (touch_val & 0x01)
    {
      trig_section++;
      touch_val >>= 1;
    }

    ESP_LOGV("custom", "water level = %d", trig_section * 5);
    publish_state(trig_section * 5);
  }

  void getLow8SectionValue(void)
  {
    memset(low_data, 0, sizeof(low_data));
    Wire.requestFrom(ATTINY2_LOW_ADDR, 8);

    while (8 != Wire.available());

    for (int i = 0; i < 8 ; i++) {
      low_data[i] = Wire.read(); // receive a byte as character
    }
    delay(10);
  }

  void getHigh12SectionValue(void)
  {
    memset(high_data, 0, sizeof(high_data));
    Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);

    while (12 != Wire.available());

    for (int i = 0; i < 12; i++) {
      high_data[i] = Wire.read();
    }
    delay(10);
  }

};

