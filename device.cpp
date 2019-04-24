#include "device.h"

#include <string>
#include <sstream>
#include <cstring> // memcpy

Device::Device(DeviceInfoEx& di)
{
	memcpy(&mDevInfo, &di, sizeof(mDevInfo));
	mState = DeviceStateIdle;
	mQuit = false;
	
}

Device::~Device()
{
	close();
	quit(true);
}

bool Device::open()
{
	if (!isOpen())
	{
		changeState(DeviceStateConnecting);
		mThread = new std::thread(&Device::start, this);
	}
	return true;
}

bool Device::close()
{
	if (isOpen())
	{
		changeState(DeviceStateDisconnecting);
	}
	return true;
}

void Device::start()
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

	while (!mQuit)
	{
		auto start_time = std::chrono::high_resolution_clock::now();
		bool success = false;
		switch (mState)
		{
		case DeviceStateIdle:
			success = runIdle();
			break;
		case DeviceStateConnecting:
			success = runConnecting();
			break;
		case DeviceStateConnected:
			success = runConnected();
			break;
		case DeviceStateDisconnecting:
			success = runDisconnecting();
			break;
		};
		// Make sure we don't hog the CPU
		auto elapsed_time = std::chrono::high_resolution_clock::now() - start_time;
		if (elapsed_time < 3ms)
			std::this_thread::sleep_for(3ms - elapsed_time);
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

bool Device::isOpen()
{
	mDevInfo.is_open = state() == DeviceStateConnected;
	return mDevInfo.is_open;
}

bool Device::quit(bool wait_for_quit)
{
	mMutex.lock();
	mQuit = true;
	mMutex.unlock();
	if (mThread)
	{
		if (wait_for_quit)
			mThread->join();
		delete mThread;
		mThread = NULL;
	}
	return true;
}


bool Device::addPath(DeviceChannel channel, std::string path)
{
	if (mDevInfo.channel_paths.find(channel) != mDevInfo.channel_paths.end())
		return false;
	mDevInfo.channel_paths.insert(std::make_pair(channel, path));
	return true;
}

std::string Device::path(DeviceChannel channel)
{
	if (mDevInfo.channel_paths.find(channel) == mDevInfo.channel_paths.end())
		return std::string();
	return mDevInfo.channel_paths[channel];
}
