#ifndef WIFI_CONNECTION_HPP
#define WIFI_CONNECTION_HPP

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <SoftwareSerial.h>

#include "credentials.hpp"

template <typename SI>
bool wifi_manager_connect(SI& dbg_serial);

template <typename SI>
bool hardcode_connect(SI& dbg_serial);

template <typename SI>
bool connect_to_wifi(SI& dbg_serial, bool use_manager = false);

#endif
