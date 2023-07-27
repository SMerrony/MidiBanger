/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"

#define SERVO_MIN_DUTY 500
#define SERVO_MAX_DUTY 2400
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE 180

typedef struct {
    uint pin;
    uint slice_num;
} servo_t;

servo_t setup_servo(uint pin);

void servo_start(const servo_t *servo, uint start_angle);
void servo_stop(const servo_t *servo);
void servo_set_angle(const servo_t *servo, uint angle);

#endif