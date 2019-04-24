#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif

#include "neoradio2device.h"

#include "radio2_frames.h"

#include <cassert>
#include <string>
#include <sstream>
#include <iterator>

neoRADIO2Device::neoRADIO2Device(DeviceInfoEx& di)
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
	_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_RECEIVING_DATA);
	_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_RECEIVED_DATA);
	_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_CRC_OKAY);
	_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_ERROR);
	_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_CRC_ERROR);
	_InsertEnumIntoMap(mCommandStateNames, COMMAND_STATE_FINISHED);
}

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

std::vector<neoRADIO2Device*> neoRADIO2Device::_findAll()
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
				interface_number = hdi->path[strlen(hdi->path) - 1] - 0x30;
			}
#endif
			//hdi->serial_number
			DeviceInfoEx di;
			di.is_open = false;
			di.is_blocking = true;
			memset(&di.di, 0, sizeof(di.di));
			if (hdi->product_string)
			{
				di.di.name = new char[64]{ 0 };
				std::wcstombs(di.di.name, hdi->product_string, 64);
			}
			if (hdi->serial_number)
			{
				di.di.serial_str = new char[64]{ 0 };
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
		DEBUG_PRINT_ANNOYING("HidDevice::runConnecting() failed!");
		return success;
	}

	uint8_t buffer[64]{0};
	uint16_t buffer_size = sizeof(buffer);

#if !defined(DEBUG_RADIO2_THREAD_DO_NOTHING)
	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// Setup FT260 Clock 4.4.3
	DEBUG_PRINT_ANNOYING("Setup FT260 Clock");
	memset(buffer, 0, sizeof(buffer));
	buffer[0] = 0xA1; // report id;
	buffer[1] = 0x1; // Set Clock
	buffer[2] = 0x0; // 0x0 = 12MHz, 0x1 = 24MHz, 0x2 = 48MHz
	buffer_size = sizeof(buffer);
	success = write(buffer, &buffer_size, CHANNEL_SPECIAL) && buffer_size == sizeof(buffer);
	if (!success)
	{
		DEBUG_PRINT("Setup FT260 Clock failed!");
		return success;
	}

	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.4.8 Select GPIOA Function
	DEBUG_PRINT_ANNOYING("Select GPIOA Function TX_LED");
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
	DEBUG_PRINT_ANNOYING("Select GPIOG Function RX_LED");
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
	DEBUG_PRINT_ANNOYING("GPIO Write Request - GPIOH Output");
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
	DEBUG_PRINT_ANNOYING("Configure UART - %d baudrate", baudrate);
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
	DEBUG_PRINT_ANNOYING("Changing state to connected!");
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
	using namespace std::chrono;

	mMutex.lock();
	mIsRunning = true;
	mMutex.unlock();

	while (!mQuit)
	{
		// Nothing to do if we aren't connected
		if (state() != DeviceStateConnected)
		{
			std::this_thread::sleep_for(1ms);
			continue;
		}
		mMutex.lock();
		switch (mLastState)
		{
		case PROCESS_STATE_IDLE:
		{
			if (!processStateIdle())
			{
				mMutex.unlock();
				// Make sure we don't hog the CPU when Idle
				std::this_thread::sleep_for(1ms);
				mMutex.lock();
				break;
			}
		}
		case PROCESS_STATE_HEADER:
			if (!processStateHeader())
				break;
		case PROCESS_STATE_DATA:
			if (!processStateData())
				break;
		case PROCESS_STATE_CRC:
			if (!processStateCRC())
				break;
		case PROCESS_COMPLETE_PACKET:
			if (!processStateCompletePacket())
				break;
		case PROCESS_STATE_FINISHED:
			processStateFinished();
			break;
		};
		mMutex.unlock();
	}
	mMutex.lock();
	mIsRunning = false;
	mMutex.unlock();
}

bool neoRADIO2Device::processStateIdle()
{
	if (canRead(CHANNEL_1) >= 1)
	{
		memset(mCommBuffer, 0, sizeof(mCommBuffer));
		uint16_t read_size = 1;
		// Can we read a byte and is it valid?
		if (!read(mCommBuffer, &read_size, CHANNEL_1))
		{
			DEBUG_PRINT("Failed to read!");
			return false;
		}
		if (!isValidHeaderId(mCommBuffer[0]))
		{
			DEBUG_PRINT("WARNING: Dropping %d bytes due to invalid start of frame (data: 0x%x)", read_size, mCommBuffer[0]);
			return false;
		}
		mLastframe.reset();
		mLastframe.frame()->header.start_of_frame = mCommBuffer[0];
		mLastState = PROCESS_STATE_HEADER;
	}
	return mLastState == PROCESS_STATE_HEADER;
}

