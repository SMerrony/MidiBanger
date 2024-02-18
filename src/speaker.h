/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "pico/stdlib.h"

void setup_speaker(const uint pin);
void play_speaker_note(const float freq, const uint8_t velocity);
