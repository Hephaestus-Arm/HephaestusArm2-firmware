/*
 * SerialMotor.h
 *
 *  Created on: May 11, 2020
 *      Author: hephaestus
 */

#ifndef SRC_PID_SERIALMOTOR_H_
#define SRC_PID_SERIALMOTOR_H_

#include "PIDMotor.h"

class SerialMotor: public PIDMotor {
private:
	int64_t CachedPosition=0;
	int64_t offset =0;
public:
	SerialMotor();
	~SerialMotor();
	void setCachedPosition(int64_t current){
		CachedPosition=current;
	}
	int64_t getPosition(){
		return CachedPosition-offset;
	}
	void setOutput(int32_t out){
		//dummy, not used
	}
	void overrideCurrentPositionHardware(int64_t val){
		offset=val-CachedPosition;
	}
	//This function should analogRead the current sense from the motor driver
	//and convert the value to current in milliamps
	double calcCur(void){
		return 0;
	}
};

#endif /* SRC_PID_SERIALMOTOR_H_ */