bool neoRADIO2Device::processStateHeader()
{
	DEBUG_PRINT_ANNOYING("PROCESS_STATE_HEADER");
	// Do we have enough data to serialize the header?
	uint16_t buffer_size = sizeof(mLastframe.frame()->header) - sizeof(mLastframe.frame()->header.start_of_frame);
	if (canRead(CHANNEL_1) < buffer_size)
		return false;
	memset(mCommBuffer, 0, sizeof(mCommBuffer));
	if (!read(mCommBuffer, &buffer_size, CHANNEL_1))
	{
		return false; // TODO: We are essentially dropping bytes here
	}
	// Copy the buffer into the header
	uint8_t* header_ptr = (uint8_t*)&mLastframe.frame()->header;
	// Increment the pointer past the start of frame since we already have it
	header_ptr += sizeof(mLastframe.frame()->header.start_of_frame);
	memcpy(header_ptr, mCommBuffer, buffer_size);

	// Is the current frame a bitfield?
	bool is_bitfield = isHostHeaderId(mLastframe.frame()->header.start_of_frame);
	mLastframe.setIsBitField(is_bitfield);

	// Verify header fields are correct
	if (isDeviceHeaderId(mLastframe.frame()->header.start_of_frame))
	{
		// Verify Device Frame Header
		if (mLastframe.frame()->header.device >= 8)
		{
			DEBUG_PRINT("ERROR: Header device field is out of range %d", mLastframe.frame()->header.device);
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
			return false;
		}
		if (mLastframe.frame()->header.bank >= 8)
		{
			DEBUG_PRINT("ERROR: Header bank field is out of range %d", mLastframe.frame()->header.bank);
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
			return false;
		}
		if (mDeviceFrameCommandNames.find(mLastframe.frame()->header.command_status) == mDeviceFrameCommandNames.end())
		{
			DEBUG_PRINT("ERROR: Header command_status field is out of range %d", mLastframe.frame()->header.command_status);
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
			return false;
		}
	}
	else if (isHostHeaderId(mLastframe.frame()->header.start_of_frame))
	{
		// Verify Host Frame Header - We really should never hit this since we should always be sending this packet.
		if (mHostFrameCommandNames.find(mLastframe.frame()->header.command_status) == mHostFrameCommandNames.end())
		{
			DEBUG_PRINT("ERROR: Header command_status field is out of range %d", mLastframe.frame()->header.command_status);
			mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
			return false;
		}
	}
	else
	{
		DEBUG_PRINT("BUG: unrecognized start of frame %d", mLastframe.frame()->header.start_of_frame);
		return false;
	}

	DEBUG_PRINT_ANNOYING("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
	mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_RECEIVED_HEADER, is_bitfield);

	mLastState = PROCESS_STATE_DATA;
	return mLastState == PROCESS_STATE_DATA;
}

bool neoRADIO2Device::processStateData()
{
	DEBUG_PRINT_ANNOYING("PROCESS_STATE_DATA");
	if (mLastframe.frame()->header.len == 0)
	{
		// skip the data process because we won't have any
		mLastState = PROCESS_STATE_CRC;
		return true;
	}

	// Do we have enough data to serialize the header's data?
	uint16_t buffer_size = mLastframe.frame()->header.len;
	if (canRead(CHANNEL_1) <= buffer_size)
		return false;

	bool is_bitfield = mLastframe.isBitField();
	if (!read(mCommBuffer, &buffer_size, CHANNEL_1))
	{
		mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
		mLastState = PROCESS_STATE_FINISHED;
		return false; // TODO: We are essentially dropping bytes here
	}
	// Copy the data into mLastframe
	if (buffer_size > sizeof(mLastframe.frame()->data))
	{
		mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
		mLastState = PROCESS_STATE_FINISHED;
		return false; // TODO: We are essentially dropping bytes here
	}
	// Add data and Update the command
	std::vector<uint8_t> data;
	memcpy(&mLastframe.frame()->data, mCommBuffer, buffer_size);
	for (int i = 0; i < buffer_size; ++i)
		data.push_back(mCommBuffer[i]);
	memcpy(data.data(), mCommBuffer, buffer_size);
	mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_RECEIVED_DATA, is_bitfield);
	mDCH.updateData(&mLastframe.frame()->header, data, is_bitfield);
	DEBUG_PRINT("Processed %d databytes...", buffer_size);
	if (buffer_size)
	{
		DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
	}
	mLastState = PROCESS_STATE_CRC;
	
	return mLastState == PROCESS_STATE_CRC;
}

