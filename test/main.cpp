#include <iostream>
#include <sstream>

#include <ice.h>
#include <neoradio2.h>

#include <chrono>
#include <thread>

std::string deviceName(Neoradio2DeviceInfo& device)
{
	std::stringstream ss;
	ss << device.name;
	if (device.serial_str)
		ss << " " << device.serial_str;
	return ss.str();
}

bool get_device_info(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int bank);

int main(int argc, char* argv[])
{
	using namespace std::chrono;

	try
	{
		auto mgr = ice::LibraryManager::getLibraryManager();
		mgr.add("libneoradio2", "libneoradio2.dll");
		std::cout << mgr["libneoradio2"].getPath() << "\n";

		ice::Function <void(void)> find_hid_devices(&mgr["libneoradio2"], "find_hid_devices");
		ice::Function <int(Neoradio2DeviceInfo*, unsigned int*)> neoradio2_find_devices(&mgr["libneoradio2"], "neoradio2_find_devices");
		ice::Function <int(neoradio2_handle*, Neoradio2DeviceInfo*)> neoradio2_open(&mgr["libneoradio2"], "neoradio2_open");
		ice::Function <int(neoradio2_handle*, int)> neoradio2_app_start(&mgr["libneoradio2"], "neoradio2_app_start");
		ice::Function <int(neoradio2_handle*, int)> neoradio2_enter_bootloader(&mgr["libneoradio2"], "neoradio2_enter_bootloader");
		ice::Function <int(neoradio2_handle*)> neoradio2_close(&mgr["libneoradio2"], "neoradio2_close");

		Neoradio2DeviceInfo devices[8];
		unsigned int device_count = 8;
		if (neoradio2_find_devices(devices, &device_count) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_find_devices() failed!\n";
			return 1;
		}

		std::cout << "Found " << device_count << " Devices...\n";

		for (auto i=decltype(device_count){0}; i < device_count; ++i)
		{
			int bank = 0xFF;
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

			// Always make sure we are in bootloader to start with
			std::cout << "==============================================================================\n";
			std::cout << "Entering bootloader...\n";
			if (neoradio2_enter_bootloader(&handle, bank) != NEORADIO2_SUCCESS)
				std::cout << "Failed to enter bootloader...\n";

			std::cout << "Getting device information:\n";
			if (!get_device_info(handle, device, bank))
				std::cerr << "Failed to get device info...\n";

			std::cout << "==============================================================================\n";
			std::cout << "Entering Application...\n";
			if (neoradio2_app_start(&handle, bank) != NEORADIO2_SUCCESS)
				std::cout << "Failed to start app...\n";

			std::cout << "Getting device information:\n";
			if (!get_device_info(handle, device, bank))
				std::cerr << "Failed to get device info...\n";

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


bool get_device_info(neoradio2_handle& handle, Neoradio2DeviceInfo& device, int bank)
{
	try
	{
		auto mgr = ice::LibraryManager::getLibraryManager();
		mgr.add("libneoradio2", "libneoradio2.dll");
		//std::cout << mgr["libneoradio2"].getPath() << "\n";
		ice::Function <int(neoradio2_handle*, int)> neoradio2_chain_identify(&mgr["libneoradio2"], "neoradio2_chain_identify");
		ice::Function <int(neoradio2_handle*, int, int*)> neoradio2_chain_is_identified(&mgr["libneoradio2"], "neoradio2_chain_is_identified");
		ice::Function <int(neoradio2_handle*, int, int*)> neoradio2_app_is_started(&mgr["libneoradio2"], "neoradio2_app_is_started");

		ice::Function <int(neoradio2_handle*, int, unsigned int*)> neoradio2_get_serial_number(&mgr["libneoradio2"], "neoradio2_get_serial_number");
		ice::Function <int(neoradio2_handle*, int, int*, int*, int*)> neoradio2_get_manufacturer_date(&mgr["libneoradio2"], "neoradio2_get_manufacturer_date");
		ice::Function <int(neoradio2_handle*, int, int*, int*)> neoradio2_get_firmware_version(&mgr["libneoradio2"], "neoradio2_get_firmware_version");
		ice::Function <int(neoradio2_handle*, int, int*, int*)> neoradio2_get_hardware_revision(&mgr["libneoradio2"], "neoradio2_get_hardware_revision");
		ice::Function <int(neoradio2_handle*, int, int*)> neoradio2_get_device_type(&mgr["libneoradio2"], "neoradio2_get_device_type");


		int is_identified = 0;
		int is_started = 0;
		int major, minor;
		int year, month, day;
		std::cout << "Identifying Chain on " << deviceName(device) << "\n";
		if (neoradio2_chain_identify(&handle, bank) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_chain_identify() failed: " << deviceName(device) << "!\n";
		}
		if (neoradio2_chain_is_identified(&handle, 0xFF, &is_identified) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_is_chain_identified() failed: " << deviceName(device) << "!\n";
		}
		std::cout << "Chain is identified: " << (is_identified ? "True" : "False") << "\n";

		if (neoradio2_app_is_started(&handle, bank, &is_started) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_app_is_started() failed: " << deviceName(device) << "!\n";
		}
		std::cout << "Firmware State: " << (is_started ? "Application Level" : "Bootloader Level") << "\n";
		if (neoradio2_get_manufacturer_date(&handle, bank, &year, &month, &day) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_get_manufacturer_date() failed: " << deviceName(device) << "!\n";
		}
		else
			std::cout << "Manufacturer Date: " << month << "/" << day << "/" << year << "\n";

		if (neoradio2_get_firmware_version(&handle, bank, &major, &minor) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_get_firmware_version() failed: " << deviceName(device) << "!\n";
		}
		else
			std::cout << "Firmware Version: " << major << "." << minor << "\n";

		if (neoradio2_get_hardware_revision(&handle, bank, &major, &minor) != NEORADIO2_SUCCESS)
		{
			std::cerr << "neoradio2_get_hardware_revision() failed: " << deviceName(device) << "!\n";
		}
		else
			std::cout << "Hardware Revision: " << major << "." << minor << "\n";

		return true;
	}
	catch (ice::Exception& ex)
	{
		std::cerr << "EXCEPTION: " << ex.whatString() << "\n";
		return false;
	}
	return false;
}
