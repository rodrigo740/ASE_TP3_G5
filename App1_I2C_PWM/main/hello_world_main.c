#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

// Defintions

static const char *TAG = "TC74";

#define TC74_SLAVE_ADDR_A0   0x48
#define TC74_SLAVE_ADDR_A1   0x49
#define TC74_SLAVE_ADDR_A2   0x4A
#define TC74_SLAVE_ADDR_A3   0x4B
#define TC74_SLAVE_ADDR_A4   0x4C
#define TC74_SLAVE_ADDR_A5   0x4D  /*!< default slave address for TC74 sensor */
#define TC74_SLAVE_ADDR_A6   0x4E
#define TC74_SLAVE_ADDR_A6   0x4F

#define READ_TEMP_REGISTER          0x00
#define READ_WRITE_CONFIG_REGISTER  0x01
#define SET_NORM_OP_VALUE           0x00  /*!< sets the 7th bit of configuration register to normal mode */
#define SET_STANBY_VALUE            0x80  /*!< sets the 7th bit of configuration register to standby mode */

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define I2C_MASTER_SCL_IO GPIO_NUM_22               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO GPIO_NUM_21               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM    I2C_NUMBER(0)             /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000                   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                 /*!< I2C master doesn't need buffer */

#define TC74_SLAVE_ADDR  TC74_SLAVE_ADDR_A5         /*!< slave address for TC74 sensor */
#define WRITE_BIT I2C_MASTER_WRITE                  /*!< I2C master write */
#define READ_BIT  I2C_MASTER_READ                   /*!< I2C master read */
#define ACK_CHECK_EN 0x1                            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                           /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                                 /*!< I2C ack value */
#define NACK_VAL 0x1                                /*!< I2C nack value */

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz


static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode             = I2C_MODE_MASTER;
    conf.sda_io_num       = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en    = GPIO_PULLUP_ENABLE;
    conf.scl_io_num       = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en    = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


esp_err_t i2c_master_read_temp(i2c_port_t i2c_num, uint8_t *tmprt)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TC74_SLAVE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, READ_TEMP_REGISTER, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TC74_SLAVE_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, tmprt, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 300 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


esp_err_t i2c_master_read_tc74_config(i2c_port_t i2c_num, uint8_t *mode)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TC74_SLAVE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, READ_WRITE_CONFIG_REGISTER, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TC74_SLAVE_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, mode, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 300 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


esp_err_t i2c_master_set_tc74_mode(i2c_port_t i2c_num,uint8_t mode)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, TC74_SLAVE_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, READ_WRITE_CONFIG_REGISTER, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, mode, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 300 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

static void i2c_temperature_task(void *arg){

  // setup the sensor
  ESP_ERROR_CHECK(i2c_master_init());

  // signed integer value of 8 bits
  uint8_t temperature_value;

  // signed integer value of 8 bits
  uint8_t operation_mode;

  // set standby mode for testing (5uA consuption)
  // i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_STANBY_VALUE);

  // set normal mode for testing (200uA consuption)
  // i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_NORM_OP_VALUE);

    bool flag = false;

  // periodically read temp values from sensor and set the sensor to power saving mode
  while(1){

    i2c_master_read_tc74_config(I2C_MASTER_NUM,&operation_mode);
    // ESP_LOGI(TAG,"Operation mode is : %d",operation_mode);
    // set normal mode for testing (200uA consuption)
    i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_NORM_OP_VALUE);
    vTaskDelay(250 / portTICK_RATE_MS);
    i2c_master_read_temp(I2C_MASTER_NUM,&temperature_value);
    ESP_LOGI(TAG,"Temperature is : %d",temperature_value);

    if (temperature_value >= 30) {
        // Set the LEDC peripheral configuration
        example_ledc_init();
        flag = true;
        // Set duty to 50%
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
    }else{
        ESP_LOGI(TAG,"Temperature %d is lower than 30",temperature_value);
        if(flag) {
            ledc_stop(LEDC_MODE, LEDC_CHANNEL, 0);
            flag = false;
        }
    }

    i2c_master_read_tc74_config(I2C_MASTER_NUM,&operation_mode);
    // ESP_LOGI(TAG,"Operation mode is : %d",operation_mode);
    // set standby mode for testing (5uA consuption)
    i2c_master_set_tc74_mode(I2C_MASTER_NUM, SET_STANBY_VALUE);

    vTaskDelay(8000 / portTICK_RATE_MS);
  }
}

void app_main(void)
{
    

    // sensor handling task
    xTaskCreate(i2c_temperature_task, "i2c_temperature_task", 1024 * 2, (void *)0, 10, NULL);
}