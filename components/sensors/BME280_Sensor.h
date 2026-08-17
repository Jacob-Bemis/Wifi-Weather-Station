#ifndef BME280_Sensor_H
#define BME280_Sensor_H
#include <inttypes.h>

void busSetup(void); 
void readCalibration(void);

struct temperature{
    uint8_t dig_T1;
    uint8_t dig_T2;
    uint8_t dig_T3;
};

struct humidity{
    uint8_t dig_H1;
    uint8_t dig_H2;
    uint8_t dig_H3;
    uint8_t dig_H4;
    uint8_t dig_H5;
    uint8_t dig_H6;
};

#endif