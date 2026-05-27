#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define FAN_PIN GPIO_NUM_6
#define TAG "FAN_CONTROL"

#define TIME_WORK_SEC 3
#define TIME_IDLE_SEC 9

static esp_timer_handle_t fan_timer;
static volatile bool fan_state = false;

static void timer_callback(void* arg) {
    if (!fan_state) {
        fan_state = true;
        gpio_set_level(FAN_PIN, 0); // LOW = Увімкнено
        ESP_LOGI(TAG, "Вентилятор УВІМКНЕНО на %d сек.", TIME_WORK_SEC);
        esp_timer_start_once(fan_timer, (uint64_t)TIME_WORK_SEC * 1000000ULL);
    } else {
        fan_state = false;
        gpio_set_level(FAN_PIN, 1); // HIGH = Вимкнено
        ESP_LOGI(TAG, "Вентилятор ВИМКНЕНО на %d сек.", TIME_IDLE_SEC);
        esp_timer_start_once(fan_timer, (uint64_t)TIME_IDLE_SEC * 1000000ULL);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Старт системи на ESP-IDF...");

    gpio_reset_pin(FAN_PIN);
    gpio_set_direction(FAN_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(FAN_PIN, 1); 

    const esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
        .name = "fan_timer"
    };

    esp_timer_create(&timer_args, &fan_timer);

    ESP_LOGI(TAG, "Таймер запущено. Очікування: %d сек.", TIME_IDLE_SEC);
    esp_timer_start_once(fan_timer, (uint64_t)TIME_IDLE_SEC * 1000000ULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}