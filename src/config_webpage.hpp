#ifndef CONFIG_WEBPAGE_HPP
#define CONFIG_WEBPAGE_HPP

#include <map>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include "Configs.hpp"

void setup_config_webpage(AsyncWebServer* server, Configs* configs, HardwareSerial* dbg_serial);

#endif
