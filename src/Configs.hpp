#ifndef CONFIGS_HPP
#define CONFIGS_HPP

#include <map>
#include <Preferences.h>
#include <SoftwareSerial.h>

enum saved_configs {
	RTU_CONFIG = 1 << 0,
	DBG_CONFIG = 1 << 1
};

const std::map<const EspSoftwareSerial::Config, const String> sws_dbg_str {
	// debug strings are kept in english because most libraries use english
	// on their logging
	{SWSERIAL_8N1, "SWSERIAL_8N1"},
	{SWSERIAL_8E1, "SWSERIAL_8E1"},
	{SWSERIAL_8O1, "SWSERIAL_8O1"},
	{SWSERIAL_8N2, "SWSERIAL_8N2"},
	{SWSERIAL_8E2, "SWSERIAL_8E2"},
	{SWSERIAL_8O2, "SWSERIAL_8O2"}
};

class RTUConfig {
private:
	SoftwareSerial *rtu_serial;
	unsigned int rtu_baudrate;
	EspSoftwareSerial::Config rtu_serial_config;

public:
	void update();

	friend class Configs;
};

class DebugConfig {
private:
	HardwareSerial *dbg_serial;
	unsigned int dbg_baudrate;
	SerialConfig dbg_serial_config;

	friend class Configs;
};

class Configs {
private:
	Preferences preferences;
	RTUConfig rtu_config;
	DebugConfig dbg_config;

public:
	Configs(SoftwareSerial* rtu_serial, HardwareSerial* dbg_serial,
					unsigned int rtu_baudrate = 9600, EspSoftwareSerial::Config rtu_serial_config = SWSERIAL_8N1,
					unsigned int dbg_baudrate = 115200, SerialConfig dbg_serial_config = SERIAL_8N1);
	~Configs();

	int begin();

	void set_rtu_baudrate(unsigned int baudrate);
	void set_dbg_baudrate(unsigned int baudrate);
	void set_rtu_serial_config(EspSoftwareSerial::Config config);
	void set_dbg_serial_config(SerialConfig config);

	unsigned int rtu_baudrate();
	unsigned int dbg_baudrate();
	EspSoftwareSerial::Config rtu_serial_config();
	SerialConfig dbg_serial_config();

	void update();
};

#endif
