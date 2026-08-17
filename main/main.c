#include "../components/sensors/BME280_Sensor.h"


void app_main(void){
    busSetup();
    readCalibration();
}