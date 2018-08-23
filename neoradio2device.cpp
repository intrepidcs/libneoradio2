#include "neoradio2device.h"

#include "radio2_frame.h"

#include <cassert>
#include <string>
#include <sstream>

//#define DEBUG_RADIO2_THREAD_DO_NOTHING

bool isDeviceHeaderId(uint8_t id)
{
	return id == 0x55;
}

bool isHostHeaderId(uint8_t id)
{
	return id == 0xAA;
}

bool isValidHeaderId(uint8_t id)
{
	return isHostHeaderId(id) || isDeviceHeaderId(id);
}

neoRADIO2Device::~neoRADIO2Device()
{
	quit(true);
}

bool neoRADIO2Device::quit(bool wait_for_quit)
{
	mMutex.lock();
	mQuit = true;
	mMutex.unlock();
	if (wait_for_quit)
		mThread->join();
	delete mThread;
	mThread = nullptr;
	return true;
}

// this code will loop forever until you return false or user requested a quit()
bool neoRADIO2Device::runIdle()
{
	return HidDevice::runIdle();
}

bool neoRADIO2Device::runConnecting()
{
	mSpecialChannel = CHANNEL_1;

	const int baudrate = 250000;
	auto success =  HidDevice::runConnecting();
	if (!success)
	{
		DEBUG_PRINT("HidDevice::runConnecting() failed!");
		return success;
	}

	uint8_t buffer[64]{0};
	uint16_t buffer_size = sizeof(buffer);

#if !defined(DEBUG_RADIO2_THREAD_DO_NOTHING)
	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// Setup FT260 Clock 4.4.3
	DEBUG_PRINT("Setup FT260 Clock");
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0xA1; // report id;
	buffer[1] = 0x1; // Set Clock
	buffer[2] = 0x2; // 0x0 = 12MHz, 0x1 = 24MHz, 0x2 = 48MHz
	buffer_size = sizeof(buffer);
	success = write(buffer, &buffer_size, CHANNEL_SPECIAL) && buffer_size == sizeof(buffer);
	if (!success)
	{
		DEBUG_PRINT("Setup FT260 Clock failed!");
		return success;
	}

	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.4.8 Select GPIOA Function
	DEBUG_PRINT("Select GPIOA Function TX_LED");
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0xA1; // report id;
	buffer[1] = 0x08; // GPIOA Function
	buffer[2] = 0x04; //TX_LED
	buffer_size = sizeof(buffer);
	success = write(buffer, &buffer_size, CHANNEL_SPECIAL) && buffer_size == sizeof(buffer);
	if (!success)
	{
		DEBUG_PRINT("Select GPIOA Function TX_LED failed!");
		return success;
	}

	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.4.9 Select GPIOG Function
	DEBUG_PRINT("Select GPIOG Function RX_LED");
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0xA1; // report id;
	buffer[1] = 0x09; // GPIOG Function
	buffer[2] = 0x05; // RX_LED
	buffer_size = sizeof(buffer);
	success = write(buffer, &buffer_size, CHANNEL_SPECIAL) && buffer_size == sizeof(buffer);
	if (!success)
	{
		DEBUG_PRINT("Select GPIOG Function RX_LED failed!");
		return success;
	}

	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.7.1 GPIO Write Request
	DEBUG_PRINT("GPIO Write Request - GPIOH Output");
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0xB0; // report id;
	buffer[1] = 0x00; // GPIO0
	buffer[2] = 0x00; // Input
	buffer[3] = 0x80; // GPIOH
	buffer[4] = 0x80; // Output
	buffer_size = sizeof(buffer);
	success = write(buffer, &buffer_size, CHANNEL_SPECIAL) && buffer_size == sizeof(buffer);
	if (!success)
	{
		DEBUG_PRINT("GPIO Write Request - GPIOH Output failed!");
		return success;
	}

	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.4.17 Configure UART 
	DEBUG_PRINT("Configure UART - %d baudrate", baudrate);
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0xA1; // report id;
	buffer[1] = 0x41; // UART_CONFIG;
	buffer[2] = 0x04; // UART_CONFIG_FLOW_CTRL_OFF;
	buffer[3] = (baudrate & 0xFF);
	buffer[4] = ((baudrate >> 8) & 0xFF);
	buffer[5] = ((baudrate >> 16) & 0xFF);
	buffer[6] = ((baudrate >> 24) & 0xFF);
	buffer[7] = 0x08; // UART_CONFIG_DATA_BIT_8;
	buffer[8] = 0x00; // UART_CONFIG_PARITY_OFF;
	buffer[9] = 0x00; // UART_CONFIG_STOP_BIT_1;
	buffer[10] = 0x00; // UART_CONFIG_BREAKING_OFF;
	buffer_size = sizeof(buffer);
	success = write(buffer, &buffer_size, CHANNEL_SPECIAL) && buffer_size == sizeof(buffer);
	if (!success)
	{
		DEBUG_PRINT("Configure UART - %d baudrate failed!", baudrate);
		return success;
	}

	/*
	// Identify the chain before we do anything
	using namespace std::chrono;
	DEBUG_PRINT("Configure Chain - Initial");
	if (!identifyChain(0xFF, 3s))
	{
		changeState(DeviceStateDisconnecting);
		return false;
	}
	*/
	mThread = new std::thread(&neoRADIO2Device::start, this);
	DEBUG_PRINT("Changing state to connected!");
	changeState(DeviceStateConnected);
	return success;
#else
	changeState(DeviceStateConnected);
	return true;
#endif // defined(DEBUG_RADIO2_THREAD_DO_NOTHING)
}

