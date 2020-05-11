/*
 * LewanSoulPlanner.h
 *
 *  Created on: May 11, 2020
 *      Author: hephaestus
 */

#ifndef SRC_PID_LEWANSOULPLANNER_H_
#define SRC_PID_LEWANSOULPLANNER_H_
#include "../pid/PIDMotor.h"
#include "../pid/SerialMotor.h"
#include <lx16a-servo.h>
#include "../../config.h"
#define plannerLoopTimeMs 30
enum LewanSoulState_t {
	StartupSerial, WaitForHomePress,WaitForHomeRelease,WaitingForCalibrationToFinish,WaitingToRun,running
// Add more states here and be sure to add them to the cycle
};
class LewanSoulPlanner {
	SerialMotor ** upstream;
	LX16ABus servoBus;
	LX16AServo ** motors;
	int num=0;
	LewanSoulState_t state=StartupSerial;
	long timeOfLastRun = 0;
	long timeOfHomingPressed=0;
public:
	LewanSoulPlanner( int num, SerialMotor ** list);
	~LewanSoulPlanner();
	void loop();
	void update();
	bool calibrate();
};

#endif /* SRC_PID_LEWANSOULPLANNER_H_ */
