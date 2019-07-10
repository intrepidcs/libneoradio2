#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	
typedef union _neoRADIO2AOUT_header {
	uint8_t byte;
	struct {
		unsigned ch1 :1;
		unsigned ch2 :1;
		unsigned ch3 :1;
		unsigned reserved :4;
		unsigned noCal:1;
	} bits;
} neoRADIO2AOUT_header;

#ifdef __cplusplus
}
#endif
