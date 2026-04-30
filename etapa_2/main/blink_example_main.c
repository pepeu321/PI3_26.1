#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

// CONFIGURAÇÕES 
#define ADC_CHANNEL        ADC_CHANNEL_0   // GPIO1 (ajuste se necessário)
#define ADC_UNIT           ADC_UNIT_1
#define ADC_ATTEN          ADC_ATTEN_DB_11 // até ~3.3V
#define ADC_BITWIDTH       ADC_BITWIDTH_DEFAULT

#define NUM_SAMPLES        200             // média
#define SENSITIVITY        0.04f           // 40 mV/A (ACS758 típico)
#define VCC                3.3f

// VARIÁVEIS GLOBAIS
adc_oneshot_unit_handle_t adc_handle;
adc_cali_handle_t adc_cali_handle = NULL;

float offset_voltage = 1.65; // 1.65 valor tipico -> vcc/2


// FUNÇÃO PARA LER TENSÃO 
float read_voltage()
{
    int adc_raw = 0;
    int voltage = 0;

    int soma = 0;

	//for, pra ler ADC melhor, tirando medias
    for (int i = 0; i < NUM_SAMPLES; i++) {
        adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw);
        soma += adc_raw;
    }

    adc_raw = soma / NUM_SAMPLES;

    adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage);

    return voltage / 1000.0; // mV -> V
}

// CALIBRAÇÃO DE OFFSET 
void calibrate_offset()
{
    printf("Calibrando offset... NÃO passe corrente!\n");

    vTaskDelay(pdMS_TO_TICKS(2000));

    float soma = 0;

    for (int i = 0; i < 100; i++) {
        soma += read_voltage();
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    offset_voltage = soma / 100.0;

    printf("Offset calibrado: %.3f V\n\n", offset_voltage);
}

// APP MAIN 
void app_main(void)
{
    // Inicializa ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
    };
    adc_oneshot_new_unit(&init_config, &adc_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH,
        .atten = ADC_ATTEN_DB_12,
    };
    adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config);

    // Calibração do ADC
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH,
    };
    adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);

    // Calibra offset
    calibrate_offset();

    while (1) {

        float voltage = read_voltage();

        // Corrente = (Vout - offset) / sensibilidade
        float current = (voltage - offset_voltage) / SENSITIVITY;

        printf("Tensao: %.3f V | Corrente: %.2f A\n", voltage, current);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}