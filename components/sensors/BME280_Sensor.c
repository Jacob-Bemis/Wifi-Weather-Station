#include "BME280_Sensor.h"
#include "driver/i2c_master.h"
#include <stdio.h>
#include <inttypes.h>

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

void readCalibration(void){
    char message1[] = "First Calibration:";
    printf("%s\n", message1);
    uint8_t buf[1] = {0x88};
    uint8_t read_buffer[26];
    i2c_master_transmit_receive(dev_handle, buf, sizeof(uint8_t), read_buffer, 26, -1);
    for (int i =0; i < 26; i++ ){
        printf("Hex: 0x%02" PRIX8 "\n", read_buffer[i]);
    }

    char message2[] = "Second Calibration:";
    printf("%s\n", message2);
    uint8_t buf2[1] = {0xE1};
    uint8_t read_buffer2[7];
    i2c_master_transmit_receive(dev_handle, buf2, sizeof(uint8_t), read_buffer2, 7, -1);
    for (int i =0; i < 7; i++ ){
        printf("Hex: 0x%02" PRIX8 "\n", read_buffer2[i]);
    }
}