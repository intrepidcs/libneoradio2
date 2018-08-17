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
	uint32_t	serialNumber;
	uint16_t	manufacture_year;
	uint8_t     manufacture_month;
	uint8_t	    manufacture_day;
	uint8_t		deviceType;
	uint8_t		deviceNumber;
	uint8_t		deviceBank;
	uint8_t		firmwareVersion_major;
	uint8_t		firmwareVersion_minor;
	uint8_t		hardware_revMinor;
	uint8_t		hardware_revMajor;
	uint8_t     current_state;
} neoRADIO2frame_identifyResponse;

typedef struct _neoRADIO2frame_header {
	uint8_t	headerAA;
	uint8_t command;
	uint8_t destination_device;
	uint8_t destination_bank; //bank is bitmasked so 0b0001000 is the fourth bank
	uint8_t len;
	uint8_t checksum;
} PACKED neoRADIO2frame_header;

typedef struct _neoRADIO2frame_DeviceReportHeader {
	uint8_t header55;
	uint8_t report_device;
	uint8_t report_bank;
	uint8_t report_status;
	uint8_t len;
} PACKED neoRADIO2frame_DeviceReportHeader;

typedef struct _neoRADIO2_ReceiveData {
	neoRADIO2frame_DeviceReportHeader header;
	uint8_t data[36];
} PACKED  neoRADIO2_ReceiveData;

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
	NEORADIO2_COMMAND_BL_WRITE          =   0xFB,
	NEORADIO2_COMMAND_BL_VERIFY		    =   0xFC,
	NEORADIO2_COMMAND_ENTERBOOT         =   0xFF,
} _neoRADIO2frame_commands;

typedef struct _neoRADIO2AOUTframe_data {
	uint16_t DAC11;
	uint16_t DAC12;
	uint16_t DAC23;
} PACKED neoRADIO2AOUTframe_data;


typedef struct _neoRADIO2frame_deviceSettings {
	uint16_t reportRate;
	uint32_t chan1Config;
	uint32_t chan2Config;
	uint32_t chan3Config;
} PACKED neoRADIO2frame_deviceSettings;

typedef enum _neoRADIO2states {
	NEORADIO2STATE_RUNNING          =0,
	NEORADIO2STATE_INBOOTLOADER		=1,
} neoRADIO2states;

typedef enum _neoRADIO2AIN_EnableSetting {
	NEORADIO2AIN_CONFIG_DISABLE          = 0x00,
	NEORADIO2AIN_CONFIG_LOWRANGE         = 0x01,
	NEORADIO2AIN_CONFIG_LOWRANGE_500MV   = 0x02,
	NEORADIO2AIN_CONFIG_LOWRANGE_1000MV  = 0x03,
	NEORADIO2AIN_CONFIG_LOWRANGE_2000MV  = 0x04,
	NEORADIO2AIN_CONFIG_LOWRANGE_4000MV  = 0x05,
	NEORADIO2AIN_CONFIG_HIGHRANGE_2V     = 0x06,
	NEORADIO2AIN_CONFIG_HIGHRANGE_4V     = 0x07,
	NEORADIO2AIN_CONFIG_HIGHRANGE_8V     = 0x08,
	NEORADIO2AIN_CONFIG_HIGHRANGE_16V    = 0x09,
	NEORADIO2AIN_CONFIG_HIGHRANGE_32V    = 0x0A
} neoRADIO2AIN_EnableSetting;

typedef enum _neoRADIO2TC_EnableSetting {
	NEORADIO2TC_CONFIG_DISABLE  =   0x00,
	NEORADIO2TC_CONFIG_TC       =   0x01,
	NEORADIO2TC_CONFIG_CJ       =   0x02,
	NEORADIO2TC_CONFIG_TCCJ     =   0x03
} neoRADIO2TC_EnableSetting;

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
