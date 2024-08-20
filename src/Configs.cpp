#include "Configs.hpp"

#include <Preferences.h>
#include <SoftwareSerial.h>

void RTUConfig::update() {
	rtu_serial->end();
	rtu_serial->begin(rtu_baudrate, rtu_serial_config);
}

Configs::Configs(SoftwareSerial *rtu_serial, HardwareSerial *dbg_serial,
									unsigned int rtu_baudrate, EspSoftwareSerial::Config rtu_serial_config,
									unsigned int dbg_baudrate, SerialConfig dbg_serial_config)
{
	rtu_config.rtu_serial 			 = rtu_serial;
	rtu_config.rtu_baudrate			 = rtu_baudrate;
	rtu_config.rtu_serial_config = rtu_serial_config;

	dbg_config.dbg_serial				 = dbg_serial;
	dbg_config.dbg_baudrate			 = dbg_baudrate;
	dbg_config.dbg_serial_config = dbg_serial_config;
}

Configs::~Configs()
{
	preferences.end();
}

void Configs::begin()
{
	preferences.begin("configs", false);

	if (not preferences.isKey("rtu_configs")) {
		preferences.putBytes("rtu_configs", &rtu_config, sizeof(RTUConfig));
	}

	if (not preferences.isKey("dbg_configs")) {
		preferences.putBytes("dbg_configs", &dbg_config, sizeof(DebugConfig));
	}
}

void Configs::set_rtu_baudrate(unsigned int baudrate)
{
	rtu_config.rtu_baudrate = baudrate;
	preferences.putBytes("rtu_configs", &rtu_config, sizeof(RTUConfig));
}

void Configs::set_dbg_baudrate(unsigned int baudrate)
{
	dbg_config.dbg_baudrate = baudrate;
	preferences.putBytes("dbg_configs", &dbg_config, sizeof(DebugConfig));
}

void Configs::set_rtu_serial_config(EspSoftwareSerial::Config config)
{
	rtu_config.rtu_serial_config = config;
	preferences.putBytes("rtu_configs", &rtu_config, sizeof(RTUConfig));
}

void Configs::set_dbg_serial_config(SerialConfig config)
{
	dbg_config.dbg_serial_config = config;
	preferences.putBytes("dbg_configs", &dbg_config, sizeof(DebugConfig));
}

unsigned int Configs::rtu_baudrate()
{
	preferences.getBytes("rtu_configs", &rtu_config, sizeof(RTUConfig));
	return rtu_config.rtu_baudrate;
}

unsigned int Configs::dbg_baudrate()
{
	preferences.getBytes("dbg_configs", &dbg_config, sizeof(DebugConfig));
	return dbg_config.dbg_baudrate;
}

EspSoftwareSerial::Config Configs::rtu_serial_config()
{
	preferences.getBytes("rtu_configs", &rtu_config, sizeof(RTUConfig));
	return rtu_config.rtu_serial_config;
}

SerialConfig Configs::dbg_serial_config()
{
	preferences.getBytes("dbg_configs", &dbg_config, sizeof(DebugConfig));
	return dbg_config.dbg_serial_config;
}

void Configs::update()
{
	rtu_config.update();
}
