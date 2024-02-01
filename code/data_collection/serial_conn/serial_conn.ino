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

const int START     = 0xAA;
const int GET_DATA  = 0xAB;
const int STOP      = 0xAC;
const int HANDSHAKE = 0xAD;

bool LED = LOW;

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
    idle_loop();
}

void get_mpu_data() {
    // Get new sensor events with the readings
    mpu.getEvent(&accel, &gyro, &temp);
    
    // Get timestamp in microseconds
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
    
    // format message and set value
    snprintf(buff, BUFF_SIZE, "@%s,%f,%f,%f,%f,%f,%f,#", String(time_us), accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    Serial.print(buff);
}

void idle_loop(){
    int inbyte;
    bool remain_inside = true;
    while(remain_inside){
        LED = !LED;
        digitalWrite(13, LED);
        
        inbyte = Serial.read();
        switch (inbyte)
        {
        case START:
            remain_inside = false;
            break;
        
        case HANDSHAKE:
            delay(50);
            Serial.println("LETS GO");
            delay(50);
            break;
        
        default:
            if(inbyte != -1){
                Serial.print("got byte ");
                Serial.println(inbyte, HEX);
            }
            break;
        }
        delay(500);
    }
    digitalWrite(13, LOW);
}

void loop() {

    get_mpu_data();
    
    if(Serial.read() == STOP){
        idle_loop();
    }
}
