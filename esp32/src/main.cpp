#include <Arduino.h>
#include <Adafruit_INA219.h>
#include <Wire.h>

Adafruit_INA219 ina219;
#define I2C_SDA_PIN 23
#define I2C_SCL_PIN 22

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);// 1000000
  ina219.begin();
  ina219.setCalibration_16V_400mA();
}

void loop() {
  delay(500);
  Serial.println((ina219.getShuntVoltage_mV()/1000)+ (ina219.getBusVoltage_V()));
  Serial.println(ina219.getCurrent_mA());
  Serial.println("____");
  }
