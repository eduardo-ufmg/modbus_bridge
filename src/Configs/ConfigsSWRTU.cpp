#include "ConfigsSWRTU.hpp"
#include "ConfigsTemplate.hpp"

#include <map>
#include <Preferences.h>
#include <HardwareSerial.h>

template <>
Configs<SoftwareSerial, HardwareSerial>::
		Configs(SoftwareSerial *rtu_serial, unsigned int rtu_baudrate, SWSConfig rtu_serial_config,
						HardwareSerial *dbg_serial, unsigned int dbg_baudrate, HWSConfig dbg_serial_config) {

	rtu_config.serial = rtu_serial;
	rtu_config.baudrate = rtu_baudrate;
	rtu_config.serial_config = rtu_serial_config;

	dbg_config.dbg_serial = dbg_serial;
	dbg_config.dbg_baudrate = dbg_baudrate;
	dbg_config.dbg_serial_config = dbg_serial_config;
}

template <>
void Configs<SoftwareSerial, HardwareSerial>::rtu_serial_config(SWSConfig config) {
	rtu_config.serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
}

template <>
void Configs<SoftwareSerial, HardwareSerial>::dbg_serial_config(HWSConfig config) {
	dbg_config.dbg_serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
}

template <>
SWSConfig Configs<SoftwareSerial, HardwareSerial>::rtu_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
	return rtu_config.serial_config;
}

template <>
HWSConfig Configs<SoftwareSerial, HardwareSerial>::dbg_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
	return dbg_config.dbg_serial_config;
}

template class Configs<SoftwareSerial, HardwareSerial>;
