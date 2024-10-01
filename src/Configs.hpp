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

	void set_rtu_baudrate(unsigned int baudrate);
	void set_rtu_serial_config(SConfigSelector_t<RTU> config);

	void set_dbg_baudrate(unsigned int baudrate);
	void set_dbg_serial_config(SConfigSelector_t<DBG> config);

	unsigned int get_rtu_baudrate();
	SConfigSelector_t<RTU> get_rtu_serial_config();

	unsigned int get_dbg_baudrate();
	SConfigSelector_t<DBG> get_dbg_serial_config();

	void update();
};

#endif // CONFIGS_HPP
