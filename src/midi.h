/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MIDI_H
#define MIDI_H

#include "pico/stdlib.h"

#define NOTE_OFF 8
#define NOTE_ON  9

void setup_midinotes();
void handle_event(const uint8_t msg[3]);
void process_midi();

#endif