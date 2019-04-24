#include "hiddevice.h"

#include <iostream>
#include <sstream>

HidDevice::HidDevice(DeviceInfoEx& di)
	: Device(di)
{
	hid_init();
	// Insert the hid_paths to the map
	if (di.channel_paths.size())
	for (auto i : di.channel_paths)
	{
		HidBuffer* buffer = new HidBuffer();
		buffer->path = i.second;
		buffer->reset();
		mBuffers[i.first] = buffer;
	}
}

HidDevice::~HidDevice()
{
	for (auto i : mBuffers)
	{
		if (i.second)
			delete i.second;
		i.second = NULL;
	}
	mBuffers.clear();
}

bool HidDevice::addPath(DeviceChannel channel, std::string path)
{
	HidBuffer* buffer = new HidBuffer();
	buffer->path = path;
	buffer->reset();
	mBuffers[channel] = buffer;

	return Device::addPath(channel, path);
}

std::vector<HidDevice*> HidDevice::_findAll()
{
	
	std::vector<HidDevice*> devs;

	hid_device_info* hdi = NULL;
	hid_device_info* first_hdi = hdi;
	if (hdi = hid_enumerate(0, 0))
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

		auto hid_dev = new HidDevice(di);
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
bool HidDevice::runIdle()
{
	return true;
}

bool HidDevice::runConnecting()
{
	for (auto& buffer : mBuffers)
	{
		auto buf = buffer.second;
		if (buf->handle != NULL)
			continue;
		if ((buf->handle = hid_open_path(buf->path.c_str())) == NULL)
			return false; // NULL on error
		if (hid_set_nonblocking(buf->handle, 1) != 0)
			return false; // 0 on success, -1 on error

		// Setup the buffer
		buf->reset();
	}
	return true;
}

bool HidDevice::runConnected()
{
	for (auto& buffer : mBuffers)
	{
		auto buf = buffer.second;
		if (!buf)
			continue;
		buf->rx_lock.lock();
		uint8_t temp[64]{};
		int length = 0;
		length = hid_read(buf->handle, (unsigned char*)temp, sizeof(temp));
		if (length > 0)
		{
#ifdef DEBUG_ANNOYING
			std::stringstream ss;
			for (int i=0; i < length && i < 30; ++i)
				ss << "0x" << std::hex << (int)temp[i] << " ";
			DEBUG_PRINT("hid_read(): len: %d channel: %d\n\t\tdata: %s", length, buffer.first, ss.str().c_str());
#endif // DEBUG_ANNOYING
			if (!isReportIdValid(temp[0]))
			{
				DEBUG_PRINT("ERROR: Not a valid FT260 Report ID! (data: 0x%x length: %d)", temp[0], length);
				buf->rx_lock.unlock();
				return false;
			}
			if (length < 2)
			{
				DEBUG_PRINT("ERROR: Not a valid FT260 Report ID! (length too small: %d)", length);
			}
			const int report_id_size = temp[1];
#ifdef DEBUG_ANNOYING
			std::stringstream ss2;
			for (int i=0; i < report_id_size && i < 30; ++i)
				ss2 << "0x" << std::hex << (int)temp[i+2] << " ";
			DEBUG_PRINT("hid_read_parsed(): len: %d channel: %d\n\t\tdata: %s", report_id_size, buffer.first, ss2.str().c_str());
#endif // DEBUG_ANNOYING
			FIFO_Push(&buf->rx_fifo, &temp[2], report_id_size);
		}
		buf->rx_lock.unlock();
		// TODO Error handling (length == -1)
		
		buf->tx_lock.lock();
		auto count = FIFO_GetCount(&buf->tx_fifo);
		if (count)
		{
			FIFO_Pop(&buf->tx_fifo, temp, count);
#ifdef DEBUG_ANNOYING
			std::stringstream ss2;
			for (int i=0; i < count && i < 30; ++i)
				ss2 << "0x" << std::hex << (int)temp[i] << " ";
			DEBUG_PRINT("hid_write(): len: %d channel: %d\n\t\tdata: %s", count, buffer.first, ss2.str().c_str());
#endif // DEBUG_ANNOYING
			length = hid_write(buf->handle, (unsigned char*)temp, count);
			// TODO Error handling (length == -1)
		}
		buf->tx_lock.unlock();
	}
	return true;
}

bool HidDevice::runDisconnecting()
{
	for (auto& buffer : mBuffers)
	{
		auto buf = buffer.second;
		if (buf->handle != NULL)
			hid_close(buf->handle);
		buf->handle = NULL;
	}
	return true;
}

bool HidDevice::read(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel)
{
	if (channel > channelCount())
		return false;

	if (channel == CHANNEL_SPECIAL)
		channel = mSpecialChannel;
	//if (channel == CHANNEL_SPECIAL)
	//	return sendFeatureReport(buffer, buffer_size, mSpecialChannel);

	HidBuffer* buf = mBuffers[channel];
	std::lock_guard<std::mutex> lock(buf->rx_lock);
	auto count = FIFO_GetCount(&buf->rx_fifo);

	if (count < *buffer_size)
	{
		// There isn't enough available in the buffer
		*buffer_size = count;
		return false;
	}
	FIFO_Pop(&buf->rx_fifo, buffer, *buffer_size);
#ifdef _DEBUG
    memcpy(mDebugRxBufferCopy+mDebugRxBufferCopyIndex, buffer, *buffer_size);
    mDebugRxBufferCopyIndex += *buffer_size;
#endif
#ifdef DEBUG_ANNOYING
	std::stringstream ss;
	for (int i=0; i < *buffer_size && i < 30; ++i)
		ss << "0x" << std::hex << (int)buffer[i] << " ";
	DEBUG_PRINT("read(): len: %d channel: %d\n\t\tdata: %s", *buffer_size, channel, ss.str().c_str());
#endif // DEBUG_ANNOYING
	return true;
}

bool HidDevice::write(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel)
{
	if (channel > channelCount())
		return false;

	if (channel == CHANNEL_SPECIAL)
		return sendFeatureReport(buffer, buffer_size, mSpecialChannel);

	HidBuffer* buf = mBuffers[channel];
	std::lock_guard<std::mutex> lock(buf->tx_lock);
	auto count = FIFO_GetFreeSpace(&buf->tx_fifo);

	if (count < *buffer_size)
		return false; // We don't have enough space in the buffer
#ifdef DEBUG_ANNOYING
	std::stringstream ss;
	for (int i=0; i < *buffer_size && i < 30; ++i)
		ss << "0x" << std::hex << (int)buffer[i] << " ";
	DEBUG_PRINT("write(): len: %d channel: %d\n\t\tdata: %s", *buffer_size, channel, ss.str().c_str());
#endif // DEBUG_ANNOYING
	FIFO_Push(&buf->tx_fifo, buffer, *buffer_size);
#ifdef _DEBUG
    memcpy(mDebugTxBufferCopy+mDebugTxBufferCopyIndex, buffer, *buffer_size);
    mDebugTxBufferCopyIndex += *buffer_size;
#endif
	return true;
}

uint16_t HidDevice::canRead(DeviceChannel channel)
{
	if (channel > channelCount() || channel == CHANNEL_SPECIAL)
		return 0;

	HidBuffer* buf = mBuffers[channel];
	std::lock_guard<std::mutex> lock(buf->rx_lock);
	return FIFO_GetCount(&buf->rx_fifo);
}

uint16_t HidDevice::canWrite(DeviceChannel channel)
{
	if (channel > channelCount() || channel == CHANNEL_SPECIAL)
		return 0;

	HidBuffer* buf = mBuffers[channel];
	std::lock_guard<std::mutex> lock(buf->tx_lock);
	return FIFO_GetFreeSpace(&buf->tx_fifo);
}

bool HidDevice::sendFeatureReport(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel)
{
	uint8_t temp_buf[64]{0};

	if (channel > channelCount())
		return false;

	HidBuffer* buf = mBuffers[channel];
	std::lock_guard<std::mutex> lock(buf->tx_lock);

	if (*buffer_size > sizeof(temp_buf))
		return false;

	// We need exactly 64 bytes to send the report message
	// windows seems to truncate to 64 bytes...
	memcpy(temp_buf, buffer, *buffer_size);

	DEBUG_PRINT_ANNOYING("hid_send_feature_report():\n\tlen: %d, channel: %d, path: %s", *buffer_size, channel, buf->path.c_str());
	auto length = hid_send_feature_report(buf->handle, (unsigned char*)temp_buf, sizeof(temp_buf));
	return length == 64;
}


bool HidDevice::isReportIdValid(uint8_t& data)
{
	// http://www.ftdichip.com/Support/Documents/ProgramGuides/AN_394_User_Guide_for_FT260.pdf
	// 4.3 FT260 Report ID List
	return 
#ifdef FT260_REPORT_ID_ALL
		(data == 0xA0) || // Chip code
		(data == 0xA1) || // System Setting
		(data == 0xB0) || // GPIO
		(data == 0xB1) || // Interrupt Status (UART)
		(data == 0xC0) || // I2C Status
		(data == 0xC2) || // I2C Read Request
		(data >= 0xD0) && (data <= 0xDE) || // I2C Report
		(data == 0xE0) || // UART Status
		(data == 0xE2) || // UART RI and DCD Status
#endif // FT260_REPORT_ID_ALL
		(data >= 0xF0) && (data <= 0xFE); // UART Report
}