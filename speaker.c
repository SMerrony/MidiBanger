/**
 * SPDX-FileCopyrightText: 2023 Stephen Merrony
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "speaker.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"

static uint speaker_pin;
static pwm_config spkr_config;
static uint slice_num;


void setup_speaker(const uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(pin);
    spkr_config = pwm_get_default_config();
    speaker_pin = pin;
}

/* play_speaker_note is a quick hack to check that the MIDI is basically working.
   @param freq is expressed in Hertz
   @param velocity is 0 .. 127 as per MIDI spec. N.B. With a directly connected tiny passive speaker,
   			the volume control doesn't really work. 
*/
void play_speaker_note(const float freq, const uint8_t velocity) {
	float div = (float)clock_get_hz(clk_sys) / (freq * 10000);
	uint duty_cycle = (5000 * velocity) / 127;   // in theory max. vol is 50% duty cycle
	pwm_config_set_clkdiv(&spkr_config, div);
	pwm_config_set_wrap(&spkr_config, 10000); 
	pwm_init(slice_num, &spkr_config, true);     // start the pwm running according to the config
	pwm_set_gpio_level(speaker_pin, duty_cycle); //connect the pin to the pwm engine and set the on/off level. 
}