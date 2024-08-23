#include "modbus_bridge_cb.hpp"

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
}

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
  
  dbg_serial->print("received TCP request from ");
  dbg_serial->print(IPAddress(src->ipaddr));
  dbg_serial->printf("| Fn: %d, len: %d\r\n", data[0], len);

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
  
  dbg_serial->printf("received RTU response from %d| Fn: %d , len: %d\r\n", src->slaveId, data[0], len);

  tcp->setTransactionId(transRunning);
  uint16_t succeed = tcp->rawResponce(srcIp, data, len, slaveRunning);
  if (!succeed) {
    dbg_serial->print("failed to");
  }

  dbg_serial->print("respond TCP to client ");
  dbg_serial->println(srcIp);

  transRunning = 0;
  slaveRunning = 0;
  return Modbus::EX_PASSTHROUGH;
}
