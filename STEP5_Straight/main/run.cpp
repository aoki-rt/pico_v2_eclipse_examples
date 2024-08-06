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
#include "parameter.h"

RUN g_run;

void runInterruptControl(void){
	if(g_run.moveFlagGet()){
		g_run.interruptControl();
	}
}

void runIntteruptRight(void){
	if (g_run.moveFlagGet()) {
		g_run.stepIncRight();
	}
}

void runIntteruptLeft(void){
	if (g_run.moveFlagGet()) {
		g_run.stepIncLeft();
	}
}


RUN::RUN(){
    accel = 0.0;
    speed = MIN_SPEED;
    step_hz_r = MIN_HZ;
    step_hz_l = MIN_HZ;
    motor_move = 0;
}

void RUN::interruptControl(void){
	speed = speed + accel;

	if(speed > max_speed){
		speed = max_speed;
	}
	if(speed < min_speed){
		speed = min_speed;
	}

	step_hz_l = step_hz_r = (unsigned short)(speed / PULSE);

}

void RUN::accelerate(int len, int tar_speed)
{
	int obj_step;
	g_device.pwmtimerStop();
	max_speed = tar_speed;
	accel = 1.5;
	step_r = 0;
	step_l = 0;
	speed = min_speed = MIN_SPEED;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwmHzSet(step_hz_l,step_hz_r);
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.moveDir(MOT_FORWARD,MOT_FORWARD);

	g_device.pwmtimerStart();
	motor_move = 1;

	while ((step_r + step_l) < obj_step) {
		continue;
	}
}

void RUN::oneStep(int len, int tar_speed)
{
	int obj_step;
	max_speed = tar_speed;
	accel = 0.0;
	step_r = 0;
	step_l = 0;
	speed = min_speed = tar_speed;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwmHzSet(step_hz_l,step_hz_r);
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.moveDir(MOT_FORWARD,MOT_FORWARD);

	while ((step_r + step_l) < obj_step) {
		continue;
	}
}

void RUN::decelerate(int len, int tar_speed)
{
	int obj_step;
	max_speed = tar_speed;
	accel = 0.0;
	step_r = 0;
	step_l = 0;
	speed = min_speed = tar_speed;
	step_hz_r = step_hz_l = (unsigned short)(speed / PULSE);
	g_device.pwmHzSet(step_hz_l,step_hz_r);
	obj_step = (int)((float)len * 2.0 / PULSE);
	g_device.moveDir(MOT_FORWARD,MOT_FORWARD);

	while ((len - (step_r + step_l) / 2.0 * PULSE) >
	(((speed * speed) - (MIN_SPEED * MIN_SPEED)) / (2.0 * 1000.0 * 1.5))) {
		continue;
	}
	accel = -1.5;
	min_speed = MIN_SPEED;

	while((step_r + step_l) < obj_step){
		continue;
	}
	motor_move = 0;
	g_device.pwmtimerStop();
}

bool RUN::moveFlagGet(void)
{
	return motor_move;
}

void RUN::stepIncRight(void)
{
	step_r = step_r+1;
	g_device.pwmHzSetRight(step_hz_r);
}

void RUN::stepIncLeft(void)
{
	step_l = step_l+1;
	g_device.pwmHzSetLeft(step_hz_l);
}
