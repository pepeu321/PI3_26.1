Etapa 2
#######

.. contents::
   :local:
   :depth: 2


Visão geral
***********

A etapa 2 ...


Desenvolvimento
***************

Apresentar o desenvolvimento da etapa contendo detalhes de implementação (se houver) de hardware e software. Adicionar pesqusisas realizadas bem como testes realizados.


Teste do encoder óptico no microcontrolador.
======
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

Esquemátco da placa de potência.
======

Esquemátco da placa de controle.
======



(Outras subseções se necessário)
================================


Referências
*************************************

- `nRF Connect SDK <https://developer.nordicsemi.com/nRF_Connect_SDK/doc/2.4.2/nrf/getting_started/modifying.html#configure-application>`_


