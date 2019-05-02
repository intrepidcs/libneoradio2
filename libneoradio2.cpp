// libneoradio2.cpp : Defines the exported functions for the DLL application.
//

#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif

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
std::chrono::milliseconds _blocking_timeout(2000);
#endif


static std::mutex _lock;

Device* _getDevice(neoradio2_handle handle)
{
#ifdef DEBUG_ANNOYING
	DEBUG_PRINT("_device_map size: %d", _device_map.size());
#endif DEBUG_ANNOYING
	std::lock_guard<std::mutex> lock(_lock);
	auto iter = _device_map.find(handle);
	if (iter != _device_map.end())
		return iter->second;
	return NULL;
}

Device* _createNewDevice(neoradio2_handle* handle, Neoradio2DeviceInfo* device)
{
	DEBUG_PRINT("creating New Device: %s %s", device->name, device->serial_str);
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

std::tuple<CommandStateType, int> _StatusType_to_cmd(StatusType& type)
{
	switch (type)
	{
	case StatusChain:
	case StatusAppStart:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_IDENTIFY);
		break;
	case StatusPCBSN:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_READ_PCBSN);
		break;
	case StatusSensorRead:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_SENSOR);
		break;
	case StatusSensorWrite:
		return std::make_tuple(CommandStateType::CommandStateHost, NEORADIO2_COMMAND_WRITE_DATA);
		break;
	case StatusSettingsRead:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_READ_SETTINGS);
		break;
	case StatusSettingsWrite:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_WRITE_SETTINGS);
		break;
	case StatusCalibration:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_CAL);
		break;
	case StatusCalibrationPoints:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_CALPOINTS);
		break;
	case StatusCalibrationStored:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_CAL_STORE);
		break;
	case StatusCalibrationInfo:
		return std::make_tuple(CommandStateType::CommandStateDevice, NEORADIO2_STATUS_CAL_INFO);
		break;
	case StatusLedToggle:
		return std::make_tuple(CommandStateType::CommandStateHost, NEORADIO2_COMMAND_TOGGLE_LED);
		break;
	default:
		return std::make_tuple(CommandStateType::CommandStateUnknown, -1);
	};
	return std::make_tuple(CommandStateType::CommandStateUnknown, -1);
}

