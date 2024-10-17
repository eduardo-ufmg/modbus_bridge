#ifndef RTU_SERIAL_PROCESSOR_HPP
#define RTU_SERIAL_PROCESSOR_HPP

#include <Arduino.h>
#include <Stream.h>
#include <StreamBuf.h>
#include <SoftwareSerial.h>

void set_rtu_serial(Stream* _rtu_serial);
void set_dbg_serial(Stream* _dbg_serial);

template <typename RTU, typename DBG>
void process_rtu_serial();

Stream* expose_modbus_db();

#endif // RTU_SERIAL_PROCESSOR_HPP
