#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int leftWheel = 9;      // Left wheel forward
const int leftWheelBack = 5;  // Left wheel backward
const int rightWheel = 6;     // Right wheel forward
const int rightWheelBack = 7; // Right wheel backward

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
    };
    
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String rxValue = pCharacteristic->getValue().c_str();
        Serial.print("Received Value: ");
        Serial.println(rxValue);
        
        if (rxValue.length() > 0) {
            // Stop all motors first
            stopAllMotors();
            
            switch(rxValue[0]) {
                case 'F':
                    Serial.println("Moving forward!");
                    digitalWrite(leftWheel, HIGH);
                    digitalWrite(rightWheel, HIGH);
                    break;
                    
                case 'B':
                    Serial.println("Moving backward!");
                    digitalWrite(leftWheelBack, HIGH);
                    digitalWrite(rightWheelBack, HIGH);
                    break;
                    
                case 'L':
                    Serial.println("Turning left!");
                    digitalWrite(rightWheel, HIGH);      // Only right wheel forward
                    break;
                    
                case 'R':
                    Serial.println("Turning right!");
                    digitalWrite(leftWheel, HIGH);       // Only left wheel forward
                    break;
            }
            
            // Keep motors running for a short duration
            delay(250);
            stopAllMotors();
            Serial.println("Stopped");
        }
    }
    
    void stopAllMotors() {
        digitalWrite(leftWheel, LOW);
        digitalWrite(leftWheelBack, LOW);
        digitalWrite(rightWheel, LOW);
        digitalWrite(rightWheelBack, LOW);
    }
};

void setup() {
    Serial.begin(9600);
    Serial.println("Starting BLE...");
    
    // Configure motor pins
    pinMode(leftWheel, OUTPUT);
    pinMode(leftWheelBack, OUTPUT);
    pinMode(rightWheel, OUTPUT);
    pinMode(rightWheelBack, OUTPUT);
    
    // Test each motor briefly at startup
    Serial.println("Testing motors...");
    
    Serial.println("Testing left forward");
    digitalWrite(leftWheel, HIGH);
    delay(500);
    digitalWrite(leftWheel, LOW);
    
    Serial.println("Testing right forward");
    digitalWrite(rightWheel, HIGH);
    delay(500);
    digitalWrite(rightWheel, LOW);
    
    Serial.println("Testing left backward");
    digitalWrite(leftWheelBack, HIGH);
    delay(500);
    digitalWrite(leftWheelBack, LOW);
    
    Serial.println("Testing right backward");
    digitalWrite(rightWheelBack, HIGH);
    delay(500);
    digitalWrite(rightWheelBack, LOW);
    
    // Initialize BLE
    BLEDevice::init("CodeCell-Car");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    pCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    
    BLEDevice::startAdvertising();
    Serial.println("Ready! Look for 'CodeCell-Car' in web app");
}

void loop() {
    // Restart advertising if disconnected
    if (!deviceConnected) {
        BLEDevice::startAdvertising();
        delay(500);
    }
    delay(20);
}
