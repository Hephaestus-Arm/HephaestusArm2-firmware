 /*
 * config.h
 *
 *  Created on: Nov 5, 2018
 *      Author: hephaestus
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#define TEAM_NAME "KevBot"


/**
 * Enable for all H-bridges
 */
#define INDICATOR             13
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)
		/**
 * Gripper pin for Servo
 */
#define SERVO_PIN 23

#define HOME_SWITCH_PIN 32
#elif defined(_VARIANT_ARDUINO_ZERO_)|| defined(__SAMD51__)
/**
 * Gripper pin for Servo
 */
#define SERVO_PIN 12


#define HOME_SWITCH_PIN 11
#else
#error "NO coms layer supported!"
#endif



#endif /* SRC_CONFIG_H_ */
