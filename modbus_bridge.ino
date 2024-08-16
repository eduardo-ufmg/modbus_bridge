#include <ModbusTCP.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

#include "./src/wifi_connection.hpp"
#include "./src/modbus_bridge_cb.hpp"

#define USE_WIFI_MANAGER true

const unsigned long rtu_baudrate = 115200;
const unsigned long dbg_baudrate = 115200;

ModbusRTU rtu;
ModbusTCP tcp;

/**
 * @brief Create a software serial object for communication.
 *
 * @param rxPin The pin to receive data.
 * @param txPin The pin to transmit data.
 */
SoftwareSerial sSerial(D1, D2);

/**
 * @brief Name serial interfaces according to their purpose.
 */
SoftwareSerial&   rtu_serial = sSerial;
HardwareSerial& debug_serial = Serial;

void setup()
{
  rtu_serial.begin(rtu_baudrate, SWSERIAL_8N1);
  debug_serial.begin(dbg_baudrate, SERIAL_8N1);

  bool wifi_connected = connect_to_wifi(debug_serial, USE_WIFI_MANAGER);

  if (wifi_connected) {
    debug_serial.println();
    debug_serial.print("WiFi connected! ");
    debug_serial.print("IP address: ");
    debug_serial.println(WiFi.localIP());
  } else {
    debug_serial.println("Couldn't connect. Program will halt.");
    ESP.deepSleep(0);
  }

  // Pass pointers to the objects used by the callbacks.
  setup_callbacks(&rtu, &tcp, &rtu_serial, &debug_serial);

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
