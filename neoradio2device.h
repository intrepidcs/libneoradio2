#ifndef __NEORADIO2_DEVICE_H_
#define __NEORADIO2_DEVICE_H_

#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif

#include "hiddevice.h"
#include "neoradio2framehandler.h"
#include "devicecommandhandler.h"
#include <unordered_map>

#include "radio2_frames.h"


#define _InsertEnumIntoMap(m, cmd) m[cmd] = #cmd

class neoRADIO2Device : public HidDevice
{
public:

	neoRADIO2Device(DeviceInfoEx& di);
	virtual ~neoRADIO2Device();

	bool quit(bool wait_for_quit=true);

	static std::vector<neoRADIO2Device*> _findAll();

	// this code will loop forever until you return false or user requested a quit()
	virtual bool runIdle();
	virtual bool runConnecting();
	virtual bool runConnected();
	virtual bool runDisconnecting();

	virtual bool isOpen();

	//virtual bool readUart(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel);
	bool writeUartFrame(neoRADIO2frame* frame, DeviceChannel channel);

	bool requestIdentifyChain(std::chrono::milliseconds timeout);
	bool isChainIdentified(std::chrono::milliseconds timeout);
	bool getIdentifyResponse(int device, int bank, neoRADIO2frame_identifyResponse& response, std::chrono::milliseconds);

	bool getChainCount(int& count, bool identify, std::chrono::milliseconds timeout);

	bool startApplication(int device, int bank, std::chrono::milliseconds timeout);
	bool isApplicationStarted(int device, int bank, std::chrono::milliseconds timeout);
	bool enterBootloader(int device, int bank, std::chrono::milliseconds timeout);

	bool getSerialNumber(int device, int bank, unsigned int& sn, std::chrono::milliseconds timeout);
	bool getManufacturerDate(int device, int bank, int& year, int& month, int& day, std::chrono::milliseconds timeout);
	bool getDeviceType(int device, int bank, int device_type, std::chrono::milliseconds timeout);
	bool getFirmwareVersion(int device, int bank, int& major, int& minor, std::chrono::milliseconds timeout);
	bool getHardwareRevision(int device, int bank, int& major, int& minor, std::chrono::milliseconds timeout);

	bool requestPCBSN(int device, int bank, std::chrono::milliseconds timeout);
	bool getPCBSN(int device, int bank, std::string& pcbsn);

	bool requestSensorData(int device, int bank, int enable_cal, std::chrono::milliseconds timeout);
	bool readSensorData(int device, int bank, std::vector<uint8_t>& data);

	// neoRADIO2_deviceSettings
	bool requestSettings(int device, int bank, std::chrono::milliseconds timeout);
	bool readSettings(int device, int bank, neoRADIO2_settings& settings);

	bool writeSettings(int device, int bank, neoRADIO2_settings& settings, std::chrono::milliseconds timeout);

	bool requestCalibration(int device, int bank, const neoRADIO2frame_calHeader& header, std::chrono::milliseconds timeout);
	bool readCalibration(int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout);

	bool requestCalibrationPoints(int device, int bank, const neoRADIO2frame_calHeader& header, std::chrono::milliseconds timeout);
	bool readCalibrationPoints(int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout);


	bool writeCalibration(int device, int bank, const neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout);
	bool writeCalibrationPoints(int device, int bank, const neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout);
	bool requestStoreCalibration(int device, int bank, std::chrono::milliseconds timeout);
	bool isCalibrationStored(int device, int bank, bool& stored, std::chrono::milliseconds timeout);
	bool requestCalibrationInfo(int device, int bank, std::chrono::milliseconds timeout);
	bool readCalibrationInfo(int device, int bank, neoRADIO2frame_calHeader& header, std::chrono::milliseconds timeout);

	bool toggleLED(int device, int bank, int ms, std::chrono::milliseconds timeout);

protected:

	typedef enum _CommandStates
	{
		COMMAND_STATE_RESET = 0,
		COMMAND_STATE_RECEIVED_HEADER,
		COMMAND_STATE_RECEIVED_DATA,
		COMMAND_STATE_ERROR,
		COMMAND_STATE_CRC_ERROR,
		COMMAND_STATE_FINISHED,
	} CommandStates;

	typedef enum _ProcessStates
	{
		PROCESS_STATE_IDLE = 0,
		PROCESS_STATE_HEADER,
		PROCESS_STATE_DATA,
		PROCESS_STATE_CRC,
		PROCESS_STATE_FINISHED,
	} ProcessStates;

	bool processStateIdle();
	bool processStateHeader();
	bool processStateData();
	bool processStateCRC();
	bool processStateFinished();

	ProcessStates mLastState;

	neoRADIO2FrameHandler<neoRADIO2frame> mLastframe;
	//std::vector<CommandHandlerBanks<unsigned int, _CommandStates> > mBankCmds;

	//void addDeviceBanks(unsigned int count);
	//CommandHandlerBanks<unsigned int, _CommandStates>* getDeviceBanks(int device_index);

	DeviceCommandHandler<CommandStates> mDCH;

	std::string frameToString(neoRADIO2frame& frame, bool is_bitfield);
	std::map<int, std::string> mHostFrameCommandNames;
	std::map<int, std::string> mDeviceFrameCommandNames;
	std::map<int, std::string> mCommandStateNames;

	// ProcessState Buffer
	static const int COMM_BUFFER_SIZE = 1024*10;
	uint8_t mCommBuffer[COMM_BUFFER_SIZE];

private:
	bool mIsRunning;
	bool mQuit;
	std::thread* mThread;
	std::mutex mMutex;

	int mDeviceCount;

	void updateDeviceCount(int device_count);

	// this is the actual thread loop that calls run()
	void start();

	// Calculate the CRC, this was stolen from firmware in crc8.c
	uint8_t crc8_Calc(uint8_t* data, int len);
	bool generateFrameChecksum(neoRADIO2frame* frame);
	bool verifyFrameChecksum(neoRADIO2frame* frame, uint8_t* calculated_checksum=nullptr);

	//bool resetCommands(int start_of_frame, int cmd, int banks);
};

#endif // __NEORADIO2_DEVICE_H_
