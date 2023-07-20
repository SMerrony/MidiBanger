#include <math.h>
#include <stdio.h>

#include "bsp/board.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "tusb.h"

#define SPEAKER_PIN 15

uint slice_num;
pwm_config spkr_config;

static float midinotes[128];

void setup_midinotes() {
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
	}
}

void play_speaker_note(float freq, uint duty_cycle) {
	float div = (float)clock_get_hz(clk_sys) / (freq * 10000);

	pwm_config_set_clkdiv(&spkr_config, div);
	pwm_config_set_wrap(&spkr_config, 10000); 
	pwm_init(slice_num, &spkr_config, true); // start the pwm running according to the config
	pwm_set_gpio_level(SPEAKER_PIN, duty_cycle); //connect the pin to the pwm engine and set the on/off level. 
}

void handle_msg(const uint8_t msg[3]) {
    // int ch;
    int event;

    // ch = msg[0] & 0xf;
    event = (msg[0] >> 4) & 0xf;

	switch (event) {
		case 8:		// Note Off
			play_speaker_note(1.0, 0);
			break;
		case 9:		// Note On
			play_speaker_note(midinotes[msg[1]], 5000);
			break;
		default:
			break;
	}
}

void midi_task(void) {
    static bool sysex = false;
    uint8_t msg[3];
    int n_data;

    while((n_data = tud_midi_n_available(0, 0))) {
		msg[0] = 0; msg[1] = 0; msg[2] = 0;
		if ((n_data = tud_midi_n_stream_read(0, 0, msg, 3))) {
			if (sysex) {
			for (int i = 0; i < 3; i++) {
				// printf("%02x ", msg[i]);
				if (msg[i] == 0xf7) {
					sysex = false;
					break;
				}
			}
			} else {
				sysex = (msg[0] == 0xf0);
				handle_msg(msg);
			}
		}
    }
}

int main() {

	// stdio_init_all();
	board_init();
	// tusb_init();
	tud_init(BOARD_TUD_RHPORT);

	busy_wait_ms(5000);  // debugging - time to connect terminal
	// printf("DEBUG: MidiPlayer starting\n");

	// set up PWM output for the speaker
	gpio_set_function(SPEAKER_PIN, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(SPEAKER_PIN);
	spkr_config = pwm_get_default_config();

	setup_midinotes();
	// printf("DEBUG: Frequency of Midi note 69 (A4) is: %f\n", midinotes[69]);

	// for (int mn = 69; mn <= 69 + 12; ++mn) {
	// 	play_speaker_note(midinotes[mn], 5000);
	// 	busy_wait_ms(1000);
	// }

	play_speaker_note(1.0, 0);

	while (true) {
		tud_task();
		midi_task();
	}

	// printf("DEBUG: MidiPlayer finished\n");
}
