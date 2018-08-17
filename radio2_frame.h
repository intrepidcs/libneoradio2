#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#define PACKED 
#else
#define PACKED __attribute__((packed))
#endif

	typedef enum _neoRADIO2_deviceTypes {
		NEORADIO2_DEVTYPE_TC		= 0,
		NEORADIO2_DEVTYPE_DIO		= 1,
		NEORADIO2_DEVTYPE_PWRRLY	= 2,
		NEORADIO2_DEVTYPE_AIN		= 3,
		NEORADIO2_DEVTYPE_AOUT		= 4,
		NEORADIO2_DEVTYPE_CANHUB	= 5,
		NEORADIO2_DEVTYPE_HOST  	= 0xFF,
	} neoRADIO2_deviceTypes;

	typedef struct _neoRADIO2frame_identifyResponse {
		uint32_t	serial_number;
		uint16_t	manufacture_year;
		uint8_t     manufacture_month;
		uint8_t	    manufacture_day;
		uint8_t		device_type;
		uint8_t		device_number;
		uint8_t		device_bank;
		uint8_t		firmwareVersion_major;
		uint8_t		firmwareVersion_minor;
		uint8_t		hardware_revMinor;
		uint8_t		hardware_revMajor;
		uint8_t     current_state;
	} neoRADIO2frame_identifyResponse;

	typedef struct _neoRADIO2frame_header {
		uint8_t	start_of_frame; //0xAA for host packets or 0x55 for device packets
		uint8_t command_status;
		uint8_t device;
		uint8_t bank; //bank is bitmasked so 0b0001000 is the fourth bank
		uint8_t len;
	} PACKED neoRADIO2frame_header;

	typedef struct _neoRADIO2frame {
		neoRADIO2frame_header header;
		uint8_t data[64];
		uint8_t crc;
	} PACKED  neoRADIO2frame;

	typedef struct _neoRADIO2frame_identify {
		uint8_t device_type;
		uint8_t device_number;
		uint8_t bank_number;
	} PACKED neoRADIO2frame_identify;

	typedef enum _neoRADIO2frame_commands {
		NEORADIO2_COMMAND_START             =   0x01,
		NEORADIO2_COMMAND_IDENTIFY          =   0x02,
		NEORADIO2_COMMAND_WRITE_DATA        =   0x03,
		NEORADIO2_COMMAND_READ_DATA         =   0x04,
		NEORADIO2_COMMAND_WRITE_SETTINGS    =   0x05,
		NEORADIO2_COMMAND_READ_SETTINGS     =   0x06,
		NEORADIO2_COMMAND_WRITE_CAL         =   0x07,
		NEORADIO2_COMMAND_READ_CAL          =   0x08,
		NEORADIO2_COMMAND_TOGGLE_LED        =   0x09,
		NEORADIO2_COMMAND_BL_WRITEBUFFER    =   0xFA,
		NEORADIO2_COMMAND_BL_WRITETOFLASH	=   0xFB,
		NEORADIO2_COMMAND_BL_VERIFY		    =   0xFC,
		NEORADIO2_COMMAND_ENTERBOOT         =   0xFF,
	} _neoRADIO2frame_commands;

	typedef enum _neoRADIO2frame_deviceStatus {
		NEORADIO2_STATUS_OK					=   0x00,
		NEORADIO2_STATUS_ERROR				=   0x01,
		NEORADIO2_STATUS_IDENTIFY			=   0x02,
		NEORADIO2_STATUS_READ_SETTINGS		=   0x03,
		NEORADIO2_STATUS_NEED_ID			=   0xFF,
	} neoRADIO2frame_deviceStatus;

	typedef struct _neoRADIO2AOUTframe_data {
		uint16_t DAC11;
		uint16_t DAC12;
		uint16_t DAC23;
	} PACKED neoRADIO2AOUTframe_data;

	typedef struct _neoRADIO2_deviceSettings {
		uint32_t sample_rate;
		uint32_t channel_1_config;
		uint32_t channel_2_Config;
		uint32_t channel_3_Config;
	} PACKED neoRADIO2_deviceSettings;

	typedef enum _neoRADIO2states {
		NEORADIO2STATE_RUNNING          =0,
		NEORADIO2STATE_INBOOTLOADER		=1,
	} neoRADIO2states;


#define NEORADIO2_DESTINATION_CHANNEL1 0x01
#define NEORADIO2_DESTINATION_CHANNEL2 0x02
#define NEORADIO2_DESTINATION_CHANNEL3 0x04
#define NEORADIO2_DESTINATION_CHANNEL4 0x08
#define NEORADIO2_DESTINATION_CHANNEL5 0x10
#define NEORADIO2_DESTINATION_CHANNEL6 0x20
#define NEORADIO2_DESTINATION_CHANNEL7 0x40
#define NEORADIO2_DESTINATION_CHANNEL8 0x80

	typedef union _bytesToFloat {
		float fp;
		uint8_t b[sizeof(float)];
	} bytesToFloat;

#ifdef _MSC_VER
#pragma pack(pop)
#undef PACKED 
#else
#undef PACKED
#endif


#ifdef __cplusplus
}
#endif
