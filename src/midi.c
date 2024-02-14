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
static notepins_t notepins = NOTE_PIN_INITIALISER;
static noteangles_t noteangles = NOTE_PAN_INITIALISER
static int noteservos[127];

static float midinotes[128];	// the frequencies for each standard MIDI note
static uint8_t last_note = 0;

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	int srv = 0;
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
		if (MULTI_PERCUSSION) {
			if (notepins[m] != NOTE_NOT_MAPPED) {
				servo_setup(srv, notepins[m], SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
				noteservos[m] = srv;
				busy_wait_ms(200); // reduce PSU stress by not starting all servos at the same time
				srv++;
			}
		}
	}
	if (PAN_AND_TILT) {
		servo_setup(PAN_SERVO, PAN_PIN, SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
		servo_start(PAN_SERVO, PAN_REST_ANGLE);
		busy_wait_ms(200); // reduce PSU stress
		servo_setup(TILT_SERVO, TILT_PIN, SERVO_MIN_DUTY, SERVO_MAX_DUTY, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
		servo_start(TILT_SERVO, TILT_REST_ANGLE);
	}

	if (MULTI_PERCUSSION) {
		multicore_launch_core1(resetter_task);
	}
}

void handle_event(const uint8_t msg[3]) {
    int ch;
    int event;
	uint8_t this_note = 0;

    ch = msg[0] & 0xf;

	if (ch == OUR_CHANNEL) {
		event = (msg[0] >> 4) & 0xf;
		this_note = msg[1];

		switch (event) {
			case NOTE_OFF:
				if (MULTI_PERCUSSION) {
					if (notepins[msg[1]] != NOTE_NOT_MAPPED) {
						if (WITH_SPEAKER) {
							play_speaker_note(1.0, 0);
						}
						servo_set_angle(noteservos[this_note], 0);
					}
				}
				if (PAN_AND_TILT) {
					if (noteangles[this_note] != NOTE_NOT_MAPPED) {
						if (WITH_SPEAKER) {
							play_speaker_note(1.0, 0);
						}
					}
				}
				break;
			case NOTE_ON:
				if (MULTI_PERCUSSION) {
					if (notepins[this_note] != NOTE_NOT_MAPPED) {
						if (WITH_SPEAKER) {
							play_speaker_note(midinotes[this_note], msg[2]);
						}
						servo_set_angle(noteservos[this_note], 90);
						// test auto-return for percussion...
						servo_set_reset_time(noteservos[this_note], delayed_by_ms(get_absolute_time(), PERCUSSIVE_RETURN_MS));
					}
				}
				if (PAN_AND_TILT) {
					if (noteangles[this_note] != NOTE_NOT_MAPPED) {
						if (WITH_SPEAKER) {
							play_speaker_note(midinotes[this_note], msg[2]);
						}
						// Pan if necessary...
						if (this_note != last_note){
							servo_set_angle(PAN_SERVO, noteangles[this_note]);
							busy_wait_ms(PAN_SETTLE_MS);
						}
						// Tilt (strike)...
						servo_set_angle(TILT_SERVO, TILT_STRIKE_ANGLE);
						busy_wait_ms(PERCUSSIVE_RETURN_MS);
						servo_set_angle(TILT_SERVO, TILT_REST_ANGLE);
					}
				}
				break;
			default:
				break;
		}
		last_note = this_note;
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