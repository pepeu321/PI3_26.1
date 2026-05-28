#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "wheel.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    wheel_Init();

    while (1)
    {
        float rpm = wheel_GetRPM();

        ESP_LOGI(TAG, "RPM = %.2f", rpm);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}