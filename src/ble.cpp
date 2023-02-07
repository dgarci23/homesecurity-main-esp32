#include <ble.h>

void createBLE() {
    Serial.println("Starting BLE work!");
    BLEDevice::init(BLE_NAME); // Create BLE device
    BLEServer *pServer = BLEDevice::createServer(); // define as server
    createBLEService(pServer);
    advertiseBLEService(pServer);
}

void createBLEService(BLEServer *pServer) {
    BLEService *pService = pServer->createService(BLE_ID); // create the service with UUID
    BLECharacteristic *pCharacteristic = pService->createCharacteristic( // 
                                            CHARACTERISTIC_UUID,
                                            BLECharacteristic::PROPERTY_READ |
                                            BLECharacteristic::PROPERTY_WRITE | 
                                            BLECharacteristic::PROPERTY_NOTIFY
                                        );
    pCharacteristic->setValue("Hello World says Neil");
    pService->start();
}

void advertiseBLEService(BLEServer *pServer) {
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_ID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
    while (true) {
        BLEService *pService = pServer->getServiceByUUID(BLE_ID);
        BLECharacteristic *pCharacteristic = pService->getCharacteristic(CHARACTERISTIC_UUID);
        pCharacteristic->notify();
        delay(2000);
    }
}

/*void setup() {
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_ID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}*/