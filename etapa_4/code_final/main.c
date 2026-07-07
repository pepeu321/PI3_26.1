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
#include "motor.h"

#define WIFI_SSID "lpae_wifi"
#define WIFI_PASS "esp-8266"

#define TS_CONTROLE_S 0.1f
#define KP_PI 6.04f
#define KI_PI 30.5f

#define DUTY_MIN 0.0f
#define DUTY_MAX 1023.0f
#define DUTY_MIN_MOVIMENTO 300.0f

static bool coap_task_started = false;
static const char *TAG = "MAIN";

static float ref_rpm = 0.0f;
static float erro_integral = 0.0f;
static float duty_atual = 0.0f;
static int direcao_motor = 1;


void task_encoder(void *pvParameters)
{
    while (1)
    {
        wheel_UpdateRPM();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void task_controle_PI(void *pvParameters)
{
    while (1)
    {
        float rpm = wheel_GetRPM();
        float erro = ref_rpm - rpm;

        float duty_unsat =
            KP_PI * erro +
            KI_PI * erro_integral;

        float duty = duty_unsat;

        if (duty > DUTY_MAX)
            duty = DUTY_MAX;

        if (duty < DUTY_MIN)
            duty = DUTY_MIN;

        if (ref_rpm > 0.0f && duty > 0.0f && duty < DUTY_MIN_MOVIMENTO)
        {
            duty = DUTY_MIN_MOVIMENTO;
        }

        if (ref_rpm > 0.0f && duty_unsat < DUTY_MAX)
        {
            erro_integral += erro * TS_CONTROLE_S;
        }

        if (ref_rpm <= 0.0f)
        {
            erro_integral = 0.0f;
            duty = 0.0f;
        }

        duty_atual = duty;

        motor_SetDuty((int)duty);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

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

        ref_rpm = atof(buffer);

        if (ref_rpm < 0.0f)
            ref_rpm = 0.0f;

        if (ref_rpm == 0.0f)
        {
            erro_integral = 0.0f;
            duty_atual = 0.0f;
            motor_Stop();
        }

        ESP_LOGI(TAG, "Referencia recebida: %.2f RPM", ref_rpm);
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
    char resp[128];

    float rpm = wheel_GetRPM();
    float erro = ref_rpm - rpm;

    sprintf(
        resp,
        "REF: %.2f | RPM: %.2f | ERRO: %.2f | DUTY: %.0f | DIR: %d",
        ref_rpm,
        rpm,
        erro,
        duty_atual,
        direcao_motor);

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

        motor_Stop();
        erro_integral = 0.0f;
        duty_atual = 0.0f;

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
    coap_startup();

    coap_context_t *ctx = coap_new_context(NULL);

    if (ctx == NULL)
    {
        ESP_LOGE(TAG, "Erro ao criar contexto CoAP");
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

    coap_resource_t *resource_vel =
        coap_resource_init(
            coap_make_str_const("vel"),
            0);

    coap_resource_t *resource_dir =
        coap_resource_init(
            coap_make_str_const("dir"),
            0);

    coap_register_handler(
        resource_vel,
        COAP_REQUEST_POST,
        hnd_post_vel);

    coap_register_handler(
        resource_vel,
        COAP_REQUEST_GET,
        hnd_get_vel);

    coap_register_handler(
        resource_dir,
        COAP_REQUEST_POST,
        hnd_post_dir);

    coap_add_resource(ctx, resource_vel);
    coap_add_resource(ctx, resource_dir);

    ESP_LOGI(TAG, "Recursos /vel e /dir criados");

	while (1)
	{
	    coap_io_process(ctx, 100);
	    vTaskDelay(pdMS_TO_TICKS(10));
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

        ESP_LOGI(
            TAG,
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
}