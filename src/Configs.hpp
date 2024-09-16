#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#include <map>
#include <type_traits>
#include <Preferences.h>
#include <SoftwareSerial.h>

#include "OppositeSerial.hpp"

typedef SerialConfig							HWSConfig;
typedef EspSoftwareSerial::Config SWSConfig;

template <typename RTU, typename DBG>
class Configs;

enum saved_configs {
	RTU_CONFIG = 1 << 0,
	DBG_CONFIG = 1 << 1
};

// const char *DEFCONFIG_PREFERENCES_PREFS_NAME = "configs";
// const char *DEFCONFIG_PREFERENCES_RTU_KEY		= "rtu_configs";
// const char *DEFCONFIG_PREFERENCES_DBG_KEY		= "dbg_configs";

template <typename SI>
struct SConfigSelector {
	using type = typename std::conditional<std::is_same<SI, HardwareSerial>::value, HWSConfig, SWSConfig>::type;
};

template <typename SI>
using SConfigSelector_t = typename SConfigSelector<SI>::type;

template <typename SI>
class RTUConfig {
private:
	SI *serial;
	unsigned int baudrate;
	SConfigSelector_t<SI> serial_config;

public:
	void update();

friend class Configs<SI, typename OppositeSerial<SI>::Type>;
};

template <typename SI>
class DBGConfig {
private:
	SI *serial;
	unsigned int baudrate;
	SConfigSelector_t<SI> serial_config;

friend class Configs<typename OppositeSerial<SI>::Type, SI>;
};

template <typename RTU, typename DBG>
class Configs {
private:
	Preferences preferences;
	RTUConfig<RTU> rtu_config;
	DBGConfig<DBG> dbg_config;

public:
	Configs(RTU *rtu_serial, unsigned int rtu_baudrate, SConfigSelector_t<RTU> rtu_serial_config,
					DBG *dbg_serial, unsigned int dbg_baudrate, SConfigSelector_t<DBG> dbg_serial_config);
	~Configs();

	int begin();

	void rtu_baudrate(unsigned int baudrate);
	void rtu_serial_config(SConfigSelector_t<RTU> config);

	void dbg_baudrate(unsigned int baudrate);
	void dbg_serial_config(SConfigSelector_t<DBG> config);

	unsigned int rtu_baudrate();
	SConfigSelector_t<RTU> rtu_serial_config();

	unsigned int dbg_baudrate();
	SConfigSelector_t<DBG> dbg_serial_config();

	void update();
};

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

	preferences.begin("configs", false);

	if (preferences.isKey("rtu_configs")) {
		saved_config |= saved_configs::RTU_CONFIG;
	} else {
		preferences.putBytes("rtu_configs", &rtu_config, sizeof(RTUConfig<RTU>));
	}

	if (preferences.isKey("dbg_configs")) {
		saved_config |= saved_configs::DBG_CONFIG;
	} else {
		preferences.putBytes("dbg_configs", &dbg_config, sizeof(DBGConfig<DBG>));
	}

	preferences.getBytes("rtu_configs", &rtu_config, sizeof(RTUConfig<RTU>));
	preferences.getBytes("dbg_configs", &dbg_config, sizeof(DBGConfig<DBG>));

	return saved_config;
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::rtu_baudrate(unsigned int baudrate) {
	rtu_config.baudrate = baudrate;
	preferences.putBytes("rtu_configs", &rtu_config, sizeof(RTUConfig<RTU>));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::rtu_serial_config(SConfigSelector_t<RTU> config) {
	rtu_config.serial_config = config;
	preferences.putBytes("rtu_configs", &rtu_config, sizeof(RTUConfig<RTU>));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::dbg_baudrate(unsigned int baudrate) {
	dbg_config.baudrate = baudrate;
	preferences.putBytes("dbg_configs", &dbg_config, sizeof(DBGConfig<DBG>));
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::dbg_serial_config(SConfigSelector_t<DBG> config) {
	dbg_config.serial_config = config;
	preferences.putBytes("dbg_configs", &dbg_config, sizeof(DBGConfig<DBG>));
}

template <typename RTU, typename DBG>
unsigned int Configs<RTU, DBG>::rtu_baudrate() {
	preferences.getBytes("rtu_configs", &rtu_config, sizeof(RTUConfig<RTU>));
	return rtu_config.baudrate;
}

template <typename RTU, typename DBG>
SConfigSelector_t<RTU> Configs<RTU, DBG>::rtu_serial_config() {
	preferences.getBytes("rtu_configs", &rtu_config, sizeof(RTUConfig<RTU>));
	return rtu_config.serial_config;
}

template <typename RTU, typename DBG>
unsigned int Configs<RTU, DBG>::dbg_baudrate() {
	preferences.getBytes("dbg_configs", &dbg_config, sizeof(DBGConfig<DBG>));
	return dbg_config.baudrate;
}

template <typename RTU, typename DBG>
SConfigSelector_t<DBG> Configs<RTU, DBG>::dbg_serial_config() {
	preferences.getBytes("dbg_configs", &dbg_config, sizeof(DBGConfig<DBG>));
	return dbg_config.serial_config;
}

template <typename RTU, typename DBG>
void Configs<RTU, DBG>::update() {
	rtu_config.update();
}

#endif // CONFIGS_HPP