bool neoRADIO2Device::processStateCRC()
{
	DEBUG_PRINT_ANNOYING("PROCESS_STATE_CRC");
	uint16_t buffer_size = sizeof(mLastframe.frame()->crc);
	// Do we have enough data to serialize the header's crc?
	if (canRead(CHANNEL_1) < buffer_size)
		return false;
	bool is_bitfield = mLastframe.isBitField();
	if (!read(mCommBuffer, &buffer_size, CHANNEL_1))
	{
		mDCH.updateCommand(&mLastframe.frame()->header, COMMAND_STATE_ERROR, is_bitfield);
		DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
		mLastState = PROCESS_STATE_FINISHED;
		return false; // TODO: We are essentially dropping bytes here
	}
	// add the crc to the frame
	mLastframe.frame()->crc = mCommBuffer[0];
	bool checksum_passed = true;
	mLastState = PROCESS_COMPLETE_PACKET;
#if !defined(SKIP_CHECKSUM_VERIFY)
	// CRC is going to fail since we changed the bank from index, lets put it back for calculation
	//for (i=0; mLastframe.frame()->header.bank >>= 1; ++i) {}
	//mLastframe.frame()->header.bank = i;
	uint8_t calculated_checksum = 0;
	checksum_passed = verifyFrameChecksum(mLastframe.frame(), &calculated_checksum);
	//mLastframe.frame()->header.bank = (1 << mLastframe.frame()->header.bank);
	mDCH.updateCommand(&mLastframe.frame()->header, (checksum_passed ? COMMAND_STATE_CRC_OKAY : COMMAND_STATE_CRC_ERROR), is_bitfield);
	if (!checksum_passed)
	{
		DEBUG_PRINT("ERROR: CRC Failure: Got %d, Expected %d", mLastframe.frame()->crc, calculated_checksum);
	}
	DEBUG_PRINT_ANNOYING("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
#endif
	mLastframe.setIsChecksumValid(checksum_passed);
    if (!checksum_passed)
        return false;
	return checksum_passed;
}

bool neoRADIO2Device::processStateCompletePacket()
{
	mLastState = PROCESS_STATE_FINISHED;
	auto frame = mLastframe.frame();
	if (isHostHeaderId(frame->header.start_of_frame))
	{
		/*
		switch (mLastframe.frame()->header.command_status)
		{
		default:
			// Nothing to process, return successful.
			return true;
		};
		*/
		return true;
	}
	else if (isDeviceHeaderId(frame->header.start_of_frame))
	{
		switch (mLastframe.frame()->header.command_status)
		{
		case NEORADIO2_STATUS_READ_SETTINGS:
			return processFrameReadSettings();
			break;
		default:
			// Nothing to process, return successful.
			return true;
		};
	}
	else
	{
		DEBUG_PRINT("%d is not a valid start of frame to process.", frame->header.start_of_frame);
		return false;
	}
	return false;
}

bool neoRADIO2Device::processStateFinished()
{
	DEBUG_PRINT_ANNOYING("PROCESS_STATE_FINISHED");
	bool is_bitfield = mLastframe.isBitField();

	// Update the last command states to "Finished" here if they were "successful"
	bool success = false;
	success = mDCH.updateCommandIf(&mLastframe.frame()->header, COMMAND_STATE_CRC_OKAY, COMMAND_STATE_FINISHED, is_bitfield);
	success = mDCH.updateCommandIf(&mLastframe.frame()->header, COMMAND_STATE_RECEIVED_DATA, COMMAND_STATE_FINISHED, is_bitfield);

	// Figure out how many devices are on the chain here
	if (isDeviceHeaderId(mLastframe.frame()->header.start_of_frame) && mLastframe.frame()->header.command_status == NEORADIO2_STATUS_IDENTIFY)
	{
		auto device_count = mLastframe.frame()->header.device + 1;
		updateDeviceCount(device_count > mDeviceCount ? device_count : mDeviceCount);
		DEBUG_PRINT_ANNOYING("Device Count = %d", mDeviceCount);
	}
	DEBUG_PRINT("%s", frameToString(*mLastframe.frame(), is_bitfield).c_str());
	DEBUG_PRINT_ANNOYING("PROCESS_STATE_FINISHED COMPLETE\n");
	mLastState = PROCESS_STATE_IDLE;
	return true;
}

bool neoRADIO2Device::processFrameReadSettings()
{
	auto frame = mLastframe.frame();

	// Make sure we actually have the data
	if (frame->header.len > getSettingsPartSize())
	{
		mDCH.updateCommand(&frame->header, COMMAND_STATE_ERROR, mLastframe.isBitField());
		DEBUG_PRINT("ERROR: frame header len (%d) does not match NEORADIO2_SETTINGS_PARTSIZE (%d)", frame->header.len, getSettingsPartSize());
		return false;
	}

	// How many parts of the settings do we have left?
	auto parts_remaining = mDCH.getExtraState(&frame->header, NEORADIO2_STATUS_READ_SETTINGS);
	if (parts_remaining < 0)
	{
		mDCH.updateCommand(&frame->header, COMMAND_STATE_ERROR, mLastframe.isBitField());
		DEBUG_PRINT("ERROR: Received an extra Setting frame when we already have all");
		return false;
	}

	mDCH.updateCommand(&frame->header, COMMAND_STATE_RECEIVING_DATA, mLastframe.isBitField());

	// grab the partial data
	std::vector<uint8_t> cmd_data = mDCH.getData(&frame->header);
	if (cmd_data.size() > sizeof(neoRADIO2_SettingsPart))
	{
		DEBUG_PRINT("ERROR: frame data size %d does not match sizeof(neoRADIO2_SettingsPart)", cmd_data.size());
		return false;
	}
	neoRADIO2_SettingsPart part = {};
	memcpy(&part, cmd_data.data(), sizeof(part));

	// make sure we aren't out of range
	if (part.part > getSettingsPartsCount())
	{
		mDCH.updateCommand(&frame->header, COMMAND_STATE_ERROR, mLastframe.isBitField());
		DEBUG_PRINT("ERROR: neoRADIO2_SettingsPart.Part (%d) is greater than getSettingsSizePartsCount(): %d", part.part, getSettingsPartsCount());
		return false;
	}
	DEBUG_PRINT("Received Settings Part %d of %d...", part.part, getSettingsPartsCount());

	// Grab the data already stored
	auto data = mDCH.getExtraData(&frame->header);
	// Make sure the vector is big enough for us
	data.resize(getSettingsSize());

	// Push the partial data into the real vector.
	const auto offset = NEORADIO2_SETTINGS_PARTSIZE * part.part;

	std::copy(&part.data[0], &part.data[frame->header.len-1], data.begin() + offset);
	// Update the data with the new partial
	mDCH.updateExtraData(&frame->header, data, mLastframe.isBitField());

	
	if (parts_remaining == 0)
	{
		mDCH.updateCommand(&frame->header, COMMAND_STATE_RECEIVED_DATA, mLastframe.isBitField());
	}
	else
	{
		// we need to request more data
		neoRADIO2frame tx_frame =
		{
			{ // header
				0xAA, // start_of_frame
				NEORADIO2_COMMAND_READ_SETTINGS, // command_status
				(uint8_t)(frame->header.device & 0xFF),
				(uint8_t)((1 << frame->header.bank) & 0xFF), // bank
				1, // len
			},
			{ // data
				(uint8_t)parts_remaining-1,
			},
			0 // crc
		};


		mDCH.updateCommand(&frame->header, COMMAND_STATE_RESET, mLastframe.isBitField());

		// send the packets
		using namespace std::chrono_literals;
		
		//mMutex.unlock();
		//std::this_thread::sleep_for(50ms);
		//mMutex.lock();
		if (!writeUartFrame(&tx_frame, CHANNEL_1))
		{
			mDCH.updateCommand(&tx_frame.header, COMMAND_STATE_ERROR, true);
			return false;
		}
	}

	// Update the parts remaining
	parts_remaining -= 1;
	mDCH.updateExtra(&frame->header, parts_remaining, mLastframe.isBitField());


	return true;
}


bool neoRADIO2Device::isOpen()
{
	return Device::isOpen();
}

//! Sends a frame over uart to the neoRAD-IO2
//! @param frame Frame to send over
//! @param channel DeviceChannel to communicate on, this should generally be CHANNEL_1
//! @see DeviceChannel
//! @see neoRADIO2frame
//! @return true if successfully wrote the frame to uart.
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

//! Sends a request to the chain to identify itself
//! @param timeout Timeout in milliseconds to wait for the request to complete.
//! @see isChainIdentified()
//! @see getIdentifyResponse()
//! @see getChainCount()
//! @return true if chain was successfully identified.
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
	bool success = mDCH.isStateSet(0x55, 0, frame.header.bank, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_FINISHED, true, timeout);
	//std::this_thread::sleep_for(1s);
	return success;
}

