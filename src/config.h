/**
 * SPDX-FileCopyrightText: 2024 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CONFIG_H
#define CONFIG_H

#define USB_MANUFACTURER "SMerrony"
#define USB_PRODUCT      "MidiBanger1"

// MIDI Channel to listen on. 
// N.B. This is expressed in the commonly-used MIDI channel numbering system 
// where the first channel is number 1 and the last is 16 (they are mapped internally to 0 ... 15).
#define MIDI_CHANNEL 1

// Speaker is just for debugging, set to false otherwise (it uses 1 of the 16 PWM channels)
#define WITH_SPEAKER true
#define SPEAKER_PIN 15 

// Servo characteristics, these are the spec. values, you may be able to exceed them.
#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

// Set MULTI_PERCUSSION to true if this is a multiple-mallet type instrument where the 'hammers' 
// are returned after striking.  This uses the second core on the RP2020.
#define MULTI_PERCUSSION false		

// Set PAN_AND_TILT to true if this is a (single hammer) pan-and-tilt player.
#define PAN_AND_TILT true

// Period before 'hammer' starts to return (may influence length of stroke)
#define PERCUSSIVE_RETURN_MS 75 

// do not edit...
#define NOTE_NOT_MAPPED -1
// The MIDI notes, C4 is middle-C in MIDI-speak.
typedef int notepins_t[128];
typedef int noteangles_t[128];
// ...end of do not edit section

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

// Indices for the P&T servos
#define PAN_SERVO 0
#define TILT_SERVO 1                                

#define PAN_PIN  3              // GPIO pin for the pan servo control signal
#define TILT_PIN 4              // ...and for the tilt servo

#define PAN_SETTLE_MS 175       // Time to wait for panning to settle before striking note
#define PAN_REST_ANGLE 76       // Default postion at start-up
#define TILT_REST_ANGLE 80      // Default hammer height
#define TILT_STRIKE_ANGLE 77    // Striking height

#define NOTE_PAN_INITIALISER { [0 ... 127] = NOTE_NOT_MAPPED,\
                                [72] = 102,\
                                [74] = 96,\
                                [76] = 90,\
                                [77] = 83,\
                                [79] = 76,\
                                [81] = 68,\
                                [83] = 62,\
                                [84] = 56 };

#endif
