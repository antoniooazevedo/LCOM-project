// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "video.h"
#include "keyboard.h"

extern uint8_t scancode;
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  // lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  // lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (waiting_ESC)() {
	int ipc_status;
	message msg;
	uint8_t kbd_irq_set;
	uint8_t r;

	if (keyboard_subscribe_int(&kbd_irq_set) != 0) return 1;

	while(scancode != KBC_BRK_ESC_KEY) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d", r);
			continue;
		}

		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if (msg.m_notify.interrupts & kbd_irq_set) {
						keyboard_ih();
					}
					break;
				default:
					break;
			}
		}
	}

	if (keyboard_unsubscribe_int() != 0) return 1;

	return 0;
} 

int (video_test_init)(uint16_t mode, uint8_t delay) {
    if (vg_start(mode) != 0) return 1;

    sleep(delay);

    if (vg_exit() != 0) return 1;

    return 1;
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    if (vg_start(mode) != 0) return 1;

	normalize_color(&color);

    if (vg_draw_rectangle(x, y, width, height, color) != 0) return 1;

	if (waiting_ESC() != 0) return 1;

    if (vg_exit() != 0) return 1;

    return 0;
}

int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
	if (vg_start(mode) != 0) return 1;

	uint8_t horizontal = get_hres() / no_rectangles;
	uint8_t vertical = get_vres() / no_rectangles;

	for (uint8_t i = 0; i < no_rectangles; i++) {
		for (uint8_t j = 0; j < no_rectangles; j++) {
			uint32_t color;

			if (get_memory_model() == DIRECT_COLOR) {
				uint32_t R = Red(j, step, first);
				uint32_t G = Green(i, step, first);
				uint32_t B = Blue(j, i, step, first);
				color = direct_mode(R, G, B);
			} else {
				color = indexed_mode(j, i, step, first, no_rectangles);
			}

			if (vg_draw_rectangle(j * horizontal, i * vertical, horizontal, vertical, color) != 0) return 1;
		}
	}

	if (waiting_ESC() != 0) return 1;

	if (vg_exit() != 0) return 1;

  	return 0;
}

int (video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
	if (vg_start(VBE_768p_INDEXED) != 0) return 1;

	if (vg_display_xpm(xpm, x, y) != 0) return 1;

	if (waiting_ESC() != 0) return 1;

	if (vg_exit() != 0) return 1;

	return 1;
}

int (video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
	int ipc_status;
	message msg;
	uint8_t timer_irq_set, kbd_irq_set;
	uint8_t r;

	if (timer_subscribe_int(&timer_irq_set) != 0) return 1;

	if (keyboard_subscribe_int(&kbd_irq_set) != 0) return 1;

	if (vg_start(VBE_768p_INDEXED) != 0) return 1;

	if (vg_display_xpm(xpm, xi, yi) != 0) return 1; 

	enum direction {horizontal, vertical};

	enum direction dir;

	if (xi == xf) dir = vertical;
	else if (yi == yf) dir = horizontal;
	else return 1;

	while (scancode != KBC_BRK_ESC_KEY) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;
		
		if (is_ipc_notify(ipc_status)) {
			switch (_ENDPOINT_P(msg.m_source)) {
				case HARDWARE:
					if (msg.m_notify.interrupts & kbd_irq_set) {
						keyboard_ih();
					}
					
					if (msg.m_notify.interrupts & timer_irq_set) {
						//printf("xi: %d, yi: %d\n", xi, yi);
						timer_int_handler();
						if (timer_counter % (sys_hz() / fr_rate) == 0){
							//if (vg_erase_xpm(xpm, xi, yi) != 0) return 1;

							if (dir == vertical) {
								yi += speed;
								if (yi > yf) yi = yf;
							} else if (dir == horizontal) {
								xi += speed;
								if (xi > xf) xi = xf;
							}
							if (vg_display_xpm(xpm, xi, yi) != 0) return 1;
							vg_copy_buffer();
						}
					}
					break;
				default:
					break;
			}
		}
	}

	if (vg_exit() != 0) return 1;

	if (keyboard_unsubscribe_int() != 0) return 1;

	if (timer_unsubscribe_int() != 0) return 1;

	return 0;
}

int(video_test_controller)() {
	/* To be completed */
	printf("%s(): under construction\n", __func__);

	return 1;
}
