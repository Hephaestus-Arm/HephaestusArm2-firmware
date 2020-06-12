/*
 * ZeroHIDSimplePacketComs.cpp
 *
 *  Created on: Jun 11, 2020
 *      Author: hephaestus
 */

#include "ZeroHIDSimplePacketComs.h"
#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
uint8_t const desc_hid_report[] =
{
  TUD_HID_REPORT_DESC_GENERIC_INOUT(64)
};
static int availible=0;
static uint8_t  bufferReturn[64];
static Adafruit_USBD_HID usb_hid;
static bool initFlag=false;

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // not used in this example
  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
	for(int i=0;i<bufsize&&i<64;i++){
		bufferReturn[i]=buffer[i];
	}
	availible=bufsize;
}
ZeroHIDSimplePacketComs::ZeroHIDSimplePacketComs() {

}

ZeroHIDSimplePacketComs::~ZeroHIDSimplePacketComs() {
	// TODO Auto-generated destructor stub
}


/**
* Non blocking function to check if a packet is availible from the physical layer
*/
 bool ZeroHIDSimplePacketComs::isPacketAvailible(){
	 if(!initFlag){
		 initFlag=true;
		  usb_hid.enableOutEndpoint(true);
		  usb_hid.setPollInterval(2);
		  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
		  usb_hid.setReportCallback(get_report_callback, set_report_callback);
		  availible=0;
		  usb_hid.begin();
	 }
	 return availible>0;
 }
/**
* User function that fills the buffer from the data in the physical layer
* this data should already be framed, checksummed and validated as a valid packet
*/
 int32_t ZeroHIDSimplePacketComs::getPacket(uint8_t * buffer,uint32_t numberOfBytes){
	 for(int i=0;i<availible&&i<64;i++){
		buffer[i]=bufferReturn[i];
	}
	 numberOfBytes=availible;
	 availible=0;
	 return numberOfBytes;
 }
/**
* User function that sends the buffer to the physical layer
* this data should already be framed, checksummed and validated as a valid packet
*/
 int32_t ZeroHIDSimplePacketComs::sendPacket(uint8_t * buffer,uint32_t numberOfBytes){
	 if(!initFlag){
		 return 0;
	 }
	 usb_hid.sendReport(0, buffer, numberOfBytes);
	 return numberOfBytes;
 }
#endif
