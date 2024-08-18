#include <ModbusTCP.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

#include <Preferences.h>

#define WEBSERVER_H
#include <ESP8266WebServer.h>
#include <ESPAsyncWebServer.h>

#include "./src/Configs.hpp"
#include "./src/config_webpage.hpp"
#include "./src/wifi_connection.hpp"
#include "./src/modbus_bridge_cb.hpp"

#define USE_WIFI_MANAGER true

ModbusRTU rtu;
ModbusTCP tcp;

SoftwareSerial sSerial(D1, D2);

/**
 * @brief Name serial interfaces according to their purpose.
 */
SoftwareSerial& rtu_serial = sSerial;
HardwareSerial& dbg_serial = Serial;

Configs configs(&rtu_serial, &dbg_serial, 115200, SWSERIAL_8N1, 115200, SERIAL_8N1);

AsyncWebServer server(80);

void setup()
{ 
  configs.begin();

  dbg_serial.begin(configs.dbg_baudrate(), configs.dbg_serial_config());
  rtu_serial.begin(configs.rtu_baudrate(), configs.rtu_serial_config());

  setup_config_webpage(&server, &configs);

  bool use_wifi_manager = USE_WIFI_MANAGER;
  bool wifi_connected = connect_to_wifi(dbg_serial, use_wifi_manager);

  if (wifi_connected) {
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

  // Pass pointers to the objects used by the callbacks.
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
