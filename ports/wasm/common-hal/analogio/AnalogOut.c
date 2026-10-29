// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Mirrors ports/espressif/common-hal/analogio/AnalogOut.c on the ESP32: the
// 8-bit DACs are on GPIO25 and GPIO26. Like the espressif port, the pin is not
// claimed.

#include <stdint.h>

#include "py/runtime.h"
#include "shared-bindings/analogio/AnalogOut.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "host.h"
#include "vclock.h"

static const mcu_pin_obj_t *active_pins[2];

static void dac_output(const mcu_pin_obj_t *pin, uint8_t dac_value) {
    vidix_host_gpio_config((double)vclock_now_ns(), pin->number,
        VIDIX_GPIO_MODE_DAC, VIDIX_GPIO_PULL_NONE, dac_value);
}

void common_hal_analogio_analogout_construct(analogio_analogout_obj_t *self,
    const mcu_pin_obj_t *pin) {
    uint8_t channel;
    if (pin == &pin_GPIO25) {
        channel = 0;
    } else if (pin == &pin_GPIO26) {
        channel = 1;
    } else {
        raise_ValueError_invalid_pin();
    }
    self->pin = pin;
    active_pins[channel] = pin;
    dac_output(pin, 0);
}

bool common_hal_analogio_analogout_deinited(analogio_analogout_obj_t *self) {
    return self->pin == NULL;
}

static void release(const mcu_pin_obj_t *pin) {
    vidix_host_gpio_config((double)vclock_now_ns(), pin->number,
        VIDIX_GPIO_MODE_DISABLED, VIDIX_GPIO_PULL_NONE, 0);
}

void common_hal_analogio_analogout_deinit(analogio_analogout_obj_t *self) {
    if (self->pin == NULL) {
        return;
    }
    release(self->pin);
    active_pins[self->pin == &pin_GPIO25 ? 0 : 1] = NULL;
    self->pin = NULL;
}

void common_hal_analogio_analogout_set_value(analogio_analogout_obj_t *self,
    uint16_t value) {
    dac_output(self->pin, (value * 255) / 65535);
}

void analogout_reset(void) {
    for (uint8_t c = 0; c < MP_ARRAY_SIZE(active_pins); c++) {
        if (active_pins[c] != NULL) {
            release(active_pins[c]);
        }
        active_pins[c] = NULL;
    }
}
