/**@file template.ino */

#include <SimplePacketComs.h>
#include <server/NameCheckerServer.h>

#include "src/RobotControlCenter.h"
#include "config.h"

RobotControlCenter * controlCenter;
void setup() {
	controlCenter = new RobotControlCenter(new String(TEAM_NAME));
}

void loop() {
	controlCenter->loop(); // run the state machine pulse
}
