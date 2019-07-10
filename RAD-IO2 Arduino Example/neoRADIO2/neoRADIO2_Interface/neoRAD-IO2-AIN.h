#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum _neoRADIO2AIN_EnableSetting {
	NEORADIO2AIN_CONFIG_DISABLE          = 0x00,
	NEORADIO2AIN_CONFIG_LOWRANGE_250MV   = 0x01,
	NEORADIO2AIN_CONFIG_LOWRANGE_1000MV  = 0x02,
	NEORADIO2AIN_CONFIG_LOWRANGE_5000MV  = 0x03,
	NEORADIO2AIN_CONFIG_HIGHRANGE_8V     = 0x04,
	NEORADIO2AIN_CONFIG_HIGHRANGE_16V    = 0x05,
	NEORADIO2AIN_CONFIG_HIGHRANGE_42V    = 0x06,
} neoRADIO2AIN_EnableSetting;

#ifdef __cplusplus
}
#endif
