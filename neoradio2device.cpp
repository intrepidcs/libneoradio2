#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif

#include "neoradio2device.h"

#include "radio2_frames.h"

#include <cassert>
#include <string>
#include <sstream>
#include <iterator>

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
	bool is_bitfield = false;
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
		std::vector<uint8_t> data;
		switch (mLastState)
		{
		case PROCESS_STATE_IDLE:
#ifdef DEBUG_ANNOYING
			DEBUG_PRINT("PROCESS_STATE_IDLE");
#endif // DEBUG_ANNOYING
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
			DEBUG_PRINT("PROCESS_STATE_HEADER");
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

			// NEORADIO2_COMMAND_IDENTIFY is both a bitfield and index, to avoid a headache here lets set the device/bank to zero
			if (isHostHeaderId(mLastframe.frame()->header.start_of_frame) && mLastframe.frame()->header.command_status == NEORADIO2_COMMAND_IDENTIFY)
			{
				mLastframe.frame()->header.device = 0x0;
				mLastframe.frame()->header.bank = 0x0;
			}
			is_bitfield = true;
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_RECEIVED_HEADER, is_bitfield);
			DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
			mLastState = PROCESS_STATE_DATA;
			break;
		case PROCESS_STATE_DATA:
			DEBUG_PRINT("PROCESS_STATE_DATA");
			if (mLastframe.frame()->header.len == 0)
			{
				// skip the data process because we won't have any
				mLastState = PROCESS_STATE_CRC;
				break;
			}

			// Do we have enough data to serialize the header's data?
			buffer_size = mLastframe.frame()->header.len; 
			if (canRead(CHANNEL_1) <= buffer_size)
				break;
			if (!read(buffer, &buffer_size, CHANNEL_1))
			{
				mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
				mLastState = PROCESS_STATE_FINISHED;
				break; // TODO: We are essentially dropping bytes here
			}
			// Copy the data into mLastframe
			if (buffer_size > sizeof(mLastframe.frame()->data))
			{
				mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
				mLastState = PROCESS_STATE_FINISHED;
				break; // TODO: We are essentially dropping bytes here
			}
			// Add data and Update the command
			memcpy(&mLastframe.frame()->data, buffer, buffer_size);
			for (int i=0; i < buffer_size; ++i)
				data.push_back(buffer[i]);
			memcpy(data.data(), buffer, buffer_size);
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_RECEIVED_DATA, is_bitfield);
			mDCH.updateData(&mLastframe.frame()->header, data, is_bitfield);
			DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
			mLastState = PROCESS_STATE_CRC;
			break;
		case PROCESS_STATE_CRC:
			DEBUG_PRINT("PROCESS_STATE_CRC");
			buffer_size = sizeof(mLastframe.frame()->crc);
			// Do we have enough data to serialize the header's crc?
			if (canRead(CHANNEL_1) < buffer_size)
				break;
			if (!read(buffer, &buffer_size, CHANNEL_1))
			{
				mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
				DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
				mLastState = PROCESS_STATE_FINISHED;
				break; // TODO: We are essentially dropping bytes here
			}
			// add the crc to the frame
			mLastframe.frame()->crc = buffer[0];
#if !defined(SKIP_CHECKSUM_VERIFY)
			// CRC is going to fail since we changed the bank from index, lets put it back for calculation
			//for (i=0; mLastframe.frame()->header.bank >>= 1; ++i) {}
			//mLastframe.frame()->header.bank = i;
			calculated_checksum = 0;
			checksum_passed = verifyFrameChecksum(mLastframe.frame(), &calculated_checksum);
			//mLastframe.frame()->header.bank = (1 << mLastframe.frame()->header.bank);
			mDCH.updateCommand(&mLastframe.frame()->header, (checksum_passed ? COMMAND_STATE_FINISHED : COMMAND_STATE_CRC_ERROR), is_bitfield);
			mLastState = PROCESS_STATE_FINISHED;
			if (!checksum_passed)
			{
				DEBUG_PRINT("ERROR: CRC Failure: Got %d, Expected %d", mLastframe.frame()->crc, calculated_checksum);
			}
			DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
