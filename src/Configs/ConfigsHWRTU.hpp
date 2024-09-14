#ifndef CONFIGS_HWRTU_HPP
#define CONFIGS_HWRTU_HPP

#include "ConfigsTemplate.hpp"

#include <map>
#include <Preferences.h>
#include <SoftwareSerial.h>

template <>
class Configs<HardwareSerial, SoftwareSerial>;

template <>
class RTUConfig<HardwareSerial> {
private:
	HardwareSerial *serial;
	unsigned int baudrate;
	HWSConfig serial_config;

public:
	void update();

friend class Configs<HardwareSerial, SoftwareSerial>;
};

template <>
class DBGConfig<SoftwareSerial> {
private:
	SoftwareSerial *dbg_serial;
	unsigned int dbg_baudrate;
	SWSConfig dbg_serial_config;

friend class Configs<SoftwareSerial, HardwareSerial>;
};

template <>
class Configs<HardwareSerial, SoftwareSerial> {
private:
	Preferences preferences;
	RTUConfig<HardwareSerial> rtu_config;
	DBGConfig<SoftwareSerial> dbg_config;

public:
	Configs(HardwareSerial *rtu_serial, unsigned int rtu_baudrate, HWSConfig rtu_serial_config,
					SoftwareSerial *dbg_serial, unsigned int dbg_baudrate, SWSConfig dbg_serial_config);
	~Configs();

	int begin();

	void rtu_baudrate(unsigned int baudrate);
	void rtu_serial_config(HWSConfig config);

	void dbg_baudrate(unsigned int baudrate);
	void dbg_serial_config(SWSConfig config);

	unsigned int rtu_baudrate();
	HWSConfig rtu_serial_config();

	unsigned int dbg_baudrate();
	SWSConfig dbg_serial_config();

	void update();
};

#endif // CONFIGS_HWRTU_HPP
