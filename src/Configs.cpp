#include "Configs.hpp"

#include <Preferences.h>
#include <SoftwareSerial.h>

namespace {
	const char *prefs_name = "configs";
	const char *rtu_key = "rtu_configs";
	const char *dbg_key = "dbg_configs";
}

void RTUConfig::update() {
	rtu_serial->end();
	rtu_serial->begin(rtu_baudrate, rtu_serial_config);
}

Configs::Configs(SoftwareSerial* rtu_serial, HardwareSerial* dbg_serial,
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

int Configs::begin()
{
	int saved_config = 0;

	preferences.begin(prefs_name, false);

	if (preferences.isKey(rtu_key)) {
		saved_config |= saved_configs::RTU_CONFIG;
	} else {
		preferences.putBytes(rtu_key, &rtu_config, sizeof(RTUConfig));
	}

	if (preferences.isKey(dbg_key)) {
		saved_config |= saved_configs::DBG_CONFIG;
	} else {
		preferences.putBytes(dbg_key, &dbg_config, sizeof(DebugConfig));
	}

	preferences.getBytes(rtu_key, &rtu_config, sizeof(RTUConfig));
	preferences.getBytes(dbg_key, &dbg_config, sizeof(DebugConfig));

	return saved_config;
}

void Configs::set_rtu_baudrate(unsigned int baudrate)
{
	rtu_config.rtu_baudrate = baudrate;
	preferences.putBytes(rtu_key, &rtu_config, sizeof(RTUConfig));
}

void Configs::set_dbg_baudrate(unsigned int baudrate)
{
	dbg_config.dbg_baudrate = baudrate;
	preferences.putBytes(dbg_key, &dbg_config, sizeof(DebugConfig));
}

void Configs::set_rtu_serial_config(EspSoftwareSerial::Config config)
{
	rtu_config.rtu_serial_config = config;
	preferences.putBytes(rtu_key, &rtu_config, sizeof(RTUConfig));
}

void Configs::set_dbg_serial_config(SerialConfig config)
{
	dbg_config.dbg_serial_config = config;
	preferences.putBytes(dbg_key, &dbg_config, sizeof(DebugConfig));
}

unsigned int Configs::rtu_baudrate()
{
	return rtu_config.rtu_baudrate;
}

unsigned int Configs::dbg_baudrate()
{
	return dbg_config.dbg_baudrate;
}

EspSoftwareSerial::Config Configs::rtu_serial_config()
{
	return rtu_config.rtu_serial_config;
}

SerialConfig Configs::dbg_serial_config()
{
	return dbg_config.dbg_serial_config;
}

void Configs::update()
{
	rtu_config.update();
}
