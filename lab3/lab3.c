#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"

extern uint32_t counter;
extern uint8_t scancode;
extern int timer_counter;

int main(int argc, char *argv[]) {
	// sets the language of LCF messages (can be either EN-US or PT-PT)
	lcf_set_language("EN-US");

	// enables to log function invocations that are being "wrapped" by LCF
	// [comment this out if you don't want/need it]
	// lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

	// enables to save the output of printf function calls on a file
	// [comment this out if you don't want/need it]
	// lcf_log_output("/home/lcom/labs/lab3/output.txt");

	// handles control over to LCF
	// [LCF handles command line arguments and invokes the right function]
	if (lcf_start(argc, argv))
		return 1;

	// LCF clean up tasks
	// [must be the last statement before return]
	lcf_cleanup();

	return 0;
}

int (kbd_test_scan)() {
	int ipc_status;
	uint8_t irq_set;
	message msg;
	uint8_t r;

	if (keyboard_subscribe_int(&irq_set) != 0) return 1;

	while (scancode != KBC_BRK_ESC_KEY) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver receiver failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if (msg.m_notify.interrupts & irq_set) {
						keyboard_ih();
						kbd_print_scancode(!(scancode & KBC_MSB_SCNCD), scancode == KBC_2BYTE_CODE ? 2 : 1, &scancode);
					}
					break;
				default:
					break;
			}
		}
	}

	if (keyboard_unsubscribe_int() != 0) return 1;
	
	if (kbd_print_no_sysinb(counter) != 0) return 1;

	return 0;
}

int (kbd_test_poll)() {
	/* To be completed by the students */
	printf("%s is not yet implemented!\n", __func__);

	return 1;
}

int (kbd_test_timed_scan)(uint8_t n) {
	int ipc_status;
	uint8_t irq_set_timer, irq_set_kbd;
	message msg;
	uint8_t r;

	if (timer_set_frequency(0, 60) != 0) return 1;

	if (timer_subscribe_int(&irq_set_timer) != 0) return 1;

	if (keyboard_subscribe_int(&irq_set_kbd) != 0) return 1;

	while ((scancode != KBC_BRK_ESC_KEY) && (timer_counter < 60 * n)) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver receiver failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if (msg.m_notify.interrupts & irq_set_timer) {
						timer_int_handler();
					}

					if (msg.m_notify.interrupts & irq_set_kbd) {
						keyboard_ih();
						kbd_print_scancode(!(scancode & KBC_MSB_SCNCD), scancode == KBC_2BYTE_CODE ? 2 : 1, &scancode);
						timer_counter = 0;
					}
					break;

				default: break;
			}
		}
	}

	if (timer_unsubscribe_int() != 0) return 1;

	if (keyboard_unsubscribe_int() != 0) return 1;

	if (kbd_print_no_sysinb(counter) != 0) return 1;

	return 0;
}
