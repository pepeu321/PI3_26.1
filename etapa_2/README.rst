Etapa 2
#######

.. contents::
   :local:
   :depth: 2


Visão geral
***********

Na Etapa 2, foram realizados testes individuais com os sensores que serão utilizados com o microcontrolador, e o desenvolvimento dos esquemáticos dos hardwares do sistema. As atividades desenvolvidas nesta etapa são:

📌 Teste do encoder óptico no microcontrolador

📌 Teste do sensor de corrente no microcontrolador

📌 Esquemátco da placa de potência

📌 Esquemátco da placa de controle

Desenvolvimento
***************

Apresentar o desenvolvimento da etapa contendo detalhes de implementação (se houver) de hardware e software. Adicionar pesqusisas realizadas bem como testes realizados.


Teste do encoder óptico no microcontrolador.
======
wheel.h

.. code-block:: vhdl

   #ifndef MAIN_WHEEL_H_
   #define MAIN_WHEEL_H_
   
   #include "driver/pulse_cnt.h"
   
   // Pino do encoder (D0)
   #define ENCODER_GPIO  14
   
   void wheel_Init(void);
   void wheel_GetEncoderPulses(int *pulsos);
   
   #endif   


wheel.c

.. code-block:: vhdl

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


Main.c

.. code-block:: vhdl

   #include <stdio.h>
   #include <stdio.h>
   #include "freertos/FreeRTOS.h"
   #include "freertos/task.h"
   #include "esp_log.h"
   #include "wheel.h"
   
   static const char *TAG = "MAIN";
   
   // Encoder com 20 ranhuras
   #define PULSOS_POR_VOLTA 20
   
   void app_main(void)
   {
       ESP_LOGI(TAG, "Iniciando sistema...");
   
       wheel_Init();
   
       int last_pulsos = 0;
   
       while (1)
       {
           int pulsos;
   
           wheel_GetEncoderPulses(&pulsos);
   
           int delta = pulsos - last_pulsos;
           last_pulsos = pulsos;
   
           float dt = 0.05; // 100 ms
   
           float rpm = (delta / (float)PULSOS_POR_VOLTA) * (60.0 / dt);
   
           ESP_LOGI(TAG, "RPM: %.2f", rpm);
   
           vTaskDelay(pdMS_TO_TICKS(50));
       }
   }

Teste do sensor de corrente no microcontrolador.
======

Main.c
   
.. code-block:: vhdl  



Esquemátco da placa de potência.
======

Esquemátco da placa de controle.
======



(Outras subseções se necessário)
================================


Referências
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


