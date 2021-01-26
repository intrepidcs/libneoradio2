#include <iostream>
#include <sstream>

#include <ice.h>
#include <libneoradio2.h>

#include <chrono>
#include <thread>

// TODO: Need to make a header file for this stuff
#ifdef _MSC_VER
#pragma pack(push,1)
#define PACKED 
#else
#define PACKED __attribute__((packed))
#endif

/*
typedef struct _neoRADIO2_deviceSettings {
	uint32_t sample_rate;
	uint32_t channel_1_config;
	uint32_t channel_2_Config;
	uint32_t channel_3_Config;
} PACKED neoRADIO2_deviceSettings;

*/

// These settings go into channel_1_config
typedef enum _neoRADIO2TC_EnableSetting {
	NEORADIO2TC_CONFIG_DISABLE  =   0x00,
	NEORADIO2TC_CONFIG_TC       =   0x01,
	NEORADIO2TC_CONFIG_CJ       =   0x02,
	NEORADIO2TC_CONFIG_TCCJ     =   0x03
} neoRADIO2TC_EnableSetting;
// END TODO

std::string deviceName(Neoradio2DeviceInfo& device)
{
	std::stringstream ss;
	ss << device.name;
	if (device.serial_str)
		ss << " " << device.serial_str;
	return ss.str();
}

bool get_device_info(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int dev, int bank);
bool get_device_sensor_info(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int dev, int bank);
bool toggle_led(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int dev, int bank);

int main(int argc, char* argv[])
{
	using namespace std::chrono;

	try
	{
		ice::LibraryManager* mgr = ice::LibraryManager::getLibraryManager();
		mgr->add("libneoradio2", "libneoradio2.dll");
		
		auto lib = mgr->get("libneoradio2");
		std::cout << lib->getPath() << "\n";

		ice::Function <int(Neoradio2DeviceInfo*, unsigned int*)> neoradio2_find(lib, "neoradio2_find");
		ice::Function <int(neoradio2_handle*, Neoradio2DeviceInfo*)> neoradio2_open(lib, "neoradio2_open");
		ice::Function <int(neoradio2_handle*, int, int)> neoradio2_app_start(lib, "neoradio2_app_start");
		ice::Function <int(neoradio2_handle*, int, int)> neoradio2_enter_bootloader(lib, "neoradio2_enter_bootloader");
		ice::Function <int(neoradio2_handle*, int, int)> neoradio2_request_pcbsn(lib, "neoradio2_request_pcbsn");
		ice::Function <int(neoradio2_handle*, int, int)> neoradio2_request_sensor_data(lib, "neoradio2_request_sensor_data");
		ice::Function <int(neoradio2_handle*, int, int)> neoradio2_request_settings(lib, "neoradio2_request_settings");
		ice::Function <int(neoradio2_handle*, int*, int)> neoradio2_get_chain_count(lib, "neoradio2_get_chain_count");
		ice::Function <int(neoradio2_handle*, int, int, uint8_t*, int)> neoradio2_write_sensor(lib, "neoradio2_write_sensor");
		ice::Function <int(neoradio2_handle*)> neoradio2_close(lib, "neoradio2_close");

		Neoradio2DeviceInfo devices[8];
		unsigned int device_count = 8;
		if (neoradio2_find(devices, &device_count) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_find() failed!\n";
			return 1;
		}

		std::cout << "Found " << device_count << " Devices...\n";

		for (auto i=decltype(device_count){0}; i < device_count; ++i)
		{
			int is_identified = 0;
			int is_started = 0;
			auto& device = devices[i];
			neoradio2_handle handle;
			// Open the device
			std::cout << "Opening " << deviceName(device) << "\n";
			if (neoradio2_open(&handle, &device) != NEORADIO2_SUCCESS)
			{
				std::cerr << "Failed to open: " << deviceName(device) << "!\n";
				continue;
			}
			std::cout << "Handle: " << handle << "\n";
			//std::this_thread::sleep_for(1s);

			int dev_count = 0;
			if (neoradio2_get_chain_count(&handle, &dev_count, 1) != NEORADIO2_SUCCESS)
				std::cerr << "Failed to get chain count!\n";
			
			std::cout << "Found " << dev_count << " device(s) on the chain...\n";
			std::this_thread::sleep_for(3s);

			for (int d = 0; d < dev_count; ++d)
			{
				std::cout << "\n==============================================================================\n";
				std::cout << "Device " << d + 1 << "...\n";
				std::cout << "==============================================================================\n";
				unsigned int dev = d;
				//#define NO_BOOTLOADER
#if !defined(NO_BOOTLOADER)
				// Always make sure we are in bootloader to start with

				std::cout << "Entering Bootloader...\n";
				//if (neoradio2_enter_bootloader(&handle, dev, 0xFF) != NEORADIO2_SUCCESS)
					std::cout << "Failed to enter bootloader...\n";
				//std::this_thread::sleep_for(1s);

				for (int x = 0; x < 1; ++x)
				{
					std::cout << "Getting device information on bank " << x << "...\n";
					//if (!get_device_info(handle, device, dev, x))
						std::cerr << "Failed to get device info...\n";
				}
#endif
				std::cout << "==============================================================================\n";
				std::cout << "Entering Application...\n";
				if (neoradio2_app_start(&handle, dev, 0xFF) != NEORADIO2_SUCCESS)
					std::cout << "Failed to enter Application...\n";

				//std::this_thread::sleep_for(1s);

				if (neoradio2_request_pcbsn(&handle, dev, 0xFF) != NEORADIO2_SUCCESS)
					std::cerr << "Failed to request PCBSN...\n";

				//std::this_thread::sleep_for(1s);

				for (int x = 0; x < 1; ++x)
				{
					std::cout << "Getting device information on bank " << x << "...\n";
					if (!get_device_info(handle, device, dev, x))
						std::cerr << "Failed to get device info...\n";
				}

				for (int v = 0; v <= 5; v++)
				{
					const float conv = 65535.0 / 5;
					uint16_t intval = (float)v * conv;
					uint8_t mb = 0xFF & (intval >> 8);
					uint8_t lb = 0xFF & (intval);
					uint8_t buf[3] = {0x81, mb, lb};
					if (neoradio2_write_sensor(&handle, dev, 1, buf, sizeof(buf)) != NEORADIO2_SUCCESS)
					{
						std::cerr << "Failed set relays...\n";
					}
					std::this_thread::sleep_for(1s);
				}
				if (neoradio2_request_sensor_data(&handle, dev, 0xFF) != NEORADIO2_SUCCESS)
					std::cerr << "Failed to neoradio2_request_sensor_data...\n";

				if (neoradio2_request_settings(&handle, dev, 0xFF) != NEORADIO2_SUCCESS)
					std::cerr << "Failed to neoradio2_request_settings...\n";

				//std::this_thread::sleep_for(1s);

				for (int x=0; x < 8; ++x)
				{
					std::cout << "Getting sensor information on bank " << x << "...\n";
					if (!get_device_sensor_info(handle, device, dev, x))
						std::cerr << "Failed to get sensor info...\n";
				}


				for (int x=0; x < 8; ++x)
				{
					std::cout << "Toggling LED on bank " << x << "...\n";
					if (!toggle_led(handle, device, dev, (1 << x)))
						std::cerr << "Failed to toggle_led...\n";
				}
				
			}
			std::cout << "Closing " << deviceName(device) << "\n";
			if (neoradio2_close(&handle) != NEORADIO2_SUCCESS)
			{
				std::cerr << "Failed to close: " << deviceName(device) << "!\n";
				continue;
			}
		}
	}
	catch (ice::Exception& ex)
	{
		std::cerr << "EXCEPTION: " << ex.whatString() << "\n";
		return 1;
	}
	return 0;
}

