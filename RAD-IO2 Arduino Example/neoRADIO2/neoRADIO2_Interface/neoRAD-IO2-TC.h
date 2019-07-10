#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _neoRADIO2TC_EnableSetting {
	NEORADIO2TC_CONFIG_DISABLE  =   0x00,
	NEORADIO2TC_CONFIG_TC       =   0x01,
	NEORADIO2TC_CONFIG_CJ       =   0x02,
	NEORADIO2TC_CONFIG_TCCJ     =   0x03
} neoRADIO2TC_EnableSetting;


#ifdef __cplusplus
}
#endif
