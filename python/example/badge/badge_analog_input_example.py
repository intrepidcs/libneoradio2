# pip install neoradio2

import neoradio2
import time

def enable_device_io(device, io_mask, enable_mask=0xFF):
    # The second device bank 1 in the "chain" controls all the IO
    neoradio2.write_sensor(device, 1, 1, io_mask, enable_mask)

if __name__ == "__main__":
    # Find all devices
    devices = neoradio2.find()
    if not len(devices):
        print("Failed to find any neoRAD-IO2 Devices!")
        exit(1)
    
    # Open first device
    print("Opening {} {}...".format(devices[0].name, devices[0].serial_str))
    device = neoradio2.open(devices[0])
    print("Opened {} {}...".format(devices[0].name, devices[0].serial_str))
    
    # Make sure we are in application firmware and chain is identified.
    neoradio2.app_start(device, 0, 0xFF)
    neoradio2.chain_identify(device)
        
    # Read all the AIN values
    neoradio2.request_sensor_data(device, 0x00, 0xFF, True)
    names = ["AIN1", "AIN2", "AIN3", "AIN4", "AIN5", "AIN6", "TEMP", "POT "]
    ain_values = []
    for x in range(8):
        voltage = neoradio2.read_sensor_float(device, 0x00, x)
        ain_values.append(voltage)
        print("{}: {:.2f}V".format(names[x], voltage))
        
    neoradio2.close(device)