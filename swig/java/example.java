public class example {
    public static void main(String argv[]) {
        // Load the library
        System.loadLibrary("neoradio2");
        System.out.println(new neoradio2());
        // Find neoRAD-IO-2 Devices
        neoradio2.Neoradio2DeviceInfo[] devices = new neoradio2.Neoradio2DeviceInfo[neoradio2.NEORADIO2_MAX_DEVS];
        int deviceCount = neoradio2.NEORADIO2_MAX_DEVS;
        if (neoradio2.neoradio2_find(devices, deviceCount) != neoradio2.NEORADIO2_SUCCESS) {
            System.out.println("neoradio2.neoradio2_find() failed!");
            return;
        }
        System.out.println("Found %d device(s)", deviceCount);
        // Open the devices
        for (int i=0; i < deviceCount; ++i) {
            System.out.print("Opening %s %s... ", devices[i].name, devices[i].serial_str);
            neoradio2.neoradio2_handle handle;
            if (neoradio2.neoradio2_open(handle, devices[i]) != neoradio.NEORADIO2_SUCCESS) {
                System.out.println("neoradio2.neoradio2_open() failed!");
                continue;
            }
            System.out.println("Opened!");
            
            do_stuff_here(handle);
            
            System.out.print("Closing %s %s... ", device_infos[i].name, device_infos[i].serial_str);
            if (neoradio2.neoradio2_close(handle) != neoradio2.NEORADIO2_SUCCESS)
            {
                System.out.print("neoradio2.neoradio2_close() failed!\n");
                return;
            }
            System.out.print("Closed!\n");
        }
        System.out.println("Done.");
    }
    
    public static void do_stuff_here(neoradio2.neoradio2_handle handle) {
        // Device is open, this should be the first thing we do.
        System.out.print("Identifying Chain... ");
        if (neoradio2.neoradio2_chain_identify(handle) != neoradio2.NEORADIO2_SUCCESS)
        {
            System.out.print("neoradio2.neoradio2_chain_identify() failed!\n");
            return;
        }
        System.out.print("Identified!\n");

        System.out.print("Starting Application Firmware... ");
        if (neoradio2.neoradio2_app_start(handle, 0, 0xFF) != neoradio2.NEORADIO2_SUCCESS)
        {
            System.out.print("neoradio2.neoradio2_app_start() failed!\n");
            return;
        }
        System.out.print("Started!\n");
    }
}
