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

	neoRADIO2Device(DeviceInfoEx& di)
		: HidDevice(di)
	{
		mIsRunning = false;
		mQuit = false;
		mLastState = PROCESS_STATE_IDLE;
		mDeviceCount = 1;

		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_START);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_IDENTIFY);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_WRITE_DATA);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_READ_DATA);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_WRITE_SETTINGS);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_READ_SETTINGS);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_DONT_USE1);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_DONT_USE2);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_TOGGLE_LED);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_READ_PCBSN);

		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_READ_CAL);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_WRITE_CAL);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_WRITE_CALPOINTS);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_STORE_CAL);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_READ_CALPOINTS);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_READ_CAL_INFO);
		
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_BL_WRITEBUFFER);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_BL_WRITETOFLASH);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_BL_VERIFY);
		_InsertEnumIntoMap(mHostFrameCommandNames, NEORADIO2_COMMAND_ENTERBOOT);

		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_SENSOR);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_FIRMWARE);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_IDENTIFY);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_READ_SETTINGS);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_READ_PCBSN);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_CAL);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_CAL_STORE);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_CAL_INFO);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_CALPOINTS);
		_InsertEnumIntoMap(mDeviceFrameCommandNames, NEORADIO2_STATUS_NEED_ID);

		_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_RESET);
		_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_RECEIVED_HEADER);
		_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_RECEIVED_DATA);
		_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_ERROR);
		_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_CRC_ERROR);
		_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_FINISHED);
	}
	virtual ~neoRADIO2Device();

	bool quit(bool wait_for_quit=true);


	static std::vector<neoRADIO2Device*> _findAll()
	{
		std::vector<neoRADIO2Device*> devs;

		hid_device_info* hdi = NULL;
		hid_device_info* first_hdi = hdi;
		if (hdi = hid_enumerate(0x93C, 0x1300))
			while (hdi != NULL)
			{
				auto interface_number = hdi->interface_number;
#ifdef __APPLE__
				// https://github.com/signal11/hidapi/issues/326
				// hidapi after 02/11/17 shouldn't have this problem
				if (interface_number == -1)
				{
					interface_number = hdi->path[strlen(hdi->path)-1] - 0x30;
				}
#endif
				//hdi->serial_number
				DeviceInfoEx di;
				di.is_open = false;
				di.is_blocking = true;
				memset(&di.di, 0, sizeof(di.di));
				if (hdi->product_string)
				{
					di.di.name = new char[64]{0};
					std::wcstombs(di.di.name, hdi->product_string, 64);
				}
				if (hdi->serial_number)
				{
					di.di.serial_str = new char[64]{0};
					std::wcstombs(di.di.serial_str, hdi->serial_number, 64);
				}
				di.di.product_id = hdi->product_id;
				di.di.vendor_id = hdi->vendor_id;

				auto hid_dev = new neoRADIO2Device(di);
				if (interface_number == 0)
				{
					hid_dev->addPath(CHANNEL_0, hdi->path);
					if (hdi->next)
						hid_dev->addPath(CHANNEL_1, hdi->next->path);
					hdi = hdi->next->next;
				}
				else if (interface_number == 1)
				{
					if (hdi->next)
						hid_dev->addPath(CHANNEL_0, hdi->next->path);
					hid_dev->addPath(CHANNEL_1, hdi->path);
					hdi = hdi->next->next;
				}
				else
					hdi = hdi->next;
				devs.push_back(hid_dev);
			}
		hid_free_enumeration(first_hdi);
		return devs;
	}

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
	bool readSettings(int device, int bank, neoRADIO2_deviceSettings& settings);

	bool writeSettings(int device, int bank, neoRADIO2_deviceSettings& settings, std::chrono::milliseconds timeout);

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
