// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include <emscripten/stack.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "supervisor/background_callback.h"
#include "supervisor/cpu.h"
#include "supervisor/port.h"
#include "supervisor/shared/safe_mode.h"
#include "supervisor/shared/tick.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "common-hal/microcontroller/Pin.h"
#if CIRCUITPY_ANALOGIO
#include "common-hal/analogio/AnalogOut.h"
#endif
#if CIRCUITPY_BUSIO
#include "common-hal/busio/SPI.h"
#endif

#include "host.h"
#include "supervisor/vidix_files.h"
#include "vclock.h"

// Backing store for the supervisor heap (TLSF), which also holds the VM heap.
static uint32_t heap[WASM_PORT_HEAP_SIZE / sizeof(uint32_t)];

safe_mode_t port_init(void) {
    return SAFE_MODE_NONE;
}

// Called once after the filesystem is up and again after every VM run, so the
// host's file writes land between runs like a save over USB.
void reset_port(void) {
    #if CIRCUITPY_ANALOGIO
    analogout_reset();
    #endif
    reset_all_pins();
    #if CIRCUITPY_BUSIO
    spi_reset();
    #endif
    vidix_files_apply();
    vidix_host_event(VIDIX_EVENT_RESET_PORT);
}

void reset_to_bootloader(void) {
    vidix_host_reset();
    while (true) {
    }
}

void reset_cpu(void) {
    vidix_host_reset();
    while (true) {
    }
}

// The Emscripten shadow stack grows down from its base. Live pointers are
// spilled onto it (Binaryen --spill-pointers), so scanning it finds GC roots.
uint32_t *port_stack_get_top(void) {
    return (uint32_t *)emscripten_stack_get_base();
}

uint32_t *port_stack_get_limit(void) {
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"
    return port_stack_get_top() - (CIRCUITPY_DEFAULT_STACK_SIZE + CIRCUITPY_EXCEPTION_STACK_SIZE) / sizeof(uint32_t);
    #pragma GCC diagnostic pop
}

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs) {
    (void)regs;
    return (mp_uint_t)emscripten_stack_get_current();
}

uint32_t *port_heap_get_bottom(void) {
    return heap;
}

uint32_t *port_heap_get_top(void) {
    return heap + MP_ARRAY_SIZE(heap);
}

static uint32_t saved_word;
void port_set_saved_word(uint32_t value) {
    saved_word = value;
}

uint32_t port_get_saved_word(void) {
    return saved_word;
}

// Reading the tick counter costs time, as on hardware. Besides realism this
// guarantees progress in C loops that poll ticks without executing bytecode or
// idling (e.g. wait_for_safe_mode_reset()). Calibrated in milestone M4.
#define TICK_READ_COST_NS (500)

// Ticks are 1/1024 s with 32 subticks, as with a 32 kHz RTC.
uint64_t port_get_raw_ticks(uint8_t *subticks) {
    vclock_advance_ns(TICK_READ_COST_NS);
    uint64_t ticks_32khz = vclock_now_ns() / 1000 * 32768 / 1000000;
    if (subticks != NULL) {
        *subticks = ticks_32khz % 32;
    }
    return ticks_32khz / 32;
}

// First virtual nanosecond at which port_get_raw_ticks() returns tick.
static uint64_t tick_start_ns(uint64_t tick) {
    return (tick * 1000000000ULL + 1023) / 1024;
}

static bool ticks_enabled;
static uint64_t last_tick_delivered;
static uint64_t wake_tick = UINT64_MAX;

// Stand-in for the tick timer interrupt: missed ticks collapse into one call,
// which is fine because supervisor_tick() only schedules background work.
static void deliver_ticks(void) {
    if (!ticks_enabled) {
        return;
    }
    uint64_t now = port_get_raw_ticks(NULL);
    if (now != last_tick_delivered) {
        last_tick_delivered = now;
        supervisor_tick();
    }
}

void port_enable_tick(void) {
    ticks_enabled = true;
    last_tick_delivered = port_get_raw_ticks(NULL);
}

void port_disable_tick(void) {
    ticks_enabled = false;
}

void port_interrupt_after_ticks(uint32_t ticks) {
    wake_tick = port_get_raw_ticks(NULL) + ticks;
}

void port_idle_until_interrupt(void) {
    common_hal_mcu_disable_interrupts();
    if (!background_callback_pending()) {
        uint64_t wake = wake_tick;
        if (ticks_enabled) {
            wake = MIN(wake, port_get_raw_ticks(NULL) + 1);
        }
        double wake_ns = wake == UINT64_MAX ? INFINITY : (double)tick_start_ns(wake);
        double resume_ns = vidix_host_idle((double)vclock_now_ns(), wake_ns);
        vclock_advance_to_ns((uint64_t)resume_ns);
        wake_tick = UINT64_MAX;
        vclock_sync();
    }
    common_hal_mcu_enable_interrupts();
    deliver_ticks();
}

void port_post_code_py(void) {
    vidix_host_event(VIDIX_EVENT_CODE_DONE);
}

void port_background_task(void) {
    vclock_poll();
    deliver_ticks();
}
