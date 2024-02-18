/**
 * SPDX-FileCopyrightText: 2024 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#define USB_MANUFACTURER   "SMerrony"

// MIDI Channel to listen on. 
// N.B. This is expressed in the commonly-used MIDI channel numbering system 
// where the first channel is number 1 and the last is 16 (they are mapped internally to 0 ... 15).
#define MIDI_CHANNEL 1

// Speaker is just for debugging, set to false otherwise (it uses 1 of the 16 PWM channels)
#define WITH_SPEAKER false
#define SPEAKER_PIN 15 

// Servo characteristics, these are the spec. values, you may be able to exceed them.
#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

// do not edit...
#define NOTE_NOT_MAPPED 0
// The MIDI notes, C4 is middle-C in MIDI-speak.
typedef int notepins_t[128];
typedef uint16_t noteangle_t[128];
// Indices for the P&T servos - i.e which of the servos[] array to use
#define PAN_SERVO 0
#define TILT_SERVO 1  
// ...end of do not edit section

/*
// The mapping of MIDI note numbers to GPIO pins for controlling the servos...
#define NOTE_PIN_INITIALISER { [0 ... 127] = NOTE_NOT_MAPPED,\
                                [72] = 2,\
                                [74] = 3,\
                                [76] = 4,\
                                [77] = 5,\
                                [79] = 6,\
                                [81] = 7,\
                                [83] = 8,\
                                [84] = 9 };
*/                                

#define PAN_PIN  6              // GPIO pin for the pan servo control signal
#define TILT_PIN 10             // ...and for the tilt servo

// #include "chicco_toy_glock_config.h"
#include "janod_toy_glock_config.h"