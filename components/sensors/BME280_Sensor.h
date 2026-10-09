#ifndef BME280_Sensor_H
#define BME280_Sensor_H
#include <inttypes.h>

void busSetup(void); 

struct Temperature{
    uint32_t adc_T
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
};

struct Humidity{
    uint16_t adc_H
    int16_t dig_H4;
    int16_t dig_H5;
    int16_t dig_H2;
    uint8_t dig_H1;
    uint8_t dig_H3;
    int8_t dig_H6;
};

extern Humidity humValues;
extern Temperature tempValues;

void readCalibration(Temperature *temp, Humidity *hum);
bool readRawData(Temperature *temp, Humidity *hum);
bool measCheck(void);
void regConfig(void);


#endif