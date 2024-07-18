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

#include "adjust.h"
#include "device.h"
#include "fast.h"
#include "run.h"
#include "search.h"
#include "sensor.h"
#include "map_manager.h"
#include "misc.h"
#include "flash.h"
#include "parameter.h"


signed char g_mode;

void mode_exec(int mode)
{
	g_device.motor_enable();
	g_device.pwmtimer_stop();

	delay(1000);

	switch (mode) {
    	case 1:
    		g_search.lefthand();
    		break;
    	case 2:  //adach method
    		g_map_control.positionInit();
			g_search.adachi(g_map_control.goal_mx, g_map_control.goal_my);
			g_run.rotate(right, 2);
			g_map_control.nextDir(right);
			g_map_control.nextDir(right);
			g_misc.appeal_goal();
			g_search.adachi(0, 0);
			g_run.rotate(right, 2);
			g_map_control.nextDir(right);
			g_map_control.nextDir(right);
			g_flash.map_write();
			break;
    	case 3:  //shortest running
			g_flash.map_copy();
			g_map_control.positionInit();
			g_fast.run(g_map_control.goal_mx, g_map_control.goal_my);
			g_run.rotate(right, 2);
			g_map_control.nextDir(right);
			g_map_control.nextDir(right);
			g_misc.appeal_goal();
			g_fast.run(0, 0);
			g_run.rotate(right, 2);
			g_map_control.nextDir(right);
			g_map_control.nextDir(right);
			break;
    	case 4:
    		break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			g_device.motor_disable();
			g_adjust.menu();  //to adjust menu
			break;
		default:
			break;
	}
	g_device.motor_disable();
}

extern "C" void app_main(void)
{
	delay(1000);
	spiffs_begin();
	g_device.all_init();
	g_map_control.goal_mx = GOAL_X;
	g_map_control.goal_my = GOAL_Y;

	g_device.buzzer_disable();
	g_mode = 1;

    while (true) {
    	g_device.LED_set((int)g_mode);
    	switch (g_device.switch_get()) {
    	    case SW_RM:
    	    	g_mode = g_misc.button_inc(g_mode, 15, 1);
    	    	break;
    		case SW_LM:
    			g_misc.button_ok();
    			mode_exec(g_mode);
    	}
    	delay(10);
    }
}
