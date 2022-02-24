/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#ifdef ARDUINO_ARCH_ESP32

#include "../../inc/mvCNCConfigPre.h"

#if BOTH(WIFISUPPORT, OTASUPPORT)

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <driver/timer.h>

void OTA_init() {
  ArduinoOTA
    .onStart([]() {
      timer_pause(TIMER_GROUP_0, TIMER_0);
      timer_pause(TIMER_GROUP_0, TIMER_1);

      // U_FLASH or U_SPIFFS
      String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      char *str;
      switch (error) {
        case OTA_AUTH_ERROR:    str = "Auth Failed";    break;
        case OTA_BEGIN_ERROR:   str = "Begin Failed";   break;
        case OTA_CONNECT_ERROR: str = "Connect Failed"; break;
        case OTA_RECEIVE_ERROR: str = "Receive Failed"; break;
        case OTA_END_ERROR:     str = "End Failed";     break;
      }
      Serial.println(str);
    });

  ArduinoOTA.begin();
}

void OTA_handle() {
  ArduinoOTA.handle();
}

#endif // WIFISUPPORT && OTASUPPORT
#endif // ARDUINO_ARCH_ESP32
