#include "wifi_connection.hpp"

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

namespace {
 const unsigned wifi_connection_timeout_seconds = 10;
}

/**
 * Connects to WiFi using WiFiManager.
 * 
 * @param dbg_serial The hardware serial object used for dbgging.
 * @return True if the connection is successful, false otherwise.
 */
bool wifi_manager_connect(HardwareSerial& dbg_serial)
{
  dbg_serial.println("Connect to WiFi using WiFiManager.");
  dbg_serial.printf("Access Point: %s, Password: %s, IP: %s\r\n",
                        WIFI_MANAGER_AP_NAME, WIFI_MANAGER_AP_PASSWORD, "192.168.4.1"/*I*/);
  WiFiManager wifiManager;
  return wifiManager.autoConnect(WIFI_MANAGER_AP_NAME, WIFI_MANAGER_AP_PASSWORD);
}

/**
 * Connects to a WiFi network using hardcoded SSID and password.
 * 
 * @param dbg_serial The hardware serial object used for dbgging.
 * @return True if the connection is successful, false otherwise.
 */
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

/**
 * @brief Connects to WiFi using either WiFi Manager or hardcoded credentials.
 *
 * This function attempts to connect to WiFi using either WiFi Manager or hardcoded credentials,
 * depending on the value of the `use_manager` parameter.
 *
 * @param dbg_serial The hardware serial object used for dbgging.
 * @param use_manager  A boolean flag indicating whether to use WiFi Manager or not.
 *
 * @return `true` if the connection is successful, `false` otherwise.
 */
bool connect_to_wifi(HardwareSerial& dbg_serial, bool use_manager = false)
{
  if (use_manager) {
    return wifi_manager_connect(dbg_serial);
  } else {
    return hardcode_connect(dbg_serial);
  }
}

/*
I TODO: get IP address from WiFiManager, instead of hardcoding it.
*/
