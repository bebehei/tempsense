#include <stdlib.h>
#include <hidapi.h>
#include <cstdio>
#include <ctime>

#define ERR_NO_SENSOR 6
#define ERR_SENSOR_NOT_AVAIL 7

int main(int argc, char **argv) {
    
    hid_device *handle = hid_open(0x16c0, 0x0480, nullptr);
    if (!handle) {
        fprintf(stderr, "No temperature sensor found!\n");
        return 1;
    }
    
    const int MAX_STR = 255;
    wchar_t wstr[MAX_STR];
    
    int res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
    if (res == -1) {
        fprintf(stderr, "Could not get manufacturer string!\n");
        return 2;
    }
    
    res = hid_get_product_string(handle, wstr, MAX_STR);
    if (res == -1) {
        fprintf(stderr, "Could not get product string!\n");
        return 3;
    }

    if( argc < 2 ) {
        fprintf(stderr, "No Sensor specified.\n");
        return ERR_NO_SENSOR;
    }

    unsigned int sensor_req = atoi(argv[1]);

    unsigned char buf[65];

    for (;;) {
        int num = hid_read(handle, buf, 64);
        if (num < 0) {
            fprintf(stderr, "Could not read from device!\n");
            return 4;
        }
        if (num == 64) {
            unsigned int sensor_all = (unsigned int)buf[0];
            unsigned int sensor_cur = (unsigned int)buf[1];

            short temp = *(short *)&buf[4]; //holy fuck!

            if (sensor_req > sensor_all){
                fprintf(stderr, "Requested sensor is not available!\n");
                return ERR_SENSOR_NOT_AVAIL;
            }

            if (sensor_req == sensor_cur){
                printf("%d %+.1f\n", sensor_cur, (float)temp/10.0f);
                break;
            }
        }
    }
    return 0;
}

