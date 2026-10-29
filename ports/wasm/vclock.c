// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include "vclock.h"

#include "host.h"
#include "supervisor/serial_rx.h"
#include "supervisor/vidix_files.h"

static uint64_t now_ns;
static uint64_t last_sync_ns;

// Bytecodes executed since the last fold, counted by MICROPY_VM_HOOK_DISPATCH.
uint32_t vclock_ops;

// Placeholder until calibrated against the real board (milestone M4).
#define NS_PER_OP (300ULL)

static inline void fold_ops(void) {
    now_ns += (uint64_t)vclock_ops * NS_PER_OP;
    vclock_ops = 0;
}

// Sync with the host at most this often in virtual time, so inputs scheduled by
// the host land with millisecond resolution even in busy loops.
#define SYNC_INTERVAL_NS (1000000ULL)

uint64_t vclock_now_ns(void) {
    fold_ops();
    return now_ns;
}

void vclock_advance_ns(uint64_t ns) {
    now_ns += ns;
}

void vclock_advance_to_ns(uint64_t ns) {
    fold_ops();
    if (ns > now_ns) {
        now_ns = ns;
    }
}

void vclock_handle_host_flags(uint32_t flags) {
    if (flags & VIDIX_HOST_SERIAL_RX) {
        serial_rx_poll_host();
    }
    if (flags & VIDIX_HOST_FILES_PENDING) {
        vidix_files_pending();
    }
}

void vclock_sync(void) {
    fold_ops();
    last_sync_ns = now_ns;
    vclock_handle_host_flags(vidix_host_sync((double)now_ns));
}

void vclock_poll(void) {
    fold_ops();
    if (now_ns - last_sync_ns >= SYNC_INTERVAL_NS) {
        vclock_sync();
    }
}
