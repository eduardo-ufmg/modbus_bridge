#include "wifi_connection.hpp"

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

namespace {
 const unsigned wifi_connection_timeout_seconds = 10;
}

bool wifi_manager_connect(HardwareSerial& dbg_serial)
{
  dbg_serial.println("Connect to WiFi using WiFiManager.");
  WiFiManager wifiManager;
  return wifiManager.autoConnect(WIFI_MANAGER_AP_NAME, WIFI_MANAGER_AP_PASSWORD);
}

bool hardcode_connect(HardwareSerial& dbg_serial)
{
  dbg_serial.print("Connecting to ");
  dbg_serial.println(SSID);

  WiFi.begin(SSID, PASS);

  unsigned elapsed_wifi_connect_seconds = 0;
  while (elapsed_wifi_connect_seconds < wifi_connection_timeout_seconds) {
    dbg_serial.print(".");
    delay(1000);

    elapsed_wifi_connect_seconds ++;

    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
  }

  dbg_serial.println();
  dbg_serial.println("Connection Failed.");

  return false;
}

bool connect_to_wifi(HardwareSerial& dbg_serial, bool use_manager = false)
{
  if (use_manager) {
    return wifi_manager_connect(dbg_serial);
  } else {
    return hardcode_connect(dbg_serial);
  }
}
