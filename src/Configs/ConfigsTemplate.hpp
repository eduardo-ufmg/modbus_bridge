#ifndef CONFIG_TEMPLATE_HPP
#define CONFIG_TEMPLATE_HPP

#include <map>
#include <Preferences.h>
#include <SoftwareSerial.h>

#include "../OppositeSerial.hpp"

typedef SerialConfig							HWSConfig;
typedef EspSoftwareSerial::Config SWSConfig;
typedef int												OtherConfig;

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
	OtherConfig serial_config;

public:
	void update();

friend class Configs<SI, typename OppositeSerial<SI>::Type>;
};

template <typename SI>
class DBGConfig {
private:
	SI *dbg_serial;
	unsigned int dbg_baudrate;
	OtherConfig dbg_serial_config;

friend class Configs<typename OppositeSerial<SI>::Type, SI>;
};

template <typename RTU, typename DBG>
class Configs {
private:
	Preferences preferences;
	RTUConfig<RTU> rtu_config;
	DBGConfig<DBG> dbg_config;

public:
	Configs(RTU *rtu_serial, unsigned int rtu_baudrate, OtherConfig rtu_serial_config,
					DBG *dbg_serial, unsigned int dbg_baudrate, OtherConfig dbg_serial_config);
	~Configs();

	int begin();

	void rtu_baudrate(unsigned int baudrate);
	void rtu_serial_config(OtherConfig config);

	void dbg_baudrate(unsigned int baudrate);
	void dbg_serial_config(OtherConfig config);

	unsigned int rtu_baudrate();
	OtherConfig rtu_serial_config();

	unsigned int dbg_baudrate();
	OtherConfig dbg_serial_config();

	void update();
};

const char *CONFIG_PREFERENCES_PREFS_NAME = "configs";
const char *CONFIG_PREFERENCES_RTU_KEY		= "rtu_configs";
const char *CONFIG_PREFERENCES_DBG_KEY		= "dbg_configs";

#endif // CONFIG_TEMPLATE_HPP
