// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// CIRCUITPY lives in a RAM array. The host can read and replace the raw image
// through the exported accessors, e.g. to persist it between sessions. The
// host fills it before main() runs; erased flash is 0xff.

#include "supervisor/internal_flash.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <emscripten/emscripten.h>

#include "mpconfigport.h"
#include "supervisor/flash.h"

static uint8_t flash[WASM_PORT_FLASH_SIZE];

EMSCRIPTEN_KEEPALIVE uint8_t *vidix_flash_ptr(void) {
    return flash;
}

EMSCRIPTEN_KEEPALIVE uint32_t vidix_flash_size(void) {
    return sizeof(flash);
}

void supervisor_flash_init(void) {
}

uint32_t supervisor_flash_get_block_size(void) {
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void) {
    return WASM_PORT_FLASH_SIZE / FILESYSTEM_BLOCK_SIZE;
}

void port_internal_flash_flush(void) {
}

mp_uint_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks) {
    memcpy(dest, flash + block * FILESYSTEM_BLOCK_SIZE, num_blocks * FILESYSTEM_BLOCK_SIZE);
    return 0;
}

mp_uint_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks) {
    memcpy(flash + lba * FILESYSTEM_BLOCK_SIZE, src, num_blocks * FILESYSTEM_BLOCK_SIZE);
    return 0;
}

void supervisor_flash_release_cache(void) {
}