bool neoRADIO2Device::runConnected()
{
	return HidDevice::runConnected();
}

bool neoRADIO2Device::runDisconnecting()
{
	return HidDevice::runDisconnecting();
}


void neoRADIO2Device::start()
{
	
#ifdef ENABLE_DEBUG_PRINT
	auto id = std::this_thread::get_id();
	std::stringstream temp;
	temp << "THREAD ID: " << id;
	DEBUG_PRINT("Started... %s", temp.str().c_str());
#endif // ENABLE_DEBUG_PRINT
	using namespace std::chrono;

	mMutex.lock();
	mIsRunning = true;
	mMutex.unlock();

	uint8_t buffer[1024]{0};
	uint16_t buffer_size = 0;

	uint8_t* header_ptr;
	uint8_t calculated_checksum = 0;
	bool identify_at_least_once = false;
	bool checksum_passed = false;
	int i=0;
	while (!mQuit)
	{
#if defined(DEBUG_RADIO2_THREAD_DO_NOTHING)
		std::this_thread::sleep_for(1ms);
		continue;
#else
		// Nothing to do if we aren't connected
		if (state() != DeviceStateConnected)
		{
#ifdef DEBUG_ANNOYING
			DEBUG_PRINT("NOT CONNECTED!");
#endif // DEBUG_ANNOYING
			std::this_thread::sleep_for(1ms);
			continue;
		}
#if defined(IDENTIFY_CHAIN_ON_CONNECT)
		if (!identify_at_least_once)
		{
			identify_at_least_once = identifyChain();
		}
#endif // IDENTIFY_CHAIN_ON_CONNECT
		auto start_time = std::chrono::high_resolution_clock::now();
		bool success = false;
		// If you hate yourself, uncomment this line:
		//DEBUG_PRINT("Last State: %d", mLastState);
		switch (mLastState)
		{
		case PROCESS_STATE_IDLE:
			if (canRead(CHANNEL_1) >= 1)
			{
				memset(buffer, 0, sizeof(buffer));
				buffer_size = 1;
				// Can we read a byte and is it valid?
				if (!read(buffer, &buffer_size, CHANNEL_1))
				{
					DEBUG_PRINT("Failed to read!");
					break;
				}
				auto data = buffer[0];
				if (!isValidHeaderId(data))
				{
					DEBUG_PRINT("WARNING: Dropping %d bytes due to invalid start of frame (data: 0x%x)", buffer_size, buffer[0]);
					break;
				}
				mLastframe.reset();
				mLastframe.frame()->header.start_of_frame = data;
				mLastState = PROCESS_STATE_HEADER;
			}
			break;
		case PROCESS_STATE_HEADER:
			// Do we have enough data to serialize the header?
			buffer_size = sizeof(mLastframe.frame()->header) - sizeof(mLastframe.frame()->header.start_of_frame);
			if (canRead(CHANNEL_1) < buffer_size)
				break;
			memset(buffer, 0, sizeof(buffer));
			if (!read(buffer, &buffer_size, CHANNEL_1))
			{
				break; // TODO: We are essentially dropping bytes here
			}
			header_ptr = (uint8_t*)&mLastframe.frame()->header;
			// Increment the pointer past the start of frame since we already have it
			header_ptr += sizeof(mLastframe.frame()->header.start_of_frame);
			memcpy(header_ptr, buffer, buffer_size);

			// The bank is an index and not a bitmask when the response comes back from the device
			// lets change that here
			mLastframe.frame()->header.bank = (1 << mLastframe.frame()->header.bank);
			DEBUG_PRINT("mLastframe.frame()->header.bank = 0x%x", mLastframe.frame()->header.bank);
			// Copy the command_status into the correct CommandHandler
			mBankCmds.updateBankCmd(mLastframe.frame(), COMMAND_STATE_RECEIVED_HEADER);
			if (isHostHeaderId(mLastframe.frame()->header.start_of_frame))
			{
				DEBUG_PRINT("Received Host Header          (cmd: %d, data_len: %d, device: %d, bank: %d)", mLastframe.frame()->header.command_status, mLastframe.frame()->header.len, mLastframe.frame()->header.device, mLastframe.frame()->header.bank);
			}
			else
			{
				DEBUG_PRINT("Received Device Report Header (cmd: %d, data_len: %d, device: %d, bank: %d)", mLastframe.frame()->header.command_status, mLastframe.frame()->header.len, mLastframe.frame()->header.device, mLastframe.frame()->header.bank);
			}
			mLastState = PROCESS_STATE_DATA;
			break;
		case PROCESS_STATE_DATA:
			if (mLastframe.frame()->header.len == 0)
			{
				// skip the data process because we won't have any
				mLastState = PROCESS_STATE_CRC;
				break;
			}

			// Do we have enough data to serialize the header's data?
			buffer_size = mLastframe.frame()->header.len; // - sizeof(mLastframe.frame()->crc);
			if (canRead(CHANNEL_1) <= buffer_size)
				break;
			if (!read(buffer, &buffer_size, CHANNEL_1))
			{
				mBankCmds.updateBankCmd(mLastframe.frame(), COMMAND_STATE_ERROR);
				mLastState = PROCESS_STATE_FINISHED;
				break; // TODO: We are essentially dropping bytes here
			}
			// Copy the data into mLastframe
			if (buffer_size > sizeof(mLastframe.frame()->data))
			{
				mBankCmds.updateBankCmd(mLastframe.frame(), COMMAND_STATE_ERROR);
				mLastState = PROCESS_STATE_FINISHED;
				break; // TODO: We are essentially dropping bytes here
			}
			memcpy(&mLastframe.frame()->data, buffer, buffer_size);
			// Add data and Update the command
			mBankCmds.updateBankData(mLastframe.frame());
			mLastState = PROCESS_STATE_CRC;
			break;
		case PROCESS_STATE_CRC:
			buffer_size = sizeof(mLastframe.frame()->crc);
			// Do we have enough data to serialize the header's crc?
			if (canRead(CHANNEL_1) < buffer_size)
				break;
			if (!read(buffer, &buffer_size, CHANNEL_1))
			{
				mBankCmds.updateBankCmd(mLastframe.frame(), COMMAND_STATE_ERROR);
				mLastState = PROCESS_STATE_FINISHED;
				break; // TODO: We are essentially dropping bytes here
			}
			// add the crc to the frame
			mLastframe.frame()->crc = buffer[0];
#if !defined(SKIP_CHECKSUM_VERIFY)
			// CRC is going to fail since we changed the bank from index, lets put it back for calculation
			for (i=0; mLastframe.frame()->header.bank >>= 1; ++i) {}
			mLastframe.frame()->header.bank = i;
			calculated_checksum = 0;
			checksum_passed = verifyFrameChecksum(mLastframe.frame(), &calculated_checksum);
			mLastframe.frame()->header.bank = (1 << mLastframe.frame()->header.bank);
			mBankCmds.updateBankCmd(mLastframe.frame(), (checksum_passed ? COMMAND_STATE_FINISHED : COMMAND_STATE_CRC_ERROR));
			mLastState = PROCESS_STATE_FINISHED;
			if (!checksum_passed)
			{
				DEBUG_PRINT("ERROR: CRC Failure: Got %d, Expected %d", mLastframe.frame()->crc, calculated_checksum);
			}
#endif
			break;
		case PROCESS_STATE_FINISHED:
			auto bank = 0;
			for (int i=8; i > 0; --i)
				if (mLastframe.frame()->header.bank & (1 << i))
					bank = i;
			if (bank >= 8)
				bank = 0;
			// Figure out how many devices are on the chain here
			if (isDeviceHeaderId(mLastframe.frame()->header.start_of_frame) && mLastframe.frame()->header.command_status == NEORADIO2_STATUS_IDENTIFY)
			{
				auto device_count = mLastframe.frame()->header.device + 1;
				updateDeviceCount(device_count > mDeviceCount ? device_count : mDeviceCount);
				DEBUG_PRINT("Device Count = %d", mDeviceCount);
			}
			auto cmd = mBankCmds.getCmdOffset(mLastframe.frame());
			if (isHostHeaderId(mLastframe.frame()->header.start_of_frame))
				DEBUG_PRINT("Finalized Host command:          %s - status: %d (device: %d bank: %d)", mBankCmds[0]->name(cmd).c_str(),
					mBankCmds[bank]->getState(mLastframe.frame()->header.command_status), mLastframe.frame()->header.device, bank);
			else
				DEBUG_PRINT("Finalized Device Report command: %s - status: %d (device: %d bank: %d)", mBankCmds[0]->name(cmd).c_str(),
					mBankCmds[bank]->getState(mLastframe.frame()->header.command_status), mLastframe.frame()->header.device, bank);
			/* TODO
			if (isHostHeaderId(mLastframe.frame()->header.start_of_frame))
			{
				DEBUG_PRINT("Finalized Host command: %s - status: %d", mSentCmds.name(mLastframe.frame()->header.command_status).c_str(),
					mSentCmds.getState(mLastframe.frame()->header.command_status));
			}
			else
			{
				DEBUG_PRINT("Finalized Device command: %s - status: %d", mDeviceCmds.name(mLastframe.frame()->header.command_status).c_str(),
					mDeviceCmds.getState(mLastframe.frame()->header.command_status));
			}
			*/
			mLastState = PROCESS_STATE_IDLE;
			break; // TODO
		}

		// Make sure we don't hog the CPU
		auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
		if (elapsed_time < 3ms)
			std::this_thread::sleep_for(3ms - elapsed_time);
#endif // DEBUG_RADIO2_THREAD_DO_NOTHING
	}

	mMutex.lock();
	mIsRunning = false;
	mMutex.unlock();

#ifdef ENABLE_DEBUG_PRINT
	auto id2 = std::this_thread::get_id();
	std::stringstream temp2;
	temp2 << "THREAD ID: " << id2;
	DEBUG_PRINT("Stopping... %s", temp2.str().c_str());
#endif // ENABLE_DEBUG_PRINT
}

