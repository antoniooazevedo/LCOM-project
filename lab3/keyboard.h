#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lcom/lcf.h"
#include "kbc.h"

int (keyboard_subscribe_int)(uint8_t *bit_no);

int (keyboard_unsubscribe_int)();

void (keyboard_ih)();

int (keyboard_restore)();

#endif 
