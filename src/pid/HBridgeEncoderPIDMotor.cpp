/*
 * HBridgeEncoderPIDMotor.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hephaestus
 */

#include "HBridgeEncoderPIDMotor.h"

HBridgeEncoderPIDMotor::HBridgeEncoderPIDMotor() {
	// TODO Auto-generated constructor stub
	this->directionPin = -1;
	this->pwmPin=-1;
	setOutputBoundingValues(-HBRIDGE_MAX, HBRIDGE_MAX, 0, HBRIDGE_DEADBAND, HBRIDGE_DEADBAND,
			16.0 * // Encoder CPR
			50.0 * // Motor Gear box ratio
			1.0 * // motor to wheel stage ratio
			(1.0 / 360.0) * // degrees per revolution
			4,
			186.0 * 60.0 * 360.0,0);
	encoder=NULL;
}

HBridgeEncoderPIDMotor::~HBridgeEncoderPIDMotor() {
	// TODO Auto-generated destructor stub
}

void HBridgeEncoderPIDMotor::attach(int pwmPin, int directionPin, int encoderA,
		int encoderB) {
	encoder= new Encoder(encoderA, encoderB);
	analogWriteResolution(8);
	this->directionPin = directionPin;
	analogWriteFrequency(pwmPin, 20000);
	this->pwmPin=pwmPin;
	pidinit();
	pinMode(directionPin, OUTPUT);
	setOutput(0);
}
int64_t HBridgeEncoderPIDMotor::getPosition() {
	return encoder->read();
}

void HBridgeEncoderPIDMotor::setOutput(int32_t out) {
	if (out > 0) {
		digitalWrite(directionPin, HIGH); // turn the LED on (HIGH is the voltage level)
	} else {
		digitalWrite(directionPin, LOW);
	}
	int myOut = abs(out);
	if (myOut < getOutputMaxDeadbad() && out != 0)
		myOut = getOutputMaxDeadbad();
	analogWrite(pwmPin, myOut);
	//motor.write(myOut);
}
void HBridgeEncoderPIDMotor::overrideCurrentPositionHardware(int64_t val) {
	encoder->write(val);
}
//This function should analogRead the current sense from the motor driver
//and convert the value to current in milliamps
double HBridgeEncoderPIDMotor::calcCur(void) {
	//read analog value
	//convert to volts
	//converts to current in milliamps
	return 0;
}
