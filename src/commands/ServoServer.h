/*
 * ServoServer.h
 *
 *  Created on: May 24, 2020
 *      Author: hephaestus
 */

#ifndef SRC_COMMANDS_SERVOSERVER_H_
#define SRC_COMMANDS_SERVOSERVER_H_

#include <PacketEvent.h>
#include <Servo.h>

class ServoServer: public PacketEventAbstract {
private:
	Servo servo;
public:
	ServoServer();
	virtual ~ServoServer();

	void event(float * buffer);
};

#endif /* SRC_COMMANDS_SERVOSERVER_H_ */
