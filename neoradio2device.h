#ifndef __NEORADIO2_DEVICE_H_
#define __NEORADIO2_DEVICE_H_

#include "hiddevice.h"
#include "command_handler.h"
#include "commandhandlerbank.h"
#include "neoradio2framehandler.h"
#include <unordered_map>

#include "radio2_frame.h"


class neoRADIO2Device : public HidDevice
{
public:

	neoRADIO2Device(DeviceInfoEx& di)
		: HidDevice(di)
	{
		// Host frame commands
		mBankCmds.addCmdOffset(0xAA, 0x100);
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_START, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_IDENTIFY, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_WRITE_DATA, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_READ_DATA, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_WRITE_SETTINGS, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_READ_SETTINGS, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_WRITE_CAL, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_READ_CAL, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_TOGGLE_LED, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_READ_PCBSN, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_BL_WRITEBUFFER, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_BL_WRITETOFLASH, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_BL_VERIFY, 0x100));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_COMMAND_ENTERBOOT, 0x100));

		// Device Report frame commands
		mBankCmds.addCmdOffset(0x55, 0);
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_STATUS_SENSOR, 0));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_STATUS_FIRMWARE, 0));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_STATUS_IDENTIFY, 0));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_STATUS_READ_SETTINGS, 0));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_STATUS_NEED_ID, 0));
		mBankCmds.addCommand(cmd_handler_add_param_offset(NEORADIO2_STATUS_READ_PCBSN, 0));

		mIsRunning = false;
		mQuit = false;
		mLastState = PROCESS_STATE_IDLE;

		mDeviceCount = 1;
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

	bool identifyChain(std::chrono::milliseconds timeout);
	bool isChainIdentified(std::chrono::milliseconds timeout);
	bool doesChainNeedIdentify(std::chrono::milliseconds timeout);
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
	bool getPCBSN(int device, int bank, std::string& pcb_sn, std::chrono::milliseconds timeout);

	bool readSensor(int device, int bank, std::vector<uint8_t>& data, std::chrono::milliseconds timeout);

	// neoRADIO2_deviceSettings
	bool readSettings(int device, int bank, neoRADIO2_deviceSettings& settings, std::chrono::milliseconds timeout);
	bool writeSettings(int device, int bank, neoRADIO2_deviceSettings& settings, std::chrono::milliseconds timeout);

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
	CommandHandlerBanks<unsigned int, _CommandStates> mBankCmds;

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

	bool resetCommands(int start_of_frame, int cmd, int banks);
};

#endif // __NEORADIO2_DEVICE_H_
