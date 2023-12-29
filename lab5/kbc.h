#ifndef KBC_H
#define KBC_H

#include <lcom/lcf.h>
#include "i8042.h"

#define DELAY_US 20000

int (kbc_get_status)(uint8_t *st);

int (kbc_read_data)(uint8_t port, uint8_t *output, bool mouse);

int (kbc_write_cmd)(uint8_t port, uint8_t cmd);

#endif
