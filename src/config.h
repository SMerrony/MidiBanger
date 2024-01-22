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

// Set PERCUSSION to true if this is a mallet-type instrument where the 'hammer' 
// is returned after striking.  This uses the second core on the RP2020.
#define PERCUSSION true		
// Period before 'hammer' starts to return (may influence length of stroke)
#define PERCUSSIVE_RETURN_MS 75 

// do not edit...
#define NOTE_NOT_MAPPED -1

// The MIDI notes, C4 is middle-C in MIDI-speak.
typedef int notepins_t[128];
// ...end of do not edit section

// The mapping of MIDI note numbers to GPIO pins for controlling the servos...
#define NOTEPIN_INITIALISER { [0 ... 127] = NOTE_NOT_MAPPED,\
                                [72] = 2,\
                                [74] = 3,\
                                [76] = 4,\
                                [77] = 5,\
                                [79] = 6,\
                                [81] = 7,\
                                [83] = 8,\
                                [84] = 9 };



#endif
