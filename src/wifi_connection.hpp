#ifndef WIFI_CONNECTION_HPP
#define WIFI_CONNECTION_HPP

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "credentials.hpp"

bool wifi_manager_connect(HardwareSerial& dbg_serial);
bool hardcode_connect(HardwareSerial& dbg_serial);
bool connect_to_wifi(HardwareSerial& dbg_serial, bool use_manager);

#endif
