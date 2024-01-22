/**
 * SPDX-FileCopyrightText: 2024 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CONFIG_H
#define CONFIG_H

#define USB_MANUFACTURER "SMerrony"
#define USB_PRODUCT      "MidiPlayer"

// Speaker is just for debugging
#define WITH_SPEAKER
#define SPEAKER_PIN 15

// Servo characteristics
#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

#define PERCUSSION true		    // is this is a mallet-type instrument where the 'hammer' is returned after striking?
#define PERCUSSIVE_RETURN_MS 75 // period before 'hammer' starts to return (may influence length of stroke)

#define NOTE_NOT_MAPPED -1

// The MIDI notes, C4 is middle-C in MIDI-speak.
typedef int notepins_t[128];

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
