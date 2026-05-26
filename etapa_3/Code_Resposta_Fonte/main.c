#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/gptimer.h"

#include "wheel.h"

static const char *TAG = "MAIN";

#define PULSOS_POR_VOLTA 20

#define TEMPO_TOTAL_MS     4000
#define PERIODO_AMOSTRA_MS 100

#define NUM_AMOSTRAS (TEMPO_TOTAL_MS / PERIODO_AMOSTRA_MS)

static volatile bool sample_flag = false;


// CALLBACK TIMER

static bool timer_callback(
    gptimer_handle_t timer,
    const gptimer_alarm_event_data_t *edata,
    void *user_ctx)
{
    sample_flag = true;
    return false;
}


 //TIMER

static gptimer_handle_t init_sample_timer(void)
{
    gptimer_handle_t timer = NULL;

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000,
    };

    ESP_ERROR_CHECK(
        gptimer_new_timer(
            &timer_config,
            &timer));

    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_callback,
    };

    ESP_ERROR_CHECK(
        gptimer_register_event_callbacks(
            timer,
            &cbs,
            NULL));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = PERIODO_AMOSTRA_MS * 1000,
        .flags = {
            .auto_reload_on_alarm = true,
        },
    };

    ESP_ERROR_CHECK(
        gptimer_set_alarm_action(
            timer,
            &alarm_config));

    ESP_ERROR_CHECK(gptimer_enable(timer));
    ESP_ERROR_CHECK(gptimer_start(timer));

    return timer;
}


// TASK AQUISICAO

void task_aquisicao(void *pvParameters)
{
    wheel_Init();

    int last_pulsos = 0;

    float vetor_rpm[NUM_AMOSTRAS];

    float rpm_filtrado = 0;

    ESP_LOGI(TAG, "5 s");
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "4 s");
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "3 s");
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "2 s");
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "1 s");
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "AGORA!");

    gptimer_handle_t timer = init_sample_timer();

    int n = 0;

    while (n < NUM_AMOSTRAS)
    {
        if (sample_flag)
        {
            sample_flag = false;

            int pulsos;

            wheel_GetEncoderPulses(&pulsos);

            int delta = pulsos - last_pulsos;

            last_pulsos = pulsos;

            float dt = PERIODO_AMOSTRA_MS / 1000.0f;

            float rpm =
                (delta / (float)PULSOS_POR_VOLTA)
                * (60.0f / dt);

            // filtro exponencial
            rpm_filtrado =
                0.7f * rpm_filtrado +
                0.3f * rpm;

            vetor_rpm[n] = rpm_filtrado;

            n++;
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    ESP_ERROR_CHECK(gptimer_stop(timer));
    ESP_ERROR_CHECK(gptimer_disable(timer));
    ESP_ERROR_CHECK(gptimer_del_timer(timer));

    ESP_LOGI(TAG, "Fim da aquisicao");

    // CSV
    printf("\n");
    printf("tempo,rpm\n");

    for (int i = 0; i < NUM_AMOSTRAS; i++)
    {
        float tempo =
            i * (PERIODO_AMOSTRA_MS / 1000.0f);

        printf("%.2f,%.2f\n",
               tempo,
               vetor_rpm[i]);
    }

    vTaskDelete(NULL);
}



void app_main(void)
{
    ESP_LOGI(TAG, "Sistema iniciado");

    xTaskCreate(
        task_aquisicao,
        "task_aquisicao",
        4096,
        NULL,
        5,
        NULL);
}