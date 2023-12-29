#ifndef MOUSE_H
#define MOUSE_H

#include <lcom/lcf.h>
#include "kbc.h"

int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

int (mouse_write_cmd)(uint8_t cmd);

void (mouse_sync_bytes)();

void (mouse_parse_packet)();

#endif
