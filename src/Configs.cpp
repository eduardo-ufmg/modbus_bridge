#include "Configs.hpp"

#include <map>
#include <type_traits>
#include <Preferences.h>
#include <SoftwareSerial.h>

#include "OppositeSerial.hpp"

namespace {
	const char *CONFIG_PREFERENCES_PREFS_NAME = "configs";
	const char *CONFIG_PREFERENCES_RTU_KEY		= "rtu_configs";
	const char *CONFIG_PREFERENCES_DBG_KEY		= "dbg_configs";
}

template <typename SI>
void RTUConfig<SI>::update() {
	serial->flush();
	serial->end();
	serial->begin(baudrate, serial_config);
	serial->flush();
}

template <typename RTU, typename DBG>
Configs<RTU, DBG>::Configs(
									RTU *rtu_serial, unsigned int rtu_baudrate, SConfigSelector_t<RTU> rtu_serial_config,
									DBG *dbg_serial, unsigned int dbg_baudrate, SConfigSelector_t<DBG> dbg_serial_config) {

	rtu_config.serial = rtu_serial;
	rtu_config.baudrate = rtu_baudrate;
	rtu_config.serial_config = rtu_serial_config;

	dbg_config.serial = dbg_serial;
	dbg_config.baudrate = dbg_baudrate;
	dbg_config.serial_config = dbg_serial_config;
}

template <typename RTU, typename DBG>
Configs<RTU, DBG>::~Configs() {
	preferences.end();
	rtu_config.serial->end();
	dbg_config.serial->end();
}

template <typename RTU, typename DBG>
int Configs<RTU, DBG>::begin() {
	// can't print before initializing the dbg_serial,
	// so report the saved configs for debugging in main
	int saved_config = 0;

	preferences.begin(CONFIG_PREFERENCES_PREFS_NAME, false);

	if (preferences.isKey(CONFIG_PREFERENCES_RTU_KEY)) {
		saved_config |= saved_configs::RTU_CONFIG;
	} else {
		preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig<RTU>));
	}

	if (preferences.isKey(CONFIG_PREFERENCES_DBG_KEY)) {
		saved_config |= saved_configs::DBG_CONFIG;
	} else {
		preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DBGConfig<DBG>));
	}

	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig<RTU>));
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DBGConfig<DBG>));

	return saved_config;
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::set_rtu_baudrate(unsigned int baudrate) {
	rtu_config.baudrate = baudrate;
	preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig<RTU>));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::set_rtu_serial_config(SConfigSelector_t<RTU> config) {
	rtu_config.serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig<RTU>));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::set_dbg_baudrate(unsigned int baudrate) {
	dbg_config.baudrate = baudrate;
	preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DBGConfig<DBG>));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::set_dbg_serial_config(SConfigSelector_t<DBG> config) {
	dbg_config.serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DBGConfig<DBG>));
}

template <typename RTU, typename DBG>
unsigned int Configs<RTU, DBG>::get_rtu_baudrate() {
	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig<RTU>));
	return rtu_config.baudrate;
}

template <typename RTU, typename DBG>
SConfigSelector_t<RTU> Configs<RTU, DBG>::get_rtu_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig<RTU>));
	return rtu_config.serial_config;
}

template <typename RTU, typename DBG>
unsigned int Configs<RTU, DBG>::get_dbg_baudrate() {
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DBGConfig<DBG>));
	return dbg_config.baudrate;
}

template <typename RTU, typename DBG>
SConfigSelector_t<DBG> Configs<RTU, DBG>::get_dbg_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DBGConfig<DBG>));
	return dbg_config.serial_config;
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::update() {
	rtu_config.update();
}

template class Configs<HardwareSerial, SoftwareSerial>;
template class Configs<SoftwareSerial, HardwareSerial>;


