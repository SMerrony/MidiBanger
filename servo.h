/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SERVO_H
#define SERVO_H

#include "pico/stdlib.h"

typedef struct {
    uint pin;
    uint slice_num;
    uint16_t zero_duty;
    uint16_t full_duty;
    uint16_t min_angle;
    uint16_t max_angle;
    uint16_t duty_range;    // calculated for efficiency
    uint16_t angle_range;   // calculated for efficiency
} servo_t;

void servo_setup(servo_t *servo, uint pin, uint16_t zero_duty, uint16_t full_duty, uint16_t min_angle, uint16_t max_angle);
void servo_start(const servo_t *servo, uint16_t start_angle);
void servo_stop(const servo_t *servo);
void servo_set_angle(const servo_t *servo, uint16_t angle);

#endif