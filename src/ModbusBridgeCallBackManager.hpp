#ifndef MODBUS_BRIDGE_CB_MNGR_HPP
#define MODBUS_BRIDGE_CB_MNGR_HPP

#include <ModbusTCP.h>
#include <ModbusRTU.h>
#include <SoftwareSerial.h>
#include <map>

template<typename DBG>
class ModbusBridgeCallBackManager {
private:
	ModbusRTU* rtu;
	ModbusTCP* tcp;

	DBG* dbg_serial;

	IPAddress src_ip;
	uint16_t current_trans;
	uint8_t active_slave;

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

	ModbusBridgeCallBackManager() = default;
	ModbusBridgeCallBackManager(const ModbusBridgeCallBackManager&) = delete;
	ModbusBridgeCallBackManager& operator=(const ModbusBridgeCallBackManager&) = delete;
	~ModbusBridgeCallBackManager() = default;

	bool cb_rtu_trans(Modbus::ResultCode event, uint16_t transactionId, void* data);
	Modbus::ResultCode cb_rtu_raw(uint8_t* data, uint8_t len, void* custom);
	Modbus::ResultCode cb_tcp_raw(uint8_t* data, uint8_t len, void* custom);
	
	String format_data(uint8_t* data, uint8_t len);
	String get_function_description(int data_fn);

public:
	static ModbusBridgeCallBackManager& getInstance();

	void setup_manager(ModbusRTU* ptr_rtu, ModbusTCP* ptr_tcp, DBG* ptr_dbg_serial);
	bool set_rtu_raw_cb();
	bool set_tcp_raw_cb();
};

#endif // MODBUS_BRIDGE_CB_MNGR_HPP
