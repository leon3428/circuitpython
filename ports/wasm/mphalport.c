// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "mphalport.h"
#include "vclock.h"

void mp_hal_delay_us(mp_uint_t delay) {
    // Busy-wait on real hardware; here it just costs virtual time.
    vclock_advance_ns((uint64_t)delay * 1000);
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}
