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
          Serial.print("Device name: ");
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
          Serial.print("Temprature : ");
          Serial.print(temperature);

          memset(tempbuffer, 0, sizeof(tempbuffer));

          Serial.println("");
          memcpy(tempbuffer, buffer+16, 2);
          d = strtol(tempbuffer, NULL, 16);
          float humidity = (float)d;
          Serial.print("Humidity : ");
          Serial.print(humidity);

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

      // if (advertisedDevice->haveServiceUUID())
      // {
      //   BLEUUID devUUID = advertisedDevice->getServiceUUID();
      //   Serial.print("Found ServiceUUID: ");
      //   Serial.println(devUUID.toString().c_str());
      //   Serial.println("");
      // }
      // else
      // {
      //   if (advertisedDevice->haveManufacturerData() == true)
      //   {
      //     std::string strManufacturerData = advertisedDevice->getManufacturerData();

      //     uint8_t cManufacturerData[100];
      //     strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

      //     if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00)
      //     {
      //       Serial.println("Found an iBeacon!");
      //       BLEBeacon oBeacon = BLEBeacon();
      //       oBeacon.setData(strManufacturerData);
      //       Serial.printf("iBeacon Frame\n");
      //       Serial.printf("ID: %04X Major: %d Minor: %d UUID: %s Power: %d\n", oBeacon.getManufacturerId(), ENDIAN_CHANGE_U16(oBeacon.getMajor()), ENDIAN_CHANGE_U16(oBeacon.getMinor()), oBeacon.getProximityUUID().toString().c_str(), oBeacon.getSignalPower());
      //     }
      //     else
      //     {
      //       Serial.println("Found another manufacturers beacon!");
      //       Serial.printf("strManufacturerData: %d ", strManufacturerData.length());
      //       for (int i = 0; i < strManufacturerData.length(); i++)
      //       {
      //         Serial.printf("[%X]", cManufacturerData[i]);
      //       }
      //       Serial.printf("\n");
      //     }
      //   }
      //   return;
      // }

      // BLEUUID eddyUUID = (uint16_t)0xfeaa;

      // if (advertisedDevice->getServiceUUID().equals(eddyUUID))
      // {
      //   std::string serviceData = advertisedDevice->getServiceData(eddyUUID);
      //   if (serviceData[0] == 0x10)
      //   {
      //     Serial.println("Found an EddystoneURL beacon!");
      //     BLEEddystoneURL foundEddyURL = BLEEddystoneURL();

      //     foundEddyURL.setData(serviceData);
      //     std::string bareURL = foundEddyURL.getURL();
      //     if (bareURL[0] == 0x00)
      //     {
      //       Serial.println("DATA-->");
      //       for (int idx = 0; idx < serviceData.length(); idx++)
      //       {
      //         Serial.printf("0x%08X ", serviceData[idx]);
      //       }
      //       Serial.println("\nInvalid Data");
      //       return;
      //     }

      //     Serial.printf("Found URL: %s\n", foundEddyURL.getURL().c_str());
      //     Serial.printf("Decoded URL: %s\n", foundEddyURL.getDecodedURL().c_str());
      //     Serial.printf("TX power %d\n", foundEddyURL.getPower());
      //     Serial.println("\n");
      //   }
      //   else if (serviceData[0] == 0x20)
      //   {
      //     Serial.println("Found an EddystoneTLM beacon!");
      //     BLEEddystoneTLM foundEddyURL = BLEEddystoneTLM();
      //     foundEddyURL.setData(serviceData);

      //     Serial.printf("Reported battery voltage: %dmV\n", foundEddyURL.getVolt());
      //     Serial.printf("Reported temperature from TLM class: %.2fC\n", (double)foundEddyURL.getTemp());
      //     int temp = (int)serviceData[5] + (int)(serviceData[4] << 8);
      //     float calcTemp = temp / 256.0f;
      //     Serial.printf("Reported temperature from data: %.2fC\n", calcTemp);
      //     Serial.printf("Reported advertise count: %d\n", foundEddyURL.getCount());
      //     Serial.printf("Reported time since last reboot: %ds\n", foundEddyURL.getTime());
      //     Serial.println("\n");
      //     Serial.print(foundEddyURL.toString().c_str());
      //     Serial.println("\n");
      //   }
      // }
    
    }
};

void setup()
{
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
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  // Serial.print("Devices found: ");
  // Serial.println(foundDevices.getCount());
  // Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}
