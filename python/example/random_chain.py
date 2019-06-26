

import neoradio2

def print_status(device, status_msg):
    print(status_msg, "on {} {}".format(device.name, device.serial_str))

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
                print_status(device, "\t{} Device type: {}".format(d+1, dev_type))
            
            print_status(device, "Closing")
            neoradio2.close(h)
    except Exception as ex:
        print(ex)
    finally:
        input("Press any key to continue...")
    