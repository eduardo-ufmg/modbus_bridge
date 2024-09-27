#ifndef MODBUS_BRIDGE_HPP
#define MODBUS_BRIDGE_HPP

#include <ModbusTCP.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

#include "SERIAL_TYPES.h"

void setup_callbacks(ModbusRTU* rtu, ModbusTCP* tcp, RTU_SERIAL_TYPE* rtu_serial, DBG_SERIAL_TYPE* dbg_serial);

bool cbRtuTrans(Modbus::ResultCode event, uint16_t transactionId, void* data);
Modbus::ResultCode cbTcpRaw(uint8_t* data, uint8_t len, void* custom);
Modbus::ResultCode cbRtuRaw(uint8_t* data, uint8_t len, void* custom);

#endif