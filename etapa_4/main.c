#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"

#include "nvs_flash.h"
#include "esp_netif.h"

#include "coap3/coap_forward_decls.h"
#include "coap3/coap.h"

//Modulos
#include "wheel.h"
#include "motor.h"

//WIFI
#define WIFI_SSID "lpae_wifi"
#define WIFI_PASS "esp-8266"

//Controle
#define TS_CONTROLE_S 0.1f
#define KP_PI     2.04              
#define KI_PI      34.91             

#define DUTY_MIN 0.0f
#define DUTY_MAX 1023.0f

//Globais
static bool coap_task_started = false;
static const char *TAG = "MAIN";

//static int duty_atual = 0; //post manda duty
static float ref_rpm = 0.0f; //post manda rpm
static float erro_integral = 0.0f; 
//direcao
static int direcao_motor = 1;

//Tarefa encoder
void task_encoder(void *pvParameters)
{
    while (1)
    {
        wheel_UpdateRPM();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

//Tarefa Controle
void task_controle_PI(void *pvParameters)
{
    while (1)
    {
        float rpm = wheel_GetRPM();
        float erro = ref_rpm - rpm;

        float duty_unsat = KP_PI * erro + KI_PI * erro_integral;
            
        float duty = duty_unsat;

        if (duty > 1023)
            duty = DUTY_MAX;

        if (duty < 0)
            duty = DUTY_MIN;
		
//evita que integral cresça sem limite com PWM já saturado
        if (duty == duty_unsat)
        {
            erro_integral += erro * TS_CONTROLE_S;
        }

        if (ref_rpm <= 0.0f)
        {
            erro_integral = 0.0f;
            duty = 0.0f;
        }

        motor_SetDuty((int)duty);

        //ESP_LOGI(
        //    TAG,
        //    "REF: %.2f | RPM: %.2f | ERRO: %.2f | DUTY: %.0f",
        //    ref_rpm,
        //    rpm,
        //    erro,
        //    duty);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}




//Tarefa 
void hnd_post_vel(
    coap_resource_t *resource,
    coap_session_t *session,
    const coap_pdu_t *request,
    const coap_string_t *query,
    coap_pdu_t *response)
{
    size_t size;
    const uint8_t *data;

    if (coap_get_data(request, &size, &data))
    {
        char buffer[20];

        if (size >= sizeof(buffer))
            size = sizeof(buffer) - 1;

        memcpy(buffer, data, size);
        buffer[size] = '\0';

       // duty_atual = atoi(buffer);

       // if (duty_atual < 0)
       //     duty_atual = 0;
	  //	
      //  if (duty_atual > 1023)
       //     duty_atual = 1023;

      //  motor_SetDuty(duty_atual);

     //   ESP_LOGI(TAG, "Duty recebido via CoAP: %d", duty_atual);
        
        ref_rpm  = atoi(buffer);

        if (ref_rpm  < 0)
            ref_rpm  = 0;

        if (ref_rpm == 0.0f)
        {
            erro_integral = 0.0f;
            motor_Stop();
        }
            
        
        ESP_LOGI(TAG, "RPM referencia recebido: %.2f RPM", ref_rpm);    
		
        //motor_SetDuty(ref_rpm );
        
    }

    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);

    const char *resp_str = "OK";

    coap_add_data(
        response,
        strlen(resp_str),
        (const uint8_t *)resp_str);
}

void hnd_get_vel(
    coap_resource_t *resource,
    coap_session_t *session,
    const coap_pdu_t *request,
    const coap_string_t *query,
    coap_pdu_t *response)
{
    char resp[64];

    float rpm = wheel_GetRPM();

    sprintf(resp, "%.2f", rpm);

    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);

    coap_add_data(
        response,
        strlen(resp),
        (const uint8_t *)resp);
}


void hnd_post_dir(
    coap_resource_t *resource,
    coap_session_t *session,
    const coap_pdu_t *request,
    const coap_string_t *query,
    coap_pdu_t *response)
{
    size_t size;
    const uint8_t *data;

    if (coap_get_data(request, &size, &data))
    {
        char buffer[20];

        if (size >= sizeof(buffer))
            size = sizeof(buffer) - 1;

        memcpy(buffer, data, size);
        buffer[size] = '\0';

        if (strcmp(buffer, "fwd") == 0)
        {
            direcao_motor = 1;
            motor_SetDirection(1);
        }
        else if (strcmp(buffer, "rev") == 0)
        {
            direcao_motor = -1;
            motor_SetDirection(-1);
        }
    }

    coap_pdu_set_code(response, COAP_RESPONSE_CODE_CHANGED);

    const char *resp_str = "OK";

    coap_add_data(
        response,
        strlen(resp_str),
        (const uint8_t *)resp_str);
}


void coap_server_task(void *pvParameters)
{
    //ESP_LOGI(TAG, "CoAP task iniciou");

    coap_startup();

    coap_context_t *ctx = coap_new_context(NULL);

    if (ctx == NULL)
    {
        //ESP_LOGE(TAG, "Erro ao criar contexto CoAP");
        vTaskDelete(NULL);
        return;
    }

    coap_address_t serv_addr;
    coap_address_init(&serv_addr);

    serv_addr.addr.sin.sin_family = AF_INET;
    serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
    serv_addr.addr.sin.sin_port = htons(5683);

    coap_endpoint_t *endpoint =
        coap_new_endpoint(
            ctx,
            &serv_addr,
            COAP_PROTO_UDP);

    if (endpoint == NULL)
    {
        ESP_LOGE(TAG, "Erro ao criar endpoint CoAP");
        vTaskDelete(NULL);
        return;
    }

    coap_resource_t *resource =
        coap_resource_init(
            coap_make_str_const("vel"),
            0);
           
    coap_resource_t *resource_dir =
    coap_resource_init(
        coap_make_str_const("dir"),
        0);

    coap_register_handler(
        resource,
        COAP_REQUEST_POST,
        hnd_post_vel);

    coap_register_handler(
        resource,
        COAP_REQUEST_GET,
        hnd_get_vel);
        
    coap_register_handler(
    resource_dir,
    COAP_REQUEST_POST,
    hnd_post_dir);

    coap_add_resource(ctx, resource);
    coap_add_resource(ctx, resource_dir);

    ESP_LOGI(TAG, "Recurso /vel criado");

    while (1)
    {
        coap_io_process(ctx, COAP_IO_WAIT);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}



static void wifi_event_handler(
    void *arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void *event_data)
{
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event =
            (ip_event_got_ip_t *)event_data;

        ESP_LOGI(TAG,
                 "IP: " IPSTR,
                 IP2STR(&event->ip_info.ip));

        if (!coap_task_started)
        {
            coap_task_started = true;

            xTaskCreate(
                coap_server_task,
                "coap_server",
                8192,
                NULL,
                5,
                NULL);
        }
    }

    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
       // ESP_LOGI(TAG, "Reconectando...");
        esp_wifi_connect();
    }
}

void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            NULL));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL,
            NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());
}


void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    wheel_Init();

    motor_Init();

    motor_Stop();

    xTaskCreate(
        task_encoder,
        "encoder_task",
        2048,
        NULL,
        5,
        NULL);
        
    xTaskCreate(
        task_controle_PI,
        "controle_PI",
        4096,
        NULL,
        4,
        NULL);

    wifi_init_sta();

    //ESP_LOGI(TAG, "Sistema iniciado");
}