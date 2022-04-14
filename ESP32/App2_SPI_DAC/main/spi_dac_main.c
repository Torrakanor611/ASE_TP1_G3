#include "driver/dac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define EEPROM_HOST     HSPI_HOST
#define PIN_NUM_MISO    19 
#define PIN_NUM_MOSI    23
#define PIN_NUM_SCLK    18
#define PIN_NUM_CS      5
#define EEPROM_CLOCK    500000




void app_main(void)
{

    //Enable dac channel
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