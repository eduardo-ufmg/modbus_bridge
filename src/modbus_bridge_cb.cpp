#include "modbus_bridge_cb.hpp"

#include <ModbusTCP.h>
#include <ModbusRTU.h>

#include <SoftwareSerial.h>

/**
 * @brief Unamed namespace so that the variables are only visible in this file,
 * 				where they are relevant.
 */
namespace {
	IPAddress srcIp;

	uint16_t transRunning;
	uint8_t slaveRunning;

	ModbusRTU* rtu;
	ModbusTCP* tcp;

	HardwareSerial* dbg_serial;
	SoftwareSerial* rtu_serial;
}

void setup_callbacks(ModbusRTU *ptr_rtu, ModbusTCP *ptr_tcp, SoftwareSerial *ptr_rtu_serial, HardwareSerial *ptr_dbg_serial)
{
	rtu				 = ptr_rtu;
	tcp				 = ptr_tcp;
	rtu_serial = ptr_rtu_serial;
	dbg_serial = ptr_dbg_serial;

	transRunning = 0;
	slaveRunning = 0;
}

/**
 * @brief Callback function for Modbus RTU transaction.
 *
 * This function is called when a Modbus RTU transaction is completed.
 * It checks the event result and updates the transaction status accordingly.
 *
 * @param event The result code of the Modbus RTU transaction.
 * @param transactionId The ID of the transaction.
 * @param data A pointer to the data associated with the transaction.
 * @return true.
 */
bool cbRtuTrans(Modbus::ResultCode event, uint16_t transactionId, void* data)
{
  if (event != Modbus::EX_SUCCESS) {
    dbg_serial->printf("modbusRTU transaction result: %02X\r\n", event);
    transRunning = 0;
  }
  return true;
}

/**
 * Callback function for handling TCP raw requests in the Modbus bridge.
 * 
 * @param data The data received in the TCP request.
 * @param len The length of the data received.
 * @param custom A pointer to custom data passed to the callback function.
 * @return The result code of the Modbus operation.
 */
Modbus::ResultCode cbTcpRaw(uint8_t* data, uint8_t len, void* custom)
{
  auto src = (Modbus::frame_arg_t*) custom;
  
  dbg_serial->print("received TCP request from ");
  dbg_serial->print(IPAddress(src->ipaddr));
  dbg_serial->printf("\t-> Fn: %02X, len: %d\r\n", data[0], len);

  if (transRunning) {
    tcp->setTransactionId(src->transactionId);
    tcp->errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_SLAVE_DEVICE_BUSY);
    return Modbus::EX_SLAVE_DEVICE_BUSY;
  }

  dbg_serial->printf("request RTU:\t%d\r\n", src->unitId);

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

/**
 * @brief Callback function for handling raw RTU responses.
 *
 * This function is called when a raw RTU response is received.
 * It prints the received response information, sets the transaction ID for TCP communication,
 * sends the response over TCP, and updates the transaction and slave IDs. It returns the Modbus result code EX_PASSTHROUGH.
 *
 * @param data Pointer to the received data.
 * @param len Length of the received data.
 * @param custom Pointer to custom data.
 * @return Modbus result code EX_PASSTHROUGH.
 */
Modbus::ResultCode cbRtuRaw(uint8_t* data, uint8_t len, void* custom)
{
  auto src = (Modbus::frame_arg_t*)custom;
  
  dbg_serial->printf("received RTU response from %d\t-> Fn: %02X, len: %d\r\n", src->slaveId, data[0], len);

  tcp->setTransactionId(transRunning);
  uint16_t succeed = tcp->rawResponce(srcIp, data, len, slaveRunning);
  if (!succeed) {
    dbg_serial->print("failed to");
  }

  dbg_serial->print("respond TCP:\t");
  dbg_serial->println(srcIp);

  transRunning = 0;
  slaveRunning = 0;
  return Modbus::EX_PASSTHROUGH;
}
