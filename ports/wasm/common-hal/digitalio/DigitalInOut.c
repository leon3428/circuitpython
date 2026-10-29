// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Mirrors ports/espressif/common-hal/digitalio/DigitalInOut.c on the ESP32.
// The pad configuration is kept here (standing in for the IO_MUX/GPIO
// registers) and forwarded to the host, which owns the board circuitry.

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "py/runtime.h"

#include "host.h"
#include "vclock.h"

// Placeholder cost of a GPIO register access through the bindings, until
// calibrated against the real board (milestone M4).
#define GPIO_OP_COST_NS (2000)

typedef struct {
    uint8_t mode;
    uint8_t pull;
    bool level;
} pad_t;

static pad_t pads[40];

// GPIO34-39 on the ESP32 are input only and have no internal pull resistors.
static bool pin_is_input_only(uint8_t number) {
    return number >= 34;
}

static void pad_update(uint8_t number) {
    pad_t *pad = &pads[number];
    vidix_host_gpio_config((double)vclock_now_ns(), number, pad->mode, pad->pull, pad->level);
}

void common_hal_digitalio_digitalinout_never_reset(
    digitalio_digitalinout_obj_t *self) {
    never_reset_pin_number(self->pin->number);
}

digitalinout_result_t common_hal_digitalio_digitalinout_construct(
    digitalio_digitalinout_obj_t *self, const mcu_pin_obj_t *pin) {
    claim_pin(pin);
    self->pin = pin;
    pad_t *pad = &pads[pin->number];
    pad->mode = VIDIX_GPIO_MODE_INPUT;
    pad->pull = VIDIX_GPIO_PULL_NONE;
    pad_update(pin->number);
    return DIGITALINOUT_OK;
}

bool common_hal_digitalio_digitalinout_deinited(digitalio_digitalinout_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_digitalio_digitalinout_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_input(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    common_hal_digitalio_digitalinout_set_pull(self, pull);
    pads[self->pin->number].mode = VIDIX_GPIO_MODE_INPUT;
    pad_update(self->pin->number);
    return DIGITALINOUT_OK;
}

digitalinout_result_t common_hal_digitalio_digitalinout_switch_to_output(
    digitalio_digitalinout_obj_t *self, bool value,
    digitalio_drive_mode_t drive_mode) {
    common_hal_digitalio_digitalinout_set_value(self, value);
    return common_hal_digitalio_digitalinout_set_drive_mode(self, drive_mode);
}

digitalio_direction_t common_hal_digitalio_digitalinout_get_direction(
    digitalio_digitalinout_obj_t *self) {
    uint8_t mode = pads[self->pin->number].mode;
    if (mode == VIDIX_GPIO_MODE_OUTPUT || mode == VIDIX_GPIO_MODE_OPEN_DRAIN) {
        return DIRECTION_OUTPUT;
    }
    return DIRECTION_INPUT;
}

void common_hal_digitalio_digitalinout_set_value(
    digitalio_digitalinout_obj_t *self, bool value) {
    vclock_advance_ns(GPIO_OP_COST_NS);
    self->output_value = value;
    // The output latch is written even while the pin is an input, as on the ESP32.
    pads[self->pin->number].level = value;
    pad_update(self->pin->number);
}

bool common_hal_digitalio_digitalinout_get_value(
    digitalio_digitalinout_obj_t *self) {
    vclock_advance_ns(GPIO_OP_COST_NS);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_INPUT) {
        vclock_poll();
        return vidix_host_gpio_read((double)vclock_now_ns(), self->pin->number) == 1;
    }
    return self->output_value;
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_drive_mode(
    digitalio_digitalinout_obj_t *self,
    digitalio_drive_mode_t drive_mode) {
    uint8_t number = self->pin->number;
    if (pin_is_input_only(number)) {
        return DIGITALINOUT_INPUT_ONLY;
    }
    pads[number].mode = drive_mode == DRIVE_MODE_OPEN_DRAIN ?
        VIDIX_GPIO_MODE_OPEN_DRAIN : VIDIX_GPIO_MODE_OUTPUT;
    pad_update(number);
    return DIGITALINOUT_OK;
}

digitalio_drive_mode_t common_hal_digitalio_digitalinout_get_drive_mode(
    digitalio_digitalinout_obj_t *self) {
    if (pads[self->pin->number].mode == VIDIX_GPIO_MODE_OPEN_DRAIN) {
        return DRIVE_MODE_OPEN_DRAIN;
    }
    return DRIVE_MODE_PUSH_PULL;
}

digitalinout_result_t common_hal_digitalio_digitalinout_set_pull(
    digitalio_digitalinout_obj_t *self, digitalio_pull_t pull) {
    uint8_t number = self->pin->number;
    // ESP-IDF rejects pulls on input-only pins; the espressif port ignores
    // that error, so the request silently has no effect.
    if (pin_is_input_only(number) || pull == PULL_NONE) {
        pads[number].pull = VIDIX_GPIO_PULL_NONE;
    } else {
        pads[number].pull = pull == PULL_UP ? VIDIX_GPIO_PULL_UP : VIDIX_GPIO_PULL_DOWN;
    }
    pad_update(number);
    return DIGITALINOUT_OK;
}

digitalio_pull_t common_hal_digitalio_digitalinout_get_pull(
    digitalio_digitalinout_obj_t *self) {
    switch (pads[self->pin->number].pull) {
        case VIDIX_GPIO_PULL_UP:
            return PULL_UP;
        case VIDIX_GPIO_PULL_DOWN:
            return PULL_DOWN;
        default:
            return PULL_NONE;
    }
}
