// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Mirrors ports/espressif/common-hal/busio/SPI.c on the ESP32: two general
// purpose SPI peripherals (SPI2 and SPI3), the last free one is used, and pins
// are claimed but not checked (the GPIO matrix allows sharing). Nothing is
// connected to the bus except the display, which is observed one level up in
// busdisplay (see ports/wasm/display.c); bytes sent here only cost time.

#include <string.h>

#include "py/runtime.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "vclock.h"

#define SPI2_HOST (1)
#define SOC_SPI_PERIPH_NUM (3)

// SPI clocks are divided down from the 80 MHz APB clock.
#define APB_CLK_FREQ (80000000)

static bool spi_in_use[SOC_SPI_PERIPH_NUM];
static bool spi_never_reset[SOC_SPI_PERIPH_NUM];

void spi_reset(void) {
    for (uint8_t host_id = SPI2_HOST; host_id < SOC_SPI_PERIPH_NUM; host_id++) {
        if (!spi_never_reset[host_id]) {
            spi_in_use[host_id] = false;
        }
    }
}

// Closest clock at or below the request that the divider can produce, like
// spi_get_actual_clock() with a 50% duty cycle.
static uint32_t actual_clock(uint32_t baudrate) {
    if (baudrate >= APB_CLK_FREQ) {
        return APB_CLK_FREQ;
    }
    uint32_t divider = (APB_CLK_FREQ + baudrate - 1) / baudrate;
    return APB_CLK_FREQ / divider;
}

static void set_spi_config(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    self->baudrate = actual_clock(baudrate);
    self->polarity = polarity;
    self->phase = phase;
    self->bits = bits;
}

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {
    if (half_duplex) {
        mp_raise_NotImplementedError_varg(MP_ERROR_TEXT("%q"), MP_QSTR_half_duplex);
    }

    self->host_id = 0;
    for (uint8_t host_id = SPI2_HOST; host_id < SOC_SPI_PERIPH_NUM; host_id++) {
        if (!spi_in_use[host_id]) {
            self->host_id = host_id;
        }
    }
    if (self->host_id == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("All SPI peripherals are in use"));
    }
    spi_in_use[self->host_id] = true;

    set_spi_config(self, 250000, 0, 0, 8);

    self->MOSI = mosi;
    self->MISO = miso;
    self->clock = clock;
    self->locked = false;

    if (mosi != NULL) {
        claim_pin(mosi);
    }
    if (miso != NULL) {
        claim_pin(miso);
    }
    claim_pin(clock);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
    spi_never_reset[self->host_id] = true;
    common_hal_never_reset_pin(self->clock);
    if (self->MOSI != NULL) {
        common_hal_never_reset_pin(self->MOSI);
    }
    if (self->MISO != NULL) {
        common_hal_never_reset_pin(self->MISO);
    }
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return self->clock == NULL;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
    common_hal_reset_pin(self->clock);
    self->clock = NULL;
    spi_never_reset[self->host_id] = false;
    spi_in_use[self->host_id] = false;
    self->locked = false;
    common_hal_reset_pin(self->MOSI);
    common_hal_reset_pin(self->MISO);
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    set_spi_config(self, baudrate, polarity, phase, bits);
    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self) || self->locked) {
        return false;
    }
    self->locked = true;
    return true;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return self->locked;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
    self->locked = false;
}

bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    if (self->MOSI == NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_mosi);
    }
    return common_hal_busio_spi_transfer(self, data, NULL, len);
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    if (self->MISO == NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_miso);
    }
    if (self->MOSI == NULL) {
        return common_hal_busio_spi_transfer(self, NULL, data, len);
    } else {
        memset(data, write_value, len);
        return common_hal_busio_spi_transfer(self, data, data, len);
    }
}

// Fixed cost of one transfer through the ESP-IDF driver. Calibrated in M4.
#define SPI_TRANSFER_OVERHEAD_NS (5000)

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self,
    const uint8_t *data_out, uint8_t *data_in, size_t len) {
    if (len == 0) {
        return true;
    }
    if (self->MOSI == NULL && data_out != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_mosi);
    }
    if (self->MISO == NULL && data_in != NULL) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_miso);
    }
    // No device drives MISO; the line idles high.
    if (data_in != NULL) {
        memset(data_in, 0xff, len);
    }
    vclock_advance_ns(SPI_TRANSFER_OVERHEAD_NS + (uint64_t)len * 8 * 1000000000ULL / self->baudrate);
    vclock_poll();
    return true;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return self->baudrate;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return self->polarity;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return self->phase;
}
