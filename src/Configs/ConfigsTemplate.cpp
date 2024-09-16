#include "ConfigsTemplate.hpp"

#include "ConfigsSWRTU.hpp"
#include "ConfigsHWRTU.hpp"

#include <map>
#include <Preferences.h>
#include <SoftwareSerial.h>

template <typename SI>
void RTUConfig<SI>::update() {
	serial->flush();
	serial->end();
	serial->begin(baudrate, serial_config);
	serial->flush();
}

template <typename RTU, typename DBG>
Configs<RTU, DBG>(RTU *rtu_serial, unsigned int rtu_baudrate, OtherConfig rtu_serial_config,
				DBG *dbg_serial, unsigned int dbg_baudrate, OtherConfig dbg_serial_config) {

	rtu_config.serial = rtu_serial;
	rtu_config.baudrate = rtu_baudrate;
	rtu_config.serial_config = rtu_serial_config;

	dbg_config.dbg_serial = dbg_serial;
	dbg_config.dbg_baudrate = dbg_baudrate;
	dbg_config.dbg_serial_config = dbg_serial_config;
}

template <typename RTU, typename DBG>
Configs<RTU, DBG>::~Configs() {
	preferences.end();
	rtu_config.serial->end();
	dbg_config.dbg_serial->end();
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
		preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
	}

	if (preferences.isKey(CONFIG_PREFERENCES_DBG_KEY)) {
		saved_config |= saved_configs::DBG_CONFIG;
	} else {
		preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
	}

	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));

	return saved_config;
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::rtu_baudrate(unsigned int baudrate) {
	rtu_config.baudrate = baudrate;
	preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::rtu_serial_config(OtherConfig config) {
	rtu_config.serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::dbg_baudrate(unsigned int baudrate) {
	dbg_config.baudrate = baudrate;
	preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::dbg_serial_config(OtherConfig config) {
	dbg_config.dbg_serial_config = config;
	preferences.putBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
}

template <typename RTU, typename DBG>
unsigned int Configs<RTU, DBG>::rtu_baudrate() {
	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
	return rtu_config.baudrate;
}

template <typename RTU, typename DBG>
OtherConfig Configs<RTU, DBG>::rtu_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_RTU_KEY, &rtu_config, sizeof(RTUConfig));
	return rtu_config.serial_config;
}

template <typename RTU, typename DBG>
unsigned int Configs<RTU, DBG>::dbg_baudrate() {
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
	return dbg_config.baudrate;
}

template <typename RTU, typename DBG>
OtherConfig Configs<RTU, DBG>::dbg_serial_config() {
	preferences.getBytes(CONFIG_PREFERENCES_DBG_KEY, &dbg_config, sizeof(DebugConfig));
	return dbg_config.dbg_serial_config;
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::update() {
	rtu_config.update();
}
