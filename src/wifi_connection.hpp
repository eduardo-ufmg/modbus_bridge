#ifndef WIFI_CONNECTION_HPP
#define WIFI_CONNECTION_HPP

#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "credentials.hpp"

#include "SERIAL_TYPES.h"

bool wifi_manager_connect(DBG_SERIAL_TYPE& dbg_serial);
bool hardcode_connect(DBG_SERIAL_TYPE& dbg_serial);
bool connect_to_wifi(DBG_SERIAL_TYPE& dbg_serial, bool use_manager);

#endif
