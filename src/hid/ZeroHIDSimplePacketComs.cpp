/*
 * ZeroHIDSimplePacketComs.cpp
 *
 *  Created on: Jun 11, 2020
 *      Author: hephaestus
 */

#include "ZeroHIDSimplePacketComs.h"
#if defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)

ZeroHIDSimplePacketComs::ZeroHIDSimplePacketComs() {
	// TODO Auto-generated constructor stub

}

ZeroHIDSimplePacketComs::~ZeroHIDSimplePacketComs() {
	// TODO Auto-generated destructor stub
}


/**
* Non blocking function to check if a packet is availible from the physical layer
*/
 bool ZeroHIDSimplePacketComs::isPacketAvailible(){
	 return false;
 }
/**
* User function that fills the buffer from the data in the physical layer
* this data should already be framed, checksummed and validated as a valid packet
*/
 int32_t ZeroHIDSimplePacketComs::getPacket(uint8_t * buffer,uint32_t numberOfBytes){
	 return 0;
 }
/**
* User function that sends the buffer to the physical layer
* this data should already be framed, checksummed and validated as a valid packet
*/
 int32_t ZeroHIDSimplePacketComs::sendPacket(uint8_t * buffer,uint32_t numberOfBytes){

	 return numberOfBytes;
 }
#endif
