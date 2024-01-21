/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"

// #include "gpio.h"
#include "midi.h"
#include "servo.h"
#include "speaker.h"

#define SPEAKER_PIN 15

int main() {

	board_init();
	tud_init(BOARD_TUD_RHPORT);

	setup_midinotes();
	// setup_gpio();
	setup_speaker(SPEAKER_PIN);

	// for (int mn = 60; mn <= 60 + 12; ++mn) {
	// 	play_speaker_note(midinotes[mn], 5000);
	// 	busy_wait_ms(1000);
	// }

	play_speaker_note(1.0, 0);

	while (true) {
		tud_task();
		process_midi();
	}
}