//! Check to see if the chain has been identified
//! @param timeout Timeout in milliseconds to wait for the request to complete.
//! @see isChainIdentified()
//! @see getIdentifyResponse()
//! @see getChainCount()
//! @return true if chain was successfully identified.
bool neoRADIO2Device::isChainIdentified(std::chrono::milliseconds timeout)
{
	auto count = mDeviceCount;
	auto device = 0;
	bool success = true;
	for (auto i=0; i < mDeviceCount; ++i)
	{
		if (!mDCH.isStateSet(0x55, device, 0xFF, NEORADIO2_STATUS_IDENTIFY, COMMAND_STATE_FINISHED, true, timeout))
		{
			success = false;
			break;
		}
	}
	return success;
}

//! Receive the identify response frame data. Chain needs to be identified before this call.
//! @param device index of the device in the chain (0-8).
//! @param bank index of the bank in the chain (0-8).
//! @param response response data structure.
//! @param timeout Timeout in milliseconds to wait for the request to complete.
//! @see neoRADIO2frame_identifyResponse
//! @see isChainIdentified()
//! @see getIdentifyResponse()
//! @see getChainCount()
//! @return true if we received the reponse data, false if chain isn't identified or data isn't correct.
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

//! Gets the device count in the chain, chain must be identified before calling this.
//! @param count device count in the chain
//! @param identify true = identify chain if not already identified. false = don't identify.
//! @param timeout Timeout in milliseconds to wait for the request to complete.
//! @see isChainIdentified()
//! @see requestIdentifyChain()
//! @return true if we received the reponse data, false if chain isn't identified or data isn't correct.
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

