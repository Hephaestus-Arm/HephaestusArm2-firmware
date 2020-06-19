/*
 * LewanSoulPlanner.cpp
 *
 *  Created on: May 11, 2020
 *      Author: hephaestus
 */

#include "LewanSoulPlanner.h"
int32_t startingAngles []= {-9000, 8613, 3371};
int32_t upperAngles []= {9000,10000,6300};
int32_t lowerAngles []= {-9000,-4500,-9000};
char command[50]={NULL,};
int commandIndex =0;
long startProvisioning=0;
int IDToSet =0;
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

		motors[i]->calibrate(startingAngles[i],lowerAngles[i],upperAngles[i]);
		int32_t pos = motors[i]->pos_read();
		if(pos!=startingAngles[i]){
			return false;
		}
		Serial.println("Calibrated "+String(i+1));
	}
	delay(1000);
	read();
	Serial.println("Starting the motor motion after calibration");
	for(int i=0;i<num;i++){
		upstream[i]->setSetpoint(motors[i]->pos_read());// quick set to current
		//upstream[i]->startInterpolationDegrees(startingAngles[i],2000,SIN);
		//motors[i]->move_time_and_wait_for_sync(startingAngles[i], 2000);
	}
	//servoBus.move_sync_start();
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
	case provisioning:
		while(Serial.available()){
			command[commandIndex]=Serial.read();
			Serial.println(String((int)command[commandIndex]));
			if(command[commandIndex]=='\r'|| command[commandIndex]==10|| command[commandIndex]==13){
				state=runProvision;
			}
			commandIndex++;
		}
		if(millis()-startProvisioning>5000){
			//
			state=runProvision;
		}
		break;
	case runProvision:
		if(command[0]=='I' && command[1]=='D'){
			if( command[2]==' '){
				if(commandIndex==5){
					IDToSet=command[3]-0x30;
				}
				if(commandIndex==6){
					IDToSet=(command[4]-0x30)+(10*(command[3]-0x30));
				}
				if(commandIndex==7){
					IDToSet=(command[5]-0x30)+(10*(command[4]-0x30))+(100*(command[3]-0x30));
				}
				Serial.println("Processing ID "+String(IDToSet));
				int read = servoBus.id_read();
				Serial.println("Current ID was "+String(read));
				int timeout =0;
				do{
					servoBus.setRetryCount(5);
					servoBus.debug(true);
					servoBus.id_write(IDToSet);
					read=servoBus.id_read();
					servoBus.debug(false);
					Serial.println("Current ID is now "+String(read));
					if(read!=IDToSet){
						Serial.println("ERROR ID set failed");
						delay(500);
					}
					timeout++;
				}while(( read!=IDToSet) && timeout<10);
			}else{
				Serial.println("Current ID "+String(servoBus.id_read()));
			}

		}else{
			Serial.println("Provision timeout! Format:\nID 2<NL> \n will set the one connected motor ID to 2\nwhere <NL> is the new line char" );
		}
		commandIndex=0;
		IDToSet=0;
		state=WaitForHomePress;
		break;
	case StartupSerial:
		servoBus.begin(LEWAN_SERIAL_PORT,
				TX_LEWAN_SOUL, // on TX pin 1
				LEWAN_TX_ENABLE_PIN); // use pin 2 as the TX flag for buffer

		servoBus.retry = 2; // enforce synchronous real time
		//servoBus.debug(true);
		Serial.println("Beginning Trajectory Planner");
		pinMode(HOME_SWITCH_PIN, INPUT_PULLUP);
		for(int i=0;i<num;i++)
				motors[i]->disable();
		state=WaitForHomePress;
		pinMode(INDICATOR, OUTPUT);
		break;
	case WaitForHomePress:
		read();
		if(Serial.available()){
			state=provisioning;
			startProvisioning=millis();
		}
		if(!digitalRead(HOME_SWITCH_PIN)){
			timeOfHomingPressed = millis();
			state = WaitForHomeRelease;
			Serial.println("HOME PRESSED!");
			digitalWrite(INDICATOR, 0);
		}else{
			if(millis()-timeOfLastBlink>1000){
				timeOfLastBlink=millis();
				blinkState=!blinkState;
				digitalWrite(INDICATOR, blinkState?1:0);
				//Serial.println("Waiting for Home...");
			}
		}
		break;
	case WaitForHomeRelease:
		//read();
		if(millis()-timeOfLastBlink>200){
			timeOfLastBlink=millis();
			blinkState=!blinkState;
			digitalWrite(INDICATOR, blinkState?1:0);
		}
		if(millis()-timeOfHomingPressed>500 && !digitalRead(HOME_SWITCH_PIN)){// wait for motors to settle, debounce
			timeOfHomingPressed = millis();
			digitalWrite(INDICATOR, 0);
			if(calibrate()){
				state =WaitingForCalibrationToFinish;
			}else{
				Serial.println("Cal Error");
			}
		}
		break;
	case WaitingForCalibrationToFinish:
		read();
		if(millis()-timeOfLastBlink>50){
			timeOfLastBlink=millis();
			blinkState=!blinkState;
			digitalWrite(INDICATOR, blinkState?1:0);
		}
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
		digitalWrite(INDICATOR, 1);
		break;
	case WaitingToRun:
		if(millis()-timeOfLastRun>plannerLoopTimeMs){
			state=running;
			timeOfLastRun=millis();
		}
		break;
	case running:
		if(digitalRead(HOME_SWITCH_PIN)){
			update();
			state=WaitingToRun;
		}else{
			for(int i=0;i<num;i++)
				motors[i]->disable();
			state=disabled;
		}
		break;
	case disabled:
		read();
		if(digitalRead(HOME_SWITCH_PIN)){
			state=running;
		}
		break;
	}
}

