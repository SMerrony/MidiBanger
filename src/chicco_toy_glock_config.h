/**
 * SPDX-FileCopyrightText: 2024 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#define USB_PRODUCT "MidiBanger_Chicco"

// Set MULTI_PERCUSSION to true if this is a multiple-mallet type instrument where the 'hammers' 
// are returned after striking.  This uses the second core on the RP2020.
#define MULTI_PERCUSSION false		

// Set PAN_AND_TILT to true if this is a (single hammer) pan-and-tilt player.
#define PAN_AND_TILT true

// Period before 'hammer' starts to return (may influence length of stroke)
#define PERCUSSIVE_RETURN_MS 50

#define PAN_SETTLE_MS 175       // Time to wait for panning to settle before striking note
#define PAN_REST_ANGLE 74       // Default postion at start-up
#define TILT_REST_ANGLE 80      // Default hammer height
#define TILT_STRIKE_ANGLE 77    // Striking height

#define NOTE_PAN_INITIALISER { [0 ... 127] = NOTE_NOT_MAPPED,\
                                [72] = 101,\
                                [74] = 95,\
                                [76] = 89,\
                                [77] = 82,\
                                [79] = 74,\
                                [81] = 68,\
                                [83] = 61,\
                                [84] = 56 };

// we're not using NOTE_PINS here, so define an empty init...
#define NOTE_PIN_INITIALISER { [0 ... 127] = NOTE_NOT_MAPPED };