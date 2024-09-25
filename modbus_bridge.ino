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
#include "src/ModbusBridgeCallBackManager.hpp"

#define USE_WIFI_MANAGER true
#define SW_AS_RTU true

ModbusRTU rtu;
ModbusTCP tcp;

const int RX = D1;
const int TX = D2;

SoftwareSerial sSerial(RX, TX);

// name the serial ports according to their purpose
#if (SW_AS_RTU)
typedef SoftwareSerial RTUSerial;
typedef HardwareSerial DBGSerial;

RTUSerial& rtu_serial = sSerial;
DBGSerial& dbg_serial = Serial;
#else
typedef HardwareSerial RTUSerial;
typedef SoftwareSerial DBGSerial;

RTUSerial& rtu_serial = Serial;
DBGSerial& dbg_serial = sSerial;
#endif

// set the default configs according to the interface
// types for RTU and DBG
#if (SW_AS_RTU)
SWSConfig rtu_serial_config = SWSERIAL_8N1;
HWSConfig dbg_serial_config = SERIAL_8N1;
#else
HWSConfig rtu_serial_config = SERIAL_8N1;
SWSConfig dbg_serial_config = SWSERIAL_8N1;
#endif

// singleton instance to setup the callbacks
ModbusBridgeCallBackManager<DBGSerial>& callbackManager = ModbusBridgeCallBackManager<DBGSerial>::getInstance();

// default values for when there is no saved configuration
Configs<RTUSerial, DBGSerial> configs(&rtu_serial, 115200, rtu_serial_config,
																			&dbg_serial, 115200, dbg_serial_config);

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
  setup_config_webpage<DBGSerial>(&server, &configs, &dbg_serial);

  // prefer to use the WiFiManager library to connect to WiFi,
  // but hardcoded credentials are also an option
  bool use_wifi_manager = USE_WIFI_MANAGER;
	
	// pass dbg_serial for debugging purposes
  bool wifi_connected = connect_to_wifi<DBGSerial>(dbg_serial, use_wifi_manager);

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

  server.begin();

  tcp.server();

  rtu.begin(&rtu_serial);
  rtu.master();

  // Pass pointers to the objects used by the callbacks
  callbackManager.setup_manager(&rtu, &tcp, &dbg_serial);

	// set the callbacks for the RTU and TCP
	callbackManager.set_rtu_raw_cb();
	callbackManager.set_tcp_raw_cb();
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
