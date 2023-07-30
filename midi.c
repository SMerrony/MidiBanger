/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "midi.h"

#include <math.h>

#include "pico/multicore.h"
#include "tusb.h"

#include "gpio.h"
#include "servo.h"
#include "speaker.h"

#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

#define PERCUSSION true		    // is this is a mallet-type instrument where the 'hammer' is returned after striking?
#define PERCUSSIVE_RETURN_MS 75 // period before 'hammer' starts to return (may influence length of stroke)

/********************************************************************************/
/****** Start of Servo mapping - N.B. The next 3 items must be kep in sync ******/

// the number of servos are we controlling
#define NUM_SERVOS 2

// we define up to 16 PWM pins attached to servos
static const uint servo_pins[NUM_SERVOS] = {2, 1};

// the MIDI notes corresponding to each servo
static const uint16_t servo_notes[NUM_SERVOS] = {60, 64};

/****** End of Servo mapping ******/
/**********************************/

#define NOTE_NOT_MAPPED -1

static float midinotes[128];	// the frequencies for each standard MIDI note
static int map_notes_to_servos[128];

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
		map_notes_to_servos[m] = NOTE_NOT_MAPPED;
	}
	for (int s = 0; s < NUM_SERVOS; ++s) {
		servo_setup(s, servo_pins[s], SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
		map_notes_to_servos[servo_notes[s]] = s;
		busy_wait_ms(100); // reduce PSU stress by not starting all servos at the same time
	}
	if (PERCUSSION) {
		multicore_launch_core1(resetter_task);
	}
}

void handle_event(const uint8_t msg[3]) {
    // int ch;
    int event;

    // ch = msg[0] & 0xf;
    event = (msg[0] >> 4) & 0xf;

	switch (event) {
		case NOTE_OFF:		// Note Off
			play_speaker_note(1.0, 0);
			if (map_notes_to_servos[msg[1]] != NOTE_NOT_MAPPED) {
				servo_set_angle(map_notes_to_servos[msg[1]], 0);
			}
			clear_gpio(msg[1]);
			break;
		case NOTE_ON:		// Note On
			play_speaker_note(midinotes[msg[1]], msg[2]);
			if (map_notes_to_servos[msg[1]] != NOTE_NOT_MAPPED) {
				servo_set_angle(map_notes_to_servos[msg[1]], 90);
				// test auto-return for percussion...
				if (PERCUSSION)	{
					servo_set_reset_time(map_notes_to_servos[msg[1]], delayed_by_ms(get_absolute_time(), PERCUSSIVE_RETURN_MS));
				}
			}
			set_gpio(msg[1]);
			break;
		default:
			break;
	}
}

/* process_midi reads any MIDI data availble and passes on non-SysEx messages for further processing */
void process_midi() {
    static bool sysex = false;
    uint8_t msg[3];
    int n_data;

    while((n_data = tud_midi_n_available(0, 0))) {
		msg[0] = 0; msg[1] = 0; msg[2] = 0;
		if ((n_data = tud_midi_n_stream_read(0, 0, msg, 3))) {
			if (sysex) {
			for (int i = 0; i < 3; i++) {
				// swallow message(s) until the end-of-sysex marker
				if (msg[i] == 0xf7) {
					sysex = false;
					break;
				}
			}
			} else {
				sysex = (msg[0] == 0xf0);
				if (!sysex) {	// we're ignoring sysex for now
					handle_event(msg);
				}
			}
		}
    }
}