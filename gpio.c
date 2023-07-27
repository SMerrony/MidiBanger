/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "gpio.h"

#define C_PIN 16
#define D_PIN 17

void setup_gpio() {
	gpio_init(C_PIN);
	gpio_set_dir(C_PIN, GPIO_OUT);
	gpio_init(D_PIN);
	gpio_set_dir(D_PIN, GPIO_OUT);
}

void set_gpio(const uint note) {
	switch (note){
		case 60:
		case 72: 
			gpio_put(C_PIN, true);
			break;
		case 62:
		case 74: 
			gpio_put(D_PIN, true);
			break;	
	}
}

void clear_gpio(const uint note) {
	switch (note){
		case 60:
		case 72: 
			gpio_put(C_PIN, false);
			break;
		case 62:
		case 74: 
			gpio_put(D_PIN, false);
			break;	
	}
}