bool neoRADIO2Device::isOpen()
{
	return mBankCmds[0]->getState((uint8_t)NEORADIO2_COMMAND_IDENTIFY) == COMMAND_STATE_FINISHED && Device::isOpen();
}

bool neoRADIO2Device::writeUartFrame(neoRADIO2frame* frame, DeviceChannel channel)
{
	if (!frame)
		return false;
	if (!generateFrameChecksum(frame))
	{
		DEBUG_PRINT("ERROR: Failed to generate frame checksum!");
		return false;
	}
	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.6.2 UART Write Request - Report ID = 0xF0
	const int max_data_payload = 60;
	uint8_t buffer[64] ={0};
	uint16_t size = sizeof(frame->header)+2+1; // frame + ft260 header + crc
											   // copy the header into the buffer
	memcpy(&buffer[2], (uint8_t*)&frame->header, sizeof(frame->header));
	// copy the data into the buffer
	if (frame->header.len)
	{
		memcpy(&buffer[2+sizeof(frame->header)], (uint8_t*)&frame->data, frame->header.len);
		// update the size for the data
		size += frame->header.len;
	}
	// copy the crc into the buffer
	buffer[2+sizeof(frame->header)+frame->header.len] = frame->crc;
	if (size > max_data_payload)
		return false;
	// calculate the FT260 header
	buffer[0] = 0xF0;
	buffer[0] = (0xF0 + (size/4)) & 0xFF;
	buffer[1] = (uint8_t)size;

	const int sent_size = size;
	return write(buffer, &size, channel) && size == sent_size;
}

