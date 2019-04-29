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

PYBIND11_MODULE(neoradio2, m) {
	py::options options;
	options.disable_function_signatures();
	options.enable_user_defined_docstrings();

    m.doc() = R"pbdoc(
        neoRADIO2 Python Library
        -----------------------

        .. currentmodule:: neoradio2

        .. autosummary::
           :toctree: _generate
            

			Exception
			Neoradio2DeviceInfo
			neoRADIO2_deviceSettings
			neoRADIO2settings_CAN
			neoRADIO2Settings_ChannelName
			neoRADIO2_settings
			neoRADIO2frame_calHeader

            find
            is_blocking
            open
            is_opened
            close
            is_closed
            chain_is_identified
            chain_identify
            app_is_started
            app_start
            enter_bootloader
            get_serial_number
            get_manufacturer_date
            get_firmware_version
            get_hardware_revision
            get_device_type
            get_pcbsn
            read_sensor_float
            read_sensor_array
            request_settings
            read_settings
            write_settings
            get_chain_count
            toggle_led
    )pbdoc";
    
    py::register_exception<NeoRadio2Exception>(m, "Exception");
    
    // libneoradio2.h
    py::class_<Neoradio2DeviceInfo>(m, "Neoradio2DeviceInfo")
        .def(py::init([]() { return new Neoradio2DeviceInfo{0}; }))
		.def("__repr__", [](const Neoradio2DeviceInfo& self) {
			std::stringstream ss;
			ss << "<neoradio2.Neoradio2DeviceInfo '" << self.name << " " << self.serial_str << "'>";
			return ss.str();
		})
        .def_readwrite("name", &Neoradio2DeviceInfo::name)
        .def_readwrite("serial_str", &Neoradio2DeviceInfo::serial_str)
        .def_readwrite("vendor_id", &Neoradio2DeviceInfo::vendor_id)
        .def_readwrite("product_id", &Neoradio2DeviceInfo::product_id)
        ;//.def_readwrite("_reserved", &Neoradio2DeviceInfo::_reserved);
        
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
        if (neoradio2_open(&handle, device) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_open() failed");
        return handle;
        }, R"pbdoc(
		open(device)

		Open a neoRAD-IO2 Device

		Args:
			device (neoradio2.Neoradio2DeviceInfo): specified device to open, typically from neoradio2.find()

		Raises:
			neoradio2.Exception on error

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
        Check if API is blocking
        
        Returns True/False.
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
        if (neoradio2_close(&handle) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_close() failed");
        return true;
        }, R"pbdoc(
		close(handle)

		Closes the handle for the device.

		Raises:
			neoradio2.Exception on error

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
        if (neoradio2_chain_identify(&handle) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_chain_identify() failed");
        return true;
        }, R"pbdoc(
		chain_identify(handle)

		Identifies the chain. Chain needs to be identified before other commands will be successful. Each bank needs to be identified by the host before it can start receiving commands.

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
			...     neoradio2.app_is_started(handle)
			...     neoradio2.close(handle)
			...
			<neoradio2.Neoradio2DeviceInfo 'neoRAD-IO2-TC IAPP03'>
			True
			>>>
	)pbdoc");
    
    m.def("app_start", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        if (neoradio2_app_start(&handle, device, bank) != NEORADIO2_SUCCESS)
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
        if (neoradio2_get_serial_number(&handle, device, bank, &serial_number) != NEORADIO2_SUCCESS)
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
        if (neoradio2_enter_bootloader(&handle, device, bank) != NEORADIO2_SUCCESS)
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
		if (neoradio2_request_pcbsn(&handle, device, bank) != NEORADIO2_SUCCESS)
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
		if (neoradio2_request_sensor_data(&handle, device, bank, enable_cal) != NEORADIO2_SUCCESS)
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
			enable_cal (int): Enable reading based on calibration inside the unit. 0 = raw, 1 = use calibration.

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
		if (neoradio2_write_sensor(&handle, device, bank, mask, value) != NEORADIO2_SUCCESS)
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

    m.def("read_sensor_array", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
        int arr[32] = {0};
        int arr_size = 32;
        if (neoradio2_read_sensor_array(&handle, device, bank, arr, &arr_size) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_read_sensor_array() failed");
        std::vector<int> values;
        for (int i=0; i < arr_size; ++i)
            values.push_back(arr[i]);
        return values;
        }, R"pbdoc(
        Get Sensor value of the neoRAD-IO2 device bank.

		TODO
        
        Returns tuple of integers, otherwise exception is thrown.
    )pbdoc");
    
	m.def("request_settings", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		if (neoradio2_request_settings(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_settings() failed");
		return true;
	}, R"pbdoc(
		request_settings(handle, device, bank)

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
    
	m.def("read_settings", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		neoRADIO2_settings settings;
		if (neoradio2_read_settings(&handle, device, bank, &settings) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_settings() failed");
		return settings;
	}, R"pbdoc(
        Read settings of the neoRAD-IO2 device bank.
        
        Returns neoRADIO2_settings, otherwise exception is thrown.
    )pbdoc");


	m.def("write_settings", [](neoradio2_handle& handle, int device, int bank, neoRADIO2_settings& settings) {
		py::gil_scoped_release release;
		if (neoradio2_write_settings(&handle, device, bank, &settings) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_settings() failed");
		return true;
	}, R"pbdoc(
        Write settings of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

	m.def("get_chain_count", [](neoradio2_handle& handle, bool identify) {
        py::gil_scoped_release release;
		int count = 0;
		if (neoradio2_get_chain_count(&handle, &count, identify) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_chain_count() failed");
		return count;
	}, R"pbdoc(
        Gets how many devices are in the chain.
        
        Returns integer, otherwise exception is thrown.
    )pbdoc");

	m.def("toggle_led", [](neoradio2_handle& handle, int device, int bank, int ms) {
        py::gil_scoped_release release;
		if (neoradio2_toggle_led(&handle, device, bank, ms) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_toggle_led() failed");
		return true;
	}, R"pbdoc(
        Gets how many devices are in the chain.
        
        Returns integer, otherwise exception is thrown.
    )pbdoc");


	m.def("request_calibration", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
        py::gil_scoped_release release;
		if (neoradio2_request_calibration(&handle, device, bank, &header) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration() failed");
		return true;
	}, R"pbdoc(
        Request Calibration values of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
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
        Get calibration values of the neoRAD-IO2 device bank.
        
        Returns container of calibration values, otherwise exception is thrown.
    )pbdoc");


	m.def("request_calibration_points", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
        py::gil_scoped_release release;
		if (neoradio2_request_calibration_points(&handle, device, bank, &header) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration_points() failed");
		return true;
	}, R"pbdoc(
        Request Calibration point values of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
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
        Get calibration point values of the neoRAD-IO2 device bank.
        
        Returns container of calibration values, otherwise exception is thrown.
    )pbdoc");

	// LIBNEORADIO2_API int neoradio2_write_calibration(neoradio2_handle* handle, int device, int bank, int* arr, int arr_size)
	m.def("write_calibration", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float> data) {
        py::gil_scoped_release release;
		if (neoradio2_write_calibration(&handle, device, bank, &header, (float*)data.data(), data.size()) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_calibration() failed");
		return true;
	}, R"pbdoc(
        Write calibration values for the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

	m.def("write_calibration_points", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float> data) {
        py::gil_scoped_release release;
		if (neoradio2_write_calibration_points(&handle, device, bank, &header, (float*)data.data(), data.size()) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_write_calibration_points() failed");
		return true;
	}, R"pbdoc(
        Write calibration points for the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

	//LIBNEORADIO2_API int neoradio2_store_calibration(neoradio2_handle* handle, int device, int bank);
	m.def("store_calibration", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		if (neoradio2_store_calibration(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_store_calibration() failed");
		return true;
	}, R"pbdoc(
        Store the calibration for the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

	m.def("is_calibration_stored", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		int stored = false;
		if (neoradio2_is_calibration_stored(&handle, device, bank, &stored) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("is_calibration_stored() failed");
		return stored != 0;
	}, R"pbdoc(
        Check if calibration is stored for the neoRAD-IO2 device bank. Call store_calibration first.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

	// LIBNEORADIO2_API int neoradio2_get_calibration_is_valid(neoradio2_handle* handle, int device, int bank, int* is_valid);
	m.def("get_calibration_is_valid", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		int is_valid = 0;
		if (neoradio2_get_calibration_is_valid(&handle, device, bank, &is_valid) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_calibration_is_valid() failed");
		return is_valid != 0;
	}, R"pbdoc(
        Determine if the calibration is valid for the neoRAD-IO2 device bank.
        
        Returns True is valid, False if invalid. Exception is thrown on error.
    )pbdoc");

	m.def("request_calibration_info", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		if (neoradio2_request_calibration_info(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration_info() failed");
		return true;
	}, R"pbdoc(
        Request calibration info of the neoRAD-IO2 device bank.
        
        Returns True on success, otherwise exception is thrown.
    )pbdoc");

	m.def("read_calibration_info", [](neoradio2_handle& handle, int device, int bank) {
        py::gil_scoped_release release;
		neoRADIO2frame_calHeader header;
		if (neoradio2_read_calibration_info(&handle, device, bank, &header) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_calibration_info() failed");
		return header;
	}, R"pbdoc(
        Reads calibration info of the neoRAD-IO2 device bank.
        
        Returns neoRADIO2frame_calHeader on success, otherwise exception is thrown.
    )pbdoc");

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}
