#ifndef __NEORADIO2_H__
#define __NEORADIO2_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NEORADIO2_SUCCESS 0
#define NEORADIO2_FAILURE 1
#define NEORADIO2_ERR_WBLOCK 2


#define NEORADIO2_MAX_DEVS 8

#define neoradio2_handle long

	typedef struct _Neoradio2DeviceInfo
	{
		char* name;
		char* serial_str;

		int vendor_id;
		int product_id;

		//int channel_count;

		uint8_t _reserved[32];
	} Neoradio2DeviceInfo;

#ifdef __cplusplus
}
#endif


#endif // __NEORADIO2_H__
