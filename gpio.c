/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "gpio.h"

#define C_PIN 16

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
