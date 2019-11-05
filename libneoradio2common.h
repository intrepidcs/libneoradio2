#ifndef __LIBNEORADIO2_COMMON_H__
#define __LIBNEORADIO2_COMMON_H__


#include <stdint.h>
#if defined(_MSC_VER)
// I spent about an hour trying to get visual studio project files 
// to try and include this submodule header... giving up and "hardcoding" it here
#include "neoRAD-IO2-FrameDescription\radio2_frames.h"
#else
#include "radio2_frames.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _CommandStatus
{
	StatusInProgress = 0,
	StatusFinished,
	StatusError,
} CommandStatus;

typedef enum _CommandStateType
{
	CommandStateHost = 0,
	CommandStateDevice,

	CommandStateUnknown = -1,
} CommandStateType;

// API Success
#define NEORADIO2_SUCCESS 0
// API General Failure
#define NEORADIO2_FAILURE 1
// API non-blocking mode would block
#define NEORADIO2_ERR_WBLOCK 2
// API non-blocking mode in progress
#define NEORADIO2_ERR_INPROGRESS 3
// API non-blocking mode resulted in failure
#define NEORADIO2_ERR_FAILURE 4

#define NEORADIO2_MAX_DEVS 8

#define neoradio2_handle long

typedef struct _Neoradio2DeviceInfo
{
	char name[64];
	char serial_str[64];

	int vendor_id;
	int product_id;

	uint8_t _reserved[32];
} Neoradio2DeviceInfo;

typedef enum _StatusType
{
	StatusChain = 0,
	StatusAppStart,
	StatusPCBSN,
	StatusSensorRead,
	StatusSensorWrite,
	StatusSettingsRead,
	StatusSettingsWrite,
	StatusCalibration,
	StatusCalibrationPoints,
	StatusCalibrationStored,
	StatusCalibrationInfo,
	StatusLedToggle,

	StatusMax,
} StatusType;

#ifdef __cplusplus
}
#endif

#endif // __LIBNEORADIO2_COMMON_H__