// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Deterministic virtual clock. Time only moves when the simulated MCU does
// work (cost model) or idles (sleep), never with host wall time directly.

#pragma once

#include <stdbool.h>
#include <stdint.h>

uint64_t vclock_now_ns(void);
void vclock_advance_ns(uint64_t ns);
// Move the clock forward to ns. Earlier values are ignored.
void vclock_advance_to_ns(uint64_t ns);

// Called often from background tasks; syncs with the host when due.
void vclock_poll(void);
// Sync with the host immediately and handle the returned flags.
void vclock_sync(void);
void vclock_handle_host_flags(uint32_t flags);
