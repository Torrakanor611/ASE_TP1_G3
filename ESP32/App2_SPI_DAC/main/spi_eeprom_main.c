/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/dac.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "spi_eeprom.h"


/*
 This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D EEPROM (8-bit mode).
*/


#define EEPROM_HOST  VSPI_HOST
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5


static const char TAG[] = "main";

void app_main(void)
{
    esp_err_t ret;

    ESP_LOGI(TAG, "Initializing bus SPI%d...", EEPROM_HOST+1);
    spi_bus_config_t buscfg={
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(EEPROM_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    eeprom_config_t eeprom_config = {
        .cs_io = PIN_NUM_CS,
        .host = EEPROM_HOST,
        .miso_io = PIN_NUM_MISO,
    };

    eeprom_handle_t eeprom_handle;

    ESP_LOGI(TAG, "Initializing device...");
    ret = spi_eeprom_init(&eeprom_config, &eeprom_handle);
    ESP_ERROR_CHECK(ret);

    ret = spi_eeprom_write_enable(eeprom_handle);
    ESP_ERROR_CHECK(ret);

    const char test_str[] = "Hello World!";
    ESP_LOGI(TAG, "Write: %s", test_str);
    for (int i = 0; i < sizeof(test_str); i++) {
        // No need for this EEPROM to erase before write.
        ret = spi_eeprom_write(eeprom_handle, i, test_str[i]);
        ESP_ERROR_CHECK(ret);
    }

    uint8_t test_buf[32] = "";
    for (int i = 0; i < sizeof(test_str); i++) {
        ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(TAG, "Read: %s", test_buf);

    ESP_LOGI(TAG, "Example finished.");




    //Enable dac channel (GPIO 25)
    dac_output_enable(DAC_CHANNEL_1);    
    while(1){
        for(uint8_t i=0; i < 255; i++){
            //Set output voltage and generate delay
            dac_output_voltage(DAC_CHANNEL_1, i);
            vTaskDelay(10 / portTICK_RATE_MS);
        }
        for(uint8_t i=255; i > 0; i--){
            //Set output voltage and generate delay
            dac_output_voltage(DAC_CHANNEL_1, i);
            vTaskDelay(10 / portTICK_RATE_MS);
        }
        vTaskDelay(10 / portTICK_RATE_MS);

    }




}
