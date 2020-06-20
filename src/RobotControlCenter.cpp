/*
 * ExampleRobot.cpp
 *  Remember, Remember the 5th of november
 *  Created on: Nov 5, 2018
 *      Author: hephaestus
 */

#include "RobotControlCenter.h"

void RobotControlCenter::loop() {
	if (micros() - lastPrint > 500
			|| micros() < lastPrint // check for the wrap over case
					) {
		switch (state) {
		case Startup:
			setup();
			state = WaitForConnect;
			break;
		case WaitForConnect:
				state = run; // begin the main operation loop
			break;
		default:
			break;
		}
		lastPrint = micros(); // ensure 0.5 ms spacing *between* reads for Wifi to transact
	}
	if (state != Startup) {
		// If this is run before the sensor reads, the I2C will fail because the time it takes to send the UDP causes a timeout
		fastLoop();    // Run PID and wifi after State machine on all states
	}

}

RobotControlCenter::RobotControlCenter(String * mn,SimplePacketComsAbstract* comsPtr) {
	pidList[0] = &motor1;
	pidList[1] = &motor2;
	pidList[2] = &motor3;
	state = Startup;
	name = mn;
	robot = NULL;
	planner = NULL;
	coms=comsPtr;
}

void RobotControlCenter::setup() {
	if (state != Startup)
		return;
	state = WaitForConnect;

	Serial.println("Loading FW");

//	motor1.attach(MOTOR1_PWM, MOTOR1_DIR, MOTOR1_ENCA, MOTOR1_ENCB);
//	motor2.attach(MOTOR2_PWM, MOTOR2_DIR, MOTOR2_ENCA, MOTOR2_ENCB);
//	motor3.attach(MOTOR3_PWM, MOTOR3_DIR, MOTOR3_ENCA, MOTOR3_ENCB);
	// Set the setpoint the current position in motor units to ensure no motion
	motor1.setSetpoint(motor1.getPosition());
	motor2.setSetpoint(motor2.getPosition());
	motor3.setSetpoint(motor3.getPosition());

	// Set up digital servo for the gripper
	//servo.setPeriodHertz(50);
	robot = new StudentsRobot(&motor1, &motor2, &motor3);
	planner = new LewanSoulPlanner(numberOfPID, (SerialMotor **)pidList);

	// Attach coms
	coms->attach(new SetPIDSetpoint(numberOfPID, pidList)); // @suppress("Method cannot be resolved")  @suppress("Invalid arguments")
	coms->attach(new SetPIDConstants(numberOfPID, pidList)); // @suppress("Method cannot be resolved")  @suppress("Invalid arguments")
	coms->attach(new GetPIDData(numberOfPID, pidList)); // @suppress("Method cannot be resolved")  @suppress("Invalid arguments")
	coms->attach( // @suppress("Method cannot be resolved")  @suppress("Invalid arguments")
			new GetPIDConstants(numberOfPID, pidList));// @suppress("Invalid arguments")
	coms->attach(new GetPIDVelocity(numberOfPID, pidList));// @suppress("Invalid arguments")
	coms->attach(new GetPDVelocityConstants(numberOfPID, pidList));// @suppress("Invalid arguments")
	coms->attach(new SetPIDVelocity(numberOfPID, pidList));// @suppress("Invalid arguments")
	coms->attach(new SetPDVelocityConstants(numberOfPID, pidList));//  @suppress("Invalid arguments")
	coms->attach(new ServoServer());

}

void RobotControlCenter::fastLoop() {
	if (state == Startup)    // Do not run before startp
		return;
	coms->server(); // @suppress("Method cannot be resolved")
	robot->pidLoop();
	planner->loop();
	robot->updateStateMachine();

}
