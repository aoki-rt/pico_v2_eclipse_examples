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


#include "flash.h"

#include "misc.h"

MISC g_misc;

short MISC::button_inc(short data, short limit, short limit_data)
{
	data++;
	if (data > limit) {
		data = limit_data;
	}
	g_device.buzzer_enable(INC_FREQ);
	delay(30);
	g_device.buzzer_disable();
	return data;
}

void MISC::button_ok(void)
{
	g_device.buzzer_enable(DEC_FREQ);
	delay(80);
	g_device.buzzer_enable(INC_FREQ);
	delay(80);
	g_device.buzzer_disable();
}

void MISC::appeal_goal(void)
{
	unsigned char led_data;

	int wtime = 100;

	g_device.motor_disable();
	g_flash.map_write();

	for (int j = 0; j < 10; j++) {
		led_data = 1;
		for (int i = 0; i < 4; i++) {
			g_device.LED_set(led_data);
			led_data <<= 1;
			delay(wtime);
		}
		led_data = 8;
		for (int i = 0; i < 4; i++) {
			g_device.LED_set(led_data);
			led_data >>= 1;
			delay(wtime);
		}
		wtime -= 5;
	}
	delay(500);
	g_device.motor_enable();
}


