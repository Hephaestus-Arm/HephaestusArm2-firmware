/**@file template.ino */
#include <Encoder.h>
#include <PWMServo.h>
#include <SimplePacketComs.h>
#include <TeensySimplePacketComs.h>

#include "src/RobotControlCenter.h"
#include "config.h"

RobotControlCenter * controlCenter;
void setup() {
	controlCenter = new RobotControlCenter(new String(TEAM_NAME));
}

void loop() {
	controlCenter->loop(); // run the state machine pulse
}
