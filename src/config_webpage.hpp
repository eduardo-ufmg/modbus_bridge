#ifndef CONFIG_WEBPAGE_HPP
#define CONFIG_WEBPAGE_HPP

#include <map>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include "Configs.hpp"

#include "SERIAL_TYPES.h"

void setup_config_webpage(AsyncWebServer* server, Configs* configs, DBG_SERIAL_TYPE* dbg_serial);

#endif
