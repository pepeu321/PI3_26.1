#include "wheel.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_err.h"

static const char *TAG = "WHEEL"; 

static pcnt_unit_handle_t pcnt_unit = NULL;  //Handle do PCNT

#define PULSOS_POR_VOLTA 20

static int last_count = 0;  //Guardar contagem anterior
static int64_t last_pulse_time = 0;  //Guardar instante do último pulso

static float rpm = 0;  //RPM filtrado atual

// INIT- configuracao do periferico
void wheel_Init(void)
{
    ESP_LOGI(TAG, "Inicializando PCNT...");

    pcnt_unit_config_t unit_config = {
        .low_limit  = -32768,  //faixa de contagem contador
        .high_limit = 32767,
        .flags = {
            .accum_count = true, //acumula no limite ao inves de reiniciar
        },
    };

    ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

    pcnt_channel_handle_t chan = NULL;

    pcnt_chan_config_t chan_config = {  //Criacao do canal
        .edge_gpio_num = ENCODER_GPIO, //deteccao de borda
        .level_gpio_num = -1,
    };

    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &chan));

    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan,
        PCNT_CHANNEL_EDGE_ACTION_INCREASE,  //incrementa contador na borda
        PCNT_CHANNEL_EDGE_ACTION_HOLD));

    ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP));

    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 2000, //Filtro anti-ruído
    };

    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit)); //habilita
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit)); //zera
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit)); //comeca contagem

    last_pulse_time = esp_timer_get_time();

    ESP_LOGI(TAG, "Encoder inicializado");
}

// UPDATE RPM (TEMPO ENTRE PULSOS)
void wheel_UpdateRPM(void)
{
    int count = 0;

    if (pcnt_unit_get_count(pcnt_unit, &count) != ESP_OK) //le total de pulsos acumulados
        return;

    int delta = count - last_count; //Verifica quantos pulsos chegaram desde a ultima chamada.

    if (delta > 0)
    {
        int64_t now = esp_timer_get_time(); 
        int64_t dt_us = now - last_pulse_time; //Mede tempo entre pulsos

        last_pulse_time = now;
        last_count = count;

        if (dt_us > 0)
        {
            float dt = dt_us / 1000000.0f; //Conversao para segundos

            float rpm_new = 60.0f / (dt * PULSOS_POR_VOLTA); //Formula do RPM

            // filtro IIR de primeira ordem (80% valor antigo e 20% valor novo )
            rpm = 0.8f * rpm + 0.2f * rpm_new;
        }
    }
    else
    {
        // Detectacao de parada  (sem pulsos por 2s)
        int64_t now = esp_timer_get_time();

        if ((now - last_pulse_time) > 2000000)
        {
            rpm = 0;
        }
    }
}

// retorna RPM
float wheel_GetRPM(void)
{
    return rpm;
}