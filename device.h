#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <map>

#include "config.h"
#include "libneoradio2.h"


#ifdef _MSC_VER

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#if defined(ENABLE_DEBUG_PRINT)
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, "\tDEBUG: %s:%d:%s(): " fmt "\n", \
    __FILENAME__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) /* Don't do anything in release builds */
#endif

#else

#if defined(ENABLE_DEBUG_PRINT)
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt "\n", \
    __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif 

class Device;
typedef std::vector<Device*> Devices;


typedef enum
{
	CHANNEL_SPECIAL = 0,
	CHANNEL_0,
	CHANNEL_1,
	CHANNEL_2,
	CHANNEL_3,
	CHANNEL_4,
} DeviceChannel;

typedef std::map<DeviceChannel, std::string> DeviceChannelMap;

typedef struct _DeviceInfoEx
{
	Neoradio2DeviceInfo di;
	bool is_open;
	bool is_blocking;
	DeviceChannelMap channel_paths;
} DeviceInfoEx;


template<class T>
class DeviceFinder
{
public:
	static std::vector<T*> findAll()
	{
		return T::_findAll();
	}
};


class Device : public DeviceFinder<Device>
{
public:
	Device(DeviceInfoEx& di);
	virtual ~Device();

	virtual bool open();
	virtual bool close();

	bool isOpen();

	virtual Neoradio2DeviceInfo deviceInfo() const { return mDevInfo.di; }

protected:
	DeviceInfoEx mDevInfo;

	// this code will loop forever until you return false or user requested a quit()
	virtual bool runIdle()=0;
	virtual bool runConnecting()=0;
	virtual bool runConnected()=0;
	virtual bool runDisconnecting()=0;

	virtual bool read(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel)=0;
	virtual bool write(uint8_t* buffer, uint16_t* buffer_size, DeviceChannel channel)=0;

	// returns how many bytes are available in the channel's buffer
	virtual uint16_t canRead(DeviceChannel channel)=0;
	// returns how many bytes can be written into the channel's buffer
	virtual uint16_t canWrite(DeviceChannel channel)=0;

	virtual int channelCount() const { return mDevInfo.channel_paths.size(); }

	virtual bool addPath(DeviceChannel channel, std::string path);
	virtual std::string path(DeviceChannel channel);

	

	// Override this in inherited classes
	static std::vector<Device*> findAll()
	{
		return std::vector<Device*>();
	}

	bool quit(bool wait_for_quit=true);

protected:
	typedef enum
	{
		DeviceStateIdle,
		DeviceStateConnecting,
		DeviceStateConnected,
		DeviceStateDisconnecting,
	} DeviceState;

	void changeState(DeviceState state)
	{
		std::lock_guard<std::mutex> lock(mMutex);
		DEBUG_PRINT("State: old: %d, new: %d", mState, state);
		mState = state;
	}
	DeviceState state()
	{
		std::lock_guard<std::mutex> lock(mMutex);
#ifdef DEBUG_ANNOYING
		// Report every time we change
		static auto last_state = mState;
		if (mState != last_state)
		{
			DEBUG_PRINT("State: %d", mState);
			last_state = mState;
		}
#endif // DEBUG_ANNOYING
		return mState;
	}

private:
	bool mIsRunning;
	bool mQuit;
	std::thread* mThread;
	std::mutex mMutex;

	

	DeviceState mState;

	// this is the actual thread loop that calls run()
	void start();
};

#endif // __DEVICE_H__
