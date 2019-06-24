#ifndef __HIDDEVICE_H__
#define __HIDDEVICE_H__

#include "device.h"
#include "hidapi.h"
#include "fifo.h"

#include <vector>
#include <map>


const unsigned int HidBufferSize = 1024*10;

class HidBuffer
{
public:
	hid_device* handle;
	fifo_t tx_fifo;
	fifo_t rx_fifo;

	uint8_t* tx_buffer;
	uint8_t* rx_buffer;

	std::string path;

	std::mutex tx_lock;
	std::mutex rx_lock;
	
	HidBuffer(const unsigned int buffer_size=HidBufferSize)
	{
		handle = NULL;

		tx_buffer = new uint8_t[buffer_size];
		memset(tx_buffer, 0, buffer_size);
		FIFO_Init(&tx_fifo, tx_buffer, HidBufferSize);

		rx_buffer = new uint8_t[buffer_size];
		memset(rx_buffer, 0, buffer_size);
		FIFO_Init(&rx_fifo, rx_buffer, HidBufferSize);
	}

	virtual ~HidBuffer()
	{
		if (handle)
			handle = NULL;

		if (tx_buffer)
		{
			FIFO_Clear(&tx_fifo);
			delete[] tx_buffer;
			tx_buffer = NULL;
		}

		if (rx_buffer)
		{
			FIFO_Clear(&rx_fifo);
			delete[] rx_buffer;
			rx_buffer = NULL;
		}
	}

	void reset()
	{
		if (tx_buffer)
		{
			memset(tx_buffer, 0, HidBufferSize);
			FIFO_Clear(&tx_fifo);
		}
		if (rx_buffer)
		{
			memset(rx_buffer, 0, HidBufferSize);
			FIFO_Clear(&rx_fifo);
		}
	}
};
typedef std::map<DeviceChannel, HidBuffer*> HidBuffers;


class HidDevice : public Device
{
public:
	HidDevice();
	virtual ~HidDevice();

	Devices _findAll();

	// this code will loop forever until you return false or user requested a quit()
	virtual bool runIdle();
	virtual bool runConnecting();
	virtual bool runConnected();
	virtual bool runDisconnecting();


	virtual bool read(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel);
	virtual bool write(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel);

	// returns how many bytes are available in the channel's buffer
	virtual uint16_t canRead(DeviceChannel channel);
	// returns how many bytes can be written into the channel's buffer
	virtual uint16_t canWrite(DeviceChannel channel);

	bool addPath(DeviceChannel channel, std::string path);

protected:
	HidBuffers mBuffers;

	DeviceChannel mSpecialChannel;

	virtual bool sendFeatureReport(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel);

	bool isReportIdValid(uint8_t& data);

#ifdef _DEBUG
    uint8_t mDebugTxBufferCopy[1024*1000];
    uint64_t mDebugTxBufferCopyIndex = 0;

    uint8_t mDebugRxBufferCopy[1024*1000];
    uint64_t mDebugRxBufferCopyIndex = 0;
#endif

};

#endif // __HIDDEVICE_H__
