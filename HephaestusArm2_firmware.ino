/**@file template.ino */
#include <Encoder.h>
#include <Servo.h>
// NOTE you need to add -lsupc++ -lstdc++ to "append to link" compile options
#include <TeensySimplePacketComs.h>
#include <lx16a-servo.h>

#include "src/RobotControlCenter.h"
#include "config.h"

RobotControlCenter * controlCenter;
void setup() {
	while(!Serial);
	delay(1000);
	Serial.begin(115200);
	Serial.println("Starting");
	controlCenter = new RobotControlCenter(new String(TEAM_NAME));
}

void loop() {
	controlCenter->loop(); // run the state machine pulse
}
