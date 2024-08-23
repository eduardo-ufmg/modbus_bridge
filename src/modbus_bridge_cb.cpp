#include "modbus_bridge_cb.hpp"

#include <map>
#include <ModbusTCP.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

namespace {
	IPAddress srcIp;

	uint16_t transRunning;
	uint8_t slaveRunning;

	ModbusRTU* rtu;
	ModbusTCP* tcp;

	HardwareSerial* dbg_serial;
	SoftwareSerial* rtu_serial;

  const std::map<const int, const String> function_descriptions {
    {01, "Read Coils"},
    {02, "Read Discrete Inputs"},
    {03, "Read Holding Registers"},
    {04, "Read Input Registers"},
    {05, "Write Single Coil"},
    {06, "Write Single Register"},
    {15, "Write Multiple Coils"},
    {16, "Write Multiple Registers"},
    {23, "Read/Write Multiple Registers"}
  };
}

String format_data(uint8_t* data, uint8_t len);
String get_function_description(int data_fn);

void setup_callbacks(ModbusRTU* ptr_rtu, ModbusTCP* ptr_tcp, SoftwareSerial* ptr_rtu_serial, HardwareSerial* ptr_dbg_serial)
{
	rtu				 = ptr_rtu;
	tcp				 = ptr_tcp;
	rtu_serial = ptr_rtu_serial;
	dbg_serial = ptr_dbg_serial;

	transRunning = 0;
	slaveRunning = 0;
}

bool cbRtuTrans(Modbus::ResultCode event, uint16_t transactionId, void* data)
{
  if (event != Modbus::EX_SUCCESS) {
    dbg_serial->printf("modbusRTU transaction result: %02X\r\n", event);
    transRunning = 0;
  }
  return true;
}

Modbus::ResultCode cbTcpRaw(uint8_t* data, uint8_t len, void* custom)
{
  auto src = (Modbus::frame_arg_t*) custom;
  
  String fn = get_function_description(data[0]);

  dbg_serial->printf("received TCP request from %s| Fn: %s, len: %d\r\n",
                                                                        IPAddress(src->ipaddr).toString().c_str(),
                                                                        fn.c_str(),
                                                                        len);

  dbg_serial->printf("content: %s\r\n", format_data(data, len).c_str());

  if (transRunning) {
    tcp->setTransactionId(src->transactionId);
    tcp->errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_SLAVE_DEVICE_BUSY);
    return Modbus::EX_SLAVE_DEVICE_BUSY;
  }

  dbg_serial->printf("request RTU from slave %d\r\n", src->unitId);

  rtu->rawRequest(src->unitId, data, len, cbRtuTrans);
  
  if (!src->unitId) {
    tcp->setTransactionId(src->transactionId);
    tcp->errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_ACKNOWLEDGE);

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
  
  String fn = get_function_description(data[0]);

  dbg_serial->printf("received RTU response from %d| Fn: %s, len: %d\r\n",
                                                                          src->slaveId,
                                                                          fn.c_str(),
                                                                          len);

  dbg_serial->printf("content: %s\r\n", format_data(data, len).c_str());

  tcp->setTransactionId(transRunning);
  uint16_t succeed = tcp->rawResponce(srcIp, data, len, slaveRunning);
  if (!succeed) {
    dbg_serial->print("failed to");
  }

  dbg_serial->printf("respond TCP to client %s\r\n", srcIp.toString().c_str());

  transRunning = 0;
  slaveRunning = 0;
  return Modbus::EX_PASSTHROUGH;
}

String format_data(uint8_t* data, uint8_t len)
{
  String str = "";

  for (uint8_t i = 0; i < len; i++) {
    str += String(data[i], HEX) + " ";
  }

  return str;
}

String get_function_description(int data_fn)
{
  auto it = function_descriptions.find(data_fn);
  
  return it != function_descriptions.end() ? it->second : "Unknown";
}
