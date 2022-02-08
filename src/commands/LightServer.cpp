/*
 * ServoServer.cpp
 *
 *  Created on: May 24, 2020
 *      Author: hephaestus
 */

#include "LightServer.h"
#include "../../config.h"
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip(12, AUX_PIN, NEO_GRB + NEO_KHZ800);
LightServer::LightServer() :
PacketEventAbstract(2000){
	strip.begin();
	strip.fill(strip.ColorHSV(0, 0, 255));
	strip.show();
}

LightServer::~LightServer() {
	strip.fill(strip.ColorHSV(0, 0, 0));
	strip.show();
}


void LightServer::event(float * buffer){
	uint16_t hue = 65535*buffer[0];
	uint8_t saturation = 255*buffer[1];
	uint8_t value = 255*buffer[2];
	strip.fill(strip.ColorHSV(hue, saturation, value));
	strip.show();
}
