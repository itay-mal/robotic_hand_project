
// general purpose
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"

// IMU
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "forrest.hpp"

void add_sample_to_buffer();

Adafruit_MPU6050 mpu;
sensors_event_t accel, gyro, temp;

const char READY = 0xAA;

#define CAPTURE_WINDOW_US 330000  // == 80 [Hz] - sample rate
#define PERIOD_US 12500  // == 80 [Hz] - sample rate
#define THRESHOLD 0.7 // # of votes
#define AXES 6 //atributes

float buff[NUM_FEATURES] = {0};
char msg[300];
int votes[NUM_CLASSES] = {0};
struct timeval tv_now;
int64_t time_us_p;
int64_t previous_catch = 0;
int thresh_votes = THRESHOLD * NUM_TREES;

void setup() {
    Serial.begin(115200);

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

    time_us_p = 0;

    Serial.println("ESP32 ready to go!");
}

void loop() {
    
    // Get timestamp in microseconds
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
    
    if(abs(time_us - time_us_p) > PERIOD_US){
        mpu.getEvent(&accel, &gyro, &temp);
        time_us_p = time_us;
        add_sample_to_buffer();
        votes[0] = votes[1] = votes[2] = 0;
        rf_predict(buff, votes);
        // sprintf(msg,"%lld: rest:%d, up:%d, forward:%d | accX:%f accY:%f accZ:%f gyroX:%f gyroY:%f gyroZ:%f\n", time_us, votes[0], votes[1] , votes[2], accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
        // Serial.print(msg);
        
        // TODO: consider filtering on predictions in a row
        if(abs(time_us - previous_catch) > CAPTURE_WINDOW_US ){
            if(votes[1] >= thresh_votes){
                sprintf(msg,"%lld: UP, votes:%d\n", time_us, votes[1]);
                previous_catch = time_us;
                Serial.print(msg);
            } else if(votes[2] >= thresh_votes){
                sprintf(msg,"%lld: FORWARD, votes:%d\n", time_us, votes[2]);
                previous_catch = time_us;
                Serial.print(msg);
            } 
        } // CAPTURE_WINDOW_US
    } // PERIOD_US
}

void add_sample_to_buffer(){
    for(int i = AXES; i < NUM_FEATURES; i++){
        buff[i-AXES] = buff[i];
    }
    buff[NUM_FEATURES-6] = accel.acceleration.x;
    buff[NUM_FEATURES-5] = accel.acceleration.y;
    buff[NUM_FEATURES-4] = accel.acceleration.z;
    buff[NUM_FEATURES-3] = gyro.gyro.x;
    buff[NUM_FEATURES-2] = gyro.gyro.y;
    buff[NUM_FEATURES-1] = gyro.gyro.z;
}
 