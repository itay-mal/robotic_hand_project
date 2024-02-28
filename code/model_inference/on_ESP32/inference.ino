// general purpose
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"

// IMU
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
sensors_event_t accel, gyro, temp;


#define BUFF_SIZE 100 // should be enugh...
char buff[BUFF_SIZE];
struct timeval tv_now;

void setup() {
    Serial.begin(115200);

    pinMode(13, OUTPUT); // internal led

    // wait for monitor wakeup
    while (!Serial) {
        delay(10); // will pause Zero, Leonardo, etc until serial console opens
    }

    // Try to initialize MPU
    while (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        delay(10);
    }

    // set MPU defaults
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("");
    delay(100);

    Serial.println("ESP32 ready to go!");
}


void loop() {

    // get mpu measurement
    mpu.getEvent(&accel, &gyro, &temp);
    

    // push measurement into the model
    
    // report current prediction
}
