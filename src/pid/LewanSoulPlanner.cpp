/*
 * LewanSoulPlanner.cpp
 *
 *  Created on: May 11, 2020
 *      Author: hephaestus
 */

#include "LewanSoulPlanner.h"
int32_t startingAngles []= {-9000,0,9000};
int32_t upperAngles []= {9000,10000,9000};
int32_t lowerAngles []= {-9000,-2512,-9000};

LewanSoulPlanner::LewanSoulPlanner(int n, SerialMotor ** list) {
	num=n;
	motors=new LX16AServo*[num];
	for(int i=0;i<num;i++)
		motors[i]= new LX16AServo(&servoBus, i+1);
	upstream=list;

}

LewanSoulPlanner::~LewanSoulPlanner() {
		for(int i=0;i<num;i++)
			delete(motors[i]);
		delete(motors);
}
bool LewanSoulPlanner::calibrate(){
	for(int i=0;i<num;i++){
		Serial.println("Calibrating "+String(i));
		motors[i]->calibrate(startingAngles[i]);
		motors[i]->angle_limit(lowerAngles[i],upperAngles[i]);
		int32_t pos = motors[i]->pos_read();
		if(pos!=startingAngles[i]){
			return false;
		}
	}
	read();
	Serial.println("Starting the motor motion after calibration");
	for(int i=0;i<num;i++){
		upstream[i]->setSetpoint(upstream[i]->getPosition());// quick set to current
		upstream[i]->startInterpolationDegrees(0,2000,SIN);
		motors[i]->move_time_and_wait_for_sync(0, 2000);
	}
	servoBus.move_sync_start();
	return true;
}
void LewanSoulPlanner::read(){
	for(int i=0;i<num;i++){
		int32_t pos = motors[i]->pos_read();
		upstream[i]->setCachedPosition(pos);
	}
}

void LewanSoulPlanner::update(){
	long start = millis();
	servoBus.move_sync_start();
	read();
	for(int i=0;i<num;i++){
		int32_t target = upstream[i]->getSetPoint();
		if(target>motors[i]->getMaxCentDegrees()){
			target=motors[i]->getMaxCentDegrees();
			upstream[i]->setSetpoint(target);
		}
		if(target<motors[i]->getMinCentDegrees()){
			target=motors[i]->getMinCentDegrees();
			upstream[i]->setSetpoint(target);
		}
		int timingOffset = millis()-start;
		motors[i]->move_time_and_wait_for_sync(target, plannerLoopTimeMs+timingOffset+2);
	}
}
void LewanSoulPlanner::loop(){

	switch(state){
	case StartupSerial:
		servoBus.begin(&Serial1, 1, // on TX pin 1
				2); // use pin 2 as the TX flag for buffer
		servoBus.retry = 2; // enforce synchronous real time
		//servoBus.debug(true);
		Serial.println("Beginning Trajectory Planner");
		pinMode(HOME_SWITCH_PIN, INPUT_PULLUP);
		for(int i=0;i<num;i++)
				motors[i]->disable();
		state=WaitForHomePress;
		break;
	case WaitForHomePress:
		//read();
		if(!digitalRead(HOME_SWITCH_PIN)){
			timeOfHomingPressed = millis();
			state = WaitForHomeRelease;
			Serial.println("HOME PRESSED!");
		}
		break;
	case WaitForHomeRelease:
		//read();
		if(millis()-timeOfHomingPressed>500){// wait for motors to settle
			timeOfHomingPressed = millis();
			if(calibrate()){
				state =WaitingForCalibrationToFinish;
			}else{
				Serial.println("Calibration Error");
			}
		}
		break;
	case WaitingForCalibrationToFinish:
		read();
		if(!digitalRead(HOME_SWITCH_PIN)){
			//still on the homing switch
			break;
		}
		for(int i=0;i<num;i++){
			if(!upstream[i]->isInterpolationDone())
				break;// not done yet
		}
		Serial.println("Starting the planner");
		state=running;
		break;
	case WaitingToRun:
		if(millis()-timeOfLastRun>plannerLoopTimeMs){
			state=running;
			timeOfLastRun=millis();
		}
		break;
	case running:
		update();
		state=WaitingToRun;
		break;
	}
}

