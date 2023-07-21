/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>

#include "bsp/board.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "tusb.h"

#include "speaker.h"

#define SPEAKER_PIN 15

#define NOTE_OFF 8
#define NOTE_ON  9

#define C_PIN 16

static float midinotes[128];	// the frequencies for each standard MIDI note

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
	}
}

void setup_gpio() {
	gpio_init(C_PIN);
	gpio_set_dir(C_PIN, GPIO_OUT);
}

void set_gpio(const uint note) {
	if (note == 60) { // middle C
		gpio_put(C_PIN, true);
	}
}

void clear_gpio(const uint note) {
	if (note == 60) {
		gpio_put(C_PIN, false);
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
			clear_gpio(msg[1]);
			break;
		case NOTE_ON:		// Note On
			play_speaker_note(midinotes[msg[1]], msg[2]);
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

int main() {

	board_init();
	tud_init(BOARD_TUD_RHPORT);

	setup_midinotes();
	setup_gpio();
	setup_speaker(SPEAKER_PIN);

	// for (int mn = 69; mn <= 69 + 12; ++mn) {
	// 	play_speaker_note(midinotes[mn], 5000);
	// 	busy_wait_ms(1000);
	// }

	play_speaker_note(1.0, 0);

	while (true) {
		tud_task();
		process_midi();
	}
}