bool neoRADIO2Device::identifyChain(std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	const int buffer_size = 64;
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_IDENTIFY, // command_status
			0xFF, 
			0xFF, // bank
			3, // len
		},
		{ // data
			NEORADIO2_DEVTYPE_HOST,
			0xFF,
			0xFF,
		},
		0, // CRC
	};
	// send the packets
	DEBUG_PRINT("Identifying chain...");
	auto cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_STATUS_IDENTIFY);
	mBankCmds.setCommandState(cmd, COMMAND_STATE_RESET);
	mBankCmds.updateBankCmd(&frame, COMMAND_STATE_RESET);
	mBankCmds.updateBankData(&frame);

	bool success = writeUartFrame(&frame, CHANNEL_1);
	
	return success && mBankCmds.isStateSet(cmd, COMMAND_STATE_FINISHED, timeout);
}

bool neoRADIO2Device::isChainIdentified(std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	auto cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_STATUS_IDENTIFY);
	return  mBankCmds.isStateSet(cmd, COMMAND_STATE_FINISHED, timeout);
}

bool neoRADIO2Device::doesChainNeedIdentify(std::chrono::milliseconds timeout)
{
	// We don't need a bank here because if we are broadcasting STATUS_NEED_ID its all set to zero because its unknown.
	auto cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_STATUS_NEED_ID);
	return mBankCmds[0]->isStateSet(cmd, COMMAND_STATE_FINISHED, timeout);
}

