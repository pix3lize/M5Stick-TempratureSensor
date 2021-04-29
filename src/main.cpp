/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

/** NimBLE differences highlighted in comment blocks **/

/*******original********
  #include <BLEDevice.h>
  #include <BLEUtils.h>
  #include <BLEScan.h>
  #include <BLEAdvertisedDevice.h>
  #include "BLEEddystoneURL.h"
  #include "BLEEddystoneTLM.h"
  #include "BLEBeacon.h"
***********************/

#include <Arduino.h>
#include <M5StickCPlus.h>
// #include <iostream>
// #include <string>
// using namespace std;

#include <NimBLEDevice.h>
#include <NimBLEAdvertisedDevice.h>
#include "NimBLEEddystoneURL.h"
#include "NimBLEEddystoneTLM.h"
#include "NimBLEBeacon.h"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xFF00) >> 8) + (((x)&0xFF) << 8))

int scanTime = 5; //In seconds
BLEScan *pBLEScan;

String room(String code)
{
  if(code=="ATC_8FA2DB")
  {
    return "COMP2 ROOM";
  }
  else if(code=="ATC_B3204D")
  {
    return "COMP1 ROOM";
  }
    else if(code=="ATC_C951AF")
  {
    return "LIVING ROOM";
  }
  else{
    return "BED ROOM";
  }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /*** Only a reference to the advertised device is passed now
      void onResult(BLEAdvertisedDevice advertisedDevice) { **/
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
      if (advertisedDevice->haveName())
      {
        if(advertisedDevice->getName().find("ATC_") != std::string::npos)
        {
          Serial.println("");
          M5.Lcd.setTextSize(2);
          M5.Lcd.setTextColor(RED);
          M5.Lcd.println("");          
          Serial.print("Device name: ");
          M5.Lcd.println(room(advertisedDevice->getName().c_str()));
          Serial.println(advertisedDevice->getName().c_str());        
          Serial.print("RSSI:");
          Serial.println(advertisedDevice->getRSSI());

          BLEUUID devUUID = advertisedDevice->getServiceDataUUID();
          Serial.print("Found Service Data UUID: ");
          Serial.println(devUUID.toString().c_str());          
          
          std::string serviceData = advertisedDevice->getServiceData(devUUID);
          char buffer [30];
          char tempbuffer [30];

          for (int idx = 0; idx < serviceData.length(); idx++)
          {
            //Serial.printf("0x%02X ", serviceData[idx]);
            Serial.printf("[%02X]", serviceData[idx]);
            int loc = idx *2;
            sprintf(&buffer[loc], "%02X", serviceData[idx]);
            // sprintf(buffer+strlen[buffer], "%02X", serviceData[idx]);
            // Serial.print(serviceData[idx]);
          }
          Serial.println("");
          Serial.print("Buffer : ");
          Serial.print(buffer);

          Serial.println("");
          memcpy(tempbuffer, buffer+12, 4); 
          int d = strtol(tempbuffer, NULL, 16);
          float temperature = (float)d / (float)10;
          M5.Lcd.setTextColor(LIGHTGREY);
          M5.Lcd.println("Temprature");
          Serial.print("Temprature : ");
          M5.Lcd.setTextSize(3);
          M5.Lcd.setTextColor(GREEN);
          M5.Lcd.println(temperature);
          Serial.print(temperature);

          memset(tempbuffer, 0, sizeof(tempbuffer));

          Serial.println("");
          memcpy(tempbuffer, buffer+16, 2);
          d = strtol(tempbuffer, NULL, 16);
          float humidity = (float)d;
          M5.Lcd.setTextSize(2);
          M5.Lcd.setTextColor(LIGHTGREY);
          M5.Lcd.println("Humidity");
          Serial.print("Humidity : ");
          Serial.print(humidity);
          M5.Lcd.setTextSize(3);
          M5.Lcd.setTextColor(ORANGE);
          M5.Lcd.print(humidity);
          M5.Lcd.println("");

          Serial.println("");
          std::string strManufacturerData = advertisedDevice->getManufacturerData();
          uint8_t cManufacturerData[100];
          Serial.printf("Device strManufacturerData: %d ", strManufacturerData.length());
            for (int i = 0; i < strManufacturerData.length(); i++)
            {
              Serial.printf("[%X]", cManufacturerData[i]);
            }
          Serial.printf("\n");
        }

      }
    
    }
};

void setup()
{
  M5.begin();

  // text print
  M5.Lcd.fillScreen(BLACK);
  //M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Display Test!");
  M5.Axp.ScreenBreath(9);


  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  // Serial.print("Devices found: ");
  // Serial.println(foundDevices.getCount());
  // Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}
