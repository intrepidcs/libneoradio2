#include <libneoradio2.h>
#include <stdio.h>
#include <unistd.h>

#define BANK_LED1 0x10
#define BANK_LED2 0x20
#define BANK_LED3 0x40
#define BANK_LED4 0x80

#define BANK_DIO1 0x01
#define BANK_DIO2 0x02
#define BANK_DIO3 0x04
#define BANK_DIO4 0x08


static char* AIN_NAMES[] = {
    "AIN1", "AIN2", "AIN3", "AIN4", "AIN5", "AIN6", "TEMP", "POT ",
};

int initialize_device(neoradio2_handle* handle);

int toggle_io(neoradio2_handle* handle, int io_mask, int enable_mask)
{
    return neoradio2_write_sensor(handle, 1, 1, io_mask, enable_mask) == NEORADIO2_SUCCESS;
}

int read_sensors(neoradio2_handle* handle, float* values)
{
    int success = 0;
    if (neoradio2_request_sensor_data(handle, 0, 0xFF, 1) == NEORADIO2_SUCCESS)
    {
        const int AIN_COUNT = 8;
        for (int i=0; i < AIN_COUNT; ++i)
        {
            if (neoradio2_read_sensor_float(handle, 0, i, &values[i]) != NEORADIO2_SUCCESS)
            {
                continue;
            }
            else
            {
                success += 1;
            }
        }
        success = (success == AIN_COUNT);
    }
    return success;
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
        
        int initialized = initialize_device(&handle);
        if (!initialized)
        {
            printf("Failed to initialize device!\n");
            neoradio2_close(&handle);
            continue;
        }
        printf("Opened!\n");

        // Do stuff here
        printf("Turning on all LEDs...\n");
        toggle_io(&handle,
            BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4,
            BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4);
            
        printf("Turning on all DIO...\n");
        toggle_io(&handle,
            BANK_DIO1 | BANK_DIO2 | BANK_DIO3 | BANK_DIO4,
            BANK_DIO1 | BANK_DIO2 | BANK_DIO3 | BANK_DIO4);
        
        float values[8] = {0};
        read_sensors(&handle, values);
        for (int s=0; s < 8; ++s)
        {
            printf("%s: %f\n", AIN_NAMES[s], values[s]);
        }
        
        printf("Turning off all LEDs...\n");
        sleep(2);
        toggle_io(&handle,
            BANK_LED1 | BANK_LED2 | BANK_LED3 | BANK_LED4,
            0);

        printf("Closing %s %s... ", device_infos[i].name, device_infos[i].serial_str);
        if (neoradio2_close(&handle) != NEORADIO2_SUCCESS)
        {
            printf("neoradio2_close() failed!\n");
            return 1;
        }
        sleep(1);
        printf("Closed!\n");
    }
    printf("Done.\n");
    return 0;
}

int initialize_device(neoradio2_handle* handle)
{
    // Device is open, this should be the first thing we do.
    //printf("Identifying Chain... ");
    if (neoradio2_chain_identify(handle) != NEORADIO2_SUCCESS)
    {
        printf("neoradio2_chain_identify() failed!\n");
        return 0;
    }
    //printf("Identified!\n");

    //printf("Starting Application Firmware... ");
    if (neoradio2_app_start(handle, 0, 0xFF) != NEORADIO2_SUCCESS)
    {
        printf("neoradio2_app_start() failed!\n");
        return 0;
    }
    //printf("Started!\n");
    return 1;
}
