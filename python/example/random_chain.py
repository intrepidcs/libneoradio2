import neoradio2

def print_status(device, status_msg):
    print(status_msg, "on {} {}".format(device.name, device.serial_str))
    
def device_type_string(value):
    values = {
        neoradio2.DeviceTypes.TC: "neoRAD-IO-2 TC",
        neoradio2.DeviceTypes.DIO: "neoRAD-IO-2 DIO",
        neoradio2.DeviceTypes.PWRRLY: "neoRAD-IO-2 PWRRLY",
        neoradio2.DeviceTypes.AIN: "neoRAD-IO-2 AIN",
        neoradio2.DeviceTypes.AOUT: "neoRAD-IO-2 AOUT",
        neoradio2.DeviceTypes.CANHUB: "neoRAD-IO-2 CANHUB",
        neoradio2.DeviceTypes.BADGE: "neoRAD-IO-2 BADGE",
        neoradio2.DeviceTypes.HOST: "neoRAD-IO-2 HOST",
        }
    if value in values:
        return values[value]
    else:
        return "Unknown ({})".format(value)
        
def base36encode(number, alphabet='0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ'):
    """Converts an integer to a base36 string."""
    """https://stackoverflow.com/questions/1181919/python-base-36-encoding"""
    if not isinstance(number, (int)):
        raise TypeError('number must be an integer')

    base36 = ''
    sign = ''

    if number < 0:
        sign = '-'
        number = -number

    if 0 <= number < len(alphabet):
        return sign + alphabet[number]

    while number != 0:
        number, i = divmod(number, len(alphabet))
        base36 = alphabet[i] + base36

    return sign + base36
        

if __name__ == "__main__":
    try:
        devices = neoradio2.find()
        # List all the devices connected to the computer
        print("Found {} Device(s)".format(len(devices)))
        for device in devices:
            print("\t1.) {} {}".format(device.name, device.serial_str))
        print()
        
        # Connect and identify all the chains
        for device in devices:
            print_status(device, "Connecting")
            h = neoradio2.open(device)
            print_status(device, "Connected")
            
            print_status(device, "Identifying Chain")
            neoradio2.chain_identify(h)
            
            print_status(device, "Starting App")
            neoradio2.app_start(h, 0xFF, 0xFF)
            
            print_status(device, "Identifying Chain")
            neoradio2.chain_identify(h)
            
            count = neoradio2.get_chain_count(h, False)
            print_status(device, "Found {} device(s) in the chain".format(count))
            
            for d in range(count):
                dev_type = neoradio2.get_device_type(h, d, 0)
                serial_number = neoradio2.get_serial_number(h, d, 0)
                print("\tChain {} {} {}".format(d+1, device_type_string(dev_type), base36encode(serial_number)))
            
            print_status(device, "Closing")
            neoradio2.close(h)
    except Exception as ex:
        print(ex)
    finally:
        input("Press any key to continue...")
    