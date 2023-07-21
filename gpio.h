/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GPIO_H
#define GPIO_H

#include "pico/stdlib.h"

void setup_gpio();
void set_gpio(const uint note);
void clear_gpio(const uint note);

#endif