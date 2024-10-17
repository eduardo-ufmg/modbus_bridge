#include "rtu_serial_processor.hpp"

#include <Arduino.h>
#include <Stream.h>
#include <StreamBuf.h>
#include <SoftwareSerial.h>

#define BSIZE 1024
namespace {
Stream* rtu_serial;
Stream* dbg_serial;

uint8_t serialw_rtur[BSIZE];
uint8_t serialr_rtuw[BSIZE];

StreamBuf serialw_rtur_sb(serialw_rtur, BSIZE);
StreamBuf serialr_rtuw_sb(serialr_rtuw, BSIZE);

DuplexBuf modbus_db(&serialw_rtur_sb, &serialr_rtuw_sb);
DuplexBuf serial_db(&serialr_rtuw_sb, &serialw_rtur_sb);

uint8_t com_buf[BSIZE];
uint8_t log_buf[BSIZE];

StreamBuf com_sbuf(com_buf, BSIZE);
StreamBuf log_sbuf(log_buf, BSIZE);
}

void set_rtu_serial(Stream* _rtu_serial)
{
	rtu_serial = _rtu_serial;
}

void set_dbg_serial(Stream* _dbg_serial)
{
	dbg_serial = _dbg_serial;
}

template <typename DBG>
void print_log(const char *msg)
{
	DBG *typed_dbg_serial = static_cast<DBG*>(dbg_serial);

  typed_dbg_serial->print(msg);
  typed_dbg_serial->print(": ");
  while (log_sbuf.available()) {
    typed_dbg_serial->print(log_sbuf.read(), HEX);
    typed_dbg_serial->print(' ');    
  }
  typed_dbg_serial->println();
}

template <typename FT, typename TT>
void process_from_to(FT *from_s, TT *to_s, unsigned delay_us = 0)
{
  while (from_s->available()) {
    
    char c = from_s->read();

    com_sbuf.write(c);
    log_sbuf.write(c);

    if (delay_us != 0) {
      delayMicroseconds(delay_us);
		}
  }

  while (com_sbuf.available()) {
    to_s->write(com_sbuf.read());
	}
}

template <typename RTU, typename DBG>
void process_rtu_serial()
{
	RTU *typed_rtu_serial = static_cast<RTU*>(rtu_serial);

	unsigned long chtimeus = (8 + 3) * 1000000 / typed_rtu_serial->baudRate();

  if(typed_rtu_serial->available()) {

    process_from_to<RTU, DuplexBuf>(typed_rtu_serial, &serial_db, chtimeus << 1);

    print_log<DBG>("received response from RTU");

  }

  if(serial_db.available()) {

    process_from_to<DuplexBuf, RTU>(&serial_db, typed_rtu_serial);

    print_log<DBG>("sent request to RTU");

  }
}

Stream* expose_modbus_db()
{
	return &modbus_db;
}

template void process_rtu_serial<HardwareSerial, SoftwareSerial>();
template void process_rtu_serial<SoftwareSerial, HardwareSerial>();
