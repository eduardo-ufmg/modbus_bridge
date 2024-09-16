#ifndef CONFIG_WEBPAGE_HPP
#define CONFIG_WEBPAGE_HPP

#include <map>
#include <ESPAsyncWebServer.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>

#include "Configs.hpp"

#include "OppositeSerial.hpp"

template <typename SI>
void setup_config_webpage(AsyncWebServer* server, Configs<typename OppositeSerial<SI>::Type, SI>* configs, SI* dbg_serial);

#endif
