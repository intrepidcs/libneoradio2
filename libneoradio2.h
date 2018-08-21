#ifndef __LIBNEORADIO2_H__
#define __LIBNEORADIO2_H__

#ifdef LIBNEORADIO2_EXPORTS
#define LIBNEORADIO2_API __declspec(dllexport)
#else
//#define LIBNEORADIO2_API  __declspec(dllimport)
#define LIBNEORADIO2_API  
#endif

#include <stdint.h>
#include "radio2_frame.h"

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

	uint8_t _reserved[32];
} Neoradio2DeviceInfo;


LIBNEORADIO2_API int neoradio2_find_devices(Neoradio2DeviceInfo* devices, unsigned int* device_count);
LIBNEORADIO2_API int neoradio2_is_blocking();
LIBNEORADIO2_API int neoradio2_open(neoradio2_handle* handle, Neoradio2DeviceInfo* device);
LIBNEORADIO2_API int neoradio2_is_opened(neoradio2_handle* handle, int* is_opened);
LIBNEORADIO2_API int neoradio2_close(neoradio2_handle* handle);
LIBNEORADIO2_API int neoradio2_is_closed(neoradio2_handle* handle, int* is_closed);
LIBNEORADIO2_API int neoradio2_chain_is_identified(neoradio2_handle* handle, int* is_identified);
LIBNEORADIO2_API int neoradio2_chain_identify(neoradio2_handle* handle);
LIBNEORADIO2_API int neoradio2_app_is_started(neoradio2_handle* handle, int device, int bank, int* is_started);
LIBNEORADIO2_API int neoradio2_app_start(neoradio2_handle* handle, int device, int bank);
LIBNEORADIO2_API int neoradio2_enter_bootloader(neoradio2_handle* handle, int device, int bank);
LIBNEORADIO2_API int neoradio2_get_serial_number(neoradio2_handle* handle, int device, int bank, unsigned int* serial_number);
LIBNEORADIO2_API int neoradio2_get_manufacturer_date(neoradio2_handle* handle, int device, int bank, int* year, int* month, int* day);
LIBNEORADIO2_API int neoradio2_get_firmware_version(neoradio2_handle* handle, int device, int bank, int* major, int* minor);
LIBNEORADIO2_API int neoradio2_get_hardware_revision(neoradio2_handle* handle, int device, int bank, int* major, int* minor);
LIBNEORADIO2_API int neoradio2_get_device_type(neoradio2_handle* handle, int device, int bank, unsigned int* device_type);
LIBNEORADIO2_API int neoradio2_get_pcbsn(neoradio2_handle* handle, int device, int bank, char* pcb_sn);
LIBNEORADIO2_API int neoradio2_read_sensor_float(neoradio2_handle* handle, int device, int bank, float* value);
LIBNEORADIO2_API int neoradio2_read_sensor_array(neoradio2_handle* handle, int device, int bank, int* arr, int* arr_size);
LIBNEORADIO2_API int neoradio2_read_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_deviceSettings* settings);
LIBNEORADIO2_API int neoradio2_write_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_deviceSettings* settings);


#ifdef __cplusplus
}
#endif

#endif // __LIBNEORADIO2_H__