#include "ConfigsHWRTU.hpp"
#include "ConfigsTemplate.hpp"

#include <map>
#include <Preferences.h>
#include <HardwareSerial.h>

template <>
Configs<HardwareSerial, SoftwareSerial>::
		Configs(HardwareSerial *rtu_serial, unsigned int rtu_baudrate, HWSConfig rtu_serial_config,
		SoftwareSerial *dbg_serial, unsigned int dbg_baudrate, SWSConfig dbg_serial_config) {

	rtu_config.serial = rtu_serial;
	rtu_config.baudrate = rtu_baudrate;
	rtu_config.serial_config = rtu_serial_config;

	dbg_config.dbg_serial = dbg_serial;
	dbg_config.dbg_baudrate = dbg_baudrate;
	dbg_config.dbg_serial_config = dbg_serial_config;
}

template <>
void Configs<HardwareSerial, SoftwareSerial>::rtu_serial_config(HWSConfig config) {
	rtu_config.serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
}

template <>
void Configs<HardwareSerial, SoftwareSerial>::dbg_serial_config(SWSConfig config) {
	dbg_config.dbg_serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
}

template <>
HWSConfig Configs<HardwareSerial, SoftwareSerial>::rtu_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
	return rtu_config.serial_config;
}

template <>
SWSConfig Configs<HardwareSerial, SoftwareSerial>::dbg_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
	return dbg_config.dbg_serial_config;
}

template class Configs<HardwareSerial, SoftwareSerial>;
