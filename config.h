 /*
 * config.h
 *
 *  Created on: Nov 5, 2018
 *      Author: hephaestus
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#define TEAM_NAME "KevBot"

#define USE_WIFI

#define WHEEL_TRACK 175.0
#define WHEEL_RADIUS (2.0*25.4/2.0)
/**
 * Gripper pin for Servo
 */
#define SERVO_PIN 23
/**
 * Enable for all H-bridges
 */
#define INDICATOR             13

#define HOME_SWITCH_PIN 32
// Pins
/**
 * Drive motor 1 10Khz full duty PWM pin
 */
#define MOTOR1_PWM 3
/**
 * Pin for setting the direction of the H-Bridge
 */
#define MOTOR1_DIR 5
/**
 * Drive motor 2 10Khz full duty PWM pin
 */
#define MOTOR2_PWM 14
/**
 * Pin for setting the direction of the H-Bridge
 */
#define MOTOR2_DIR 16
/**
 * Drive motor 3 10Khz full duty PWM pin
 */
#define MOTOR3_PWM 37
/**
 * Pin for setting the direction of the H-Bridge
 */
#define MOTOR3_DIR 39

//Encoder pins
#define MOTOR1_ENCA 19 // White
#define MOTOR1_ENCB 18 // Yellow

#define MOTOR2_ENCA 1 // White
#define MOTOR2_ENCB 2 // Yellow

#define MOTOR3_ENCB 25 // White
#define MOTOR3_ENCA 12 // Yellow

//Stepper
#define STEPPER_DIRECTION 33
#define STEPPER_STEP      32

// Sensor Pins
#define ANALOG_SENSE_ONE		36
#define ANALOG_SENSE_TWO		39
#define ANALOG_SENSE_THREE		34
#define ANALOG_SENSE_FOUR		35

// Pins used by a perpheral, may be re-used
#define BOOT_FLAG_PIN 			0
#define I2C_SDA       			21
#define I2C_SCL       			22
#define SERIAL_PROGRAMMING_TX 	1
#define SERIAL_PROGRAMMING_RX 	3
#define WII_CONTROLLER_DETECT 	27


#endif /* SRC_CONFIG_H_ */