bool get_device_info(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int dev, int bank)
{
	try
	{
		auto mgr = ice::LibraryManager::getLibraryManager();
		auto lib = mgr->get("libneoradio2");
		ice::Function <int(neoradio2_handle*)> neoradio2_chain_identify(lib, "neoradio2_chain_identify");
		ice::Function <int(neoradio2_handle*, int*)> neoradio2_chain_is_identified(lib, "neoradio2_chain_is_identified");
		ice::Function <int(neoradio2_handle*, int, int, int*)> neoradio2_app_is_started(lib, "neoradio2_app_is_started");

		ice::Function <int(neoradio2_handle*, int, int, unsigned int*)> neoradio2_get_serial_number(lib, "neoradio2_get_serial_number");
		ice::Function <int(neoradio2_handle*, int, int, int*, int*, int*)> neoradio2_get_manufacturer_date(lib, "neoradio2_get_manufacturer_date");
		ice::Function <int(neoradio2_handle*, int, int, int*, int*)> neoradio2_get_firmware_version(lib, "neoradio2_get_firmware_version");
		ice::Function <int(neoradio2_handle*, int, int, int*, int*)> neoradio2_get_hardware_revision(lib, "neoradio2_get_hardware_revision");
		ice::Function <int(neoradio2_handle*, int, int, int*)> neoradio2_get_device_type(lib, "neoradio2_get_device_type");
		ice::Function <int(neoradio2_handle*, int, int, char*)> neoradio2_get_pcbsn(lib, "neoradio2_get_pcbsn");

		int is_identified = 0;
		int is_started = 0;
		int major, minor;
		int year, month, day;
		char pcbsn[17];

		if (neoradio2_app_is_started(&handle, dev, bank, &is_started) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_app_is_started() failed: " << deviceName(device) << "!\n";
		}
		std::cout << "\tFirmware State: " << (is_started ? "Application Level" : "Bootloader Level") << "\n";
		if (neoradio2_get_manufacturer_date(&handle, dev, bank, &year, &month, &day) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_get_manufacturer_date() failed: " << deviceName(device) << "!\n";
		}
		else
			std::cout << "\tManufacturer Date: " << month << "/" << day << "/" << year << "\n";

		if (neoradio2_get_firmware_version(&handle, dev, bank, &major, &minor) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_get_firmware_version() failed: " << deviceName(device) << "!\n";
		}
		else
			std::cout << "\tFirmware Version: " << major << "." << minor << "\n";

		if (neoradio2_get_hardware_revision(&handle, dev, bank, &major, &minor) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_get_hardware_revision() failed: " << deviceName(device) << "!\n";
		}
		else
			std::cout << "\tHardware Revision: " << major << "." << minor << "\n";

		if (is_started)
		{

			if (neoradio2_get_pcbsn(&handle, dev, bank, pcbsn) != NEORADIO2_SUCCESS)
			{
				std::cerr << "neoradio2_get_pcbsn() failed: " << deviceName(device) << "!\n";
			}
			else
			{
				std::cout << "\tPCB SN: " << pcbsn << "\n";
			}
		}
		return true;
	}
	catch (ice::Exception& ex)
	{
		std::cerr << "EXCEPTION: " << ex.whatString() << "\n";
		return false;
	}
	return false;
}

