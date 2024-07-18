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

#include "run.h"
#include "sensor.h"

 RUN g_run;


 void run_interrupt_control(void){
 	if(g_run.move_flag_get()){
 		g_run.interrupt_control();
 	}
 }

 void run_intterupt_right(void){
 	if (g_run.move_flag_get()) {
 		g_run.step_inc_right();
 	}
 }

 void run_intterupt_left(void){
 	if (g_run.move_flag_get()) {
 		g_run.step_inc_left();
 	}
 }



void RUN::interrupt_control(void)
{
	double spd_r, spd_l;

	speed = speed + accel;

	if(speed > max_speed){
		speed = max_speed;
	}
	if(speed < min_speed){
		speed = min_speed;
	}

	if(g_sensor.con_wall.enable == true){
		g_sensor.con_wall.p_error = g_sensor.con_wall.error;

		if ((g_sensor.sen_r.is_control == true) && (g_sensor.sen_l.is_control == true)) {
			g_sensor.con_wall.error = g_sensor.sen_r.error - g_sensor.sen_l.error;
		} else {
			g_sensor.con_wall.error = 2.0 * (g_sensor.sen_r.error - g_sensor.sen_l.error);
		}

		g_sensor.con_wall.diff = g_sensor.con_wall.error - g_sensor.con_wall.p_error;
		g_sensor.con_wall.sum += g_sensor.con_wall.error;
		if (g_sensor.con_wall.sum > g_sensor.con_wall.sum_max) {
			g_sensor.con_wall.sum = g_sensor.con_wall.sum_max;
		} else if (g_sensor.con_wall.sum < (-g_sensor.con_wall.sum_max)) {
			g_sensor.con_wall.sum = -g_sensor.con_wall.sum_max;
		}
		g_sensor.con_wall.control = 0.001 * speed * g_sensor.con_wall.kp * g_sensor.con_wall.error;
		spd_r = speed + g_sensor.con_wall.control;
		spd_l = speed - g_sensor.con_wall.control;
	} else {
		spd_r = speed;
    	spd_l = speed;
  	}
	if (spd_r < MIN_SPEED) spd_r = MIN_SPEED;
	if (spd_l < MIN_SPEED) spd_l = MIN_SPEED;

	step_hz_r = (unsigned short)(spd_r / PULSE);
	step_hz_l = (unsigned short)(spd_l / PULSE);

}

void RUN::straight(int len, int init_speed, int l_max_speed, int finish_speed)
{
	int obj_step;

	g_device.control_stop();
	max_speed = l_max_speed;
	accel = SEARCH_ACCEL;

	if (init_speed < MIN_SPEED) {
		speed = MIN_SPEED;
		step_r = 0;
		step_l = 0;
	} else {
		speed = init_speed;
	}
	if (finish_speed < MIN_SPEED) {
		finish_speed = MIN_SPEED;
	}
	if (init_speed < finish_speed) {
		min_speed = MIN_SPEED;
	} else {
		min_speed = finish_speed;
	}
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwm_hz_set(step_hz_l,step_hz_r);

	g_sensor.con_wall.enable = true;
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.motor_move_dir(MOT_FORWARD,MOT_FORWARD);

	g_device.control_start();
	g_device.pwmtimer_start();

	motor_move = 1;
	while ((len - (step_r + step_l) / 2.0 * PULSE) >
			 (((speed * speed) - (finish_speed * finish_speed)) / (2.0 * 1000.0 * SEARCH_ACCEL))) {
		continue;
	}
	accel = -1.0 * SEARCH_ACCEL;

	while ((step_r + step_l) < obj_step) {
		continue;
	}

	if (finish_speed == SEARCH_SPEED) {
		g_device.control_stop();
		max_speed = min_speed =(double)finish_speed;
		speed = (double)finish_speed;
		accel = 0.0;
		step_r = 0;
		step_l = 0;
		g_device.control_start();
	} else {
		motor_move = 0;
	}
}

