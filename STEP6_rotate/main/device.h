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

void delay(int timer);

extern "C"{
extern void runInterruptControl(void);
extern void runIntteruptRight(void);
extern void runIntteruptLeft(void);
}


class DEVICE
{
public:
    gptimer_handle_t timer0 = NULL;

    mcpwm_timer_handle_t pwmtimer_r = NULL;
    mcpwm_cmpr_handle_t comparator_r = NULL;
    mcpwm_oper_handle_t pwmoprator_r = NULL;

    mcpwm_timer_handle_t pwmtimer_l = NULL;
    mcpwm_cmpr_handle_t comparator_l = NULL;
    mcpwm_oper_handle_t pwmoprator_l = NULL;

    void LEDSet(int led_data);
    unsigned char switchGet(void);

	void ledcSetup(ledc_channel_t ledc_num,int hz,int resolution);
	void ledcWrite (ledc_channel_t ledc_num,int duty);
	void ledcWritetone(ledc_channel_t ledc_num,int hz);
	void ledcAttachPin(int io_num,ledc_channel_t ledc_num);
    void buzzerEnable(short f);
    void buzzerDisable(void);

    void initAll(void);


    void timerBegin(gptimer_handle_t *g_timer,int resolution_hz  ,gptimer_count_direction_t counter);
    void timerAttachInterrupt(gptimer_handle_t timer, bool (*fn)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data));
    void timerAlarmWrite(gptimer_handle_t timer, int period, bool reload);
    void timerAlarmEnable(gptimer_handle_t timer);


    void pwmtimerBegin(mcpwm_timer_handle_t * pwmtimer, int resolution_hz,mcpwm_oper_handle_t *pwmoprator, mcpwm_cmpr_handle_t * comparator);
    void pwmtimerAttachInterrupt(mcpwm_timer_handle_t timer, bool (*fn)(mcpwm_timer_handle_t timer, const mcpwm_timer_event_data_t *edata, void *user_data));
    void pwmtimerAttachPin(mcpwm_oper_handle_t pwmoprator , mcpwm_cmpr_handle_t  comparator,int io_num);

    void pwmtimerStart(void);
    void pwmtimerStop(void);
    void motorEnable(void);
    void motorDisable(void);
    void motorMoveDir(t_CW_CCW left_CW, t_CW_CCW right_CW);
	void pwmHzSet(unsigned short step_hz_l, unsigned short step_hz_r);
	void pwmHzSetRight(int step_hz_r);
	void pwmHzSetLeft(int step_hz_l);

private:

};

extern DEVICE g_device;


#endif /* MAIN_DEVICE_H_ */
