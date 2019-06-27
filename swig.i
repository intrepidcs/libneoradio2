%module neoradio2
%{

#include "libneoradio2.h"
#include "libneoradio2common.h"

extern void neoradio2_set_blocking(int blocking, long long ms_timeout);
extern int neoradio2_find(Neoradio2DeviceInfo* devices, unsigned int* device_count);
extern int neoradio2_is_blocking();
extern int neoradio2_open(neoradio2_handle* handle, Neoradio2DeviceInfo* device);
extern int neoradio2_is_opened(neoradio2_handle* handle, int* is_opened);
extern int neoradio2_close(neoradio2_handle* handle);
extern int neoradio2_is_closed(neoradio2_handle* handle, int* is_closed);
extern int neoradio2_chain_is_identified(neoradio2_handle* handle, int* is_identified);
extern int neoradio2_chain_identify(neoradio2_handle* handle);
extern int neoradio2_app_is_started(neoradio2_handle* handle, int device, int bank, int* is_started);
extern int neoradio2_app_start(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_enter_bootloader(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_get_serial_number(neoradio2_handle* handle, int device, int bank, unsigned int* serial_number);
extern int neoradio2_get_manufacturer_date(neoradio2_handle* handle, int device, int bank, int* year, int* month, int* day);
extern int neoradio2_get_firmware_version(neoradio2_handle* handle, int device, int bank, int* major, int* minor);
extern int neoradio2_get_hardware_revision(neoradio2_handle* handle, int device, int bank, int* major, int* minor);
extern int neoradio2_get_device_type(neoradio2_handle* handle, int device, int bank, unsigned int* device_type);
extern int neoradio2_request_pcbsn(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_get_pcbsn(neoradio2_handle* handle, int device, int bank, char* pcb_sn);
extern int neoradio2_request_sensor_data(neoradio2_handle* handle, int device, int bank, int enable_cal);
extern int neoradio2_read_sensor_float(neoradio2_handle* handle, int device, int bank, float* value);
extern int neoradio2_read_sensor_array(neoradio2_handle* handle, int device, int bank, int* arr, int* arr_size);
extern int neoradio2_write_sensor(neoradio2_handle* handle, int device, int bank, int mask, int value);
extern int neoradio2_write_sensor_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_request_settings(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_read_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_settings* settings);
extern int neoradio2_write_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_settings* settings);
extern int neoradio2_write_settings_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_get_chain_count(neoradio2_handle* handle, int* count, int identify);
extern int neoradio2_request_calibration(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header);
extern int neoradio2_read_calibration_array(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int* arr_size);
extern int neoradio2_request_calibration_points(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header);
extern int neoradio2_read_calibration_points_array(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int* arr_size);
extern int neoradio2_write_calibration(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int arr_size);
extern int neoradio2_write_calibration_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_write_calibration_points(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int arr_size);
extern int neoradio2_write_calibration_points_successful(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_store_calibration(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_is_calibration_stored(neoradio2_handle* handle, int device, int bank, int* stored);
extern int neoradio2_get_calibration_is_valid(neoradio2_handle* handle, int device, int bank, int* is_valid);

extern int neoradio2_request_calibration_info(neoradio2_handle* handle, int device, int bank);
extern int neoradio2_read_calibration_info(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header);


extern int neoradio2_toggle_led(neoradio2_handle* handle, int device, int bank, int ms);
extern int neoradio2_toggle_led_successful(neoradio2_handle* handle, int device, int bank);


extern int neoradio2_get_status(neoradio2_handle* handle, int device, int bank, int bitfield, StatusType type, CommandStatus* status);

%}

/* $include "libneoradio2.h" */
%include "libneoradio2common.h"