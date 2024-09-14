#ifndef CONFIGS_SWRTU_HPP
#define CONFIGS_SWRTU_HPP

#include "ConfigsTemplate.hpp"

#include <map>
#include <Preferences.h>
#include <HardwareSerial.h>

template <>
class Configs<SoftwareSerial, HardwareSerial>;

template <>
class RTUConfig<SoftwareSerial> {
private:
	SoftwareSerial *serial;
	unsigned int baudrate;
	SWSConfig serial_config;

public:
	void update();

friend class Configs<SoftwareSerial, HardwareSerial>;
};

template <>
class DBGConfig<HardwareSerial> {
private:
	HardwareSerial *dbg_serial;
	unsigned int dbg_baudrate;
	HWSConfig dbg_serial_config;

friend class Configs<HardwareSerial, SoftwareSerial>;
};

template <>
class Configs<SoftwareSerial, HardwareSerial> {
private:
	Preferences preferences;
	RTUConfig<SoftwareSerial> rtu_config;
	DBGConfig<HardwareSerial> dbg_config;

public:
	Configs(SoftwareSerial *rtu_serial, unsigned int rtu_baudrate, SWSConfig rtu_serial_config,
					HardwareSerial *dbg_serial, unsigned int dbg_baudrate, HWSConfig dbg_serial_config);
	~Configs();

	int begin();

	void rtu_baudrate(unsigned int baudrate);
	void rtu_serial_config(SWSConfig config);

	void dbg_baudrate(unsigned int baudrate);
	void dbg_serial_config(HWSConfig config);

	unsigned int rtu_baudrate();
	SWSConfig rtu_serial_config();

	unsigned int dbg_baudrate();
	HWSConfig dbg_serial_config();

	void update();
};

#endif // CONFIGS_SWRTU_HPP
