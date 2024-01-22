/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "midi.h"

#include <math.h>

#include "pico/multicore.h"
#include "tusb.h"

// #include "gpio.h"
#include "servo.h"
#include "speaker.h"

#include "config.h"

static const int OUR_CHANNEL = MIDI_CHANNEL - 1;
static notepins_t notepins = NOTEPIN_INITIALISER;
static int noteservos[127];

static float midinotes[128];	// the frequencies for each standard MIDI note

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	int srv = 0;
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
		if (notepins[m] != NOTE_NOT_MAPPED) {
			servo_setup(srv, notepins[m], SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
			noteservos[m] = srv;
			busy_wait_ms(200); // reduce PSU stress by not starting all servos at the same time
			srv++;
		}
	}

	if (PERCUSSION) {
		multicore_launch_core1(resetter_task);
	}
}

void handle_event(const uint8_t msg[3]) {
    int ch;
    int event;

    ch = msg[0] & 0xf;

	if (ch == OUR_CHANNEL) {
		event = (msg[0] >> 4) & 0xf;

		switch (event) {
			case NOTE_OFF:
				if (notepins[msg[1]] != NOTE_NOT_MAPPED) {
			 		if (WITH_SPEAKER) {
						play_speaker_note(1.0, 0);
					}
					servo_set_angle(noteservos[msg[1]], 0);
				}
				break;
			case NOTE_ON:
				if (notepins[msg[1]] != NOTE_NOT_MAPPED) {
					if (WITH_SPEAKER) {
						play_speaker_note(midinotes[msg[1]], msg[2]);
					}
					servo_set_angle(noteservos[msg[1]], 90);
					// test auto-return for percussion...
					if (PERCUSSION)	{
						servo_set_reset_time(noteservos[msg[1]], delayed_by_ms(get_absolute_time(), PERCUSSIVE_RETURN_MS));
					}
				}
				break;
			default:
				break;
		}
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