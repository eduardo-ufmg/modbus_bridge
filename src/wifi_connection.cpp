#include "wifi_connection.hpp"

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <SoftwareSerial.h>

namespace {
 const unsigned wifi_connection_timeout_seconds = 10;
}

template <typename SI>
bool wifi_manager_connect(SI& dbg_serial)
{
  dbg_serial.println("Connect to WiFi using WiFiManager.");
  WiFiManager wifiManager(dbg_serial);
  return wifiManager.autoConnect(WIFI_MANAGER_AP_NAME, WIFI_MANAGER_AP_PASSWORD);
}

template <typename SI>
bool hardcode_connect(SI& dbg_serial)
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

template <typename SI>
bool connect_to_wifi(SI& dbg_serial, bool use_manager)
{
  if (use_manager) {
    return wifi_manager_connect(dbg_serial);
  } else {
    return hardcode_connect(dbg_serial);
  }
}

template bool wifi_manager_connect<HardwareSerial>(HardwareSerial& dbg_serial);
template bool hardcode_connect<HardwareSerial>(HardwareSerial& dbg_serial);
template bool connect_to_wifi<HardwareSerial>(HardwareSerial& dbg_serial, bool use_manager = false);

template bool wifi_manager_connect<SoftwareSerial>(SoftwareSerial& dbg_serial);
template bool hardcode_connect<SoftwareSerial>(SoftwareSerial& dbg_serial);
template bool connect_to_wifi<SoftwareSerial>(SoftwareSerial& dbg_serial, bool use_manager = false);
