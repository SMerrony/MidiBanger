/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "pico/stdlib.h"

#define NOTE_OFF 8
#define NOTE_ON  9

void setup_midinotes();
void handle_event(const uint8_t msg[3]);
void process_midi();