bool neoRADIO2Device::getIdentifyResponse(int device, int bank, neoRADIO2frame_identifyResponse& response, std::chrono::milliseconds timeout)
{
	// reset the frame
	memset(&response, 0, sizeof(response));
	// can't do anything without the chain identified
	if (!isChainIdentified(timeout))
		return false;
	// Grab the data received from the chain response
	auto cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_COMMAND_IDENTIFY);
	std::vector<uint8_t> data = mBankCmds[bank]->getData(cmd);
	// Check size and finally copy data into response struct
	if (data.size() != sizeof(response))
		return false;
	memcpy(&response, data.data(), sizeof(response));
	return true;
}

bool neoRADIO2Device::getChainCount(int& count, bool identify, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	if (!isChainIdentified(0s))
	{
		if (!identify)
			return false;
		if (!identifyChain(timeout))
			return false;
	}
	std::lock_guard<std::mutex> lock(mMutex);
	count = mDeviceCount;
	return true;
}

bool neoRADIO2Device::startApplication(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_START, // command_status
			device, 
			(1 << bank), // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset all commands as we are technically resetting the processor
	mBankCmds.setCommandStateForAll(COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Give the device time to boot
	std::this_thread::sleep_for(100ms);
	// Identify the chain again
	return identifyChain(timeout) && isApplicationStarted(device, bank, timeout);
}

bool neoRADIO2Device::enterBootloader(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_ENTERBOOT, // command_status
			device, 
			(1 << bank), // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset all commands as we are technically resetting the processor
	mBankCmds.setCommandStateForAll(COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Give the device time to boot
	std::this_thread::sleep_for(100ms);
	// Identify the chain again
	return identifyChain(timeout) && !isApplicationStarted(device, bank, 0s);
}

bool neoRADIO2Device::isApplicationStarted(int device, int bank, std::chrono::milliseconds timeout)
{
	// Chain needs to be identified in order to see if we are in bootloader
	if (!isChainIdentified(timeout))
		return false;
	neoRADIO2frame_identifyResponse response;
	if (!getIdentifyResponse(device, bank, response, timeout))
		return false;
	return (response.current_state == NEORADIO2STATE_RUNNING);
}

bool neoRADIO2Device::getSerialNumber(int device, int bank, unsigned int& sn, std::chrono::milliseconds timeout)
{
	// Chain needs to be identified in order to see if we are in bootloader
	if (!isChainIdentified(timeout))
		return false;
	neoRADIO2frame_identifyResponse response;
	if (!getIdentifyResponse(device, bank, response, timeout))
		return false;
	sn = response.serial_number;
	return true;
}

bool neoRADIO2Device::getManufacturerDate(int device, int bank, int& year, int& month, int& day, std::chrono::milliseconds timeout)
{
	// Chain needs to be identified in order to see if we are in bootloader
	if (!isChainIdentified(timeout))
		return false;
	neoRADIO2frame_identifyResponse response;
	if (!getIdentifyResponse(device, bank, response, timeout))
		return false;
	year = response.manufacture_year;
	month = response.manufacture_month;
	day = response.manufacture_day;
	return true;
}

bool neoRADIO2Device::getDeviceType(int device, int bank, int device_type, std::chrono::milliseconds timeout)
{
	// Chain needs to be identified in order to see if we are in bootloader
	if (!isChainIdentified(timeout))
		return false;
	neoRADIO2frame_identifyResponse response;
	if (!getIdentifyResponse(device, bank, response, timeout))
		return false;
	device_type = response.device_type;
	return true;
}

bool neoRADIO2Device::getFirmwareVersion(int device, int bank, int& major, int& minor, std::chrono::milliseconds timeout)
{
	// Chain needs to be identified in order to see if we are in bootloader
	if (!isChainIdentified(timeout))
		return false;
	neoRADIO2frame_identifyResponse response;
	if (!getIdentifyResponse(device, bank, response, timeout))
		return false;
	major = response.firmwareVersion_major;
	minor = response.firmwareVersion_minor;
	return true;
}

bool neoRADIO2Device::getHardwareRevision(int device, int bank, int& major, int& minor, std::chrono::milliseconds timeout)
{
	// Chain needs to be identified in order to see if we are in bootloader
	if (!isChainIdentified(timeout))
		return false;
	neoRADIO2frame_identifyResponse response;
	if (!getIdentifyResponse(device, bank, response, timeout))
		return false;
	major = response.hardware_revMajor;
	minor = response.hardware_revMinor;
	return true;
}

bool neoRADIO2Device::getPCBSN(int device, int bank, std::string& pcb_sn, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	pcb_sn.clear();
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_READ_PCBSN, // command_status
			device, 
			(1 << bank), // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset command
	auto cmd = mBankCmds.getCmdOffset(0xAA, NEORADIO2_COMMAND_READ_PCBSN);
	auto response_cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_STATUS_READ_PCBSN);

	mBankCmds[bank]->setState(response_cmd, COMMAND_STATE_RESET);

	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	bool success = false;
	success = mBankCmds[bank]->isStateSet(response_cmd, COMMAND_STATE_FINISHED, timeout);
	if (success)
	{
		std::stringstream ss;
		auto cmd_data = mBankCmds[bank]->getData(response_cmd);
		for (unsigned int i=0; i < cmd_data.size(); ++i)
			ss << (char)cmd_data[i];
		pcb_sn = ss.str();
	}
	return success;
}

bool neoRADIO2Device::readSensor(int device, int bank, std::vector<uint8_t>& data, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	data.clear();
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_READ_DATA, // command_status
			device, 
			(1 << bank), // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset command
	auto cmd = mBankCmds.getCmdOffset(0xAA, NEORADIO2_COMMAND_READ_DATA);
	auto response_cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_STATUS_SENSOR);
	mBankCmds[bank]->setState(response_cmd, COMMAND_STATE_RESET);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	bool success = mBankCmds[bank]->isStateSet(response_cmd, COMMAND_STATE_FINISHED, timeout);
	if (success)
	{
		auto cmd_data = mBankCmds[bank]->getData(response_cmd);
		std::copy(cmd_data.begin(), cmd_data.end(), std::back_inserter(data));
	}
	return success;
}