#endif
			break;
		case PROCESS_STATE_FINISHED:
			DEBUG_PRINT("PROCESS_STATE_FINISHED");
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_FINISHED, is_bitfield);
			// Figure out how many devices are on the chain here
			if (isDeviceHeaderId(mLastframe.frame()->header.start_of_frame) && mLastframe.frame()->header.command_status == NEORADIO2_STATUS_IDENTIFY)
			{
				auto device_count = mLastframe.frame()->header.device + 1;
				updateDeviceCount(device_count > mDeviceCount ? device_count : mDeviceCount);
				DEBUG_PRINT("Device Count = %d", mDeviceCount);
			}
			DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
			DEBUG_PRINT("PROCESS_STATE_FINISHED COMPLETE\n");
			mLastState = PROCESS_STATE_IDLE;
			break;
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
	return Device::isOpen();
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

	DEBUG_PRINT("SENDING FRAME: %s", frameToString(*frame, true).c_str());
	return write(buffer, &size, channel) && size == sent_size;
}

bool neoRADIO2Device::requestIdentifyChain(std::chrono::milliseconds timeout)
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
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_NEED_ID, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::isChainIdentified(std::chrono::milliseconds timeout)
{
	auto count = mDeviceCount;
	auto device = 0;
	for (auto i=0; i < mDeviceCount; ++i)
		device |= (1 << i);
	return /* mDCH.isStateSet(0x55, device, 0xFF, NEORADIO2_STATUS_NEED_ID, COMMAND_STATE_RESET, true, timeout) && */
		mDCH.isStateSet(0x55, device, 0xFF, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::getIdentifyResponse(int device, int bank, neoRADIO2frame_identifyResponse& response, std::chrono::milliseconds timeout)
{
	// reset the frame
	memset(&response, 0, sizeof(response));
	// can't do anything without the chain identified
	if (!isChainIdentified(timeout))
		return false;
	// Grab the data received from the chain response
	std::vector<uint8_t> data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_IDENTIFY);
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
		if (!requestIdentifyChain(timeout))
			return false;
	}
	std::lock_guard<std::mutex> lock(mMutex);
	count = mDeviceCount;
	return true;
}

bool neoRADIO2Device::startApplication(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	if (!isChainIdentified(0s))
		if (!requestIdentifyChain(timeout))
			return false;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_START, // command_status
			(uint8_t)device, 
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_NEED_ID, COMMAND_STATE_RESET, true);
	updateDeviceCount(1);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Give the device time to boot
	std::this_thread::sleep_for(250ms);
	// Is the command set?
	return requestIdentifyChain(timeout);
}

bool neoRADIO2Device::enterBootloader(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	if (!isChainIdentified(0s))
		if (!requestIdentifyChain(timeout))
			return false;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_ENTERBOOT, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_NEED_ID, COMMAND_STATE_RESET, true);
	updateDeviceCount(1);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Give the device time to boot
	std::this_thread::sleep_for(250ms);
	// Is the command set?
	return requestIdentifyChain(timeout);
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

bool neoRADIO2Device::requestPCBSN(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	// This command is only available in application code
	// isApplicationStarted isn't a bitmask
	for (int d=0; d < 8; ++d)
		if ((d << 1) & device)
			for (int b=0; b < 8; ++b)
				if ((b << 1) & bank)
					if (!isApplicationStarted(d, b, 0s))
						return false;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_READ_PCBSN, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_PCBSN, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_PCBSN, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::getPCBSN(int device, int bank, std::string& pcbsn)
{
	pcbsn.clear();
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_READ_PCBSN, COMMAND_STATE_FINISHED, false))
		return false;
	std::vector<uint8_t> data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_READ_PCBSN);
	for (auto d : data)
		pcbsn += (char)d;
	return true;
}

bool neoRADIO2Device::requestSensorData(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	// This command is only available in application code
	// isApplicationStarted isn't a bitmask
	for (int d=0; d < 8; ++d)
		if ((d << 1) & device)
			for (int b=0; b < 8; ++b)
				if ((b << 1) & bank)
					if (!isApplicationStarted(d, b, 0s))
						return false;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_READ_DATA, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_SENSOR, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_SENSOR, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::readSensorData(int device, int bank, std::vector<uint8_t>& data)
{
	data.clear();
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_SENSOR, COMMAND_STATE_FINISHED, false))
		return false;
	std::vector<uint8_t> _data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_SENSOR);
	std::copy(_data.begin(), _data.end(), std::back_inserter(data));
	return true;
}

