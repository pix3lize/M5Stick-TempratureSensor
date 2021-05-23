# M5Stick Temprature Tracker
 M5Stick temprature tracker communicate with Xiaomi Temprature (custom sensor) sensor through low energy bluetooth connection with NIMBLE library.

 Please refer this link on how to activate xiaomi bluetooth custom firmware 
 https://hackaday.com/2020/12/08/exploring-custom-firmware-on-xiaomi-thermometers/

 The data can be use to communicate with MQTT server (future development).
 
![](m5temp.gif)

## To build and upload into M5Stick 
 1. Download PlatformIO on your visual studio code 
 2. For reference M5Stick please refer to <b>platformio.ini</b> file 
 3. Please refer to the image below

Build and upload to M5Stick
 ![](platformio-build-tempsensor.gif)

## PlatformIO  
M5StickPlus platfomio.ini config : 
```ini
[env]
upload_speed = 115200
monitor_speed = 115200

[env:m5stick-c]
platform = espressif32
board = m5stick-c
framework = arduino
lib_deps = 
  h2zero/NimBLE-Arduino@^1.1.0
  https://github.com/m5stack/M5StickC-Plus.git
```
M5Stack Core2 platfomio.ini config : 
```ini
[env]
upload_speed = 115200
monitor_speed = 115200

[env:m5stick-c]
platform = espressif32
board = m5stick-c
framework = arduino
board_build.partitions = default_16MB.csv
build_flags =
    -DBOARD_HAS_PSRAM
    -mfix-esp32-psram-cache-issue
lib_deps = 
  https://github.com/m5stack/M5Core2
```