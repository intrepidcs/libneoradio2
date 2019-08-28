#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <libneoradio2.h>

#include <tuple>
#include <array>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#pragma comment(lib, "Setupapi.lib")
#endif

namespace py = pybind11;

class NeoRadio2Exception : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};

class NeoRadio2ExceptionWouldBlock : public NeoRadio2Exception
{
	using NeoRadio2Exception::NeoRadio2Exception;
};

PYBIND11_MODULE(neoradio2, m) {
	py::options options;
	options.disable_function_signatures();
	options.enable_user_defined_docstrings();

    m.doc() = R"pbdoc(
        neoRADIO2 Python Library
        ----------------------
		TODO
    )pbdoc";
    
	py::register_exception<NeoRadio2Exception>(m, "Exception");
	py::register_exception<NeoRadio2ExceptionWouldBlock>(m, "ExceptionWouldBlock");
	
	py::enum_<StatusType>(m, "StatusType", py::arithmetic())
		.value("StatusAppStart", StatusType::StatusAppStart)
		.value("StatusChain", StatusType::StatusChain)
		.value("StatusPCBSN", StatusType::StatusPCBSN)
		.value("StatusSensorRead", StatusType::StatusSensorRead)
		.value("StatusSensorWrite", StatusType::StatusSensorWrite)
		.value("StatusSettingsRead", StatusType::StatusSettingsRead)
		.value("StatusSettingsWrite", StatusType::StatusSettingsWrite)
		.value("StatusCalibration", StatusType::StatusCalibration)
		.value("StatusCalibrationPoints", StatusType::StatusCalibrationPoints)
		.value("StatusCalibrationStored", StatusType::StatusCalibrationStored)
		.value("StatusCalibrationInfo", StatusType::StatusCalibrationInfo)
		.value("StatusLedToggle", StatusType::StatusLedToggle)
		.export_values();

	py::enum_<neoRADIO2_deviceTypes>(m, "DeviceTypes", py::arithmetic())
		.value("TC", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_TC)
		.value("DIO", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_DIO)
		.value("PWRRLY", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_PWRRLY)
		.value("AIN", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_AIN)
		.value("AOUT", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_AOUT)
		.value("CANHUB", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_CANHUB)
		.value("BADGE", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_BADGE)
		.value("HOST", neoRADIO2_deviceTypes::NEORADIO2_DEVTYPE_HOST)
		.export_values();

	py::enum_<CommandStateType>(m, "CommandStateType", py::arithmetic())
		.value("CommandStateHost", CommandStateType::CommandStateHost)
		.value("CommandStateDevice", CommandStateType::CommandStateDevice)
		.export_values();

	py::enum_<neoRADIO2CalType>(m, "CalType", py::arithmetic())
		.value("ENABLED", neoRADIO2CalType::NEORADIO2CALTYPE_ENABLED)
		.value("NOCAL", neoRADIO2CalType::NEORADIO2CALTYPE_NOCAL)
		.value("NOCAL_ENHANCED", neoRADIO2CalType::NEORADIO2CALTYPE_NOCAL_ENHANCED)
		.export_values();

	py::enum_<neoRADIO2_LEDMode>(m, "LEDMode", py::arithmetic())
		.value("OFF", neoRADIO2_LEDMode::LEDMODE_OFF)
		.value("ON", neoRADIO2_LEDMode::LEDMODE_ON)
		.value("BLINK_ONCE", neoRADIO2_LEDMode::LEDMODE_BLINK_ONCE)
		.value("BLINK_DURATION_MS", neoRADIO2_LEDMode::LEDMODE_BLINK_DURATION_MS)
		.export_values();

	py::enum_<CommandStatus>(m, "CommandStatus", py::arithmetic())
		.value("StatusInProgress", CommandStatus::StatusInProgress)
		.value("StatusFinished", CommandStatus::StatusFinished)
		.value("StatusError", CommandStatus::StatusError)
		.export_values();

    // libneoradio2.h
    py::class_<Neoradio2DeviceInfo>(m, "Neoradio2DeviceInfo")
        .def(py::init([]() { return new Neoradio2DeviceInfo{0}; }))
		.def("__repr__", [](const Neoradio2DeviceInfo& self) {
			std::stringstream ss;
			ss << "<neoradio2.Neoradio2DeviceInfo '" << self.name << " " << self.serial_str << "'>";
			return ss.str();
		})
        .def_property_readonly("name",
                      // Read Property
                      [](const Neoradio2DeviceInfo& self) {
                          return self.name;
                      })
        .def_property_readonly("serial_str",
                      // Read Property
                      [](const Neoradio2DeviceInfo& self) {
                          return self.serial_str;
                      })
        .def_readwrite("vendor_id", &Neoradio2DeviceInfo::vendor_id)
        .def_readwrite("product_id", &Neoradio2DeviceInfo::product_id)
        .def_property_readonly("_reserved",
                      // Read Property
                      [](const Neoradio2DeviceInfo& self) {
                          return self._reserved;
                      });
        
    // radio2_frame.h
	py::class_<neoRADIO2_deviceSettings>(m, "neoRADIO2_deviceSettings")
		.def(py::init([]() { return new neoRADIO2_deviceSettings{0}; }))
		.def_readwrite("poll_rate_ms", &neoRADIO2_deviceSettings::poll_rate_ms)
		.def_readwrite("channel_1_config", &neoRADIO2_deviceSettings::channel_1_config)
		.def_readwrite("channel_2_config", &neoRADIO2_deviceSettings::channel_2_config)
		.def_readwrite("channel_3_config", &neoRADIO2_deviceSettings::channel_3_config);
        
    // radio2_frame.h
	py::class_<neoRADIO2settings_CAN>(m, "neoRADIO2settings_CAN")
		.def(py::init([]() { return new neoRADIO2settings_CAN{0}; }))
		.def_readwrite("Arbid", &neoRADIO2settings_CAN::Arbid)
		.def_readwrite("Location", &neoRADIO2settings_CAN::Location)
		.def_readwrite("msgType", &neoRADIO2settings_CAN::msgType);
    
    // radio2_frame.h
	py::class_<neoRADIO2Settings_ChannelName>(m, "neoRADIO2Settings_ChannelName")
		.def(py::init([]() { return new neoRADIO2Settings_ChannelName{0}; }))
		.def_readwrite("length", &neoRADIO2Settings_ChannelName::length)
		.def_readwrite("charSize", &neoRADIO2Settings_ChannelName::charSize)
		//.def_readwrite("chars", &neoRADIO2Settings_ChannelName::chars)
		.def_property("chars", 
			[](neoRADIO2Settings_ChannelName& self) 
			{
				return std::string((char*)self.chars.u8);
			},
			[](neoRADIO2Settings_ChannelName& self, std::string value)
			{
				memcpy(self.chars.u8, value.c_str(), value.size());
			});

	// radio2_frame.h
	py::class_<neoRADIO2_settings>(m, "neoRADIO2_settings")
		.def(py::init([]() { return new neoRADIO2_settings{0}; }))
		.def_readwrite("config", &neoRADIO2_settings::config)
		.def_readwrite("name1", &neoRADIO2_settings::name1)
		.def_readwrite("name2", &neoRADIO2_settings::name2)
		.def_readwrite("name3", &neoRADIO2_settings::name3)
		.def_readwrite("can", &neoRADIO2_settings::can);


	py::class_<neoRADIO2frame_calHeader>(m, "neoRADIO2frame_calHeader")
		.def(py::init([]() { return new neoRADIO2frame_calHeader{0}; }))
		.def_readwrite("num_of_pts", &neoRADIO2frame_calHeader::num_of_pts)
		.def_readwrite("channel", &neoRADIO2frame_calHeader::channel)
		.def_readwrite("range", &neoRADIO2frame_calHeader::range)
		.def_readwrite("cal_is_valid", &neoRADIO2frame_calHeader::cal_is_valid);

	py::class_<neoRADIO2_PerfStatistics>(m, "neoRADIO2_PerfStatistics")
		.def(py::init([]() { return new neoRADIO2_PerfStatistics{ 0 }; }))
		.def_readwrite("comm_timeout_reset_cnt", &neoRADIO2_PerfStatistics::comm_timeout_reset_cnt)
		.def_readwrite("cmd_process_time_ms", &neoRADIO2_PerfStatistics::cmd_process_time_ms)
		.def_readwrite("max_cmd_process_time_ms", &neoRADIO2_PerfStatistics::max_cmd_process_time_ms)
		.def_readwrite("bytes_rx", &neoRADIO2_PerfStatistics::bytes_rx)
		.def_readwrite("bytes_tx", &neoRADIO2_PerfStatistics::bytes_tx)
		.def_readwrite("ignored_rx", &neoRADIO2_PerfStatistics::ignored_rx)
		.def_readwrite("checksum_error_cnt", &neoRADIO2_PerfStatistics::checksum_error_cnt)
		.def_readwrite("last_cmd", &neoRADIO2_PerfStatistics::last_cmd)
		.def_readwrite("buffer_current", &neoRADIO2_PerfStatistics::buffer_current)
		.def_readwrite("buffer_max", &neoRADIO2_PerfStatistics::buffer_max);

    // Functions ==============================================================
    m.def("find", []() {
        const unsigned int size = 8;
        unsigned int device_count = size;
        Neoradio2DeviceInfo temp[size];
        std::vector<Neoradio2DeviceInfo> devs;
        auto res = neoradio2_find(temp, &device_count);
        if (res != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_find() failed");
        devs.reserve(device_count);
        std::copy(std::begin(temp), std::begin(temp)+device_count, std::back_inserter(devs));
        return devs;
        }, R"pbdoc(
		find()

		Finds all neoRAD-IO2 Devices.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns a tuple of neoradio2.Neoradio2DeviceInfo
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			>>>
	)pbdoc");
    
    m.def("open", [](Neoradio2DeviceInfo* device) {
		py::gil_scoped_release release;
		neoradio2_handle handle;
		auto result = neoradio2_open(&handle, device);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
		{
			//throw NeoRadio2ExceptionWouldBlock("neoradio2_open() would block");
			return handle;
		}
		else if (neoradio2_is_blocking() && result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_open() failed");
		return handle;
		}, R"pbdoc(
		open(device)

		Open a neoRAD-IO2 Device

		Args:
			device (neoradio2.Neoradio2DeviceInfo): specified device to open, typically from neoradio2.find()

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			A handle to the device.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> len(devices)
			1
			>>> handle = neoradio2.open(devices[0])
			>>> neoradio2.close(handle)
	)pbdoc");
    
	m.def("is_blocking", []() {
		py::gil_scoped_release release;
		return neoradio2_is_blocking() == 1;
	}, R"pbdoc(
		is_blocking()

        Check if API is blocking
        
		Raises:
			None

		Returns:
			True/False.
    )pbdoc");

	m.def("set_blocking", [](int blocking, long long ms_timeout) {
		py::gil_scoped_release release;
		neoradio2_set_blocking(blocking, ms_timeout);
	}, R"pbdoc(
		set_blocking(blocking, ms_timeout)

		Sets the API to blocking or non-blocking mode.

		Raises:
			None
		
		Args:
			blocking (int): 1 = blocking, 0 = non-blocking
			ms_timeout (int): timeout in milliseconds. Only matters in blocking mode.

		Returns:
			Returns None.
    )pbdoc");
    
    m.def("is_opened", [](neoradio2_handle& handle) {
        py::gil_scoped_release release;
        int is_true = 0;
        if (neoradio2_is_opened(&handle, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_is_opened() failed");
        return is_true == 1;
        }, R"pbdoc(
		is_opened(handle)

		Checks if the handle is currently open.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True/False
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.is_opened(handle)
			...     neoradio2.close(handle)
			...
			True
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			>>>
	)pbdoc");
    
    m.def("close", [](neoradio2_handle& handle) {
		py::gil_scoped_release release;
		auto result = neoradio2_close(&handle);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_close() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_close() failed");
		return handle;
        }, R"pbdoc(
		close(handle)

		Closes the handle for the device.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success, Exception otherwise
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			>>>
	)pbdoc");

    m.def("is_closed", [](neoradio2_handle& handle) {
        py::gil_scoped_release release;
        int is_true = 0;
        if (neoradio2_is_closed(&handle, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_is_closed() failed");
        return is_true == 1;
        }, R"pbdoc(
		is_closed(handle)

		Checks if the handle is currently closed.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True/False
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.is_closed(handle)
			...     neoradio2.close(handle)
			...
			False
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			>>>
	)pbdoc");
    
    m.def("chain_is_identified", [](neoradio2_handle& handle) {
        py::gil_scoped_release release;
        int is_true = 0;
        if (neoradio2_chain_is_identified(&handle, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_chain_is_identified() failed");
        return is_true == 1;
        }, R"pbdoc(
		chain_is_identified(handle)

		Checks if the chain is currently identified. Chain needs to be identified before other commands will be successful.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True/False
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.chain_is_identified(handle)
			...     neoradio2.close(handle)
			...
			False
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			>>>
	)pbdoc");
    
    m.def("chain_identify", [](neoradio2_handle& handle) {
		py::gil_scoped_release release;
		auto result = neoradio2_chain_identify(&handle);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_chain_identify() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_chain_identify() failed");
		return true;
        }, R"pbdoc(
		chain_identify(handle)

		Identifies the chain. Chain needs to be identified before other commands will be successful. Each bank needs to be identified by the host before it can start receiving commands.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True/False
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.neoradio2_chain_identify(handle)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			True
			>>>
	)pbdoc");
    
    m.def("app_is_started", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        int is_true = 0;
        if (neoradio2_app_is_started(&handle, device, bank, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_app_is_started() failed");
        return is_true == 1;
        }, R"pbdoc(
		app_is_started(handle, device, bank)

		Check if the application firmware is started. Chain needs to be identified first.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True/False. True = Application Level, False = Bootloader Level.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.app_is_started(handle, 0, 0)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			True
			>>>
	)pbdoc");
    
    m.def("app_start", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_app_start(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_app_start() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_app_start() failed");
		return true;
        }, R"pbdoc(
		app_start(handle, device, bank)

		Starts the application firmware on the selected devices and banks. Chain needs to be identified first.
		The neoRAD-IO2 device sits in bootloader for up to 10 seconds when first powered up.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Start application firmware on all banks for device 0
			...     neoradio2.app_start(handle, 0, 0xFF) 
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			True
			>>>
	)pbdoc");

    m.def("get_serial_number", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		unsigned int serial_number = 0;
		auto result = neoradio2_get_serial_number(&handle, device, bank, &serial_number);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_get_serial_number() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_serial_number() failed");
		return serial_number;
        }, R"pbdoc(
		get_serial_number(handle, device, bank)

		Gets the serial number (base10) on the selected devices and banks. Chain needs to be identified first.
		The serial number is generally displayed in base36.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Get serial number of Bank 1
			...     neoradio2.app_start(handle, 0, 1) 
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			1106386131
			>>>
	)pbdoc");
    
    m.def("enter_bootloader", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_enter_bootloader(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_enter_bootloader() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_enter_bootloader() failed");
		return true;
        }, R"pbdoc(
		enter_bootloader(handle, device, bank)

		Starts the bootloader firmware on the selected devices and banks. Chain needs to be identified first.
		The neoRAD-IO2 device sits in bootloader for up to 10 seconds when first powered up.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Start application firmware on all banks for device 0
			...     neoradio2.enter_bootloader(handle, 0, 0xFF) 
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			True
			>>>
	)pbdoc");
    
    m.def("get_manufacturer_date", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        int month, day, year;
        if (neoradio2_get_manufacturer_date(&handle, device, bank, &month, &day, &year) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_manufacturer_date() failed");
        return std::make_tuple(month, day, year);
        }, R"pbdoc(
		get_manufacturer_date(handle, device, bank)

		Get the manufacturing date of the selected devices and banks. Chain needs to be identified first.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns a Tuple (Year, Month, Day)
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Get the manufacturing date on bank 1 for device 0
			...     neoradio2.get_manufacturer_date(handle, 0, 0x01)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			(2019, 4, 26)
			>>>
	)pbdoc");
    
    m.def("get_firmware_version", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        int major, minor;
        if (neoradio2_get_firmware_version(&handle, device, bank, &major, &minor) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_firmware_version() failed");
        return std::make_tuple(major, minor);
        }, R"pbdoc(
		get_firmware_version(handle, device, bank)

		Get the firmware version of the selected devices and banks. Chain needs to be identified first.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns a Tuple (Major, Minor)
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Get the manufacturing date on bank 1 for device 0
			...     neoradio2.get_firmware_version(handle, 0, 0x01)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			(2, 26)
			>>>
	)pbdoc");

    m.def("get_hardware_revision", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        int major, minor;
        if (neoradio2_get_hardware_revision(&handle, device, bank, &major, &minor) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_hardware_revision() failed");
        return std::make_tuple(major, minor);
        }, R"pbdoc(
		get_hardware_revision(handle, device, bank)

		Get the hardware revision of the selected devices and banks. Chain needs to be identified first.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns a Tuple (Major, Minor)
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Get the hardware revision on bank 1 for device 0
			...     neoradio2.get_hardware_revision(handle, 0, 0x01)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			(2, 0)
			>>>
	)pbdoc");
    
	m.def("get_device_type", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		unsigned int dev_type;
		if (neoradio2_get_device_type(&handle, device, bank, &dev_type) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_device_type() failed");
		return dev_type;
	}, R"pbdoc(
		get_device_type(handle, device, bank)

		TODO
        
        Returns integer, otherwise exception is thrown.
    )pbdoc");

	m.def("request_pcbsn", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_request_pcbsn(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_request_pcbsn() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_pcbsn() failed");
		return true;
	}, R"pbdoc(
		request_pcbsn(handle, device, bank)

		Request the PCB serial number of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Request/Get the PCB SN on bank 1 for device 0
			...     neoradio2.request_pcb_sn(handle, 0, 0xFF)
			...     neoradio2.get_pcbsn(handle, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			R2TC050031902002
			>>>
	)pbdoc");

    m.def("get_pcbsn", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        char pcb_sn[17] = {0};
        if (neoradio2_get_pcbsn(&handle, device, bank, pcb_sn) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_pcbsn() failed");
        return std::string(pcb_sn);
        }, R"pbdoc(
		get_pcbsn(handle, device, bank)

		Get the PCB serial number of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Request/Get the PCB SN on bank 1 for device 0
			...     neoradio2.request_pcb_sn(handle, 0, 0xFF)
			...     neoradio2.get_pcbsn(handle, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			R2TC050031902002
			>>>
	)pbdoc");

	m.def("request_sensor_data", [](neoradio2_handle& handle, int device, int bank, int enable_cal) {
		py::gil_scoped_release release;
		auto result = neoradio2_request_sensor_data(&handle, device, bank, enable_cal);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_request_sensor_data() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_sensor_data() failed");
		return true;
	}, R"pbdoc(
		request_sensor_data(handle, device, bank, enable_cal)

		Request the sensor data of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			enable_cal (int): Enable reading based on calibration inside the unit.
				CalType.ENABLED: Reads raw sensor value with using calibration values. Use this if unsure.
				CalType.NOCAL: Reads sensor value without calibration applied
				CalType.NOCAL_ENHANCED: Same as CALTYPE_NOCAL but with slower sample rate. Ignores setting poll rate.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns float on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Request/Get the sensor data on bank 8 for device 0
			...     neoradio2.request_sensor_data(handle, 0, 0xFF, neoradio2.CalType.CALTYPE_ENABLED)
			...     neoradio2.read_sensor_float(handle, 0, 7)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			4.953075885772705
			>>>
	)pbdoc");
    
	m.def("read_sensor_float", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		float value = 0;
		if (neoradio2_read_sensor_float(&handle, device, bank, &value) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_sensor_float() failed");
		return value;
	}, R"pbdoc(
		read_sensor_float(handle, device, bank)

		Get the sensor data of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns float on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Request/Get the sensor data on bank 8 for device 0
			...     neoradio2.request_sensor_data(handle, 0, 0xFF, True)
			...     neoradio2.read_sensor_float(handle, 0, 7)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			4.953075885772705
			>>>
	)pbdoc");


	m.def("write_sensor", [](neoradio2_handle& handle, int device, int bank, int mask, int value) {
		py::gil_scoped_release release;
		auto result = neoradio2_write_sensor(&handle, device, bank, mask, value);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_write_sensor() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_sensor() failed");
		return true;
	}, R"pbdoc(
		write_sensor(handle, device, bank, mask, value)

		Get the sensor data of the selected devices and banks. Chain needs to be identified first.
		This is generally used to control Relays on either neoRAD-IO2-PWRRLY or neoRAD-IO2-Badge.
		Must be in application firmware.

		Badge:
			Device 1:
				LED1 = 0x10
				LED2 = 0x20
				LED3 = 0x40
				LED4 = 0x80
				DIO1 = 0x01
				DIO2 = 0x02
				DIO3 = 0x04
				DIO4 = 0x08

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			mask (int): bank of the device to communicate with. This is a bitmask.
			value (int): bank of the device to communicate with. This is a bitmask.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Write the sensor data for device 1 (neoRAD-IO2-Badge)
			...     mask = 0x10 | 0x20 # Which channels do we want to modify
			...     value = 0x10 | 0x20 # Which channels do we want enabled
			...     neoradio2.write_sensor(handle, 1, 0, mask, value)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			>>>
	)pbdoc");

	m.def("write_sensor_successful", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		if (neoradio2_write_sensor_successful(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_sensor_successful() failed");
		return true;
	}, R"pbdoc(
		write_sensor_successful(handle, device, bank)

		Checks to see if write_sensor was successful for non-blocking mode.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
	)pbdoc");
    
	m.def("request_settings", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_request_settings(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_request_settings() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_settings() failed");
		return true;
	}, R"pbdoc(
		request_settings(handle, device, bank)

		Request the settings of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.request_settings(h, 0, 1)
			...     neoradio2.read_settings(h, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			<neoradio2.neoRADIO2_settings object at 0x02C11D40>
			>>>
	)pbdoc");
    
	m.def("read_settings", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		neoRADIO2_settings settings;
		if (neoradio2_read_settings(&handle, device, bank, &settings) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_settings() failed");
		return settings;
	}, R"pbdoc(
		read_settings(handle, device, bank)

		Read the settings of the selected devices and bank. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.request_settings(h, 0, 1)
			...     neoradio2.read_settings(h, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			<neoradio2.neoRADIO2_settings object at 0x02C11D40>
			>>>
	)pbdoc");


	m.def("write_settings", [](neoradio2_handle& handle, int device, int bank, neoRADIO2_settings& settings) {
		py::gil_scoped_release release;
		auto result = neoradio2_write_settings(&handle, device, bank, &settings);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_write_settings() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_settings() failed");
		return true;
	}, R"pbdoc(
		write_settings(handle, device, bank, settings)

		Request the settings of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			settings (neoRADIO2_settings): Settings object.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.request_settings(h, 0, 1)
			...     settings = neoradio2.read_settings(h, 0, 1)
			...     neoradio2.write_settings(h, 0, 1, settings)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			True
			>>>
	)pbdoc");

	m.def("write_default_settings", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_write_default_settings(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_write_default_settings() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_default_settings() failed");
		return true;
	}, R"pbdoc(
		neoradio2_write_default_settings(handle, device, bank)

		Request to load default settings of the selected devices and banks. Chain needs to be identified first.
		Must be in application firmware.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.write_default_settings(h, 0, 1)
			...     neoradio2.request_settings(h, 0, 1)
			...     neoradio2.read_settings(h, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			<neoradio2.neoRADIO2_settings object at 0x02C11D40>
			>>>
	)pbdoc");

	m.def("write_settings_successful", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		if (neoradio2_write_settings_successful(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_settings_successful() failed");
		return true;
	}, R"pbdoc(
		write_settings_successful(handle, device, bank)

		Checks to see if write_settings was successful in non-blocking mode.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
	)pbdoc");

	m.def("get_chain_count", [](neoradio2_handle& handle, bool identify) {
		py::gil_scoped_release release;
		int count = 0;
		auto result = neoradio2_get_chain_count(&handle, &count, identify);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_get_chain_count() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_chain_count() failed");
		return count;
	}, R"pbdoc(
		get_chain_count(handle, identify)

		Get the chain count of the selected devices. Non-blocking mode expects chain to be identified first.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			identify (bool): Identify the chain, if needed. This is ignored in non-blocking mode.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns (int) How many devices are in the chain.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.get_chain_count(handle, True)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			2
			>>>
	)pbdoc");

	m.def("toggle_led", [](neoradio2_handle& handle, int device, int bank, neoRADIO2_LEDMode mode, int led_enables, int ms) {
		py::gil_scoped_release release;
		auto result = neoradio2_toggle_led(&handle, device, bank, mode, led_enables, ms);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_toggle_led() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_toggle_led() failed");
		return true;
	}, R"pbdoc(
		toggle_led(handle, device, bank, ms)

		Toggle the leds on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			mode (LEDMode): Selects the LED mode to send to the LED.
			led_enables (int): LED of the bank to communicate with. This is a bitmask (0b00001001 - 0x09 = LED 1 and 4).
			ms (int): Time in milliseconds to keep the led illuminated for.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.toggle_led(h, 0, 1, 250)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			>>>
	)pbdoc");

	m.def("toggle_led_successful", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		if (neoradio2_toggle_led_successful(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_toggle_led_successful() failed");
		return true;
	}, R"pbdoc(
		toggle_led_successful(handle, device, bank)

		Checks if toggle_led was successful in non-blocking mode.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
	)pbdoc");

	m.def("request_calibration", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
		py::gil_scoped_release release;
		auto result = neoradio2_request_calibration(&handle, device, bank, &header);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_request_calibration() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration() failed");
		return true;
	}, R"pbdoc(
		request_calibration(handle, device, bank, header)

		Requests calibration on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			header (neoRADIO2frame_calHeader): Used to specify which channel and range to request.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     neoradio2.request_calibration(handle, 0, 1, header)
			...     cal_values = neoradio2.read_calibration_array(handle, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			>>>
	)pbdoc");

	m.def("read_calibration_array", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
        py::gil_scoped_release release;
		float arr[64] ={0};
		int arr_size = sizeof(arr);
		if (neoradio2_read_calibration_array(&handle, device, bank, &header, arr, &arr_size) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_calibration_array() failed");
		std::vector<float> values;
		for (int i=0; i < arr_size; ++i)
			values.push_back(arr[i]);
		return values;
	}, R"pbdoc(
		read_calibration_array(handle, device, bank, header)

		Reads calibration on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.
			header (neoRADIO2frame_calHeader): Used to specify which channel and range to request.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns Array of values on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     neoradio2.request_calibration(handle, 0, 1, header)
			...     cal_values = neoradio2.read_calibration_array(handle, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			>>>
	)pbdoc");


	m.def("request_calibration_points", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
		py::gil_scoped_release release;
		auto result = neoradio2_request_calibration_points(&handle, device, bank, &header);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_request_calibration_points() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration_points() failed");
		return true;
	}, R"pbdoc(
		request_calibration_points(handle, device, bank, header)

		Requests calibration points on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			header (neoRADIO2frame_calHeader): Used to specify which channel and range to request.

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns Array of values on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     neoradio2.request_calibration_points(handle, 0, 1, header)
			...     cal_points = neoradio2.read_calibration_points_array(handle, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			>>>
	)pbdoc");

	m.def("read_calibration_points_array", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
        py::gil_scoped_release release;
		float arr[64] ={0};
		int arr_size = sizeof(arr);
		if (neoradio2_read_calibration_points_array(&handle, device, bank, &header, arr, &arr_size) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_calibration_points_array() failed");
		std::vector<float> values;
		for (int i=0; i < arr_size; ++i)
			values.push_back(arr[i]);
		return values;
	}, R"pbdoc(
		read_calibration_points_array(handle, device, bank, header)

		Reads calibration points on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.
			header (neoRADIO2frame_calHeader): Used to specify which channel and range to request.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns Array of values on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     neoradio2.request_calibration_points(handle, 0, 1, header)
			...     cal_points = neoradio2.read_calibration_points_array(handle, 0, 1)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			>>>
	)pbdoc");

	// LIBNEORADIO2_API int neoradio2_write_calibration(neoradio2_handle* handle, int device, int bank, int* arr, int arr_size)
	m.def("write_calibration", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float> data) {
		py::gil_scoped_release release;
		auto result = neoradio2_write_calibration(&handle, device, bank, &header, (float*)data.data(), data.size());
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_write_calibration() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_calibration() failed");
		return true;
	}, R"pbdoc(
		write_calibration(handle, device, bank, header, data)

		Writes calibration on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			header (neoRADIO2frame_calHeader): Used to specify which channel and range to request.
			data (List): Container of calibration values to store in the device.


		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     points = [-50.0, 0.0, 75.0, 650.0]
			...     values = [-49.8, 2.1, 68.0, 590.0]
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     header.num_of_pts = len(points)
			...     neoradio2.write_calibration_points(handle, 0, 1, header, points)
			...     neoradio2.write_calibration(handle, 0, 1, header, values)
			...     neoradio2.store_calibration(handle, 0, 1, header)
			...     neoradio2.is_calibration_stored(handle, 0, 1,)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			True
			True
			>>>
	)pbdoc");

	m.def("write_calibration_successful", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		if (neoradio2_write_calibration_successful(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_calibration_successful() failed");
		return true;
	}, R"pbdoc(
		write_calibration_successful(handle, device, bank)

		Checks to see if write_calibration was successful in non-blocking mode.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).


		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
	)pbdoc");

	m.def("write_calibration_points", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float> data) {
		py::gil_scoped_release release;
		auto result = neoradio2_write_calibration_points(&handle, device, bank, &header, (float*)data.data(), data.size());
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_write_calibration_points() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_calibration_points() failed");
		return true;
	}, R"pbdoc(
		write_calibration_points(handle, device, bank, header, data)

		Writes calibration points on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).
			header (neoRADIO2frame_calHeader): Used to specify which channel and range to request.
			data (List): Container of calibration values to store in the device.


		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     points = [-50.0, 0.0, 75.0, 650.0]
			...     values = [-49.8, 2.1, 68.0, 590.0]
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     header.num_of_pts = len(points)
			...     neoradio2.write_calibration_points(handle, 0, 1, header, points)
			...     neoradio2.write_calibration(handle, 0, 1, header, values)
			...     neoradio2.store_calibration(handle, 0, 1, header)
			...     neoradio2.is_calibration_stored(handle, 0, 1,)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			True
			True
			>>>
	)pbdoc");

	m.def("write_calibration_points_successful", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		if (neoradio2_write_calibration_points_successful(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_calibration_points_successful() failed");
		return true;
	}, R"pbdoc(
		write_calibration_points_successful(handle, device, bank, header, data)

		Checks to see if write_calibration_points was successful in non-blocking mode.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
		
	)pbdoc");

	//LIBNEORADIO2_API int neoradio2_store_calibration(neoradio2_handle* handle, int device, int bank);
	m.def("store_calibration", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_store_calibration(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_store_calibration() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_store_calibration() failed");
		return true;
	}, R"pbdoc(
		store_calibration(handle, device, bank)

		Stores calibration on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     points = [-50.0, 0.0, 75.0, 650.0]
			...     values = [-49.8, 2.1, 68.0, 590.0]
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     header.num_of_pts = len(points)
			...     neoradio2.write_calibration_points(handle, 0, 1, header, points)
			...     neoradio2.write_calibration(handle, 0, 1, header, values)
			...     neoradio2.store_calibration(handle, 0, 1, header)
			...     neoradio2.is_calibration_stored(handle, 0, 1,)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			True
			True
			>>>
	)pbdoc");

	m.def("is_calibration_stored", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		int stored = false;
		if (neoradio2_is_calibration_stored(&handle, device, bank, &stored) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("is_calibration_stored() failed");
		return stored != 0;
	}, R"pbdoc(
		is_calibration_stored(handle, device, bank, header, data)

		Verifies calibration is stored on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     points = [-50.0, 0.0, 75.0, 650.0]
			...     values = [-49.8, 2.1, 68.0, 590.0]
			...     header = neoradio2.neoRADIO2frame_calHeader()
			...     header.channel = 0
			...     header.range = 0
			...     header.num_of_pts = len(points)
			...     neoradio2.write_calibration_points(handle, 0, 1, header, points)
			...     neoradio2.write_calibration(handle, 0, 1, header, values)
			...     neoradio2.store_calibration(handle, 0, 1, header)
			...     neoradio2.is_calibration_stored(handle, 0, 1,)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			True
			True
			>>>
	)pbdoc");

	// LIBNEORADIO2_API int neoradio2_get_calibration_is_valid(neoradio2_handle* handle, int device, int bank, int* is_valid);
	m.def("get_calibration_is_valid", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		int is_valid = 0;
		if (neoradio2_get_calibration_is_valid(&handle, device, bank, &is_valid) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_calibration_is_valid() failed");
		return is_valid != 0;
	}, R"pbdoc(
		get_calibration_is_valid(handle, device, bank)

		Check if calibration is Valid.

		TODO
	)pbdoc");

	m.def("request_calibration_info", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		auto result = neoradio2_request_calibration_info(&handle, device, bank);
		if (!neoradio2_is_blocking() && result == NEORADIO2_ERR_WBLOCK)
			throw NeoRadio2ExceptionWouldBlock("neoradio2_request_calibration_info() would block");
		else if (result != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration_info() failed");
		return true;
	}, R"pbdoc(
		request_calibration_info(handle, device, bank)

		Verifies calibration is stored on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is a bitmask (0b00001001 - 0x09 = Bank 1 and 4).

		Raises:
			neoradio2.Exception on error
			neoradio2.ExceptionWouldBlock on blocking error in non-blocking mode.

		Returns:
			Returns True on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.request_calibration_info(handle, 0, 1)
			...     header = neoradio2.read_calibration_info(handle, 0, 1)
			...     print(header.channel)
			...     print(header.range)
			...     print(header.num_of_pts)
			...     print(header.cal_is_valid)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			0
			0
			4
			1
			>>>
	)pbdoc");

	m.def("read_calibration_info", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		neoRADIO2frame_calHeader header;
		if (neoradio2_read_calibration_info(&handle, device, bank, &header) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_calibration_info() failed");
		return header;
	}, R"pbdoc(
		read_calibration_info(handle, device, bank)

		Verifies calibration is stored on the selected devices and banks.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index and not a bitmask.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns neoRADIO2frame_calHeader on success.
		
		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     neoradio2.request_calibration_info(handle, 0, 1)
			...     header = neoradio2.read_calibration_info(handle, 0, 1)
			...     print(header.channel)
			...     print(header.range)
			...     print(header.num_of_pts)
			...     print(header.cal_is_valid)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			0
			0
			4
			1
			>>>
	)pbdoc");

	// LIBNEORADIO2_API int neoradio2_get_status(neoradio2_handle* handle, int device, int bank, int bitfield, StatusType type, CommandStatus* status)
	m.def("get_status", [](neoradio2_handle& handle, int device, int bank, bool bitfield, StatusType type) {
		py::gil_scoped_release release;
		CommandStatus status;
		if (neoradio2_get_status(&handle, device, bank, bitfield, type, &status) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_status() failed");
		return status;
	}, R"pbdoc(
		get_status(handle, device, bank, bitfield, type)

		Get the status of commands. This is primarly used for checking states for non-blocking mode.

		Args:
			handle (int): handle to the neoRAD-IO2 Device.
			device (int): device number in the chain to communicate with. First device is 0.
			bank (int): bank of the device to communicate with. This is an index or a bitmask depending on bitfield argument.
			bitfield (bool): True = bank is a bitfield, False = bank is an index.
			type (StatusType): Status Type to get.

		Raises:
			neoradio2.Exception on error

		Returns:
			Returns StatusType.

		Example:
			>>> import neoradio2
			>>> devices = neoradio2.find()
			>>> for device in devices:
			...     print(device)
			...     handle = neoradio2.open(device)
			...     # Request/Get the sensor data on bank 8 for device 0
			...     neoradio2.set_blocking(0, 0)
			...     try:
			...          neoradio2.request_sensor_data(handle, 0, 0xFF, 1)
			...     except neoradio2.ExceptionWouldBlock as ex:
			...          pass
			...     while neoradio2.get_status(handle, 0, 0xFF, True, neoradio2.StatusType.StatusSensorRead) != neoradio2.CommandStatus.StatusFinished:
			...          time.sleep(0.001) # Execute other code here
			...     neoradio2.read_sensor_float(handle, 0, 7)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-Badge IG0001'>
			True
			4.953075885772705
			>>>

	)pbdoc");

	m.def("request_statistics", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		if (neoradio2_request_statistics(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_statistics() failed");
		return true;
	}, R"pbdoc(
		TODO
	)pbdoc");

	m.def("read_statistics", [](neoradio2_handle& handle, int device, int bank) {
		py::gil_scoped_release release;
		neoRADIO2_PerfStatistics stats;
		if (neoradio2_read_statistics(&handle, device, bank, &stats) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_statistics() failed");
		return stats;
	}, R"pbdoc(
		TODO
	)pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
