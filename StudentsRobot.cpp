/*
 * StudentsRobot.cpp
 *
 *  Created on: Dec 28, 2018
 *      Author: hephaestus
 *      Author: Kevin Harrington
 *      Author: Matthew Bowers
 *
 */

#include "StudentsRobot.h"

StudentsRobot::StudentsRobot(PIDMotor * motor1,
		PIDMotor * motor2, PIDMotor * motor3) {
	Serial.println("StudentsRobot::StudentsRobot constructor called here ");
	this->motor1 = motor1;
	this->motor2 = motor2;
	this->motor3 = motor3;

	// Set the PID Clock gating rate. The PID must be 10 times slower than the motors update rate
	motor1->myPID.sampleRateMs = 1; //
	motor2->myPID.sampleRateMs = 1; //
	motor3->myPID.sampleRateMs = 1;  // 10khz H-Bridge, 0.1ms update, 1 ms PID

	// Set default P.I.D gains
	motor1->myPID.setpid(0.1, 0.4, 30);
	motor2->myPID.setpid(0.015, 0, 0);
	motor3->myPID.setpid(0.015, 0, 0);

	motor1->velocityPID.setpid(0.1, 0, 0);
	motor2->velocityPID.setpid(0.1, 0, 0);
	motor3->velocityPID.setpid(0.1, 0, 0);
	// compute ratios and bounding
	double motorToWheel = 3;
	motor1->setOutputBoundingValues(-255, //the minimum value that the output takes (Full reverse)
			255, //the maximum value the output takes (Full forward)
			0, //the value of the output to stop moving
			126, //a positive value subtracted from stop value to creep backward
			126, //a positive value added to the stop value to creep forwards
			100, // 100 Cent-Degrees per degree
			876, // measured max degrees per second
			150 // the speed in degrees per second that the motor spins when the hardware output is at creep forwards
			);
	motor2->setOutputBoundingValues(-255, //the minimum value that the output takes (Full reverse)
			255, //the maximum value the output takes (Full forward)
			0, //the value of the output to stop moving
			44, //a positive value subtracted from stop value to creep backward
			44, //a positive value added to the stop value to creep forwards
			100, // 100 Cent-Degrees per degree
			480, // measured max degrees per second
			150	// the speed in degrees per second that the motor spins when the hardware output is at creep forwards
			);
	motor3->setOutputBoundingValues(-255, //the minimum value that the output takes (Full reverse)
			255, //the maximum value the output takes (Full forward)
			0, //the value of the output to stop moving
			44, //a positive value subtracted from stop value to creep backward
			44, //a positive value added to the stop value to creep forwards
			100, // 100 Cent-Degrees per degree
			1400, // measured max degrees per second
			50 // the speed in degrees per second that the motor spins when the hardware output is at creep forwards
			);
	// Set up the line tracker
	//pinMode(ANALOG_SENSE_ONE, ANALOG);
	//pinMode(ANALOG_SENSE_TWO, ANALOG);

}
/**
 * Separate from running the motor control,
 * update the state machine for running the final project code here
 */
