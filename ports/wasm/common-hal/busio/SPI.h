// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/microcontroller/Pin.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *MOSI;
    const mcu_pin_obj_t *MISO;
    const mcu_pin_obj_t *clock;
    // ESP-IDF spi_host_device_t: SPI2_HOST (1) or SPI3_HOST (2); 0 when none.
    uint8_t host_id;
    uint8_t bits;
    uint8_t phase;
    uint8_t polarity;
    uint32_t baudrate;
    bool locked;
} busio_spi_obj_t;

void spi_reset(void);
