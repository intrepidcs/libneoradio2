#include <libneoradio2.h>
#include <stdio.h>

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
        if (neoradio2_open(&handle, &device_infos[i]) != NEORADIO2_SUCCESS)
        {
            printf("neoradio2_open() failed!\n");
            return 1;
        }
        // Do stuff here
        if (neoradio2_close(&handle) != NEORADIO2_SUCCESS)
        {
            printf("neoradio2_close() failed!\n");
            return 1;
        }
    }
    printf("Done.\n");
    return 0;
}