import neoradio2
import time

spinner = ['|', '/', '-', '|', '\\']
spinner_index = 0
def animate_spinner(status_msg):
    global spinner_index
    print("{} {}".format(status_msg, spinner[spinner_index]), end='\r')
    spinner_index += 1
    if spinner_index >= len(spinner):
        spinner_index = 0

def animate_spinner_stop():
    global spinner_index
    spinner_index = 0
    print("")
        
def elapsed_timeout(start, function_name):
    if time.time() - start >= 5.0:
        raise Exception("Timeout hit at {}!".format(function_name))

if __name__ == "__main__":
    try:
        # Set the API to non-blocking mode
        neoradio2.set_blocking(0, 0)
        
        # Find the first device to open
        devices = neoradio2.find()
        if not len(devices):
            print("Failed to find any neoRAD-IO2 Devices!")
            exit(1)
        
        # Open the device
        handle = None
        try:
            print("Opening {}".format(devices[0]))
            handle = neoradio2.open(devices[0])
            print("Opened device!")
        except neoradio2.ExceptionWouldBlock as ex:
            print(handle)
            if not handle:
                raise Exception("Failed to create a handle for {}".format(devices[0]))
        print(handle)
        
        # Waiting to open the device, insert other code here
        start = time.time()
        while not neoradio2.is_opened(handle):
            elapsed_timeout(start, "is_opened")
            animate_spinner("Waiting for device to open...")
            time.sleep(0.05)
        animate_spinner_stop()
        
        # Identify the chain
        try:
            neoradio2.chain_identify(handle)
        except neoradio2.ExceptionWouldBlock as ex:
            pass
        
        # Waiting to identify the chain, insert other code here
        start = time.time()
        while neoradio2.get_status(handle, 0, 0xFF, True, neoradio2.StatusType.StatusChain) != neoradio2.CommandStatus.StatusFinished:
            elapsed_timeout(start, "chain_is_identified")
            animate_spinner("Waiting for chain identification...")
            time.sleep(0.05)
        animate_spinner_stop()
        
        
        # Start the application code
        try:
            neoradio2.app_start(handle, 0, 0xFF)
        except neoradio2.ExceptionWouldBlock as ex:
            pass
        
        # Waiting to start the application firmware, insert other code here
        start = time.time()
        while neoradio2.get_status(handle, 0, 0xFF, True, neoradio2.StatusType.StatusAppStart) != neoradio2.CommandStatus.StatusFinished:
            elapsed_timeout(start, "app_is_started")
            animate_spinner("Waiting for app start...")
            time.sleep(0.05)
        animate_spinner_stop()
        
        
        # Read the sensors
        try:
            neoradio2.request_sensor_data(handle, 0, 0xFF, 1)
        except neoradio2.ExceptionWouldBlock as ex:
            pass
        
        #time.sleep(10)
        # Waiting to identify the chain, insert other code here
        start = time.time()
        while neoradio2.get_status(handle, 0, 0xFF, True, neoradio2.StatusType.StatusSensorRead) != neoradio2.CommandStatus.StatusFinished:
            animate_spinner("Waiting for sensors request...")
            time.sleep(0.05)
        animate_spinner_stop()
        
        for x in range(8):
            print("Sensor {}: {:.4}V".format(x+1, neoradio2.read_sensor_float(handle, 0, x)))

        # Close the device
        try:
            neoradio2.close(handle)
            print("Closed device...")
        except neoradio2.ExceptionWouldBlock as ex:
            pass
            
        # Waiting to close the device, insert other code here
        start = time.time()
        while not neoradio2.is_closed(handle):
            elapsed_timeout(start, "is_closed")
            animate_spinner("Waiting for device to close...")
            time.sleep(0.05)
        animate_spinner_stop()
    except Exception as ex:
        print(ex)
        input("Press any key to continue...")
    finally:
        input("Press any key to continue...")
    