bool get_device_sensor_info(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int dev, int bank)
{
	try
	{
		auto mgr = ice::LibraryManager::getLibraryManager();
		auto lib = mgr->get("libneoradio2");
		ice::Function <int(neoradio2_handle*, int, int, float*)> neoradio2_read_sensor_float(lib, "neoradio2_read_sensor_float");

		ice::Function <int(neoradio2_handle*, int, int, neoRADIO2_settings*)> neoradio2_read_settings(lib, "neoradio2_read_settings");
		ice::Function <int(neoradio2_handle*, int, int, neoRADIO2_settings*)> neoradio2_write_settings(lib, "neoradio2_write_settings");

		neoRADIO2_settings settings;
		if (neoradio2_read_settings(&handle, dev, bank, &settings) != NEORADIO2_SUCCESS)
			std::cout << "neoradio2_read_settings() failed!\n";
		else
		{
			std::cout << "\tSetting Sample Rate: " << settings.config.poll_rate_ms << "\t";
			std::cout << "\tConfig1: " << settings.config.channel_1_config << "\t";
			std::cout << "\tConfig2: " << settings.config.channel_2_config << "\t";
			std::cout << "\tConfig3: " << settings.config.channel_3_config << "\n";
		}

		/*
		memset(&settings, 0, sizeof(settings));
		settings.channel_1_config = 0x3;
		if (neoradio2_write_settings(&handle, dev, bank, &settings) != NEORADIO2_SUCCESS)
			std::cout << "neoradio2_write_settings() failed!\n";

		if (neoradio2_read_settings(&handle, dev, bank, &settings) != NEORADIO2_SUCCESS)
			std::cout << "neoradio2_read_settings() failed!\n";
		else
		{
			std::cout << "\tSample Rate: " << settings.sample_rate << "\n";
			std::cout << "\tConfig1: " << settings.channel_1_config << "\n";
			std::cout << "\tConfig2: " << settings.channel_2_Config << "\n";
			std::cout << "\tConfig3: " << settings.channel_3_Config << "\n";
		}
		*/
		

		float value = 0;
		if (neoradio2_read_sensor_float(&handle, dev, bank, &value) != NEORADIO2_SUCCESS)
			std::cout << "neoradio2_read_sensor_float() failed!\n";
		else
			std::cout << "\tSensor Value: " << value << "\n";

		return true;
	}
	catch (ice::Exception& ex)
	{
		std::cerr << "EXCEPTION: " << ex.whatString() << "\n";
		return false;
	}
	return false;
}

bool toggle_led(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int dev, int bank)
{
	using namespace std::chrono;
	try
	{
		auto mgr = ice::LibraryManager::getLibraryManager();
		auto lib = mgr->get("libneoradio2");
		//std::cout << mgr["libneoradio2"].getPath() << "\n";
		ice::Function <int(neoradio2_handle*, int, int, int, int, int)> neoradio2_toggle_led(lib, "neoradio2_toggle_led");

		for (int i=0; i < 20; ++i)
		{
			if (neoradio2_toggle_led(&handle, dev, bank, 3, 0xFF, 50) != NEORADIO2_SUCCESS)
			{
				std::cout << "neoradio2_toggle_led() failed!\n";
				return false;
			}
			std::this_thread::sleep_for(100ms);
		}
		return true;
	}
	catch (ice::Exception& ex)
	{
		std::cerr << "EXCEPTION: " << ex.whatString() << "\n";
		return false;
	}
	return false;
}