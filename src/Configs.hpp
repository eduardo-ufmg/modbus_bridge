#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#include <map>
#include <Preferences.h>
#include <SoftwareSerial.h>

#include "SERIAL_TYPES.h"

enum saved_configs {
	RTU_CONFIG = 1 << 0,
	DBG_CONFIG = 1 << 1
};

const std::map<const RTU_SERIAL_CFG_TYPE, const String> sws_dbg_str {
	// debug strings are kept in english because most libraries use english
	// on their logging
	{S8N1, "S8N1"},
	{S8E1, "S8E1"},
	{S8O1, "S8O1"},
	{S8N2, "S8N2"},
	{S8E2, "S8E2"},
	{S8O2, "S8O2"}
};

class RTUConfig {
private:
	RTU_SERIAL_TYPE *rtu_serial;
	unsigned int rtu_baudrate;
	RTU_SERIAL_CFG_TYPE rtu_serial_config;

public:
	void update();

	friend class Configs;
};

class DebugConfig {
private:
	DBG_SERIAL_TYPE *dbg_serial;
	unsigned int dbg_baudrate;
	DBG_SERIAL_CFG_TYPE dbg_serial_config;

	friend class Configs;
};

class Configs {
private:
	Preferences preferences;
	RTUConfig rtu_config;
	DebugConfig dbg_config;

public:
	Configs(RTU_SERIAL_TYPE* rtu_serial, DBG_SERIAL_TYPE* dbg_serial,
					unsigned int rtu_baudrate = 9600, RTU_SERIAL_CFG_TYPE rtu_serial_config = SWSERIAL_8N1,
					unsigned int dbg_baudrate = 115200, DBG_SERIAL_CFG_TYPE dbg_serial_config = SERIAL_8N1);
	~Configs();

	int begin();

	void set_rtu_baudrate(unsigned int baudrate);
	void set_dbg_baudrate(unsigned int baudrate);
	void set_rtu_serial_config(RTU_SERIAL_CFG_TYPE config);
	void set_dbg_serial_config(DBG_SERIAL_CFG_TYPE config);

	unsigned int rtu_baudrate();
	unsigned int dbg_baudrate();
	RTU_SERIAL_CFG_TYPE rtu_serial_config();
	DBG_SERIAL_CFG_TYPE dbg_serial_config();

	void update();
};

#endif
