#include "wheel.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "WHEEL";

static pcnt_unit_handle_t pcnt_unit = NULL;

void wheel_Init(void)
{
    ESP_LOGI(TAG, "Inicializando PCNT...");

    pcnt_unit_config_t unit_config = {
        .low_limit  = -32768,   // ✔️ obrigatório no IDF v6
        .high_limit = 32767,
        .flags = {
            .accum_count = true, // ✔️ obrigatório
        },
    };

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_channel_handle_t chan = NULL;

    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = ENCODER_GPIO,
        .level_gpio_num = -1, // ✔️ encoder simples (D0)
    };

    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &chan));

    // Conta na borda de subida
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE,
        PCNT_CHANNEL_EDGE_ACTION_HOLD));

    // Mantém comportamento estável
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    // Filtro de ruído
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 2000,
    };

    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    ESP_LOGI(TAG, "Encoder inicializado (D0)");
}

void wheel_GetEncoderPulses(int *pulsos)
{
    if (pcnt_unit == NULL) {
        ESP_LOGE(TAG, "pcnt_unit NULL!");
        *pulsos = 0;
        return;
    }

    esp_err_t err = pcnt_unit_get_count(pcnt_unit, pulsos);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao ler PCNT: %s", esp_err_to_name(err));
        *pulsos = 0;
    }
}