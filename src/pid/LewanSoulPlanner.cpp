/*
 * LewanSoulPlanner.cpp
 *
 *  Created on: May 11, 2020
 *      Author: hephaestus
 */

#include "LewanSoulPlanner.h"

//#include <FlashStorage.h>
int32_t startingAngles []= {-9000, 8614, 3371};
int32_t upperAngles []= {9000,10000,6300};
int32_t lowerAngles []= {-9000,-4500,-9000};
char command[50]={0,};
int commandIndex =0;
long startProvisioning=0;
int IDToSet =0;

//FlashStorage(cal1, float);
//FlashStorage(cal2, float);
//FlashStorage(cal3, float);
//FlashStorage(lock, int);

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
		int32_t pos = startingAngles[i]-motors[i]->pos_read();
		if(pos!=0){
			Serial.println("Settling Error of"+String(pos)+", re-calibrating on index "+String(motors[i]->_id));
			return false;
		}
		Serial.println("\r\nCalibrated "+String(motors[i]->_id));
	}
	delay(1000);
	read();
	Serial.println("\r\nStarting the motor motion after calibration");
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
			Serial.println("Capping upper setpoint "+String(target)+" to "+String(motors[i]->getMaxCentDegrees()));
			target=motors[i]->getMaxCentDegrees();
			upstream[i]->setSetpoint(target);
		}
		if(target<motors[i]->getMinCentDegrees()){
			Serial.println("Capping lower setpoint "+String(target)+" to "+String(motors[i]->getMinCentDegrees()));
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
			Serial.print(command[commandIndex]);
			if(command[commandIndex]=='\r'|| command[commandIndex]=='\n'||command[commandIndex]==10|| command[commandIndex]==13){
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
				Serial.print("\r\nGOT as hex:[ ");
				for(int i=0;i<commandIndex;i++){
					Serial.print((i!=0?" 0x":", 0x")+String(command[i], HEX));
				}
				Serial.print("]\r\n");
				Serial.print("\r\nGOT as ASCII:[ ");
				for(int i=0;i<commandIndex;i++){
					if (i!=0)Serial.print(", ");
					Serial.print(command[i]);
				}
				Serial.print("]\r\n");
				Serial.println("\r\nProcessing ID "+String(IDToSet));
				int read = servoBus.id_read();
				Serial.println("\r\nCurrent ID was "+String(read));
				int timeout =0;
				do{
					servoBus.setRetryCount(5);
					servoBus.debug(true);
					servoBus.id_write(IDToSet);
					read=servoBus.id_read();
					servoBus.debug(false);
					if(read!=IDToSet || read==0){
						Serial.println("\r\nERROR ID set failed");
						delay(500);
					}else{
						Serial.println("\r\nCurrent ID is now "+String(read));
					}
					timeout++;
				}while(( read!=IDToSet) && timeout<10);
			}else{
				int read = servoBus.id_read();
				if(read!=0)
					Serial.println("\r\nCurrent ID "+String(read));
				else
					Serial.println("\r\nFAULT motor not responding ");
			}

		}else{
			Serial.println("\r\nProvision timeout! Format:\n\r\nID 2<NL> \n\r\n will set the one connected motor ID to 2\n\r\nwhere <NL> is the new line char" );
		}
		commandIndex=0;
		IDToSet=0;
		state=WaitForHomePress;
		break;
	case StartupSerial:

		servoBus.begin(LEWAN_SERIAL_PORT,
				TX_LEWAN_SOUL, // on TX pin 1
				LEWAN_TX_ENABLE_PIN); // use pin 2 as the TX flag for buffer
		servoBus.debug(true);
		servoBus.retry = 0; // enforce synchronous real time
		//servoBus.debug(true);
		Serial.println("\r\nBeginning Trajectory Planner");
		pinMode(HOME_SWITCH_PIN, INPUT_PULLUP);
		pinMode(MOTOR_DISABLE, INPUT_PULLUP);
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
			Serial.println("\r\nUSER Button PRESSED!");
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
		read();
		if(millis()-timeOfLastBlink>200){
			timeOfLastBlink=millis();
			blinkState=!blinkState;
			digitalWrite(INDICATOR, blinkState?1:0);
		}
		if(millis()-timeOfHomingPressed>300 && !digitalRead(HOME_SWITCH_PIN)){// wait for motors to settle, debounce
			timeOfHomingPressed = millis();
			digitalWrite(INDICATOR, 0);
			if(calibrate()){
				state =WaitingForCalibrationToFinish;
			}else{
				Serial.println("\r\nCal Error");
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
		Serial.println("\r\nStarting the planner");
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
		if(digitalRead(MOTOR_DISABLE)){
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
		if(digitalRead(MOTOR_DISABLE)){
			state=running;
		}else
			if(!digitalRead(HOME_SWITCH_PIN)){
				state=WaitForHomePress;
			}
		break;
	}
}

