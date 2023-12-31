// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "mouse.h"

extern struct packet packet;
extern uint8_t idx;
extern int timer_counter;

int main(int argc, char *argv[]) {
	// sets the language of LCF messages (can be either EN-US or PT-PT)
	lcf_set_language("EN-US");

	// enables to log function invocations that are being "wrapped" by LCF
	// [comment this out if you don't want/need/ it]
	// lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

	// enables to save the output of printf function calls on a file
	// [comment this out if you don't want/need it]
	// lcf_log_output("/home/lcom/labs/lab4/output.txt");

	// handles control over to LCF
	// [LCF handles command line arguments and invokes the right function]
	if (lcf_start(argc, argv))
		return 1;

	// LCF clean up tasks
	// [must be the last statement before return]
	lcf_cleanup();

	return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
    int ipc_status;
    uint8_t irq_set;
    message msg;
    uint8_t r;

	if (mouse_write_cmd(MOUSE_EN_DATA_REP) != 0) return 1;

    if (mouse_subscribe_int(&irq_set) != 0) return 1;

    while (cnt) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

		if (is_ipc_notify(ipc_status)) {
			switch(_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: 
					if (msg.m_notify.interrupts & irq_set) { 
            			mouse_ih();                           
            			mouse_sync_bytes();                      
           				if (idx == 3) { 
							mouse_parse_packet();                   
							mouse_print_packet(&packet);      
							idx = 0;
							cnt--;
            			}
        	  		}
		  			break;
				default:
					break;
      		}
    	}
	}

	if (mouse_unsubscribe_int() != 0) return 1;

	if (mouse_write_cmd(MOUSE_DIS_DATA_REP) != 0) return 1;

	return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    int ipc_status;
	uint8_t irq_set_timer, irq_set_mouse;
	message msg;
	uint8_t r;

	uint8_t freq = sys_hz();

	if (mouse_write_cmd(MOUSE_EN_DATA_REP) != 0) return 1;

	if (mouse_subscribe_int(&irq_set_mouse) != 0) return 1;

	if (timer_subscribe_int(&irq_set_timer) != 0) return 1;

	while (timer_counter < idle_time * freq) {
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;

		if (is_ipc_notify(ipc_status)) {
			switch(_ENDPOINT_P(msg.m_source)) {
				case HARDWARE: 
					if (msg.m_notify.interrupts & irq_set_mouse) { 
						mouse_ih();                           
						mouse_sync_bytes();                      
		   				if (idx == 3) { 
							mouse_parse_packet();                   
							mouse_print_packet(&packet);      
							idx = 0;
						}
						timer_counter = 0;
			  		}
					
					if (msg.m_notify.interrupts & irq_set_timer) {
						timer_int_handler();
					}
		  			
					break;
				default:
					break;
	  		}
		}
	}

	if (timer_unsubscribe_int() != 0) return 1;

	if (mouse_unsubscribe_int() != 0) return 1;

	if (mouse_write_cmd(MOUSE_DIS_DATA_REP) != 0) return 1;

    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
