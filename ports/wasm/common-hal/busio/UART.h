// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

// busio.UART is not simulated (CIRCUITPY_BUSIO_UART = 0); this only satisfies
// the build.
typedef struct {
    mp_obj_base_t base;
} busio_uart_obj_t;
