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

class NeoRadio2Exception : public std::exception 
{
        using std::exception::exception;
};

PYBIND11_MODULE(neoradio2, m) {
    m.doc() = R"pbdoc(
        neoRADIO2 Python Library
        -----------------------

        .. currentmodule:: neoradio2

        .. autosummary::
           :toctree: _generate
            
            NeoRadio2Exception
            
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
        .def_readwrite("name", &Neoradio2DeviceInfo::name)
        .def_readwrite("serial_str", &Neoradio2DeviceInfo::serial_str)
        .def_readwrite("vendor_id", &Neoradio2DeviceInfo::vendor_id)
        .def_readwrite("product_id", &Neoradio2DeviceInfo::product_id)
        ;//.def_readwrite("_reserved", &Neoradio2DeviceInfo::_reserved);
        
    // radio2_frame.h
	py::class_<neoRADIO2_deviceSettings>(m, "neoRADIO2_deviceSettings")
		.def(py::init([]() { return new neoRADIO2_deviceSettings{0}; }))
		.def_readwrite("sample_rate", &neoRADIO2_deviceSettings::sample_rate)
		.def_readwrite("channel_1_config", &neoRADIO2_deviceSettings::channel_1_config)
		.def_readwrite("channel_2_Config", &neoRADIO2_deviceSettings::channel_2_Config)
		.def_readwrite("channel_3_Config", &neoRADIO2_deviceSettings::channel_3_Config);

	py::class_<neoRADIO2frame_calHeader>(m, "neoRADIO2frame_calHeader")
		.def(py::init([]() { return new neoRADIO2frame_calHeader{0}; }))
		.def_readwrite("num_of_pts", &neoRADIO2frame_calHeader::num_of_pts)
		.def_readwrite("channel", &neoRADIO2frame_calHeader::channel)
		.def_readwrite("range", &neoRADIO2frame_calHeader::range)
		.def_readwrite("cal_type_size", &neoRADIO2frame_calHeader::cal_type_size)
		.def_readwrite("cr_is_bitmask", &neoRADIO2frame_calHeader::cr_is_bitmask)
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
        Find neoRAD-IO2 Devices
        
        Returns a tuple Neoradio2DeviceInfo
    )pbdoc");
    
    m.def("open", [](Neoradio2DeviceInfo* device) {
        neoradio2_handle handle;
        if (neoradio2_open(&handle, device) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_open() failed");
        return handle;
        }, R"pbdoc(
        Open neoRAD-IO2 Devices
        
        Returns a handle to the device.
    )pbdoc");
    
    m.def("is_blocking", []() {
        return neoradio2_is_blocking() == 1;
        }, R"pbdoc(
        Check if API is blocking
        
        Returns True/False.
    )pbdoc");
    
    m.def("is_opened", [](neoradio2_handle& handle) {
        int is_true = 0;
        if (neoradio2_is_opened(&handle, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_is_opened() failed");
        return is_true == 1;
        }, R"pbdoc(
        Check if handle/device is open.
        
        Returns True/False.
    )pbdoc");
    
    m.def("close", [](neoradio2_handle& handle) {
        if (neoradio2_close(&handle) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_close() failed");
        return true;
        }, R"pbdoc(
        Open neoRAD-IO2 Devices
        
        Returns a true on success, otherwise exception is thrown.
    )pbdoc");

    m.def("is_closed", [](neoradio2_handle& handle) {
        int is_true = 0;
        if (neoradio2_is_closed(&handle, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_is_closed() failed");
        return is_true == 1;
        }, R"pbdoc(
        Check if handle/device is closed.
        
        Returns True/False.
    )pbdoc");
    
    m.def("chain_is_identified", [](neoradio2_handle& handle) {
        int is_true = 0;
        if (neoradio2_chain_is_identified(&handle, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_chain_is_identified() failed");
        return is_true == 1;
        }, R"pbdoc(
        Check if the chain is identified.
        
        Returns True/False.
    )pbdoc");
    
    m.def("chain_identify", [](neoradio2_handle& handle) {
        if (neoradio2_chain_identify(&handle) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_chain_identify() failed");
        return true;
        }, R"pbdoc(
        Identify the chain of neoRAD-IO2 devices.
        
        Returns a true on success, otherwise exception is thrown.
    )pbdoc");
    
    
    m.def("app_is_started", [](neoradio2_handle& handle, int device, int bank) {
        int is_true = 0;
        if (neoradio2_app_is_started(&handle, device, bank, &is_true) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_app_is_started() failed");
        return is_true == 1;
        }, R"pbdoc(
        Check if the application firmware is started.
        
        Returns True/False. True = Application Level, False = Bootloader Level.
    )pbdoc");
    
    m.def("app_start", [](neoradio2_handle& handle, int device, int bank) {
        if (neoradio2_app_start(&handle, device, bank) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_app_start() failed");
        return true;
        }, R"pbdoc(
        Identify the chain of neoRAD-IO2 devices.
        
        Returns a true on success, otherwise exception is thrown.
    )pbdoc");

    m.def("get_serial_number", [](neoradio2_handle& handle, int device, int bank) {
        unsigned int serial_number = 0;
        if (neoradio2_get_serial_number(&handle, device, bank, &serial_number) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_serial_number() failed");
        return serial_number;
        }, R"pbdoc(
        Get the serial Number of the neoRAD-IO2 device bank.
        
        Returns a true on success, otherwise exception is thrown.
    )pbdoc");
    
    m.def("enter_bootloader", [](neoradio2_handle& handle, int device, int bank) {
        if (neoradio2_enter_bootloader(&handle, device, bank) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_enter_bootloader() failed");
        return true;
        }, R"pbdoc(
        Enter bootloader of the neoRAD-IO2 device bank.
        
        Returns a true on success, otherwise exception is thrown.
    )pbdoc");
    
    m.def("get_manufacturer_date", [](neoradio2_handle& handle, int device, int bank) {
        int month, day, year;
        if (neoradio2_get_manufacturer_date(&handle, device, bank, &month, &day, &year) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_manufacturer_date() failed");
        return std::make_tuple(month, day, year);
        }, R"pbdoc(
        Get Manufacturer Date of the neoRAD-IO2 device bank.
        
        Returns tuple(month, day, year) on success, otherwise exception is thrown.
    )pbdoc");
    
    m.def("get_firmware_version", [](neoradio2_handle& handle, int device, int bank) {
        int major, minor;
        if (neoradio2_get_firmware_version(&handle, device, bank, &major, &minor) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_firmware_version() failed");
        return std::make_tuple(major, minor);
        }, R"pbdoc(
        Get Firmware Version of the neoRAD-IO2 device bank.
        
        Returns tuple(major, minor) on success, otherwise exception is thrown.
    )pbdoc");

    m.def("get_hardware_revision", [](neoradio2_handle& handle, int device, int bank) {
        int major, minor;
        if (neoradio2_get_hardware_revision(&handle, device, bank, &major, &minor) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_hardware_revision() failed");
        return std::make_tuple(major, minor);
        }, R"pbdoc(
        Get Hardware Revision of the neoRAD-IO2 device bank.
        
        Returns tuple(major, minor) on success, otherwise exception is thrown.
    )pbdoc");
    
	m.def("get_device_type", [](neoradio2_handle& handle, int device, int bank) {
		unsigned int dev_type;
		if (neoradio2_get_device_type(&handle, device, bank, &dev_type) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_device_type() failed");
		return dev_type;
	}, R"pbdoc(
        Get Device Type of the neoRAD-IO2 device bank.
        
        Returns integer, otherwise exception is thrown.
    )pbdoc");

	m.def("request_pcbsn", [](neoradio2_handle& handle, int device, int bank) {
		if (neoradio2_request_pcbsn(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_pcbsn() failed");
		return true;
	}, R"pbdoc(
        Request PCBSN of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

    m.def("get_pcbsn", [](neoradio2_handle& handle, int device, int bank) {
        char pcb_sn[17] = {0};
        if (neoradio2_get_pcbsn(&handle, device, bank, pcb_sn) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_get_pcbsn() failed");
        return std::string(pcb_sn);
        }, R"pbdoc(
        Get PCB Serial Number of the neoRAD-IO2 device bank.
        
        Returns string, otherwise exception is thrown.
    )pbdoc");

	m.def("request_sensor_data", [](neoradio2_handle& handle, int device, int bank, int enable_cal) {
		if (neoradio2_request_sensor_data(&handle, device, bank, enable_cal) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_sensor_data() failed");
		return true;
	}, R"pbdoc(
        Request Sensor data of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");
    
    m.def("read_sensor_float", [](neoradio2_handle& handle, int device, int bank) {
        float value = 0;
        if (neoradio2_read_sensor_float(&handle, device, bank, &value) != NEORADIO2_SUCCESS)
            throw NeoRadio2Exception("neoradio2_read_sensor_float() failed");
        return value;
        }, R"pbdoc(
        Get Sensor value of the neoRAD-IO2 device bank.
        
        Returns float, otherwise exception is thrown.
    )pbdoc");
    
    m.def("read_sensor_array", [](neoradio2_handle& handle, int device, int bank) {
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
        
        Returns tuple of integers, otherwise exception is thrown.
    )pbdoc");
    
	m.def("request_settings", [](neoradio2_handle& handle, int device, int bank) {
		if (neoradio2_request_settings(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_settings() failed");
		return true;
	}, R"pbdoc(
        Request Settings of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");
    
	m.def("read_settings", [](neoradio2_handle& handle, int device, int bank) {
		neoRADIO2_deviceSettings settings;
		if (neoradio2_read_settings(&handle, device, bank, &settings) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_settings() failed");
		return settings;
	}, R"pbdoc(
        Get settings of the neoRAD-IO2 device bank.
        
        Returns neoRADIO2_deviceSettings, otherwise exception is thrown.
    )pbdoc");

	m.def("write_settings", &neoradio2_write_settings, R"pbdoc(
        TODO
        
        TODO
    )pbdoc");

	m.def("get_chain_count", [](neoradio2_handle& handle, bool identify) {
		int count = 0;
		if (neoradio2_get_chain_count(&handle, &count, identify) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_get_chain_count() failed");
		return count;
	}, R"pbdoc(
        Gets how many devices are in the chain.
        
        Returns integer, otherwise exception is thrown.
    )pbdoc");

	m.def("toggle_led", [](neoradio2_handle& handle, int device, int bank, int ms) {
		if (neoradio2_toggle_led(&handle, device, bank, ms) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_toggle_led() failed");
		return true;
	}, R"pbdoc(
        Gets how many devices are in the chain.
        
        Returns integer, otherwise exception is thrown.
    )pbdoc");


	m.def("request_calibration", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header) {
		if (neoradio2_request_calibration(&handle, device, bank, &header) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration() failed");
		return true;
	}, R"pbdoc(
        Request Calibration values of the neoRAD-IO2 device bank.
        
        Returns True, otherwise exception is thrown.
    )pbdoc");

	m.def("read_calibration_array", [](neoradio2_handle& handle, int device, int bank) {
		int arr[64] = {0};
		int arr_size = sizeof(arr);
		if (neoradio2_read_calibration_array(&handle, device, bank, arr, &arr_size) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_calibration_array() failed");
		std::vector<int> values;
		for (int i=0; i < arr_size; ++i)
			values.push_back(arr[i]);
		return values;
	}, R"pbdoc(
        Get calibration values of the neoRAD-IO2 device bank.
        
        Returns container of calibration values, otherwise exception is thrown.
    )pbdoc");

	// LIBNEORADIO2_API int neoradio2_write_calibration(neoradio2_handle* handle, int device, int bank, int* arr, int* arr_size)
	m.def("write_calibration", [](neoradio2_handle& handle, int device, int bank, neoRADIO2frame_calHeader& header, std::vector<float> data) {
		int arr[64] ={0};
		int arr_size = sizeof(arr);
		if (neoradio2_write_calibration(&handle, device, bank, &header, arr, &arr_size) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_read_calibration_array() failed");
		std::vector<int> values;
		for (int i=0; i < arr_size; ++i)
			values.push_back(arr[i]);
		return values;
	}, R"pbdoc(
        Get calibration values of the neoRAD-IO2 device bank.
        
        Returns container of calibration values, otherwise exception is thrown.
    )pbdoc");

	m.def("request_calibration_info", [](neoradio2_handle& handle, int device, int bank) {
		if (neoradio2_request_calibration_info(&handle, device, bank) != NEORADIO2_SUCCESS)
			throw NeoRadio2Exception("neoradio2_request_calibration_info() failed");
		return true;
	}, R"pbdoc(
        Request calibration info of the neoRAD-IO2 device bank.
        
        Returns True on success, otherwise exception is thrown.
    )pbdoc");

	m.def("read_calibration_info", [](neoradio2_handle& handle, int device, int bank) {
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
