/*
 * ServoServer.cpp
 *
 *  Created on: May 24, 2020
 *      Author: hephaestus
 */

#include "ServoServer.h"
#include "../../config.h"
#include <Arduino.h>
ServoServer::ServoServer() :
PacketEventAbstract(1962){
	servo.attach(SERVO_PIN,1000,2000);
	servo.write(180);
}

ServoServer::~ServoServer() {
	servo.detach();
}


void ServoServer::event(float * buffer){
	uint8_t * bufByte=(uint8_t*)buffer;
	uint8_t val = bufByte[0];
	if (val>180)
		val=180;
	servo.write(val);
	Serial.println("Setting servo to "+String(val));
}
