// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

// Mirror the espressif port so Python-visible behaviour matches the ESP32 board.
#define MICROPY_PY_SYS_PLATFORM             "Espressif"
#define CIRCUITPY_DIGITALIO_HAVE_INPUT_ONLY (1)
#define MICROPY_PY_FUNCTION_ATTRS           (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS  (1)

#define MICROPY_USE_INTERNAL_PRINTF         (0)

// This also includes mpconfigboard.h.
#include "py/circuitpy_mpconfig.h"

// main.c defines this newlib hook; Emscripten's musl libc doesn't declare it.
void __assert_func(const char *file, int line, const char *func, const char *expr);

// WebAssembly has no NLR assembly implementation; use setjmp/longjmp.
#define MICROPY_NLR_SETJMP                  (1)

// Count bytecodes for the virtual-time cost model (see vclock.c).
extern uint32_t vclock_ops;
#define MICROPY_VM_HOOK_DISPATCH vclock_ops++;

#define CIRCUITPY_DEFAULT_STACK_SIZE        0x6000

// Size of the RAM region handed to the supervisor heap (TLSF). The ESP32
// build gets several MB from PSRAM; match the order of magnitude.
#define WASM_PORT_HEAP_SIZE                 (4 * 1024 * 1024)

// Size of the CIRCUITPY filesystem.
#define WASM_PORT_FLASH_SIZE                (2 * 1024 * 1024)
