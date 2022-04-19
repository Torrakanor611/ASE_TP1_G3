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

#include "esp_sleep.h"
#include "esp_check.h"


/*
 This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D EEPROM (8-bit mode).
*/


#define EEPROM_HOST  VSPI_HOST
#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define PIN_NUM_CS   5

#define TIMER_WAKEUP_TIME_US    (2 * 1000 * 1000)

static const char TAG[] = "main";





/**
 * ESP timer wake up (from light sleep mode)
 **/ 

esp_err_t timer_wakeup(void)
{
    ESP_RETURN_ON_ERROR(esp_sleep_enable_timer_wakeup(TIMER_WAKEUP_TIME_US), TAG, "Configure timer as wakeup source failed");
    ESP_LOGI(TAG, "timer wakeup source is ready");
    return ESP_OK;
}





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
    ESP_ERROR_CHECK(spi_bus_initialize(EEPROM_HOST, &buscfg, SPI_DMA_CH_AUTO));

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



    //Configure timer wake up source
    timer_wakeup();

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
        //vTaskDelay(10 / portTICK_RATE_MS);
        
        esp_light_sleep_start();
        
        if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
          ESP_LOGI(TAG, "Woke up from light sleep mode by timer wource");

    }




}
