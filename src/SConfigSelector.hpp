#ifndef SCONFIGSELECTOR_HPP
#define SCONFIGSELECTOR_HPP

#include <type_traits>
#include <SoftwareSerial.h>

typedef SerialConfig							HWSConfig;
typedef EspSoftwareSerial::Config SWSConfig;

template <typename SI>
struct SConfigSelector {
	using type = typename std::conditional<std::is_same<SI, HardwareSerial>::value, HWSConfig, SWSConfig>::type;
};

template <typename SI>
using SConfigSelector_t = typename SConfigSelector<SI>::type;

#endif // SCONFIGSELECTOR_HPP
