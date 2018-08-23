import os
try:
    print(os.environ['PYTHONPATH'].split(os.pathsep))
finally:
    pass

try:
    import neoradio2
except Exception as ex:
    input(str(ex))
import time

def get_bank_info(handle, device, bank):
    application_level = "Application" if neoradio2.app_is_started(handle, device, bank) else "Bootloader"
    month, day, year = neoradio2.get_manufacturer_date(handle, device, bank)
    fw_major, fw_minor = neoradio2.get_firmware_version(handle, device, bank)
    hw_major, hw_minor = neoradio2.get_hardware_revision(handle, device, bank)
    try:
        pcb_sn = neoradio2.get_pcbsn(handle, device, bank)
    except neoradio2.Exception as ex:
        pcb_sn = str(ex)
    """
    if "Application" in application_level:
        pcb_sn = neoradio2.get_pcbsn(handle, device, bank)
    else:
        pcb_sn = "NA"
    """
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
    import time
    input("Press any key to start...")
    try:
        devices = neoradio2.find_devices()
        for device in devices:
            print("Opening {} {}...".format(device.name, device.serial_str))
            handle = neoradio2.open(device)
            print("Opened {} {}.".format(device.name, device.serial_str))
        
            print("Handle: {}".format(handle))
            
            #neoradio2.enter_bootloader(handle, 0, 2)
            #time.sleep(30)

            how_many_in_chain = neoradio2.get_chain_count(handle, True);
            print("%d devices in the chain" % how_many_in_chain)
            for d  in range(how_many_in_chain):
                for x in range(8):
                #for x in reversed(range(8)):
                    print("Entering Bootloader on device {} bank {}...".format(d+1, x+1))
                    neoradio2.enter_bootloader(handle, d, x)
                    #time.sleep(0.5)

            for d  in range(how_many_in_chain):
                for x in range(8):
                    print("Getting Info of device {} bank {}...".format(d+1, x+1))
                    get_bank_info(handle, d, x)
        
            for d  in range(how_many_in_chain):
                for x in range(8):
                    print("Entering Application on device {} bank {}...".format(d+1, x+1))
                    neoradio2.app_start(handle, d, x)
            
            for d  in range(how_many_in_chain):
                for x in range(8):
                    print("Getting Info of device {} bank {}...".format(d+1, x+1))
                    get_bank_info(handle, d, x)
        
            for d  in range(how_many_in_chain):
                for x in range(8):
                    print("Getting Sensor info of device {} bank {}...".format(d+1, x+1))
                    get_sensor_info(handle, d, x)
        
            print("Closing {} {}...".format(device.name, device.serial_str))
            neoradio2.close(handle)
    except Exception as ex:
        print("ERROR: ", ex)
    finally:
        input("Press any key to continue...")