bool neoRADIO2Device::requestSettings(int device, int bank, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	// This command is only available in application code
	// isApplicationStarted isn't a bitmask
	for (int d=0; d < 8; ++d)
		if ((d << 1) & device)
			for (int b=0; b < 8; ++b)
				if ((b << 1) & bank)
					if (!isApplicationStarted(d, b, 0s))
						return false;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_READ_SETTINGS, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_SETTINGS, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_SETTINGS, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::readSettings(int device, int bank, neoRADIO2_deviceSettings& settings)
{
	memset(&settings, 0, sizeof(settings));
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_READ_SETTINGS, COMMAND_STATE_FINISHED, false))
		return false;
	std::vector<uint8_t> _data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_READ_SETTINGS);
	if (_data.size() != sizeof(settings))
		return false;
	memcpy(&settings, _data.data(), _data.size());
	return true;
}

bool neoRADIO2Device::writeSettings(int device, int bank, neoRADIO2_deviceSettings& settings, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;
	// This command is only available in application code
	// isApplicationStarted isn't a bitmask
	for (int d=0; d < 8; ++d)
		if ((d << 1) & device)
			for (int b=0; b < 8; ++b)
				if ((b << 1) & bank)
					if (!isApplicationStarted(d, b, 0s))
						return false;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_WRITE_SETTINGS, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			sizeof(settings), // len
		},
		{ // data
		},
		0 // crc
	};
	// copy the settings into the frame
	memcpy(frame.data, &settings, sizeof(settings));

	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(&frame.header, COMMAND_STATE_FINISHED, true, timeout);
}

std::string neoRADIO2Device::frameToString(neoRADIO2frame& frame, bool is_bitfield)
{
	/*
	std::map<int, std::string> mHostFrameCommandNames;
	std::map<int, std::string> mDeviceFrameCommandNames;
	std::map<int, std::string> mCommandStateNames;
	*/
	std::stringstream ss;
	// Append Command
	if (isHostHeaderId(frame.header.start_of_frame))
	{
		ss << "Host Frame Cmd:";
		if (mHostFrameCommandNames.find(frame.header.command_status) == mHostFrameCommandNames.end())
		{
			ss << " UNKNOWN (" << frame.header.command_status << ")";
		}
		else
			ss << " " << mHostFrameCommandNames[frame.header.command_status];
	}
	else if (isDeviceHeaderId(frame.header.start_of_frame))
	{
		ss << "Device Frame Cmd:";
		if (mDeviceFrameCommandNames.find(frame.header.command_status) == mDeviceFrameCommandNames.end())
		{
			ss << " UNKNOWN (" << frame.header.command_status << ")";
		}
		else
			ss << " " << mDeviceFrameCommandNames[frame.header.command_status];
	}
	else
	{
		ss << "UNKNOWN Frame (" << frame.header.start_of_frame << ") Cmd: " << frame.header.command_status << ")";
		return ss.str();
	}
	// Append Device, Bank, Len
	ss << " Device: 0x" << std::hex << (int)frame.header.device;
	ss << " Bank: 0x" << std::hex << (int)frame.header.bank;
	ss << " Len: 0x" << std::hex << (int)frame.header.len;
	// Append State
	bool finished = false;
	if (is_bitfield)
	{
		for (int d=0; d < 8; ++d)
		{
			if (finished)
				break;
			for (int b=0; b < 8; ++b)
			{
				if (!((1 << d) & 0xFF) || !((1 << b) & 0xFF))
					continue; // Device / Bank not enabled
				CommandStates state = mDCH.getState(frame.header.start_of_frame, d, b, frame.header.command_status);
				if (mCommandStateNames.find((int)state) == mCommandStateNames.end())
					ss << " D" << d << "B" << b << " State: Unknown (" << int(state) << ")";
				else
					ss << " D" << d << "B" << b << " " << mCommandStateNames[(int)state];
#ifndef DEBUG_ANNOYING
				finished = true;
				break;
#endif
			}
		}
	}
	else
	{
		CommandStates state = mDCH.getState(frame.header.start_of_frame, frame.header.device, frame.header.bank, frame.header.command_status);
		if (mCommandStateNames.find((int)state) == mCommandStateNames.end())
			ss << " State: Unknown (" << int(state) << ")";
		else
			ss << " " << mCommandStateNames[(int)state];
	}

	return ss.str();
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