bool neoRADIO2Device::readSettings(int device, int bank, neoRADIO2_deviceSettings& settings, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	memset(&settings, 0, sizeof(settings));
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_READ_SETTINGS, // command_status
			device, 
			(1 << bank), // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset command
	auto cmd = mBankCmds.getCmdOffset(0xAA, NEORADIO2_COMMAND_READ_SETTINGS);
	auto response_cmd = mBankCmds.getCmdOffset(0x55, NEORADIO2_STATUS_READ_SETTINGS);
	mBankCmds[bank]->setState(response_cmd, COMMAND_STATE_RESET);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	bool success = false;
	success = mBankCmds[bank]->isStateSet(response_cmd, COMMAND_STATE_FINISHED, timeout);
	if (success)
	{
		auto cmd_data = mBankCmds[bank]->getData(response_cmd);
		if (cmd_data.size() != sizeof(settings))
			return false;
		memcpy(&settings, cmd_data.data(), cmd_data.size());
	}
	return success;
}

bool neoRADIO2Device::writeSettings(int device, int bank, neoRADIO2_deviceSettings& settings, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_WRITE_SETTINGS, // command_status
			device, 
			(1 << bank), // bank
			sizeof(settings), // len
		},
		{ // data
		},
		0 // crc
	};
	// copy the settings into the frame
	memcpy(frame.data, &settings, sizeof(settings));

	// Reset command
	auto cmd = mBankCmds.getCmdOffset(0xAA, NEORADIO2_COMMAND_WRITE_SETTINGS);
	//auto response_cmd = mBankCmds.getCmdOffset(0xAA, NEORADIO2_COMMAND_WRITE_SETTINGS);
	mBankCmds[bank]->setState(cmd, COMMAND_STATE_RESET);

	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mBankCmds[bank]->isStateSet(cmd, COMMAND_STATE_FINISHED, timeout);
}

