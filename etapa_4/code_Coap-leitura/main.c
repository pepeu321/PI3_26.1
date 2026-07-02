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

#include "wheel.h"

#define WIFI_SSID "lpae_wifi"
#define WIFI_PASS "esp-8266"

static bool coap_task_started = false;
static const char *TAG = "WIFI";

static int velocidade_atual = 0;


// TASK DO ENCODER

void task_encoder(void *pvParameters)
{
    while (1)
    {
        wheel_UpdateRPM();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


 // POST /vel

void hnd_post_vel(
    coap_resource_t *resource,
    coap_session_t *session,
    const coap_pdu_t *request,
    const coap_string_t *query,
    coap_pdu_t *response)
{
    size_t size;
    uint8_t *data;

    if (coap_get_data(request, &size, &data))
    {
        char buffer[20];

        memcpy(buffer, data, size);
        buffer[size] = '\0';

        velocidade_atual = atoi(buffer);

        ESP_LOGI(TAG,
                 "Velocidade recebida: %d",
                 velocidade_atual);
    }

    coap_pdu_set_code(
        response,
        COAP_RESPONSE_CODE_CHANGED);

    const char *resp_str = "OK";

    coap_add_data(
        response,
        strlen(resp_str),
        (const uint8_t *)resp_str);
}


 //GET /vel

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

    coap_pdu_set_code(
        response,
        COAP_RESPONSE_CODE_CONTENT);

    coap_add_data(
        response,
        strlen(resp),
        (const uint8_t *)resp);

    //ESP_LOGI(TAG,
             //"GET /vel -> %.2f RPM",
            // rpm);
}


 //SERVIDOR COAP

void coap_client_task(void *pvParameters)
{
    ESP_LOGI(TAG, "CoAP task iniciou");

    coap_startup();

    coap_context_t *ctx =
        coap_new_context(NULL);

    if (ctx == NULL)
    {
        ESP_LOGE(TAG,
                 "Erro ao criar contexto CoAP");

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
        ESP_LOGE(TAG,
                 "Erro ao criar endpoint CoAP");

        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG,
             "Endpoint CoAP criado");

    coap_resource_t *resource =
        coap_resource_init(
            coap_make_str_const("vel"),
            0);

    coap_register_handler(
        resource,
        COAP_REQUEST_POST,
        hnd_post_vel);

    coap_register_handler(
        resource,
        COAP_REQUEST_GET,
        hnd_get_vel);

    coap_add_resource(
        ctx,
        resource);

    ESP_LOGI(TAG,
             "Recurso /vel criado");

    while (1)
    {
        coap_io_process(
            ctx,
            COAP_IO_WAIT);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


 //EVENTOS WIFI

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
                coap_client_task,
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
        ESP_LOGI(TAG,
                 "Reconectando...");

        esp_wifi_connect();
    }
}


 //WIFI INIT

void wifi_init_sta(void)
{
    esp_netif_init();

    esp_event_loop_create_default();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg =
        WIFI_INIT_CONFIG_DEFAULT();

    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(
        WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &wifi_event_handler,
        NULL,
        NULL);

    esp_event_handler_instance_register(
        IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &wifi_event_handler,
        NULL,
        NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);

    esp_wifi_set_config(
        WIFI_IF_STA,
        &wifi_config);

    esp_wifi_start();
}


 //APP MAIN

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());

    wheel_Init();

    xTaskCreate(
        task_encoder,
        "encoder_task",
        2048,
        NULL,
        5,
        NULL);

    wifi_init_sta();

    ESP_LOGI(TAG,
             "Sistema iniciado");
}