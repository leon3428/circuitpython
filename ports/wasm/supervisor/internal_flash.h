// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

// Raw CIRCUITPY image, exported to the host.
uint8_t *vidix_flash_ptr(void);
uint32_t vidix_flash_size(void);
