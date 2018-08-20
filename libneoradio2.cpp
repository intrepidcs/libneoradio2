// libneoradio2.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libneoradio2.h"

#include "device.h"
#include "hiddevice.h"
#include "neoradio2device.h"

#include <iostream>
#include <map>
#include <mutex>

using namespace std::chrono;
using namespace std::chrono_literals;

std::map<neoradio2_handle, Device*> _device_map;

// If set to true all APIs are blocking
static bool _set_blocking = true;

#ifdef ENABLE_EXTENDED_BLOCKING_TIMEOUT
std::chrono::milliseconds _blocking_timeout(1000*120);
#else
std::chrono::milliseconds _blocking_timeout(4000);
#endif


static std::mutex _lock;

Device* _getDevice(neoradio2_handle handle)
{
	std::lock_guard<std::mutex> lock(_lock);
	auto iter = _device_map.find(handle);
	if (iter != _device_map.end())
		return iter->second;
	return NULL;
}

Device* _createNewDevice(neoradio2_handle* handle, Neoradio2DeviceInfo* device)
{
	std::lock_guard<std::mutex> lock(_lock);
	static neoradio2_handle counter = 0;
	if (!device && !handle)
		return NULL;
	for (auto& i : _device_map)
	{
		auto info = i.second->deviceInfo();
		if (info.name == device->name && info.serial_str == device->serial_str)
			return i.second;
	}
	auto devs = DeviceFinder<neoRADIO2Device>::findAll();
	for (auto& dev : devs)
	{
		auto info = dev->deviceInfo();
		if (strcmp(info.name, device->name) == 0 && strcmp(info.serial_str, device->serial_str) == 0)
		{
			*handle = ++counter;
			_device_map[*handle] = dev;
			return dev;
		}
		else
		{
			delete dev;
		}
	}
	return NULL;
}

LIBNEORADIO2_API void find_hid_devices()
{
	return;
	/*
	auto devices = HidDevice::_findAll();
	
	std::cout << "Found " << devices.size() << " HID Device(s)\n";
	auto i = 1;
	for (auto* dev : devices)
	{
		std::string name;
		if (dev->deviceInfo().name == NULL)
			name = "NULL";
		else
			name = dev->deviceInfo().name;

		std::string serial;
		if (dev->deviceInfo().serial_str == NULL)
			serial = "NULL";
		else
			serial = dev->deviceInfo().serial_str;

		std::cout << "\t" << i++ << ".)\t" << name << " " << serial << "\t" << std::hex << dev->deviceInfo().vendor_id << "/" << dev->deviceInfo().product_id << "\n";
	}
	*/
}

LIBNEORADIO2_API int neoradio2_find_devices(Neoradio2DeviceInfo* devices, unsigned int* device_count)
{
	if (!devices || !device_count)
		return NEORADIO2_FAILURE;

	auto devs = neoRADIO2Device::_findAll();
	memset(devices, 0, sizeof(Neoradio2DeviceInfo)*(*device_count));
	for (unsigned int i=0; i < devs.size() && i < *device_count; ++i)
	{
		memcpy(&devices[i], &devs.at(i)->deviceInfo(), sizeof(devices[i]));
		// TODO: Memory leak / thread leak here
		//delete devs[i];
		//devs[i] = nullptr;
	}
	*device_count = devs.size();

	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API void neoradio2_set_blocking(int blocking, long long ms_timeout)
{
	std::lock_guard<std::mutex> lock(_lock);
	_set_blocking = blocking == 1;
	if (!_set_blocking)
		_blocking_timeout = 0ms;
	else
		_blocking_timeout = std::chrono::milliseconds(ms_timeout);
}

LIBNEORADIO2_API int neoradio2_is_blocking()
{
	std::lock_guard<std::mutex> lock(_lock);
	return (int)_set_blocking;
}

LIBNEORADIO2_API int neoradio2_open(neoradio2_handle* handle, Neoradio2DeviceInfo* device)
{
	auto dev = _getDevice(*handle);
	if (!dev)
	{
		dev = _createNewDevice(handle, device);
		if (!dev)
			return NEORADIO2_FAILURE;
	}

	if (dev->open())
	{
		auto start = high_resolution_clock::now();
		std::lock_guard<std::mutex> lock(_lock);
		while (!dev->isOpen())
		{
			auto now = high_resolution_clock::now();
			auto elapsed = duration_cast<milliseconds>(now - start);
			if (elapsed >= _blocking_timeout)
				return NEORADIO2_FAILURE;
			std::this_thread::sleep_for(1ms);
		}
		return NEORADIO2_SUCCESS;
	}
	return NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_is_opened(neoradio2_handle* handle)
{
	auto dev = _getDevice(*handle);
	if (dev->isOpen())
		return NEORADIO2_SUCCESS;
	return NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_close(neoradio2_handle* handle)
{
	auto dev = _getDevice(*handle);
	if (!dev)
		return NEORADIO2_FAILURE;

	if (dev->close())
	{
		auto start = high_resolution_clock::now();
		std::lock_guard<std::mutex> lock(_lock);
		while (dev->isOpen())
		{
			auto now = high_resolution_clock::now();
			auto elapsed = duration_cast<milliseconds>(now - start);
			if (elapsed >= _blocking_timeout)
				return NEORADIO2_FAILURE;
			std::this_thread::sleep_for(1ms);
		}
		return NEORADIO2_SUCCESS;
	}
	return NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_is_closed(neoradio2_handle* handle)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_SUCCESS;
	return NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_chain_is_identified(neoradio2_handle* handle, int bank, int* is_identified)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen() && !is_identified)
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::this_thread::sleep_for(2s);
	*is_identified = radio_dev->isChainIdentified(bank, _blocking_timeout);

	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API int neoradio2_chain_identify(neoradio2_handle* handle, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	return radio_dev->identifyChain(bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_app_is_started(neoradio2_handle* handle, int bank, int* is_started)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen() && !is_started)
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	*is_started = radio_dev->isApplicationStarted(bank, _blocking_timeout);

	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API int neoradio2_app_start(neoradio2_handle* handle, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->startApplication(bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_enter_bootloader(neoradio2_handle* handle, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->enterBootloader(bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_serial_number(neoradio2_handle* handle, int bank, unsigned int* serial_number)
{
	if (!serial_number)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->getSerialNumber(bank, *serial_number, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_manufacturer_date(neoradio2_handle* handle, int bank, int* year, int* month, int* day)
{
	if (!year && !month && !day)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->getManufacturerDate(bank, *year, *month, *day, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_firmware_version(neoradio2_handle* handle, int bank, int* major, int* minor)
{
	if (!major && !minor)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->getFirmwareVersion(bank, *major, *minor, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_hardware_revision(neoradio2_handle* handle, int bank, int* major, int* minor)
{
	if (!major && !minor)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->getHardwareRevision(bank, *major, *minor, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_device_type(neoradio2_handle* handle, int bank, unsigned int* device_type)
{
	if (!device_type)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->getDeviceType(bank, *device_type, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}