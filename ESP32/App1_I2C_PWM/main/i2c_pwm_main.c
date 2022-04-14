
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "tc74.h"
#include "driver/ledc.h"
#include "esp_err.h"


static const char *TAG = "TC74";

#define LEDC_TIMER             LEDC_TIMER_1
#define LEDC_MODE              LEDC_LOW_SPEED_MODE

#define LEDC_DUTY_RES          LEDC_TIMER_13_BIT
#define LEDC_DUTY              (4095)
#define LEDC_FREQUENCY         (5000)

#define LEDC_LS_CH2_GPIO       (4)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (5)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3





/**
 * PWM LEDC Initialization
 **/

void ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,   // 13 bit resolution
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK    // Auto select clock source
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    
    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel1 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_LS_CH2_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_LS_CH2_GPIO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config_t ledc_channel2 = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_LS_CH3_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_LS_CH3_GPIO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel1));
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel2));
}



static void i2c_pwm_temperature_task(void *arg){

  // setup the sensor
  ESP_ERROR_CHECK(i2c_master_init());

  uint8_t temperature_value, old_temperature = 0;
  uint8_t operation_mode;

  // Set the LEDC peripheral configuration
  ledc_init();

  // Set initial duty to 50% of both channels/leds
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL, LEDC_DUTY));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL));
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL, LEDC_DUTY));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL));

  // periodically read temp values from sensor and set the sensor to power saving mode
  while(1){

    //Read tc74 operation mode and write data in sensor (set it in normal mode)
    i2c_master_read_tc74_config(I2C_MASTER_NUM,&operation_mode);
    i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_NORM_OP_VALUE);

    vTaskDelay(250 / portTICK_RATE_MS);

    //Read temperature
    i2c_master_read_temp(I2C_MASTER_NUM, &temperature_value);
    ESP_LOGI(TAG,"Temperature is : %d", temperature_value);

    //If temperature rizes GPIO5 up (GPIO4 LEDC duty cycle 0%)
    if (old_temperature < temperature_value){
      ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL, 0));
      ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL));
      ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL, LEDC_DUTY));
      ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL));
    }
    //If temperature falls GPIO4 up (GPIO5 LEDC duty cycle 0%)
    else if (old_temperature > temperature_value){
      ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL, 0));
      ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL));
      ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL, LEDC_DUTY));
      ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL));
    }
    //If temperature stays the same both up
    else{
      ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL, LEDC_DUTY));
      ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH2_CHANNEL));
      ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL, LEDC_DUTY));
      ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_LS_CH3_CHANNEL));
    }
    old_temperature = temperature_value;
    
    //Read tc74 operation mode and write data in sensor (set it in standby mode)
    i2c_master_read_tc74_config(I2C_MASTER_NUM, &operation_mode);
    i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_STANBY_VALUE);

    vTaskDelay(4000 / portTICK_RATE_MS);
  }
}

void app_main(void)
{
    // sensor handling task
    xTaskCreate(i2c_pwm_temperature_task, "i2c_pwm_temperature_task", 1024 * 2, (void *)0, 10, NULL);
}