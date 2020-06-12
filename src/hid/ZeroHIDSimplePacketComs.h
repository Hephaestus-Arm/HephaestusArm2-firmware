/*
 * ZeroHIDSimplePacketComs.h
 *
 *  Created on: Jun 11, 2020
 *      Author: hephaestus
 */

#ifndef SRC_HID_ZEROHIDSIMPLEPACKETCOMS_H_
#define SRC_HID_ZEROHIDSIMPLEPACKETCOMS_H_
#include <Arduino.h>
#include <SimplePacketComs.h>
#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
#include "Adafruit_TinyUSB.h"
	class ZeroHIDSimplePacketComs  : public SimplePacketComsAbstract{


	public:
		ZeroHIDSimplePacketComs();
		virtual ~ZeroHIDSimplePacketComs();
		void initializeUSB();
		Adafruit_USBD_HID  usb_hid;
		  /**
		  * Non blocking function to check if a packet is availible from the physical layer
		  */
		   bool isPacketAvailible();
		  /**
		  * User function that fills the buffer from the data in the physical layer
		  * this data should already be framed, checksummed and validated as a valid packet
		  */
		   int32_t getPacket(uint8_t * buffer,uint32_t numberOfBytes);
		  /**
		  * User function that sends the buffer to the physical layer
		  * this data should already be framed, checksummed and validated as a valid packet
		  */
		   int32_t sendPacket(uint8_t * buffer,uint32_t numberOfBytes);
	};
#endif
#endif /* SRC_HID_ZEROHIDSIMPLEPACKETCOMS_H_ */