void neoRADIO2Device::updateDeviceCount(int device_count)
{
	mMutex.lock();
	mDeviceCount = device_count;
	mMutex.unlock();
}

uint8_t neoRADIO2Device::crc8_Calc(uint8_t* data, int len)
{
	static bool _is_initialized = false;
	static uint8_t crc8_table[256];
	if (!_is_initialized)
	{
		uint8_t crc;
		const unsigned int CRC_POLYNOMIAL = 0x07;

		for (int i = 0; i < 256; i++)
		{
			crc = i;
			for (int j = 0; j < 8; j++)
			{
				crc = (crc << 1) ^ ((crc & 0x80) ? CRC_POLYNOMIAL : 0);
			}
			crc8_table[i] = crc & 0xFF;
		}
		_is_initialized = true;
	}
	volatile uint8_t crc = 0;
	for (int i = 0; i < len; i++)
	{
		crc = crc8_table[crc ^ data[i]] & 0xFF;
	}
	return crc;
}

bool neoRADIO2Device::generateFrameChecksum(neoRADIO2frame* frame)
{
	if (!frame)
		return false;
	uint8_t buffer[sizeof(*frame)];
	memcpy(buffer, &frame->header, sizeof(frame->header));
	memcpy(&buffer[sizeof(frame->header)], frame->data, frame->header.len);
	frame->crc = crc8_Calc(buffer, sizeof(frame->header)+frame->header.len);
	return true;
}

bool neoRADIO2Device::verifyFrameChecksum(neoRADIO2frame* frame, uint8_t* calculated_checksum)
{
	if (!frame)
		return false;
	uint8_t buffer[sizeof(*frame)];
	memcpy(buffer, &frame->header, sizeof(frame->header));
	memcpy(&buffer[sizeof(frame->header)], frame->data, frame->header.len);
	auto checksum = crc8_Calc(buffer, sizeof(frame->header)+frame->header.len);
	if (calculated_checksum)
		*calculated_checksum = checksum;
	return checksum == frame->crc;
}

bool neoRADIO2Device::resetCommands(int start_of_frame, int cmd, int banks)
{
	if (!mBankCmds.areBankMasksValid(banks))
		return false;
	for (int i=0; i < 8; ++i)
	{
		if ((1 << i) & banks)
		{
			auto cmd_offset = mBankCmds.getCmdOffset(start_of_frame, cmd);
			mBankCmds[i]->setState(cmd_offset, COMMAND_STATE_RESET);
			mBankCmds[i]->clearData(cmd_offset);
		}
	}
	return true;
}