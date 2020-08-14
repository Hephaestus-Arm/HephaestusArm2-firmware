/**@file template.ino */
#include <Servo.h>
#include <SimplePacketComs.h>
#include <sam.h>
// Teensy 3.5 and 3.6
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
// NOTE you need to add -lsupc++ -lstdc++ to "append to link" compile options
#include <TeensySimplePacketComs.h>
#elif defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)|| defined(ARDUINO_ARCH_SAMD)
#include "Adafruit_TinyUSB.h"

#else
#error "NO coms layer supported!"
#endif
#include <lx16a-servo.h>

#include "src/RobotControlCenter.h"
#include "config.h"

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
	// SImple packet coms implementation useing WiFi
		HIDSimplePacket coms;
#elif defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)|| defined(ARDUINO_ARCH_SAMD)

#include "src/hid/ZeroHIDSimplePacketComs.h"
		ZeroHIDSimplePacketComs coms;
#else
#error "NO coms layer supported!"
#endif



RobotControlCenter * controlCenter;
void setup() {

#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)|| defined(ARDUINO_ARCH_SAMD)
	coms.initializeUSB();
#else
  	while(!Serial){
  	 delay(10);
  	}
#endif

	Serial.begin(115200);

	  // wait until device mounted
#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)|| defined(ARDUINO_ARCH_SAMD)
	// Set a specific VID/PID
	USBDevice.setID(0x16c0, 0x486);
	while( !USBDevice.mounted() ) delay(1);
#endif
	Serial.println("Starting");
	controlCenter = new RobotControlCenter(new String(TEAM_NAME),&coms);

}

void loop() {
	controlCenter->loop(); // run the state machine pulse
}