//! Starts the application firmware on the bank, chain must be identified before calling this.
//! @param device index of the device in the chain (0-8).
//! @param bank bitmask of the bank in the chain 0x0-0xFF (ie. 0x2 = 2nd bank, 0x4 = 3rd bank, 0x6 = 2nd/3rd banks, 0xFF = All Banks).
//! @param timeout Timeout in milliseconds to wait for the request to complete.
//! @see isChainIdentified()
//! @see requestIdentifyChain()
//! @return true if we received the reponse data, false if chain isn't identified or data isn't correct.
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
	std::this_thread::sleep_for(500ms);
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
	auto state = mDCH.getState(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_PCBSN);
	bool success = mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_PCBSN, COMMAND_STATE_FINISHED, true, timeout);
	//std::this_thread::sleep_for(1s);
	return success;
}

bool neoRADIO2Device::getPCBSN(int device, int bank, std::string& pcbsn)
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
	pcbsn.clear();
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_READ_PCBSN, COMMAND_STATE_FINISHED, false))
		return false;
	std::vector<uint8_t> data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_READ_PCBSN);
	for (auto d : data)
		pcbsn += (char)d;
	return true;
}

bool neoRADIO2Device::requestSensorData(int device, int bank, int enable_cal, std::chrono::milliseconds timeout)
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
	frame.data[0] = enable_cal & 0xFF;
	frame.header.len = 1;
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_SENSOR, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	bool success = mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_SENSOR, COMMAND_STATE_FINISHED, true, timeout);
	//std::this_thread::sleep_for(1s);
	return success;
}

