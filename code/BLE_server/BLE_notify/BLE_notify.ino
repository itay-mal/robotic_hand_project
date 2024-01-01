// general purpose
#include <time.h>
#include <sys/time.h>
#include "esp_sntp.h"

// BLE server
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;

bool deviceConnected = false;
bool oldDeviceConnected = false;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// IMU
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

#define BUFF_SIZE 100 // should be enugh...
char buff[BUFF_SIZE];
struct timeval tv_now;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Connected");
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Disconnected");
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);

    // wait for monitor wakeup
    while (!Serial) {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
    }

    // Try to initialize MPU
    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
        delay(10);
    }
    }

    // set MPU defaults
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("");
    delay(100);

    // Create the BLE Device
    BLEDevice::init("ESP32");

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ   |
        BLECharacteristic::PROPERTY_WRITE  |
        BLECharacteristic::PROPERTY_NOTIFY |
        BLECharacteristic::PROPERTY_INDICATE
        );

    // Create and attach BLE Descriptors
    pCharacteristic->addDescriptor(new BLE2902());
    
    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
}

void loop() {

    // Get new sensor events with the readings
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);
    
    // Get timestamp in microseconds
    gettimeofday(&tv_now, NULL);
    int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
    
    // format message and set value
    snprintf(buff, BUFF_SIZE, "%s:%f:%f:%f:%f:%f:%f#", String(time_us), accel.acceleration.x, accel.acceleration.y, accel.acceleration.z, gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    pCharacteristic->setValue(buff);
    
    // notify changed value
    if (deviceConnected) {
        pCharacteristic->notify();
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
}
