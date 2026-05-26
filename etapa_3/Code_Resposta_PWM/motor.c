#include "motor.h"

#include "driver/ledc.h"
#include "driver/gpio.h"

#include "esp_log.h"

static const char *TAG = "MOTOR";

// PWM -> ENB
#define MOTOR_PWM_GPIO 18

// direção
#define MOTOR_IN3_GPIO 2
#define MOTOR_IN4_GPIO 21

#define PWM_FREQ 1000

#define PWM_RES LEDC_TIMER_10_BIT

// =====================================================
// INIT
// =====================================================

void motor_Init(void)
{
    // reset GPIOs
    gpio_reset_pin(MOTOR_IN3_GPIO);

    gpio_reset_pin(MOTOR_IN4_GPIO);

    // saídas
    gpio_set_direction(
        MOTOR_IN3_GPIO,
        GPIO_MODE_OUTPUT);

    gpio_set_direction(
        MOTOR_IN4_GPIO,
        GPIO_MODE_OUTPUT);

    // sentido
    gpio_set_level(MOTOR_IN3_GPIO, 1);

    gpio_set_level(MOTOR_IN4_GPIO, 0);

    ESP_LOGI(TAG, "IN3 = HIGH");

    ESP_LOGI(TAG, "IN4 = LOW");

    // timer PWM
    ledc_timer_config_t timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = PWM_RES,
        .freq_hz          = PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };

    ESP_ERROR_CHECK(
        ledc_timer_config(&timer));

    // canal PWM
    ledc_channel_config_t channel = {
        .gpio_num   = MOTOR_PWM_GPIO,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel    = LEDC_CHANNEL_0,
        .timer_sel  = LEDC_TIMER_0,
        .duty       = 0,
        .hpoint     = 0
    };

    ESP_ERROR_CHECK(
        ledc_channel_config(&channel));

    ESP_LOGI(TAG,
             "PWM inicializado GPIO %d",
             MOTOR_PWM_GPIO);
}

// =====================================================
// DUTY
// =====================================================

void motor_SetDuty(int duty)
{
    // proteção
    if (duty < 0)
        duty = 0;

    if (duty > 1023)
        duty = 1023;

    ledc_set_duty(
        LEDC_LOW_SPEED_MODE,
        LEDC_CHANNEL_0,
        duty);

    ledc_update_duty(
        LEDC_LOW_SPEED_MODE,
        LEDC_CHANNEL_0);

    ESP_LOGI(TAG, "Duty = %d", duty);
}

// =====================================================
// STOP
// =====================================================

void motor_Stop(void)
{
    motor_SetDuty(0);
}