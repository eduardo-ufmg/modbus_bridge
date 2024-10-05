#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#include <map>
#include <type_traits>
#include <Preferences.h>
#include <SoftwareSerial.h>

#include "OppositeSerial.hpp"
#include "SConfigSelector.hpp"

template <typename RTU, typename DBG>
class Configs;

enum saved_configs {
	RTU_CONFIG = 1 << 0,
	DBG_CONFIG = 1 << 1
};

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

	const char *preferences_rtu_key;
	const char *preferences_dbg_key;

public:
	Configs(RTU *rtu_serial, unsigned int rtu_baudrate, SConfigSelector_t<RTU> rtu_serial_config,
					DBG *dbg_serial, unsigned int dbg_baudrate, SConfigSelector_t<DBG> dbg_serial_config);
	~Configs();

	int begin();

	void set_rtu_baudrate(unsigned int baudrate);
	void set_rtu_serial_config(SConfigSelector_t<RTU> config);

	void set_dbg_baudrate(unsigned int baudrate);
	void set_dbg_serial_config(SConfigSelector_t<DBG> config);

	unsigned int get_rtu_baudrate();
	SConfigSelector_t<RTU> get_rtu_serial_config();

	unsigned int get_dbg_baudrate();
	SConfigSelector_t<DBG> get_dbg_serial_config();

	void update();

	const char *get_rtu_key();
	const char *get_dbg_key();
};

#endif // CONFIGS_HPP
