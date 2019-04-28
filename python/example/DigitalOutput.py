import neoradio2
import time

BANK_DIO1 = 0x01
BANK_DIO2 = 0x02
BANK_DIO3 = 0x04
BANK_DIO4 = 0x08

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

    # Turn on all the Digital Output Channels
    print("Turning on all Digital OUtput Channels...")
    enable_device_io(device, 
        BANK_DIO1 | BANK_DIO2 | BANK_DIO3 | BANK_DIO4,
        BANK_DIO1 | BANK_DIO2 | BANK_DIO3 | BANK_DIO4
        )
    #Wait
    time.sleep(5)
    # Turn off all the Digital Output Channels
    print("Turning off all Digital OUtput Channels...")
    enable_device_io(device, 
        BANK_DIO1 | BANK_DIO2 | BANK_DIO3 | BANK_DIO4,
        0
        )
    time.sleep(3)
        
    neoradio2.close(device)