bool neoRADIO2Device::readSensorData(int device, int bank, std::vector<uint8_t>& data)
{
	using namespace std::chrono;
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
			1, // len
		},
		{ // data
			(uint8_t)getSettingsPartsCount(),
		},
		0 // crc
	};

	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_SETTINGS, COMMAND_STATE_RESET, true);
	mDCH.updateExtra(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_SETTINGS, getSettingsPartsCount(), true);

	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;

	// Wait until we get the command back
	if (!mDCH.isStateSet(&frame.header, COMMAND_STATE_FINISHED, true, timeout))
		return false;

	//std::this_thread::sleep_for(1s);
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_READ_SETTINGS, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::readSettings(int device, int bank, neoRADIO2_settings& settings)
{
	memset(&settings, 0, sizeof(settings));
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_READ_SETTINGS, COMMAND_STATE_FINISHED, false))
		return false;
	std::vector<uint8_t> _data = mDCH.getExtraData(0x55, device, bank, NEORADIO2_STATUS_READ_SETTINGS);
	if (_data.size() != sizeof(settings))
		return false;
	memcpy(&settings, _data.data(), _data.size());
	return true;
}

bool neoRADIO2Device::writeSettings(int device, int bank, neoRADIO2_settings& settings, std::chrono::milliseconds timeout)
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

	const int frame_count = getSettingsPartsCount();
	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_WRITE_SETTINGS, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			sizeof(neoRADIO2_SettingsPart), // len
		},
		{ // data
		},
		0 // crc
	};
	// copy the settings into the frame
	//memcpy(frame.data, &settings, sizeof(settings));

	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	for (unsigned int i = 0; i < getSettingsPartsCount(); ++i)
	{
		neoRADIO2_SettingsPart part = {};
		part.part = i;

		// Calculate the size and offset
		int size = sizeof(part.data);
		if (i == (getSettingsPartsCount() - 1))
			size = getSettingsSize() % sizeof(part.data);
		int offset = i * sizeof(part.data);

		// Copy the part of the settings into neoRADIO2_SettingsPart
		memcpy(part.data, (uint8_t*)(&settings)+offset, size);

		// Adjust the frame length
		frame.header.len = 0; // size + sizeof(part.part);

		// Copy the neoRADIO2_SettingsPart into the frame data
		memcpy(frame.data, &part, sizeof(part));

		if (!writeUartFrame(&frame, CHANNEL_1))
			return false;
	}
	return mDCH.isStateSet(&frame.header, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::requestCalibration(int device, int bank, const neoRADIO2frame_calHeader& header, std::chrono::milliseconds timeout)
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
			NEORADIO2_COMMAND_READ_CAL, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	memcpy(frame.data, &header, sizeof(header));
	frame.header.len = sizeof(header);
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CAL, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CAL, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::readCalibration(int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout)
{
	data.clear();
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_CAL, COMMAND_STATE_FINISHED, false, timeout))
		return false;
	std::vector<uint8_t> _data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_CAL);
	if (_data.size() < sizeof(header))
		return false;
	// copy the header first
	memcpy((void*)&header, _data.data(), sizeof(header));
	int remaining_size = _data.size() - sizeof(header);
	for (unsigned int i=0; i < remaining_size/sizeof(float); ++i)
	{
		uint8_t* _temp = _data.data()+sizeof(header)+(i*sizeof(float));
		float* value = (float*)_temp;
		data.push_back(*value);
	}
	return true;
}

bool neoRADIO2Device::requestCalibrationPoints(int device, int bank, const neoRADIO2frame_calHeader& header, std::chrono::milliseconds timeout)
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
			NEORADIO2_COMMAND_READ_CALPOINTS, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	memcpy(frame.data, &header, sizeof(header));
	frame.header.len = sizeof(header);
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CALPOINTS, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CAL, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::readCalibrationPoints(int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout)
{
	data.clear();
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_CALPOINTS, COMMAND_STATE_FINISHED, false, timeout))
		return false;
	std::vector<uint8_t> _data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_CALPOINTS);
	if (_data.size() < sizeof(header))
		return false;
	// copy the header first
	memcpy((void*)&header, _data.data(), sizeof(header));
	int remaining_size = _data.size() - sizeof(header);
	for (unsigned int i=0; i < remaining_size/sizeof(float); ++i)
	{
		uint8_t* _temp = _data.data()+sizeof(header)+(i*sizeof(float));
		float* value = (float*)_temp;
		data.push_back(*value);
	}
	return true;
}

