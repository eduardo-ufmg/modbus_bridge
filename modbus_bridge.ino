#include <ESP8266WiFi.h>
#include "credentials.hpp"

#include <ModbusTCP.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

const unsigned long baudrate = 115200;
const unsigned      wifi_connection_timeout_seconds = 10;

ModbusRTU rtu;
ModbusTCP tcp;

IPAddress srcIp;

SoftwareSerial sSerial(D1, D2);

SoftwareSerial&   rtu_serial = sSerial;
HardwareSerial& debug_serial = Serial;

uint16_t transRunning = 0;
uint8_t slaveRunning = 0;

bool cbRtuTrans(Modbus::ResultCode event, uint16_t transactionId, void* data)
{
  if (event != Modbus::EX_SUCCESS) {
    debug_serial.printf("modbusRTU transaction result: %02X\r\n", event);
    transRunning = 0;
  }
  return true;
}

Modbus::ResultCode cbTcpRaw(uint8_t* data, uint8_t len, void* custom)
{
  auto src = (Modbus::frame_arg_t*) custom;
  
  debug_serial.print("received TCP request from ");
  debug_serial.print(IPAddress(src->ipaddr));
  debug_serial.printf("\t-> Fn: %02X, len: %d\r\n", data[0], len);

  if (transRunning) {
    tcp.setTransactionId(src->transactionId);
    tcp.errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_SLAVE_DEVICE_BUSY);
    return Modbus::EX_SLAVE_DEVICE_BUSY;
  }

  debug_serial.printf("request RTU:\t%d\r\n", src->unitId);

  rtu.rawRequest(src->unitId, data, len, cbRtuTrans);
  
  if (!src->unitId) {
    tcp.setTransactionId(src->transactionId);
    tcp.errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_ACKNOWLEDGE);

    transRunning = 0;
    slaveRunning = 0;
    return Modbus::EX_ACKNOWLEDGE;
  }
  
  srcIp = IPAddress(src->ipaddr);
  slaveRunning = src->unitId;
  transRunning = src->transactionId;
  
  return Modbus::EX_SUCCESS;  
}

Modbus::ResultCode cbRtuRaw(uint8_t* data, uint8_t len, void* custom)
{
  auto src = (Modbus::frame_arg_t*)custom;
  
  debug_serial.printf("received RTU response from %d\t-> Fn: %02X, len: %d\r\n", src->slaveId, data[0], len);

  tcp.setTransactionId(transRunning);
  uint16_t succeed = tcp.rawResponce(srcIp, data, len, slaveRunning);
  if (!succeed) {
    debug_serial.print("failed to");
  }

  debug_serial.print("respond TCP:\t");
  debug_serial.println(srcIp);

  transRunning = 0;
  slaveRunning = 0;
  return Modbus::EX_PASSTHROUGH;
}

bool connect_to_wifi()
{
  debug_serial.print("Connecting to ");
  debug_serial.println(SSID);

  WiFi.begin(SSID, PASS);

  unsigned elapsed_wifi_connect_seconds = 0;
  while (elapsed_wifi_connect_seconds < wifi_connection_timeout_seconds) {
    debug_serial.print(".");
    delay(1000);

    elapsed_wifi_connect_seconds ++;

    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
  }

  debug_serial.println();
  debug_serial.println("Connection Failed.");

  return false;
}

void setup()
{
  rtu_serial.begin(baudrate, SWSERIAL_8N1);
  debug_serial.begin(baudrate, SERIAL_8N1);

  bool wifi_connected = connect_to_wifi();

  if (wifi_connected) {
    debug_serial.println();
    debug_serial.print("WiFi connected! ");
    debug_serial.print("IP address: ");
    debug_serial.println(WiFi.localIP());
  } else {
    debug_serial.println("Couldn't connect. Program will halt.");
    ESP.deepSleep(0);
  }

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