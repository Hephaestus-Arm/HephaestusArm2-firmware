/*
 * ExampleRobot.h
 *
 *  Created on: Nov 5, 2018
 *      Author: hephaestus
 */

#ifndef SRC_ROBOTCONTROLCENTER_H_
#define SRC_ROBOTCONTROLCENTER_H_

#if defined(Arduino_h)
#include <Arduino.h>
#endif
#include "../config.h"
#include "../RBEPID.h"
#include "pid/PIDMotor.h"
#include "pid/LewanSoulPlanner.h"
#include "pid/SerialMotor.h"
#include <SimplePacketComs.h>

#include "commands/GetPIDConstants.h"
#include "commands/GetPIDData.h"
#include "commands/SetPIDConstants.h"
#include "commands/SetPIDSetpoint.h"

#include "../StudentsRobot.h"
#include "commands/SetPDVelocityConstants.h"
#include "commands/SetPIDVelocity.h"
#include "commands/GetPDVelocityConstants.h"
#include "commands/GetPIDVelocity.h"
#include "commands/ServoServer.h"
#include "commands/LightServer.h"

enum state_t {
	Startup, WaitForConnect, run
// Add more states here and be sure to add them to the cycle
};
#define numberOfPID  3
/**
 * RobotControlCenter is the main class for the 2000 level student code
 *
 * This class is the starting point for all the code. Student code is
 * housed in StudentsRobot class
 * @see StudentsRobot
 *
 * This class managed the overall state machine for dealing with connecting and
 * maintaining connection to the WiFi, constructing the StudentsRobot class, and managing
 * the StudentsRobot calls to updates its state machine as well as its PID loops.
 *
 */
class RobotControlCenter {
private:

	// A value to check if enough time has elapsed to tun the sensors and prints
	int64_t lastPrint = 0;
	// Change this to set your team name
	String * name;	//
	// List of PID objects to use with PID server
	PIDMotor * pidList[numberOfPID];	// = { &motor1.myPID, &motor2.myPID };

	SimplePacketComsAbstract* coms;

	//attach the PID servers
	void setupPIDServers();
	// State machine state
	state_t state = Startup;
public:
	/**
	 * RobotControlCenter constructor
	 *
	 * The name is used by the SimplePacketComs stack to locate your specific
	 * robot on the network.
	 */
	RobotControlCenter(String * name,SimplePacketComsAbstract* comsPtr);
	~RobotControlCenter() {
	}
	/**
	 * Pulse the loop function from the main thread
	 *
	 * This function is callled over and over by the INO loop()
	 */
	void loop();
	/**
	 * A pointer to the students robot
	 *
	 * NULL at startup, this is instantiated by the RobotControlCenter state machine.
	 * This variable is set as part of @see RobotControlCenter::setup
	 */
	StudentsRobot * robot;
	/**
	 * Internal setup function to set up all objects
	 *
	 * This function is called as part of the state machine by the object
	 */
	void setup();
protected:
	SerialMotor motor1;  // PID controlled motor object
	SerialMotor motor2; // PID controlled motor object
	SerialMotor motor3; // PID controlled motor object

	LewanSoulPlanner * planner;
	//

	/**
	 * 	The fast loop actions
	 *
	 * 	This should be run every loop and is internally gated for fast opperation
	 *
	 * 	@see StudentsRobot::Approve
	 * 	@see StudentsRobot::ClearFaults
	 * 	@see StudentsRobot::EStop
	 * 	@see StudentsRobot::PickOrder
	 */
	void fastLoop();

};

#endif /* SRC_ROBOTCONTROLCENTER_H_ */
