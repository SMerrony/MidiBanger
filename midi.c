/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "midi.h"

#include <math.h>

#include "tusb.h"

#include "gpio.h"
#include "servo.h"
#include "speaker.h"

#define SERVO_PIN 1
#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

static float midinotes[128];	// the frequencies for each standard MIDI note

servo_t servo;

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
	}
	servo_setup(&servo, SERVO_PIN, SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
}

void handle_event(const uint8_t msg[3]) {
    // int ch;
    int event;

    // ch = msg[0] & 0xf;
    event = (msg[0] >> 4) & 0xf;

	switch (event) {
		case NOTE_OFF:		// Note Off
			play_speaker_note(1.0, 0);
			if (msg[1] == 64) {
				servo_set_angle(&servo, 0);
			}
			clear_gpio(msg[1]);
			break;
		case NOTE_ON:		// Note On
			play_speaker_note(midinotes[msg[1]], msg[2]);
			if (msg[1] == 64) {
				servo_set_angle(&servo, 90);
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