LIBNEORADIO2_API int neoradio2_find(Neoradio2DeviceInfo* devices, unsigned int* device_count)
{
	if (!devices || !device_count)
		return NEORADIO2_FAILURE;

	auto devs = neoRADIO2Device::_findAll();
	memset(devices, 0, sizeof(Neoradio2DeviceInfo)*(*device_count));
	for (unsigned int i=0; i < devs.size() && i < *device_count; ++i)
	{
		auto dev_info = devs.at(i)->deviceInfo();
		memcpy(&devices[i], &dev_info, sizeof(devices[i]));
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
		if (_set_blocking)
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
		return NEORADIO2_ERR_WBLOCK;
	}
	return NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_is_opened(neoradio2_handle* handle, int* is_opened)
{
	if (!is_opened)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	*is_opened = dev->isOpen();
	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API int neoradio2_close(neoradio2_handle* handle)
{
	auto dev = _getDevice(*handle);
	if (!dev)
		return NEORADIO2_FAILURE;

	if (dev->close())
	{
		if (_set_blocking)
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
		return NEORADIO2_ERR_WBLOCK;
	}
	return NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_is_closed(neoradio2_handle* handle, int* is_closed)
{
	if (!is_closed)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	*is_closed = !dev->isOpen();
	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API int neoradio2_chain_is_identified(neoradio2_handle* handle, int* is_identified)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen() && !is_identified)
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	*is_identified = radio_dev->isChainIdentified(_blocking_timeout);

	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API int neoradio2_chain_identify(neoradio2_handle* handle)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	auto success = radio_dev->requestIdentifyChain(_blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_app_is_started(neoradio2_handle* handle, int device, int bank, int* is_started)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen() && !is_started)
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	*is_started = radio_dev->isApplicationStarted(device, bank, _blocking_timeout);

	return NEORADIO2_SUCCESS;
}

LIBNEORADIO2_API int neoradio2_app_start(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->startApplication(device, bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_enter_bootloader(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->enterBootloader(device, bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_get_serial_number(neoradio2_handle* handle, int device, int bank, unsigned int* serial_number)
{
	if (!serial_number)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->getSerialNumber(device, bank, *serial_number, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_get_manufacturer_date(neoradio2_handle* handle, int device, int bank, int* year, int* month, int* day)
{
	if (!year && !month && !day)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->getManufacturerDate(device, bank, *year, *month, *day, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_get_firmware_version(neoradio2_handle* handle, int device, int bank, int* major, int* minor)
{
	if (!major && !minor)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->getFirmwareVersion(device, bank, *major, *minor, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_get_hardware_revision(neoradio2_handle* handle, int device, int bank, int* major, int* minor)
{
	if (!major && !minor)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->getHardwareRevision(device, bank, *major, *minor, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_get_device_type(neoradio2_handle* handle, int device, int bank, unsigned int* device_type)
{
	if (!device_type)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->getDeviceType(device, bank, *device_type, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_request_pcbsn(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	auto success = radio_dev->requestPCBSN(device, bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_get_pcbsn(neoradio2_handle* handle, int device, int bank, char* pcb_sn)
{
	if (!pcb_sn)
		return NEORADIO2_FAILURE;

	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::string temp;
	bool success = radio_dev->getPCBSN(device, bank, temp);
	memset(pcb_sn, 0, 17);
	memcpy(pcb_sn, temp.c_str(), 17);
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_request_sensor_data(neoradio2_handle* handle, int device, int bank, int enable_cal)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	auto success = radio_dev->requestSensorData(device, bank, enable_cal, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_read_sensor_float(neoradio2_handle* handle, int device, int bank, float* value)
{
	if (!value)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<uint8_t> data;
	bool success = radio_dev->readSensorData(device, bank, data);
	if (data.size() < sizeof(*value))
		return NEORADIO2_FAILURE;
	memcpy(value, data.data(), sizeof(*value));
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_read_sensor_array(neoradio2_handle* handle, int device, int bank, int* arr, int* arr_size)
{
	if (!arr && !arr_size)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<uint8_t> data;
	bool success = radio_dev->readSensorData(device, bank, data);
	if ((int)data.size() > *arr_size)
		return NEORADIO2_FAILURE;
	for (int i=0; i < *arr_size && i < (int)data.size(); ++i)
		arr[i] = data[i];
	*arr_size = data.size();
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_write_sensor(neoradio2_handle* handle, int device, int bank, int mask, int value)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->writeSensorData(device, bank, mask, value, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_write_sensor_successful(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	// TODO
	return radio_dev->writeSensorDataSuccessful(device, bank) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_request_settings(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	auto success = radio_dev->requestSettings(device, bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_read_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_settings* settings)
{
	if (!settings)
		return NEORADIO2_FAILURE;

	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<uint8_t> data;
	bool success = radio_dev->readSettings(device, bank, *settings);
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_write_settings(neoradio2_handle* handle, int device, int bank, neoRADIO2_settings* settings)
{
	if (!settings)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->writeSettings(device, bank, *settings, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}


LIBNEORADIO2_API int neoradio2_write_settings_successful(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->writeSettingsSuccessful(device, bank) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_chain_count(neoradio2_handle* handle, int* count, int identify)
{
	if (!count)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	if (!_set_blocking)
		identify = false;

	auto success = radio_dev->getChainCount(*count, identify==1, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_request_calibration(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->requestCalibration(device, bank, *header, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_read_calibration_array(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int* arr_size)
{
	if (!arr && !arr_size)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<float> data;
	bool success = radio_dev->readCalibration(device, bank, *header, data, _blocking_timeout);
	if ((int)data.size() > *arr_size)
		return NEORADIO2_FAILURE;
	for (int i=0; i < *arr_size && i < (int)data.size(); ++i)
		arr[i] = data[i];
	*arr_size = data.size();
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_request_calibration_points(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->requestCalibrationPoints(device, bank, *header, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_read_calibration_points_array(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int* arr_size)
{
	if (!arr && !arr_size)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<float> data;
	bool success = radio_dev->readCalibrationPoints(device, bank, *header, data, _blocking_timeout);
	if ((int)data.size() > *arr_size)
		return NEORADIO2_FAILURE;
	for (int i=0; i < *arr_size && i < (int)data.size(); ++i)
		arr[i] = data[i];
	*arr_size = data.size();
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_write_calibration(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int arr_size)
{
	if (!header && !arr && !arr_size)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<float> data;
	for (int i = 0; i < arr_size; ++i)
		data.push_back(arr[i]);
	auto success = radio_dev->writeCalibration(device, bank, *header, data, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_write_calibration_successful(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;


	return radio_dev->writeCalibrationSuccessful(device, bank) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_write_calibration_points(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header, float* arr, int arr_size)
{
	if (!header && !arr && !arr_size)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	std::vector<float> data;
	for (int i = 0; i < arr_size; ++i)
		data.push_back(arr[i]);
	auto success = radio_dev->writeCalibrationPoints(device, bank, *header, data, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_write_calibration_points_successful(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	return radio_dev->writeCalibrationPointsSuccessful(device, bank) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_store_calibration(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto success = radio_dev->requestStoreCalibration(device, bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_is_calibration_stored(neoradio2_handle* handle, int device, int bank, int* stored)
{
	if (!stored)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	bool _stored = false;
	bool success = radio_dev->isCalibrationStored(device, bank, _stored, _blocking_timeout);
	*stored = _stored ? 1 : 0;
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_get_calibration_is_valid(neoradio2_handle* handle, int device, int bank, int* is_valid)
{
	if (!is_valid)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	bool valid = false;
	bool success = radio_dev->isCalibrationStored(device, bank, valid, _blocking_timeout);
	*is_valid = valid;
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_request_calibration_info(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	auto success = radio_dev->requestCalibrationInfo(device, bank, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_read_calibration_info(neoradio2_handle* handle, int device, int bank, neoRADIO2frame_calHeader* header)
{
	if (!header)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	bool success = radio_dev->readCalibrationInfo(device, bank, *header, _blocking_timeout);
	return success ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}

LIBNEORADIO2_API int neoradio2_toggle_led(neoradio2_handle* handle, int device, int bank, int ms)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	auto success = radio_dev->toggleLED(device, bank, ms, _blocking_timeout) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
	if (!_set_blocking && success == NEORADIO2_FAILURE)
		return NEORADIO2_ERR_WBLOCK;
	return success;
}

LIBNEORADIO2_API int neoradio2_toggle_led_successful(neoradio2_handle* handle, int device, int bank)
{
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;
	return radio_dev->toggleLEDSuccessful(device, bank) ? NEORADIO2_SUCCESS : NEORADIO2_FAILURE;
}


LIBNEORADIO2_API int neoradio2_get_status(neoradio2_handle* handle, int device, int bank, int bitfield, StatusType type, CommandStatus* status)
{
	if (!status)
		return NEORADIO2_FAILURE;
	auto dev = _getDevice(*handle);
	if (!dev->isOpen())
		return NEORADIO2_FAILURE;
	auto radio_dev = static_cast<neoRADIO2Device*>(dev);
	if (!radio_dev)
		return NEORADIO2_FAILURE;

	auto cmd = _StatusType_to_cmd(type);
	*status = radio_dev->getCommandState(device, bank, bitfield, std::get<0>(cmd), std::get<1>(cmd));

	return NEORADIO2_SUCCESS;
}