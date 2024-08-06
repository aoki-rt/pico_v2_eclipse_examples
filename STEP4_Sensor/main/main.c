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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_cali.h"
#include "driver/gptimer.h"

#define SLED_F 2
#define SLED_S 1

#define AD4 ADC_CHANNEL_4	//GPIO5
#define AD3 ADC_CHANNEL_3	//GPIO4
#define AD2 ADC_CHANNEL_6	//GPIO7
#define AD1 ADC_CHANNEL_5	//GPIO6
#define AD0 ADC_CHANNEL_7	//GPIO8

#define ADC_ATTEN           ADC_ATTEN_DB_12

volatile short g_sensor_value_fr;
volatile short g_sensor_value_fl;
volatile short g_sensor_value_r;
volatile short g_sensor_value_l;
volatile short g_battery_value;

gptimer_handle_t gptimer = NULL;

adc_oneshot_unit_handle_t adc1_handle = NULL;
adc_cali_handle_t adc1_cali_chan0_handle = NULL;

static bool IRAM_ATTR onTimer1Cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
	static char cnt = 0;
	int temp_r, temp_l,temp,v_temp;

	switch (cnt) {
	case 0:
		adc_oneshot_read(adc1_handle, AD4, &temp_r);
	    adc_oneshot_read(adc1_handle, AD1, &temp_l);
	    gpio_set_level(SLED_F,1);	//LED on
	    for (int i = 0; i < 10; i++){
			asm("nop \n");
		}
	    adc_oneshot_read(adc1_handle, AD4, &temp);
	    g_sensor_value_fr = temp - temp_r;
	    adc_oneshot_read(adc1_handle, AD1, &temp);
	    g_sensor_value_fl = temp - temp_l;
	    gpio_set_level(SLED_F,0);	//LED off
	    break;
	case 1:
		adc_oneshot_read(adc1_handle, AD3, &temp_r);
		adc_oneshot_read(adc1_handle, AD2, &temp_l);
		gpio_set_level(SLED_S,1);	//LED on
	    for (int i = 0; i < 10; i++){
			asm("nop \n");
		}
	    adc_oneshot_read(adc1_handle, AD3, &temp);
	    g_sensor_value_r = temp - temp_r;
	    adc_oneshot_read(adc1_handle, AD2, &temp);
	    g_sensor_value_l = temp - temp_l;
	    gpio_set_level(SLED_S,0);	//LED off
	    adc_oneshot_read(adc1_handle, AD0, &temp);
	    adc_cali_raw_to_voltage(adc1_cali_chan0_handle,temp, &v_temp);
	    g_battery_value = (double)v_temp / 1.0 * (1.0 + 10.0);
	    break;
	default:
	    printf("error¥n¥r");
	    break;
	}
	cnt++;
	if (cnt == 2) cnt = 0;

	return 0;
}

void initAll(void){
	//init gpio
	gpio_reset_pin(SLED_F);
	gpio_reset_pin(SLED_S);
	gpio_set_direction(SLED_F, GPIO_MODE_OUTPUT);
	gpio_set_direction(SLED_S, GPIO_MODE_OUTPUT);

	//interrupt timer
	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 1000000// 1MHz(1us)
	};
	gptimer_new_timer(&timer_config, &gptimer);

	gptimer_event_callbacks_t cbs = {
		.on_alarm = onTimer1Cb
	};
	gptimer_register_event_callbacks(gptimer, &cbs, NULL);

	gptimer_alarm_config_t alarm_config1 = {
		.reload_count = 0,
		.alarm_count = 500, //500*1us=500us(2kHz)
		.flags.auto_reload_on_alarm = true,
	};
	gptimer_set_alarm_action(gptimer, &alarm_config1);

	gptimer_enable(gptimer);


	//sensor ad
	adc_oneshot_unit_init_cfg_t init_config1 = {
	    .unit_id = ADC_UNIT_1,
	};
	adc_oneshot_new_unit(&init_config1, &adc1_handle);
	
	adc_oneshot_chan_cfg_t config = {
	    .bitwidth = ADC_BITWIDTH_DEFAULT,
	    .atten = ADC_ATTEN,
	};
	adc_oneshot_config_channel(adc1_handle, AD0, &config);
	adc_oneshot_config_channel(adc1_handle, AD1, &config);
	adc_oneshot_config_channel(adc1_handle, AD2, &config);
	adc_oneshot_config_channel(adc1_handle, AD3, &config);
	adc_oneshot_config_channel(adc1_handle, AD4, &config);

	adc_cali_curve_fitting_config_t cali_config = {
		.unit_id	=	ADC_UNIT_1,
		.chan 		=	AD0,
		.atten		=	ADC_ATTEN,
		.bitwidth	=	ADC_BITWIDTH_DEFAULT,
	};

	adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_chan0_handle);

}

void app_main(void)
{
	initAll();

	gptimer_start(gptimer);

    while (true) {
    	printf("r_sen	is %d\n\r", g_sensor_value_r);
    	printf("fr_sen	is %d\n\r", g_sensor_value_fr);
    	printf("fl_sen	is %d\n\r", g_sensor_value_fl);
    	printf("l_sen	is %d\n\r", g_sensor_value_l);
    	printf("VDD	is %dmV\n\r", g_battery_value);
    	vTaskDelay(100 / portTICK_PERIOD_MS);

    }
}

