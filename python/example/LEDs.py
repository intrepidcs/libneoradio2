import neoradio2
import time

BANK_LED1 = 0x10
BANK_LED2 = 0x20
BANK_LED3 = 0x40
BANK_LED4 = 0x80

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

    # Turn on all the LEDs
    print("Turning on all LEDs...")
    enable_device_io(device, 
        BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4,
        BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4
        )
    time.sleep(3)
    # Turn off all the LEDs
    print("Turning off all LEDs...")
    enable_device_io(device, 
        BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4,
        0
        )
    time.sleep(3)
    # Turn on all the LEDs again
    print("Turning on all LEDs...")
    enable_device_io(device, 
        BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4,
        BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4
        )
        
    neoradio2.close(device)