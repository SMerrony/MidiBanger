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

#define SERVO_PIN 1
#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

#define PERCUSSION true
#define PERCUSSIVE_RETURN_MS 75

#define NOTE_E 64

static float midinotes[128];	// the frequencies for each standard MIDI note
static servo_t servos[MAX_SERVOS] = {0};

/* resetter_task performs the percussive returns */
void resetter_task() {
	while (true){
		for (int s = 0; s < MAX_SERVOS; ++s) {
			if (! is_at_the_end_of_time(servos[s].reset)) {
				if (absolute_time_diff_us(servos[s].reset, get_absolute_time()) > 0) {
					servo_set_angle(&servos[s], 0);
					servos[s].reset = at_the_end_of_time;
				}
			}
		}
	}
}

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
	}
	servo_setup(&servos[0], SERVO_PIN, NOTE_E, SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
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
			servo_set_angle(servo_for_note(msg[1]), 0);
			clear_gpio(msg[1]);
			break;
		case NOTE_ON:		// Note On
			play_speaker_note(midinotes[msg[1]], msg[2]);
			servo_set_angle(servo_for_note(msg[1]), 90);
			// test auto-return for percussion...
			if (PERCUSSION)	servo_set_reset_time(servo_for_note(msg[1]), delayed_by_ms(get_absolute_time(), PERCUSSIVE_RETURN_MS));
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