void RUN::accelerate(int len, int finish_speed)
{
	int obj_step;
	g_device.control_stop();
	max_speed = finish_speed;
	accel = SEARCH_ACCEL;
	speed = min_speed = MIN_SPEED;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwm_hz_set(step_hz_l,step_hz_r);
	step_r = 0;
	step_l = 0;
	g_sensor.con_wall.enable = true;
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.motor_move_dir(MOT_FORWARD,MOT_FORWARD);
	g_device.pwmtimer_start();
	g_device.control_start();
	motor_move = 1;

	while ((step_r + step_l) < obj_step) {
		continue;
	}
	g_device.control_stop();
	max_speed = min_speed = (double)finish_speed;
	speed = (double)finish_speed;
	accel = 0.0;
	step_r = 0;
	step_l = 0;
	g_device.control_start();

}

void RUN::one_step(int len, int tar_speed)
{
	int obj_step;
	g_device.control_stop();
	max_speed = tar_speed;
	speed = min_speed = tar_speed;
	accel = 0.0;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwm_hz_set(step_hz_l,step_hz_r);
	g_sensor.con_wall.enable = true;
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.motor_move_dir(MOT_FORWARD,MOT_FORWARD);
	g_device.control_start();

	while ((step_r + step_l) < obj_step) {
		continue;
	}
	g_device.control_stop();
	max_speed = min_speed = (double)tar_speed;
	speed = (double)tar_speed;
	accel = 0.0;
	step_r = 0;
	step_l = 0;
	g_device.control_start();

}

void RUN::decelerate(int len, int tar_speed)
{
	int obj_step;
	g_device.control_stop();
	max_speed = tar_speed;
	accel = 0.0;
	speed = min_speed = tar_speed;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwm_hz_set(step_hz_l,step_hz_r);
	g_sensor.con_wall.enable = true;
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.motor_move_dir(MOT_FORWARD,MOT_FORWARD);
	g_device.control_start();

	while ((len - (step_r + step_l) / 2.0 * PULSE) >
		(((speed * speed) - (MIN_SPEED * MIN_SPEED)) / (2.0 * 1000.0 * SEARCH_ACCEL))) {
		continue;
	}
	accel = -1.0 * SEARCH_ACCEL;
	min_speed = MIN_SPEED;

	while((step_r + step_l) < obj_step){
		continue;
	}
	motor_move = 0;
	g_device.pwmtimer_stop();
	delay(300);
}

void RUN::rotate(t_direction dir, int times)
{
	int obj_step;
	g_device.control_stop();
	max_speed = SEARCH_SPEED;
	accel =TURN_ACCEL;
	speed = min_speed = MIN_SPEED;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwm_hz_set(step_hz_l,step_hz_r);
	step_r = 0;
	step_l = 0;
	g_sensor.con_wall.enable = false;
	obj_step = (int)(TREAD_WIDTH * PI / 4.0 * (float)times * 2.0 / PULSE);

	switch (dir) {
		case right:
			g_device.motor_move_dir(MOT_FORWARD,MOT_BACK);
			g_device.pwmtimer_start();
			motor_move = 1;
			break;
		case left:
			g_device.motor_move_dir(MOT_BACK,MOT_FORWARD);
			g_device.pwmtimer_start();
			motor_move = 1;
    		break;
		default:
			motor_move = 0;
			break;
	}
	g_device.control_start();

	while (((obj_step - (step_r + step_l)) * PULSE) >
		(((speed * speed) - (MIN_SPEED * MIN_SPEED)) / (2.0 * 1000.0 * TURN_ACCEL))) {
		continue;
	}
	accel = -1.0 * TURN_ACCEL;

	while ((step_r + step_l) < obj_step) {
		continue;
	}

	motor_move = 0;
	g_device.pwmtimer_stop();
	delay(300);
}

bool RUN::move_flag_get(void)
{
	return motor_move;
}

void RUN::step_inc_right(void)
{
	step_r = step_r+1;
	g_device.pwm_hz_set_right(step_hz_r);
}

void RUN::step_inc_left(void)
{
	step_l = step_l+1;
	g_device.pwm_hz_set_left(step_hz_l);
}

