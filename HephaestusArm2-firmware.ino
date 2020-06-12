/**@file template.ino */
#include <Servo.h>

// Teensy 3.5 and 3.6
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
// NOTE you need to add -lsupc++ -lstdc++ to "append to link" compile options
#include <TeensySimplePacketComs.h>
#elif defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
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
#elif defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
		uint8_t const desc_hid_report[] =
		{
		  TUD_HID_REPORT_DESC_GENERIC_INOUT(64)
		};
		Adafruit_USBD_HID usb_hid;
		ZeroHIDSimplePacketComs coms(&usb_hid);
		uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen);

		// Invoked when received SET_REPORT control request or
		// received data on OUT endpoint ( Report ID = 0, Type = 0 )
		void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize);

#else
#error "NO coms layer supported!"
#endif



RobotControlCenter * controlCenter;
void setup() {

#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
	  usb_hid.enableOutEndpoint(true);
	  usb_hid.setPollInterval(2);
	  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
	  usb_hid.setReportCallback(get_report_callback, set_report_callback);
	  usb_hid.begin();
#endif
	while(!Serial){
	delay(10);
	}
	Serial.begin(115200);

	  // wait until device mounted
#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
	  while( !USBDevice.mounted() ) delay(1);
#endif
	Serial.println("Starting");
	controlCenter = new RobotControlCenter(new String(TEAM_NAME),&coms);

}

void loop() {
	controlCenter->loop(); // run the state machine pulse
}
