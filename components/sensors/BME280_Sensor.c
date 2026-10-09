#include "BME280_Sensor.h"
#include "driver/i2c_master.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

#define TEST_I2C_PORT 0
#define I2C_MASTER_SCL_IO 16
#define I2C_MASTER_SDA_IO 15

i2c_master_dev_handle_t dev_handle;
i2c_master_bus_handle_t bus_handle;
void busSetup(void){
i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = TEST_I2C_PORT,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};
ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x76,
    .scl_speed_hz = 100000,
};
ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
}

void readCalibration(Temperature *temp, Humidity *hum){
    uint8_t buf[1] = {0x88};
    uint8_t read_buffer[26];
    i2c_master_transmit_receive(dev_handle, buf, sizeof(uint8_t), read_buffer, 26, -1);
    temp->dig_T1 = (read_buffer[1] << 8) | read_buffer[0];
    temp->dig_T2 = (read_buffer[3] << 8) | read_buffer[2];
    temp->dig_T3 = (read_buffer[5] << 8) | read_buffer[4];

    hum->dig_H1 = read_buffer[25];

    uint8_t buf2[1] = {0xE1};
    uint8_t read_buffer2[7];
    i2c_master_transmit_receive(dev_handle, buf2, sizeof(uint8_t), read_buffer2, 7, -1);
    hum->dig_H2 = (read_buffer2[1] << 8) | read_buffer2[0];
    hum->dig_H3 = read_buffer2[2];
    hum->dig_H4 = (read_buffer2[3] << 4) | (read_buffer2[4] & 0x0F);
    if ((hum->dig_H4 >> 11) == 1){
        hum->dig_H4 |= 0xF000;
    }
    if ((hum->dig_H5 >> 11) == 1){
        hum->dig_H5 |= 0xF000;
    }
    hum->dig_H5 = (read_buffer2[5] << 4) | ((read_buffer2[4] & 0xF0) >> 4);
    hum->dig_H6 = read_buffer2[6]; 
}

void regConfig(void){
    uint8_t write_buffer[2] = {0xF2, 0x02};
    i2c_master_transmit(dev_handle, write_buffer, sizeof(write_buffer), 100);

    uint8_t write_buffer2[2] = {0xF4, 0x4A};
    i2c_master_transmit(dev_handle, write_buffer2, sizeof(write_buffer2), 100);
}

bool measCheck(void){
    uint8_t buf[1] = {0xF3};
    uint8_t read_buffer[1];
    uint8_t stat = 1;
    for (int i =0; i < 8; i++){
    i2c_master_transmit_receive(dev_handle, buf, sizeof(uint8_t), read_buffer, 1, -1);
    stat = (read_buffer[0] & 0x08) >> 3;
    if (stat != 1){
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(5));
    }
    if (stat != 0){
        return false; 
    }
    return true;
}

bool readRawData(Temperature *temp, Humidity *hum){
    if (measCheck){
        uint8_t buf[1] = {0xFA};
        uint8_t read_buffer[5];
        i2c_master_transmit_receive(dev_handle, buf, sizeof(uint8_t), read_buffer, 5, -1);
    
        hum->adc_H = (read_buffer[3] << 8) | read_buffer[4];
        temp->adc_T = (read_buffer[0] << 12) | (read_buffer[1] << 4) | (read_buffer[2] >> 4); 
    } else{
        return false;
    }
    return true;
}

int32_t tempCompensation(Temperature *temp){
    int32_t tempVal1, tempVal2, tempFinal, result;
    tempVal1 = (((temp->adc_T >> 3) - (temp->dig_T1 << 1)) * (temp->dig_T2)) >> 11;
    tempVal2 = (((((temp->adc_T >> 4) - (temp->dig_T1)) * ((temp->adc_T >> 4) - (temp->dig_T1))) >> 12) * (temp->dig_T3)) >> 14;
    tempFinal = tempVal1 + tempVal2;
    result = (tempFinal * 5 + 128) >> 8;
    return result;
}