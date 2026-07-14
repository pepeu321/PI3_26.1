#include "motor.h"

#include "driver/ledc.h"
#include "driver/gpio.h"

#include "esp_log.h"

static const char *TAG = "MOTOR";

#define MOTOR_PWM_GPIO 18
#define MOTOR_IN3_GPIO 2
#define MOTOR_IN4_GPIO 21

#define PWM_FREQ 1000
#define PWM_RES LEDC_TIMER_10_BIT
#define PWM_MAX 1023

void motor_Init(void)
{
    gpio_reset_pin(MOTOR_IN3_GPIO);
    gpio_reset_pin(MOTOR_IN4_GPIO);

	//configura como saida
    gpio_set_direction(MOTOR_IN3_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_IN4_GPIO, GPIO_MODE_OUTPUT);

    //gpio_set_level(MOTOR_IN3_GPIO, 1);
    //gpio_set_level(MOTOR_IN4_GPIO, 0);
    
    //sentido inicial do motor
    motor_SetDirection(1);

	//config timer PWM
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = PWM_RES,
        .freq_hz = PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };

	//aplica config
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

	//config do canal PWM
    ledc_channel_config_t channel = {
        .gpio_num = MOTOR_PWM_GPIO, //pino que sai PWM
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0 //inicio do pulso
    };
	
	//Aplica config
    ESP_ERROR_CHECK(ledc_channel_config(&channel));

}

//Recebe duty do PI
void motor_SetDuty(int duty)
{
//garanti faixa do PWM	
    if (duty < 0)
        duty = 0;

    if (duty > PWM_MAX)
        duty = PWM_MAX;

//Define um novo duty
    ESP_ERROR_CHECK(
        ledc_set_duty(
            LEDC_LOW_SPEED_MODE,
            LEDC_CHANNEL_0,
            duty));

//Atualiza a saida PWM
    ESP_ERROR_CHECK(
        ledc_update_duty(
            LEDC_LOW_SPEED_MODE,
            LEDC_CHANNEL_0));

   
}

void motor_Stop(void)
{
    motor_SetDuty(0);
}

void motor_SetDirection(int direction)
{
    if (direction >= 0)
    {
        gpio_set_level(MOTOR_IN3_GPIO, 1);
        gpio_set_level(MOTOR_IN4_GPIO, 0);
    }
    else
    {
        gpio_set_level(MOTOR_IN3_GPIO, 0);
        gpio_set_level(MOTOR_IN4_GPIO, 1);
    }

    //ESP_LOGI(TAG, "Direcao = %d", direction);
}