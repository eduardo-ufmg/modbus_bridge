/*
  Important: This bridge doesn't support multiple
              transactions at the same time.
*/

// if needed, RTU timeout can be changed by
// #define MODBUSRTU_TIMEOUT <NEW_TIMEOUT>
#include <ModbusRTU.h>
#include <ModbusTCP.h>

#include <SoftwareSerial.h>

#include <Preferences.h>

// define WEB_SERVER_H so WiFiManager and ESPAsyncWebServer don't conflict
#define WEBSERVER_H
#include <ESP8266WebServer.h>
#include <ESPAsyncWebServer.h>

#include "src/Configs.hpp"
#include "src/config_webpage.hpp"
#include "src/wifi_connection.hpp"
#include "src/modbus_bridge_cb.hpp"

#define USE_WIFI_MANAGER true

ModbusRTU rtu;
ModbusTCP tcp;

const int RX = D1;
const int TX = D2;

SoftwareSerial sSerial(RX, TX);

// name the serial ports according to their purpose
SoftwareSerial& rtu_serial = sSerial;
HardwareSerial& dbg_serial = Serial;

// default values for when there is no saved configuration
Configs<SoftwareSerial, HardwareSerial> configs(&rtu_serial, 115200, SWSERIAL_8N1,
																								&dbg_serial, 115200, SERIAL_8N1);

AsyncWebServer server(80);

// utility function to check if a bit is set in a value
// couldn't find a better place to put this yet
bool is_bitmask_set(int val, int bit);

void setup()
{ 
  int saved_configs = configs.begin();

  bool rtu_config_saved = is_bitmask_set(saved_configs, saved_configs::RTU_CONFIG);
  bool dbg_config_saved = is_bitmask_set(saved_configs, saved_configs::DBG_CONFIG);

  dbg_serial.begin(configs.dbg_baudrate(), configs.dbg_serial_config());
  rtu_serial.begin(configs.rtu_baudrate(), configs.rtu_serial_config());

  dbg_serial.println("\r\n");

  dbg_serial.printf("There are%s saved RTU configurations.\
											Check the /configure page to view and change them.\r\n",
										rtu_config_saved ? "" : " no");

  dbg_serial.println("\r\n");

  // pass the server so it may be used for another pages in the future
  // pass configs so the same object is used in other places
  // pass dbg_serial for debugging purposes
  // passed as pointer because i'm not sure about references in lambdas
  setup_config_webpage<HardwareSerial>(&server, &configs, &dbg_serial);

  // prefer to use the WiFiManager library to connect to WiFi,
  // but hardcoded credentials are also an option
  bool use_wifi_manager = USE_WIFI_MANAGER;
	
	// pass dbg_serial for debugging purposes
  bool wifi_connected = connect_to_wifi<HardwareSerial>(dbg_serial, use_wifi_manager);

  if (wifi_connected) {
    // WiFiManager has its own logging
    if (!use_wifi_manager) {
      dbg_serial.println();
      dbg_serial.print("WiFi connected! ");
      dbg_serial.print("IP address: ");
      dbg_serial.println(WiFi.localIP());
    }
  } else {
    dbg_serial.println("Couldn't connect. Program will halt.");
    ESP.deepSleep(0);
  }

  // Pass pointers to the objects used by the callbacks
  setup_callbacks(&rtu, &tcp, &rtu_serial, &dbg_serial);

  server.begin();

  tcp.server();
  tcp.onRaw(cbTcpRaw);

  rtu.begin(&rtu_serial);
  rtu.master();
  rtu.onRaw(cbRtuRaw);
}

void loop()
{
  rtu.task();
  tcp.task();
  yield();
}

bool is_bitmask_set(int val, int bit)
{
  return val & bit;
}
