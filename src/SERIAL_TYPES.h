#ifndef SERIAL_TYPES_H
#define SERIAL_TYPES_H

#define SWS_AS_RTU_SERIAL true

#if (SWS_AS_RTU_SERIAL)
	
	#define RTU_SERIAL_TYPE SoftwareSerial
	#define RTU_SERIAL_CFG_TYPE EspSoftwareSerial::Config
	#define RTU_SERIAL_DFLT_CFG SWSERIAL_8N1
	
	#define DBG_SERIAL_TYPE HardwareSerial
	#define DBG_SERIAL_CFG_TYPE SerialConfig
	#define DBG_SERIAL_DFLT_CFG SERIAL_8N1

	#define S8N1 SWSERIAL_8N1
	#define S8E1 SWSERIAL_8E1
	#define S8O1 SWSERIAL_8O1
	#define S8N2 SWSERIAL_8N2
	#define S8E2 SWSERIAL_8E2
	#define S8O2 SWSERIAL_8O2

#else

	#define RTU_SERIAL_TYPE HardwareSerial
	#define RTU_SERIAL_CFG_TYPE SerialConfig
	#define RTU_SERIAL_DFLT_CFG SERIAL_8N1
	
	#define DBG_SERIAL_TYPE SoftwareSerial
	#define DBG_SERIAL_CFG_TYPE EspSoftwareSerial::Config
	#define DBG_SERIAL_DFLT_CFG SWSERIAL_8N1

	#define S8N1 SERIAL_8N1
	#define S8E1 SERIAL_8E1
	#define S8O1 SERIAL_8O1
	#define S8N2 SERIAL_8N2
	#define S8E2 SERIAL_8E2
	#define S8O2 SERIAL_8O2

#endif

#endif // SERIAL_TYPES_H