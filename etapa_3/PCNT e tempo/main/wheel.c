#include "wheel.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/pulse_cnt.h"

static const char *TAG = "WHEEL";

/* =========================
   PCNT
========================= */
static pcnt_unit_handle_t pcnt_unit = NULL;

/* =========================
   Medição
========================= */
static int last_count = 0;
static int64_t last_time = 0;
static int64_t delta_t = 0;
static bool first = true;

/* =========================
   Filtro
========================= */
static float rpm_filtered = 0.0f;
#define ALPHA 0.2f

/* =========================
   TASK (IMPORTANTE: mais lenta)
========================= */
static void wheel_task(void *arg)
{
    int count = 0;

    while (1)
    {
        pcnt_unit_get_count(pcnt_unit, &count);

        if (count != last_count)
        {
            int64_t now = esp_timer_get_time();

            if (!first)
            {
                delta_t = now - last_time;
            }

            last_time = now;
            last_count = count;
            first = false;
        }

        /* ⚠️ IMPORTANTE: evita WDT */
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

/* =========================
   INIT
========================= */
void wheel_Init(void)
{
    ESP_LOGI(TAG, "Inicializando PCNT...");

    pcnt_unit_config_t unit_config = {
        .low_limit  = -32768,
        .high_limit = 32767,
        .flags = {
            .accum_count = true,
        },
    };

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_channel_handle_t chan = NULL;

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = ENCODER_GPIO,
        .level_gpio_num = -1,
    };

    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &chan));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(
        chan,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        PCNT_CHANNEL_EDGE_ACTION_HOLD));

    ESP_ERROR_CHECK(pcnt_channel_set_level_action(
        chan,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    /* filtro de ruído leve */
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 5000,
    };

    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    xTaskCreate(wheel_task, "wheel_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "PCNT iniciado");
}

/* =========================
   GET RPM
========================= */
float wheel_GetRPM(void)
{
    if (first || delta_t <= 0)
        return 0.0f;

    float rpm_raw =
        (60.0f * 1000000.0f) /
        ((float)PULSOS_POR_VOLTA * delta_t);

    rpm_filtered =
        ALPHA * rpm_raw +
        (1.0f - ALPHA) * rpm_filtered;

    return rpm_filtered;
}