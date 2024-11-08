#include <libneoradio2.h>
#include <stdio.h>

void do_stuff_here(neoradio2_handle* handle)
{
    // Device is open, this should be the first thing we do.
    printf("Identifying Chain... ");
    if (neoradio2_chain_identify(handle) != NEORADIO2_SUCCESS)
    {
        printf("neoradio2_chain_identify() failed!\n");
        return;
    }
    printf("Identified!\n");

    printf("Starting Application Firmware... ");
    if (neoradio2_app_start(handle, 0, 0xFF) != NEORADIO2_SUCCESS)
    {
        printf("neoradio2_app_start() failed!\n");
        return;
    }
    printf("Started!\n");
}

int main(int argc, char* argv[])
{
    printf("Finding neoRAD-IO-2 Devices...\n");

    unsigned int count = NEORADIO2_MAX_DEVS;
    Neoradio2DeviceInfo device_infos[NEORADIO2_MAX_DEVS];
    // Find all neoRAD-IO-2 Devices
    if (neoradio2_find(device_infos, &count) != NEORADIO2_SUCCESS)
    {
        printf("neoradio2_find() failed!\n");
        return 1;
    }
    printf("Found %d device(s)...\n", count);
    for (unsigned int i=0; i < count; ++i)
    {
        neoradio2_handle handle;
        printf("Opening %s %s... ", device_infos[i].name, device_infos[i].serial_str);
        if (neoradio2_open(&handle, &device_infos[i]) != NEORADIO2_SUCCESS)
        {
            printf("neoradio2_open() failed!\n");
            return 1;
        }
        printf("Opened!\n");

        // Do stuff here
        do_stuff_here(&handle);

        printf("Closing %s %s... ", device_infos[i].name, device_infos[i].serial_str);
        if (neoradio2_close(&handle) != NEORADIO2_SUCCESS)
        {
            printf("neoradio2_close() failed!\n");
            return 1;
        }
        printf("Closed!\n");
    }
    printf("Done.\n");
    return 0;
}
