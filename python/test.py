import neoradio2
import time

def get_bank_info(handle, device, bank):
    application_level = "Bootloader" if neoradio2.app_is_started(handle, device, bank) else "Application"
    month, day, year = neoradio2.get_manufacturer_date(handle, device, bank)
    fw_major, fw_minor = neoradio2.get_firmware_version(handle, device, bank)
    hw_major, hw_minor = neoradio2.get_hardware_revision(handle, device, bank)
    pcb_sn = neoradio2.get_pcbsn(handle, device, bank)
    print('\tFirmware State: {}'.format(application_level))
    print('\tManufacturer Date: {}/{}/{}'.format(month, day, year))
    print('\tFirmware Version: {}.{}'.format(fw_major, fw_minor))
    print('\tHardware Revision: {}.{}'.format(hw_major, hw_minor))
    print('\tFirmware State: {}'.format(application_level))
    print('\tPCB Serial Number: {}'.format(pcb_sn))
    
    
def get_sensor_info(handle, device, bank):
    value = neoradio2.read_sensor_float(handle, device, bank)
    print('\tSensor Value: {}'.format(value))
    
    
    
if __name__ == "__main__":
    devices = neoradio2.find_devices()
    for device in devices:
        print("Opening {} {}...".format(device.name, device.serial_str))
        handle = neoradio2.open(device)
        print("Opened {} {}.".format(device.name, device.serial_str))
        
        print("Handle: {}".format(handle))
        
        for x in range(8):
            print("Entering Bootloader on bank {}...".format(x+1))
            neoradio2.enter_bootloader(handle, 0, x)
        
        for x in range(8):
            print("Getting Info of bank {}...".format(x+1))
            get_bank_info(handle, 0, x)
        
        for x in range(8):
            print("Entering Application on bank {}...".format(x+1))
            neoradio2.start_app(handle, 0, x)
            
        for x in range(8):
            print("Getting Info of bank {}...".format(x+1))
            get_bank_info(handle, 0, x)
        
        for x in range(8):
            print("Getting Sensor info of bank {}...".format(x+1))
            get_sensor_info(handle, 0, x)
        
        print("Closing {} {}...".format(device.name, device.serial_str))
        neoradio2.close(handle)
        