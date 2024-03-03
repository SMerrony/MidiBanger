/**
 * SPDX-FileCopyrightText: 2023,2024 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"

#include "midi.h"
#include "servo.h"
#include "speaker.h"

#include "config.h"

int main() {

	board_init();
	tud_init(BOARD_TUD_RHPORT);

	setup_midinotes();
	
	if (WITH_SPEAKER) {
		setup_speaker(SPEAKER_PIN);
		play_speaker_note(1.0, 0);
	}

	while (true) {
		tud_task();
		process_midi();
	}
}
