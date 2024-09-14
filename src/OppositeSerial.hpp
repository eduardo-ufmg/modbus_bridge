#ifndef OPPOSITE_SERIAL_HPP
#define OPPOSITE_SERIAL_HPP

#include <SoftwareSerial.h>
#include <HardwareSerial.h>

template <typename T>
struct OppositeSerial;

template <>
struct OppositeSerial<SoftwareSerial> {
	using Type = HardwareSerial;
};

template <>
struct OppositeSerial<HardwareSerial> {
	using Type = SoftwareSerial;
};

#endif // OPPOSITE_SERIAL_HPP