void StudentsRobot::updateStateMachine() {
	long now = millis();
	switch (status) {
	case StartupRobot:
		//Do this once at startup
		status = StartRunning;
		Serial.println("StudentsRobot::updateStateMachine StartupRobot here ");
		break;
	case StartRunning:
		Serial.println("Start Running");


		// Start an interpolation of the motors
//		motor1->startInterpolationDegrees(1080, 6000, SIN);
//		motor2->startInterpolationDegrees(1080, 6000, SIN);
//		motor3->startInterpolationDegrees(1080, 6000, SIN);
		status = sample_pid; // set the state machine to wait for the motors to finish
		nextStatus = Running; // the next status to move to when the motors finish
		startTime = now + 1000; // the motors should be done in 1000 ms
		nextTime = startTime + 1000; // the next timer loop should be 1000ms after the motors stop

		break;
	case Running:
		// Set up a non-blocking 1000 ms delay
		status = WAIT_FOR_TIME;
		nextTime = nextTime + 1000; // ensure no timer drift by incremeting the target
		// After 1000 ms, come back to this state
		nextStatus = Running;

		break;
	case sample_pid:
		status = WAIT_FOR_TIME;
		nextTime = nextTime + 50; // ensure no timer drift by incremeting the target
		// After 1000 ms, come back to this state
//		if(isLessThan3rev()){
//			sampleAndStore();
//			nextStatus = sample_pid;
//		}else
			nextStatus = Running;
		break;
	case WAIT_FOR_TIME:
		// Check to see if enough time has elapsed
		if (nextTime <= millis()) {
			// if the time is up, move on to the next state
			status = nextStatus;
		}
		break;
	case WAIT_FOR_MOTORS_TO_FINNISH:
		if (motor1->isInterpolationDone() && motor2->isInterpolationDone()
				&& motor3->isInterpolationDone()) {
			status = nextStatus;
		}
		break;
	case Halting:
		// save state and enter safe mode
		Serial.println("Halting State machine");
		//digitalWrite(H_BRIDGE_ENABLE, 0); // Disable and idle motors
		motor3->stop();
		motor2->stop();
		motor1->stop();

		status = Halt;
		break;
	case Halt:
		// in safe mode
		break;

	}
}

/**
 * This is run fast and should return fast
 *
 * You call the PIDMotor's loop function. This will update the whole motor control system
 * This will read from the conceder and write to the motors and handle the hardware interface.
 * Instead of allowing this to be called by the controller you may call these from a timer interrupt.
 */
void StudentsRobot::pidLoop() {
	motor1->loop();
	motor2->loop();
	motor3->loop();
}
/**
 * Approve
 *
 * @param buffer A buffer of floats containing nothing
 *
 * the is the event of the Approve button pressed in the GUI
 *
 * This function is called via coms.server() in:
 * @see RobotControlCenter::fastLoop
 */
void StudentsRobot::Approve(float * buffer) {
	// approve the procession to new state
	Serial.println("StudentsRobot::Approve");

	if (myCommandsStatus == Waiting_for_approval_to_pickup) {
		myCommandsStatus = Waiting_for_approval_to_dropoff;
	} else {
		myCommandsStatus = Ready_for_new_task;
	}
}
/**
 * ClearFaults
 *
 * @param buffer A buffer of floats containing nothing
 *
 * this represents the event of the clear faults button press in the gui
 *
 * This function is called via coms.server() in:
 * @see RobotControlCenter::fastLoop
 */
void StudentsRobot::ClearFaults(float * buffer) {
	// clear the faults somehow
	Serial.println("StudentsRobot::ClearFaults");
	myCommandsStatus = Ready_for_new_task;
	status = Running;
}

/**
 * EStop
 *
 * @param buffer A buffer of floats containing nothing
 *
 * this represents the event of the EStop button press in the gui
 *
 * This is called whrn the estop in the GUI is pressed
 * All motors shuld hault and lock in position
 * Motors should not go idle and drop the plate
 *
 * This function is called via coms.server() in:
 * @see RobotControlCenter::fastLoop
 */
void StudentsRobot::EStop(float * buffer) {
	// Stop the robot immediatly
	Serial.println("StudentsRobot::EStop");
	myCommandsStatus = Fault_E_Stop_pressed;
	status = Halting;

}
/**
 * PickOrder
 *
 * @param buffer A buffer of floats containing the pick order data
 *
 * buffer[0]  is the material, aluminum or plastic.
 * buffer[1]  is the drop off angle 25 or 45 degrees
 * buffer[2]  is the drop off position 1, or 2
 *
 * This function is called via coms.server() in:
 * @see RobotControlCenter::fastLoop
 */
void StudentsRobot::PickOrder(float * buffer) {
	float pickupMaterial = buffer[0];
	float dropoffAngle = buffer[1];
	float dropoffPosition = buffer[2];
	Serial.println(
			"StudentsRobot::PickOrder Recived from : " + String(pickupMaterial)
					+ " " + String(dropoffAngle) + " "
					+ String(dropoffPosition));
	myCommandsStatus = Waiting_for_approval_to_pickup;
}
