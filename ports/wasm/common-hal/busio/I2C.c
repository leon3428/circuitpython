// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Mirrors ports/espressif/common-hal/busio/I2C.c on the ESP32: two I2C
// controllers and a pull-up check at construction. No I2C devices are
// simulated, so every address NACKs.

#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "host.h"
#include "vclock.h"

#define I2C_CONTROLLERS (2)

// Like the ESP-IDF driver, controllers are only released by deinit.
static uint8_t controllers_in_use;

static void set_pad(const mcu_pin_obj_t *pin, uint32_t pull) {
    vidix_host_gpio_config((double)vclock_now_ns(), pin->number, VIDIX_GPIO_MODE_INPUT, pull, 0);
}

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout_us) {
    // Ensure the object starts in its deinit state.
    common_hal_busio_i2c_mark_deinit(self);

    #if CIRCUITPY_REQUIRE_I2C_PULLUPS
    // Test that the pins are in a high state. (Hopefully indicating they are pulled up.)
    set_pad(sda, VIDIX_GPIO_PULL_DOWN);
    set_pad(scl, VIDIX_GPIO_PULL_DOWN);
    common_hal_mcu_delay_us(10);
    set_pad(sda, VIDIX_GPIO_PULL_NONE);
    set_pad(scl, VIDIX_GPIO_PULL_NONE);
    // We must pull up within 3us to achieve 400khz.
    common_hal_mcu_delay_us((1200000 + frequency - 1) / frequency);
    double now = (double)vclock_now_ns();
    if (vidix_host_gpio_read(now, sda->number) == 0 || vidix_host_gpio_read(now, scl->number) == 0) {
        reset_pin_number(sda->number);
        reset_pin_number(scl->number);
        mp_raise_RuntimeError(MP_ERROR_TEXT("No pull up found on SDA or SCL; check your wiring"));
    }
    #endif

    if (controllers_in_use == I2C_CONTROLLERS) {
        mp_raise_ValueError(MP_ERROR_TEXT("All I2C peripherals are in use"));
    }
    controllers_in_use++;

    self->sda_pin = sda;
    self->scl_pin = scl;
    self->has_lock = false;
    self->frequency = frequency;
    self->timeout_ms = 1000;

    claim_pin(sda);
    claim_pin(scl);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return self->sda_pin == NULL;
}

void common_hal_busio_i2c_mark_deinit(busio_i2c_obj_t *self) {
    self->sda_pin = NULL;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
    controllers_in_use--;
    common_hal_reset_pin(self->sda_pin);
    common_hal_reset_pin(self->scl_pin);
    common_hal_busio_i2c_mark_deinit(self);
}

// Time to clock out the address byte and see the NACK.
static void address_cost(busio_i2c_obj_t *self) {
    vclock_advance_ns(9ULL * 1000000000ULL / self->frequency);
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    address_cost(self);
    return false;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self) || self->has_lock) {
        return false;
    }
    self->has_lock = true;
    return true;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return self->has_lock;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
    self->has_lock = false;
}

// ESP_FAIL (address NACK) maps to ENODEV in the espressif port.
uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr, const uint8_t *data, size_t len) {
    address_cost(self);
    return MP_ENODEV;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr, uint8_t *data, size_t len) {
    address_cost(self);
    return MP_ENODEV;
}

uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {
    address_cost(self);
    return MP_ENODEV;
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
    common_hal_never_reset_pin(self->scl_pin);
    common_hal_never_reset_pin(self->sda_pin);
}
