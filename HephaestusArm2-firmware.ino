/**@file template.ino */
#include <Servo.h>
// Teensy 3.5 and 3.6
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
// NOTE you need to add -lsupc++ -lstdc++ to "append to link" compile options
#include <TeensySimplePacketComs.h>
#elif defined(_VARIANT_ARDUINO_ZERO_)


#else
#error "NO coms layer supported!"
#endif
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
