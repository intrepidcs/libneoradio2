#pragma once
#include "stdint.h"
#include "neoRAD-IO2-TC.h"
#include "neoRAD-IO2-AIN.h"
#include "neoRAD-IO2-AOUT.h"
#include "neoRAD-IO2-PWRRLY.h"
#include "neoRADIO2_frames.h"
#include "fifo.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

#define NEORADIO2_RX_BUFFER_SIZE 170
#define NEORADIO2_TX_BUFFER_SIZE 170
#define NEORADIO2_RX_PACKET_BUFFER_SIZE 8

#define CRC_POLYNIMIAL 0x07


typedef enum _neoRADIO2_SettingsStates {
	neoRADIO2Settings_NeedsRead = 0,
	neoRADIO2Settings_NeedsWrite = 1,
	neoRADIO2Settings_Valid = 2,
} neoRADIO2_SettingsStates;


typedef enum _neoRADIO2_RunStates {
    neoRADIO2state_Disconnected,
    neoRADIO2state_ConnectedWaitForAppStart,
    neoRADIO2state_ConnectedWaitIdentResponse,
	neoRADIO2state_ConnectedWaitReadSettings,
	neoRADIO2state_ConnectedWaitWriteSettings,
    neoRADIO2state_Connected,
} neoRADIO2_RunStates;

typedef struct _neoRADIO2_Sensor_Status {
    float data;
    uint8_t isConnected;
} neoRADIO2_Sensor_Status;


typedef struct _neoRADIO2_DeviceInfo {
    neoRADIO2_RunStates State;
    uint8_t LastDevice;
    uint8_t LastBank;
    uint64_t Timeus;
    uint8_t isOnline;
    uint8_t readCount;
    uint8_t rxbuffer[NEORADIO2_RX_BUFFER_SIZE];
    uint8_t txbuffer[NEORADIO2_TX_BUFFER_SIZE];
    fifo_t rxfifo;
    fifo_t txfifo;
    neoRADIO2frame rxDataBuffer[NEORADIO2_RX_PACKET_BUFFER_SIZE];
    neoRADIO2_Sensor_Status bankSensor[9];
    uint8_t rxDataCount;
} PACKED neoRADIO2_DeviceInfo;

extern const unsigned int neoRADIO2GetDeviceNumberOfBanks[];


int neoRADIO2ProcessIncomingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus);
int neoRADIO2SetSettings(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SetOnline(neoRADIO2_DeviceInfo * deviceInfo, int online);
int neoRADIO2RequestSettings(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SettingsValid(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2SerialToString(char * string, uint32_t serial);
neoRADIO2_deviceTypes neoRADIO2GetGetDeviceType(neoRADIO2_DeviceInfo * deviceInfo, uint8_t device, uint8_t bank);
int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SendJumpToApp(neoRADIO2_DeviceInfo * deviceInfo);
int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, uint8_t device, uint8_t bank, uint8_t * data, uint8_t len, uint8_t blocking);
void neoRADIO2LookForDevicePackets(neoRADIO2_DeviceInfo * deviceInfo);
void neoRADIO2ToggleLED(neoRADIO2_DeviceInfo* deviceInfo, uint8_t device, uint8_t bank, int ms);
int neoRADIO2InitInterfaceHardware(neoRADIO2_DeviceInfo* deviceInfo);



#ifdef _MSC_VER
#pragma pack(pop)
#undef PACKED
#else
#undef PACKED
#endif

#ifdef __cplusplus
}
#endif
