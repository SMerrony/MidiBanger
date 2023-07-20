#include <math.h>

#include "bsp/board.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "tusb.h"

#define SPEAKER_PIN 15

#define NOTE_OFF 8
#define NOTE_ON  9

uint slice_num;
pwm_config spkr_config;

static float midinotes[128];	// the frequencies for each standard MIDI note

/* setup_midinotes populates the midinotes array with the frequencies
   required assuming equal temperament, and A = 400Hz 
*/
void setup_midinotes() {
	for (int m = 0; m < 128; m++) {
		midinotes[m] = pow(2.0, ((m - 69) / 12.0)) * 440.0;
	}
}

/* play_speaker_note is a quick hack to check that the MIDI is basically working.
   @param freq is expressed in Hertz
   @param velocity is 0 .. 127 as per MIDI spec. N.B. With a directly connected tiny passive speaker,
   			the volume control doesn't really work. 
*/
void play_speaker_note(float freq, uint8_t velocity) {
	float div = (float)clock_get_hz(clk_sys) / (freq * 10000);
	uint duty_cycle = (5000 * velocity) / 127;   // in theory max. vol is 50% duty cycle
	pwm_config_set_clkdiv(&spkr_config, div);
	pwm_config_set_wrap(&spkr_config, 10000); 
	pwm_init(slice_num, &spkr_config, true);     // start the pwm running according to the config
	pwm_set_gpio_level(SPEAKER_PIN, duty_cycle); //connect the pin to the pwm engine and set the on/off level. 
}

void handle_msg(const uint8_t msg[3]) {
    // int ch;
    int event;

    // ch = msg[0] & 0xf;
    event = (msg[0] >> 4) & 0xf;

	switch (event) {
		case NOTE_OFF:		// Note Off
			play_speaker_note(1.0, 0);
			break;
		case NOTE_ON:		// Note On
			play_speaker_note(midinotes[msg[1]], msg[2]);
			break;
		default:
			break;
	}
}

/* process_midi reads any MIDI data availble and passes on non-SysEx messages for further processing */
void process_midi() {
    static bool sysex = false;
    uint8_t msg[3];
    int n_data;

    while((n_data = tud_midi_n_available(0, 0))) {
		msg[0] = 0; msg[1] = 0; msg[2] = 0;
		if ((n_data = tud_midi_n_stream_read(0, 0, msg, 3))) {
			if (sysex) {
			for (int i = 0; i < 3; i++) {
				// swallow message(s) until the end-of-sysex marker
				if (msg[i] == 0xf7) {
					sysex = false;
					break;
				}
			}
			} else {
				sysex = (msg[0] == 0xf0);
				if (!sysex) {	// we're ignoring sysex for now
					handle_msg(msg);
				}
			}
		}
    }
}

int main() {

	board_init();
	tud_init(BOARD_TUD_RHPORT);

	// set up PWM output for the speaker
	gpio_set_function(SPEAKER_PIN, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(SPEAKER_PIN);
	spkr_config = pwm_get_default_config();

	setup_midinotes();

	// for (int mn = 69; mn <= 69 + 12; ++mn) {
	// 	play_speaker_note(midinotes[mn], 5000);
	// 	busy_wait_ms(1000);
	// }

	play_speaker_note(1.0, 0);

	while (true) {
		tud_task();
		process_midi();
	}
}
