// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Forwards what busdisplay sends to the VIDI X's ILI9341 panel to the host,
// which models the panel (see packages/core/src/panel.ts). Only displays whose
// FourWire bus is wired like the real panel reach it; others drive nothing.

#include "py/obj.h"
#include "shared-bindings/fourwire/FourWire.h"
#include "shared-module/busdisplay/BusDisplay.h"
#include "shared-module/fourwire/FourWire.h"

#include "host.h"
#include "vclock.h"

// CPU time displayio spends compositing one pixel before it is sent.
// Placeholder until calibrated against the real board (milestone M4).
#define FILL_NS_PER_PIXEL (250)

static bool reaches_panel(busdisplay_busdisplay_obj_t *self) {
    if (!mp_obj_is_type(self->bus.bus, &fourwire_fourwire_type)) {
        return false;
    }
    const fourwire_fourwire_obj_t *fourwire = MP_OBJ_TO_PTR(self->bus.bus);
    const busio_spi_obj_t *spi = fourwire->bus;
    return fourwire->command.pin == &pin_GPIO21 &&
           fourwire->chip_select.pin == &pin_GPIO5 &&
           spi != NULL && spi->clock == &pin_GPIO18 && spi->MOSI == &pin_GPIO23;
}

static uint32_t display_id(busdisplay_busdisplay_obj_t *self) {
    return (uint32_t)(uintptr_t)self;
}

void port_busdisplay_initialized(busdisplay_busdisplay_obj_t *self, const uint8_t *init_sequence, uint16_t init_sequence_len) {
    if (!reaches_panel(self)) {
        return;
    }
    vidix_host_display_init((double)vclock_now_ns(), display_id(self), init_sequence, init_sequence_len);
}

void port_busdisplay_pixels_sent(busdisplay_busdisplay_obj_t *self, const displayio_area_t *area, const uint8_t *pixels, uint32_t length) {
    vclock_advance_ns((uint64_t)displayio_area_size(area) * FILL_NS_PER_PIXEL);
    if (!reaches_panel(self) || self->core.colorspace.depth != 16) {
        return;
    }
    // Column and row addresses as sent by displayio_display_bus_set_region_to_update().
    vidix_host_display_write((double)vclock_now_ns(), display_id(self),
        area->x1 + self->bus.colstart, area->y1 + self->bus.rowstart,
        area->x2 + self->bus.colstart - 1, area->y2 + self->bus.rowstart - 1,
        pixels, length);
}

void port_busdisplay_refreshed(busdisplay_busdisplay_obj_t *self) {
    if (!reaches_panel(self)) {
        return;
    }
    vidix_host_display_refreshed((double)vclock_now_ns(), display_id(self));
}
