#include "mouse.h"

int mouse_hook_id = 2;
uint8_t packetBytes[3];
uint8_t byte;
uint8_t idx = 0;
struct packet packet;

int (mouse_subscribe_int)(uint8_t *bit_no) {
    *bit_no = BIT(mouse_hook_id);

    return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}

int (mouse_unsubscribe_int)() {
    return sys_irqrmpolicy(&mouse_hook_id);
}

void (mouse_ih)() {
    kbc_read_data(KBC_OUT_BUF, &byte, true);
}

int (mouse_write_cmd)(uint8_t cmd) {
    uint8_t status;
    uint8_t attempts = 5;

    do {
        attempts--;
        if (kbc_write_cmd(KBC_IN_BUF_CMD, MOUSE_WRITE_BYTE)) return 1;
        if (kbc_write_cmd(KBC_IN_BUF_ARG, cmd)) return 1;

        tickdelay(micros_to_ticks(DELAY_US));

        if (util_sys_inb(KBC_OUT_BUF, &status)) return 1;
        if (status == KBC_ACK) return 0; 
    } while (status != KBC_ACK && attempts);

    return 1;
}

void (mouse_sync_bytes)() {
    if (idx == 0 && (byte & BIT(3))) {
        packetBytes[idx] = byte;
        idx++;
    } else if (idx > 0) {
        packetBytes[idx] = byte;
        idx++;
    }
}

void (mouse_parse_packet)() {
    for (int i = 0; i < 3; i++) {
        packet.bytes[i] = packetBytes[i];
    }

    packet.lb = packetBytes[0] & MOUSE_LB;
    packet.mb = packetBytes[0] & MOUSE_MB;
    packet.rb = packetBytes[0] & MOUSE_RB;
    packet.x_ov = packetBytes[0] & MOUSE_X_OVERFLOW;
    packet.y_ov = packetBytes[0] & MOUSE_Y_OVERFLOW;

    packet.delta_x = (packetBytes[0] & MOUSE_X_SIGNAL) ? (0xFF00 | packetBytes[1]) : packetBytes[1];
    packet.delta_y = (packetBytes[0] & MOUSE_Y_SIGNAL) ? (0xFF00 | packetBytes[2]) : packetBytes[2];
}
