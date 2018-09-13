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


if __name__ == "__main__":
    for device in neoradio2.find():
        print("Opening {} {}...".format(device.name, device.serial_str))
        handle = neoradio2.open(device)
        print("Opened {} {}...".format(device.name, device.serial_str))

        neoradio2.app_start(handle, 0, 0xFF)

        try:
            while True:
                s = time.time()
                neoradio2.request_calibration_info(handle, 0, 1)
                e = time.time()
                msg = str(e-s)
                for x in range(8):
                    cal_info = neoradio2.read_calibration_info(handle, 0, x)
                    print("num_of_pts:    {}".format(cal_info.num_of_pts))
                    print("channel:       {}".format(cal_info.channel))
                    print("range:         {}".format(cal_info.range))
                    print("cal_type_size: {}".format(cal_info.cal_type_size))
                    print("cr_is_bitmask: {}".format(cal_info.cr_is_bitmask))
                    print("cal_is_valid:  {}".format(cal_info.cal_is_valid))
                time.sleep(0.1)
        except Exception as ex:
            print(ex)
            time.sleep(1)
        finally:
            neoradio2.close(handle)
            input("Press any key to continue...")
            #time.sleep(3)
        #time.sleep(10)

"""
if __name__ == "__main__":
    for device in neoradio2.find():
        print("Opening {} {}...".format(device.name, device.serial_str))
        handle = neoradio2.open(device)
        print("Opened {} {}...".format(device.name, device.serial_str))

        neoradio2.app_start(handle, 0, 0xFF)

        try:
            while True:
                s = time.time()
                neoradio2.request_calibration(handle, 0, 0xFF)
                e = time.time()
                msg = str(e-s)
                for x in range(8):
                    value = neoradio2.read_calibration_array(handle, 0, x)
                    #try:
                    #    neoradio2.toggle_led(handle, 0, 0xFF, 255)
                    #except neoradio2.Exception as ex:
                    #    print(ex)
                    #value = neoradio2.get_manufacturer_date(handle, 0, x)
                    msg += ", {}".format(value)
                print(msg)
                time.sleep(0.1)
        except Exception as ex:
            print(ex)
            time.sleep(1)
        finally:
            neoradio2.close(handle)
            input("Press any key to continue...")
            #time.sleep(3)
        #time.sleep(10)
"""
"""
if __name__ == "__main__":
    for device in neoradio2.find():
        print("Opening {} {}...".format(device.name, device.serial_str))
        handle = neoradio2.open(device)
        print("Opened {} {}...".format(device.name, device.serial_str))

        neoradio2.app_start(handle, 0, 0xFF)

        try:
            while True:
                s = time.time()
                neoradio2.request_sensor_data(handle, 1, 0xFF)
                e = time.time()
                msg = str(e-s)
                for x in range(8):
                    value = neoradio2.read_sensor_float(handle, 1, x)
                    #try:
                    #    neoradio2.toggle_led(handle, 0, 0xFF, 255)
                    #except neoradio2.Exception as ex:
                    #    print(ex)
                    #value = neoradio2.get_manufacturer_date(handle, 0, x)
                    msg += ", {}".format(value)
                print(msg)
                time.sleep(0.1)
        except Exception as ex:
            print(ex)
            time.sleep(3)
        finally:
            neoradio2.close(handle)
            input("Press any key to continue...")
            time.sleep(3)
        time.sleep(10)
"""

"""
def get_bank_info(handle, device, bank):
    application_level = "Application" if neoradio2.app_is_started(handle, device, bank) else "Bootloader"
    month, day, year = neoradio2.get_manufacturer_date(handle, device, bank)
    fw_major, fw_minor = neoradio2.get_firmware_version(handle, device, bank)
    hw_major, hw_minor = neoradio2.get_hardware_revision(handle, device, bank)
    try:
        pcb_sn = neoradio2.get_pcbsn(handle, device, bank)
    except neoradio2.Exception as ex:
        pcb_sn = str(ex)

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
        devices = neoradio2.find()
        for device in devices:
            print("Opening {} {}...".format(device.name, device.serial_str))
            handle = neoradio2.open(device)
            print("Opened {} {}.".format(device.name, device.serial_str))
        
            print("Handle: {}".format(handle))
            
            #neoradio2.enter_bootloader(handle, 0, 2)
            #time.sleep(30)

            how_many_in_chain = neoradio2.get_chain_count(handle, True);
            print("%d devices in the chain" % how_many_in_chain)
            for d in range(how_many_in_chain):
                    print("Entering Bootloader on device {}...".format(d+1))
                    neoradio2.enter_bootloader(handle, d, 0xFF)
                    #time.sleep(0.5)

            for d in range(how_many_in_chain):
                for x in range(8):
                    print("Getting Info of device {} bank {}...".format(d+1, x+1))
                    get_bank_info(handle, d, x)
        
            for d in range(how_many_in_chain):
                    print("Entering Application on device {}...".format(d+1))
                    neoradio2.app_start(handle, d, 0xFF)
            
            for d in range(how_many_in_chain):
                neoradio2.request_pcbsn(handle, d, 0xFF)
                time.sleep(0.5)
                for x in range(8):
                    
                    print("Getting Info of device {} bank {}...".format(d+1, x+1))
                    get_bank_info(handle, d, x)
        
            
                neoradio2.request_sensor_data(handle, d, 0xFF)
                time.sleep(0.5)
                for x in range(8):
                    print("Getting Sensor info of device {} bank {}...".format(d+1, x+1))
                    get_sensor_info(handle, d, x)

            for d in range(how_many_in_chain):
                print("Toggling LEDs on device {}...".format(d+1))
                for x in range(50):
                    neoradio2.toggle_led(handle, d, 0xFF, 50)
                    time.sleep(0.1)
        
            print("Closing {} {}...".format(device.name, device.serial_str))
            neoradio2.close(handle)
    except Exception as ex:
        print("ERROR: ", ex)
    finally:
        input("Press any key to continue...")

"""