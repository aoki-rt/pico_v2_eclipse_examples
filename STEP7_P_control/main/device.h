// Copyright 2024 RT Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MAIN_DEVICE_H_
#define MAIN_DEVICE_H_


extern "C"{
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/mcpwm_types.h"
#include "driver/mcpwm_prelude.h"
#include "driver/gptimer.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"
}

#include "mytypedef.h"
#include "parameter.h"



#define LED0 42
#define LED1 41
#define LED2 15
#define LED3 16

#define BLED0 18
#define BLED1 17

#define SW_L 13
#define SW_R 14

#define BUZZER 40

#define MOTOR_EN 12
#define CW_R 21
#define CW_L 11
#define PWM_R 45
#define PWM_L 46

#define SW_LM 1
//#define SW_CM 2
#define SW_RM 4

#define BUZZER_CH LEDC_CHANNEL_0

#define SLED_F 2
#define SLED_S 1

#define AD4 ADC_CHANNEL_4	//GPIO5
#define AD3 ADC_CHANNEL_3	//GPIO4
#define AD2 ADC_CHANNEL_6	//GPIO7
#define AD1 ADC_CHANNEL_5	//GPIO6
#define AD0 ADC_CHANNEL_7	//GPIO8

#define ADC_ATTEN           ADC_ATTEN_DB_12

void delay(int timer);

extern "C"{
extern void run_interrupt_control(void);
extern void run_intterupt_right(void);
extern void run_intterupt_left(void);
extern void sensor_interrupt_control(void);
}

class DEVICE
{
public:
    gptimer_handle_t timer0 = NULL;
    gptimer_handle_t timer1 = NULL;

    adc_oneshot_unit_handle_t adc1_handle = NULL;
    adc_cali_handle_t adc1_cali_handle = NULL;

    mcpwm_timer_handle_t pwmtimer_r = NULL;
    mcpwm_cmpr_handle_t comparator_r = NULL;
    mcpwm_oper_handle_t pwmoprator_r = NULL;

    mcpwm_timer_handle_t pwmtimer_l = NULL;
    mcpwm_cmpr_handle_t comparator_l = NULL;
    mcpwm_oper_handle_t pwmoprator_l = NULL;

    void LED_set(int led_data);
    unsigned char switch_get(void);

	void ledc_setup(ledc_channel_t ledc_num,int hz,int resolution);
	void ledc_write (ledc_channel_t ledc_num,int duty);
	void ledc_writetone(ledc_channel_t ledc_num,int hz);
	void ledc_attach_pin(int io_num,ledc_channel_t ledc_num);
    void buzzer_enable(short f);
    void buzzer_disable(void);

    void all_init(void);


    void timer_begin(gptimer_handle_t *g_timer,int resolution_hz  ,gptimer_count_direction_t counter);
    void timer_attach_interrupt(gptimer_handle_t timer, bool (*fn)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data));
    void timer_alarm_write(gptimer_handle_t timer, int period, bool reload);
    void timer_alarm_enable(gptimer_handle_t timer);

    void adc1_begin(adc_oneshot_unit_handle_t *adc1);
    void adc1_attach_pin(adc_oneshot_unit_handle_t adc1,int io_num);
    void adc1_cali(int io_num);


    void pwmtimer_begin(mcpwm_timer_handle_t * pwmtimer, int resolution_hz,mcpwm_oper_handle_t *pwmoprator, mcpwm_cmpr_handle_t * comparator);
    void pwmtimer_attach_interrupt(mcpwm_timer_handle_t timer, bool (*fn)(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data));
    void pwmtimer_attach_pin(mcpwm_oper_handle_t pwmoprator , mcpwm_cmpr_handle_t  comparator,int io_num);

    void pwmtimer_start(void);
    void pwmtimer_stop(void);
    void motor_enable(void);
    void motor_disable(void);
    void motor_move_dir(t_CW_CCW left_CW, t_CW_CCW right_CW);
	void pwm_hz_set(int step_hz_l,int step_hz_r);
	void pwm_hz_set_right(int step_hz_r);
	void pwm_hz_set_left(int step_hz_l);

	void sensor_get_f(volatile short *fl_value, volatile short *fr_value);
	void sensor_get_s(volatile short *l_value, volatile short *r_value);
	short voltage_get(void);

private:

};

extern DEVICE g_device;

#endif /* MAIN_DEVICE_H_ */