bool neoRADIO2Device::writeCalibration(int device, int bank, const neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout)
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
			NEORADIO2_COMMAND_WRITE_CAL, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};

	if (data.size() > sizeof(frame.data))
		return false;
	memcpy(frame.data, &header, sizeof(header));
	for (unsigned int i=0; i < data.size(); ++i)
	{
		float* ptr = (float*)&frame.data[sizeof(header)];
		ptr[i] = data[i];
	}
	frame.header.len = sizeof(header)+((uint8_t)data.size()*sizeof(data[0]));
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0xAA, frame.header.device, frame.header.bank, NEORADIO2_COMMAND_WRITE_CAL, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::writeCalibrationPoints(int device, int bank, const neoRADIO2frame_calHeader& header, std::vector<float>& data, std::chrono::milliseconds timeout)
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
			NEORADIO2_COMMAND_WRITE_CALPOINTS, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	if (data.size() > sizeof(frame.data))
		return false;
	memcpy(frame.data, &header, sizeof(header));
	for (unsigned int i=0; i < data.size(); ++i)
	{
		float* ptr = (float*)&frame.data[sizeof(header)];
		ptr[i] = data[i];
	}
	frame.header.len = sizeof(header)+((uint8_t)data.size()*sizeof(data[0]));
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0xAA, frame.header.device, frame.header.bank, NEORADIO2_COMMAND_WRITE_CALPOINTS, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::requestStoreCalibration(int device, int bank, std::chrono::milliseconds timeout)
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
			NEORADIO2_COMMAND_STORE_CAL, // command_status
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
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CAL_STORE, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0xAA, frame.header.device, frame.header.bank, NEORADIO2_COMMAND_STORE_CAL, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::isCalibrationStored(int device, int bank, bool& stored, std::chrono::milliseconds timeout)
{
	stored = false;
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_CAL_STORE, COMMAND_STATE_FINISHED, false, timeout))
		return false;
	std::vector<uint8_t> _data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_CAL_STORE);
	if (!_data.size())
		return false;
	stored = _data[0] != 0;
	return true;
}

bool neoRADIO2Device::requestCalibrationInfo(int device, int bank, std::chrono::milliseconds timeout)
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
			NEORADIO2_COMMAND_READ_CAL_INFO, // command_status
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
	mDCH.updateCommand(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CAL_INFO, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	//bool success = mDCH.isStateSet(0xAA, frame.header.device, frame.header.bank, NEORADIO2_COMMAND_READ_CAL_INFO, COMMAND_STATE_FINISHED, true, timeout); 
	return mDCH.isStateSet(0x55, frame.header.device, frame.header.bank, NEORADIO2_STATUS_CAL_INFO, COMMAND_STATE_FINISHED, true, timeout);
}

bool neoRADIO2Device::readCalibrationInfo(int device, int bank, neoRADIO2frame_calHeader& header, std::chrono::milliseconds timeout)
{
	memset(&header, 0, sizeof(header));
	if (!mDCH.isStateSet(0x55, device, bank, NEORADIO2_STATUS_CAL_INFO, COMMAND_STATE_FINISHED, false))
		return false;
	std::vector<uint8_t> _data = mDCH.getData(0x55, device, bank, NEORADIO2_STATUS_CAL_INFO);
	if (_data.size() != sizeof(header))
		return false;
	memcpy(&header, _data.data(), sizeof(header));
	return true;
}

bool neoRADIO2Device::toggleLED(int device, int bank, int ms, std::chrono::milliseconds timeout)
{
	using namespace std::chrono;

	neoRADIO2frame frame =
	{
		{ // header
			0xAA, // start_of_frame
			NEORADIO2_COMMAND_TOGGLE_LED, // command_status
			(uint8_t)device,
			(uint8_t)bank, // bank
			0, // len
		},
		{ // data
		},
		0 // crc
	};
	//if (ms <= 255)
	//{
		frame.data[0] = ms & 0xFF;
		frame.header.len = 1;
	//}
	//else
	//{
	//	*(uint32_t*)&frame.data[0] = ms;
	//	frame.header.len = sizeof(uint32_t);
	//}
	// Reset commands
	mDCH.updateCommand(&frame.header, COMMAND_STATE_RESET, true);
	// send the packets
	if (!writeUartFrame(&frame, CHANNEL_1))
		return false;
	// Is the command set?
	return mDCH.isStateSet(0xAA, frame.header.device, frame.header.bank, NEORADIO2_COMMAND_TOGGLE_LED, COMMAND_STATE_FINISHED, true, timeout);
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
    ss << " CRC: 0x" << std::hex << (int)frame.crc;
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
	//mMutex.lock();
	mDeviceCount = device_count;
	//mMutex.unlock();
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
    if (checksum != frame->crc)
        return false;

	return checksum == frame->crc;
}