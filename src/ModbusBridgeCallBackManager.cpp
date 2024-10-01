#include "ModbusBridgeCallBackManager.hpp"

#include <ModbusTCP.h>
#include <ModbusRTU.h>
#include <SoftwareSerial.h>
#include <map>

template<typename DBG>
bool ModbusBridgeCallBackManager<DBG>::cb_rtu_trans(Modbus::ResultCode event, uint16_t transactionId, void* data)
{
  if (event != Modbus::EX_SUCCESS) {
    dbg_serial->printf("modbusRTU transaction result: %02X\r\n", event);
    current_trans = 0;
  }
  return true;
}

template<typename DBG>
Modbus::ResultCode ModbusBridgeCallBackManager<DBG>::cb_rtu_raw(uint8_t* data, uint8_t len, void* custom)
{
	auto src = static_cast<Modbus::frame_arg_t*>(custom);
  
  String fn = get_function_description(data[0]);

  dbg_serial->printf("received RTU response from %d| Fn: %s, len: %d\r\n",
                                                                          src->slaveId,
                                                                          fn.c_str(),
                                                                          len);

  dbg_serial->printf("content: %s\r\n", format_data(data, len).c_str());

  tcp->setTransactionId(current_trans);
  uint16_t succeed = tcp->rawResponce(src_ip, data, len, active_slave);
  if (!succeed) {
    dbg_serial->print("failed to");
  }

  dbg_serial->printf("respond TCP to client %s\r\n", src_ip.toString().c_str());

  current_trans = 0;
  active_slave = 0;
  return Modbus::EX_PASSTHROUGH;
}

template<typename DBG>
Modbus::ResultCode ModbusBridgeCallBackManager<DBG>::cb_tcp_raw(uint8_t* data, uint8_t len, void* custom)
{
  auto src = static_cast<Modbus::frame_arg_t*>(custom);
  
  String fn = get_function_description(data[0]);

  dbg_serial->printf("received TCP request from %s| Fn: %s, len: %d\r\n",
                                                                        IPAddress(src->ipaddr).toString().c_str(),
                                                                        fn.c_str(),
                                                                        len);

  dbg_serial->printf("content: %s\r\n", format_data(data, len).c_str());

  if (current_trans) {
    tcp->setTransactionId(src->transactionId);
    tcp->errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_SLAVE_DEVICE_BUSY);
    return Modbus::EX_SLAVE_DEVICE_BUSY;
  }

  dbg_serial->printf("request RTU from slave %d\r\n", src->unitId);

  rtu->rawRequest(src->unitId, data, len, [this](Modbus::ResultCode event, uint16_t transactionId, void* data)
		-> bool {return cb_rtu_trans(event, transactionId, data);}
	);
  
  if (!src->unitId) {
    tcp->setTransactionId(src->transactionId);
    tcp->errorResponce(IPAddress(src->ipaddr), (Modbus::FunctionCode)data[0], Modbus::EX_ACKNOWLEDGE);

    current_trans = 0;
    active_slave = 0;
    return Modbus::EX_ACKNOWLEDGE;
  }
  
  src_ip = IPAddress(src->ipaddr);
  active_slave = src->unitId;
  current_trans = src->transactionId;
  
  return Modbus::EX_SUCCESS;  
}

template<typename DBG>
String ModbusBridgeCallBackManager<DBG>::format_data(uint8_t* data, uint8_t len)
{
  String str = "";

  for (uint8_t i = 0; i < len; i++) {
    str += String(data[i], HEX) + " ";
  }

  return str;
}

template<typename DBG>
String ModbusBridgeCallBackManager<DBG>::get_function_description(int data_fn)
{
  auto it = function_descriptions.find(data_fn);
  return it != function_descriptions.end() ? it->second : "Unknown";
}

template<typename DBG>
ModbusBridgeCallBackManager<DBG>& ModbusBridgeCallBackManager<DBG>::getInstance() {
	static ModbusBridgeCallBackManager<DBG> instance;
	return instance;
}

template<typename DBG>
void ModbusBridgeCallBackManager<DBG>::setup_manager(ModbusRTU* ptr_rtu, ModbusTCP* ptr_tcp, DBG* ptr_dbg_serial) {
	rtu = ptr_rtu;
	tcp = ptr_tcp;
	dbg_serial = ptr_dbg_serial;

	active_slave = 0;
	current_trans = 0;
	src_ip = IPAddress(0, 0, 0, 0);
}

template<typename DBG>
bool ModbusBridgeCallBackManager<DBG>::set_rtu_raw_cb() {
	return rtu->onRaw([this](uint8_t* data, uint8_t len, void* custom) -> Modbus::ResultCode {
		return cb_rtu_raw(data, len, custom);
	});
}

template<typename DBG>
bool ModbusBridgeCallBackManager<DBG>::set_tcp_raw_cb() {
	return tcp->onRaw([this](uint8_t* data, uint8_t len, void* custom) -> Modbus::ResultCode {
		return cb_tcp_raw(data, len, custom);
	});
}

template class ModbusBridgeCallBackManager<SoftwareSerial>;
template class ModbusBridgeCallBackManager<HardwareSerial>;
