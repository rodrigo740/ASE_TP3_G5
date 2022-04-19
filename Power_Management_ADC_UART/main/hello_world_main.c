#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <stdlib.h>
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_sleep.h"
#include <sys/time.h>
#include "light_sleep_example.h"


static const int BUF_SIZE = 1024;

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

#define DEFAULT_VREF    1100
#define NO_OF_SAMPLES   64


static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_3;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_2;

uint32_t adc_reading = 0;
uint32_t voltage;
static QueueHandle_t uart2_queue;


void init(void) {
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 20, &uart2_queue, 0);
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

static void uart_event_task(void *pvParameters){
    static const char *EVENT_TAG = "EVENT_TASK";
    esp_log_level_set(EVENT_TAG, ESP_LOG_INFO);
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(BUF_SIZE);
    for(;;) {
        
        if(xQueueReceive(uart2_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, BUF_SIZE);
            ESP_LOGI(EVENT_TAG, "uart[%d] event:", UART_NUM_2);
            switch(event.type) {
                //Event of UART receving data
                case UART_DATA:
                    ESP_LOGI(EVENT_TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(UART_NUM_2, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(EVENT_TAG, "[DATA EVT]:");
                    uart_write_bytes(UART_NUM_2, (const char*) dtmp, event.size);
                    break;
                
                case UART_FIFO_OVF:
                    ESP_LOGI(EVENT_TAG, "hw fifo overflow");
                    uart_flush_input(UART_NUM_2);
                    xQueueReset(uart2_queue);
                    break;
                
                case UART_BUFFER_FULL:
                    ESP_LOGI(EVENT_TAG, "ring buffer full");
                    uart_flush_input(UART_NUM_2);
                    xQueueReset(uart2_queue);
                    break;
                
                case UART_BREAK:
                    ESP_LOGI(EVENT_TAG, "uart rx break");
                    break;
                
                case UART_PARITY_ERR:
                    ESP_LOGI(EVENT_TAG, "uart parity error");
                    break;
                
                case UART_FRAME_ERR:
                    ESP_LOGI(EVENT_TAG, "uart frame error");
                    break;
            
                default:
                    ESP_LOGI(EVENT_TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}



static void tx_task(void *arg){
    static const char *TX_TASK_TAG = "TX_TASK";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    int txBytes;
    char data[70];
    while (1) {
        sprintf(data, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        txBytes = uart_write_bytes(UART_NUM_2, data, strlen(data));
        ESP_LOGI(TX_TASK_TAG, "Wrote %d bytes", txBytes);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

static void rx_task(void *arg){
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
        }
    }
    free(data);
}

static void adc_task(void *arg){
    
    uint32_t average = 0;
    uint32_t v;
    int count = 0;

    adc2_config_channel_atten((adc2_channel_t)channel, atten);
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    while(1){

        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            int raw;
            adc2_get_raw((adc2_channel_t)channel, width, &raw);
            adc_reading += raw;
        }
        adc_reading /= NO_OF_SAMPLES;
        voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        average += voltage;
        count++;
        v = abs((average/count)-voltage);

        if (count == 5) {
            count = 0; 
            average = 0;
        }
        
        
        if (v < 10)
        {
            printf("Entering light sleep\n");
            uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);

            int64_t t_before_us = esp_timer_get_time();

            esp_light_sleep_start();

            int64_t t_after_us = esp_timer_get_time();

            printf("Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                    "timer", t_after_us / 1000, (t_after_us - t_before_us) / 1000);
        }
        

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



void app_main(void){
    /* Enable wakeup from light sleep by timer */
    example_register_timer_wakeup();
    init();
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(adc_task, "adc2_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
}