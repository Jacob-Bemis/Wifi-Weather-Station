idf_component_register(
    SRCS "BME280_Sensor.c"
    INCLUDE_DIRS "."
    REQUIRES esp_driver_i2c
)