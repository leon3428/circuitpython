// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Mirrors ports/espressif/common-hal/analogio/AnalogIn.c on the ESP32. The host
// models the board's analog circuitry and the ESP32 ADC (11 dB attenuation,
// calibration) and returns the calibrated voltage; the scaling to 16 bits is
// done here exactly as on the board.

#include "common-hal/analogio/AnalogIn.h"
#include "shared-bindings/analogio/AnalogIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"

#include "host.h"
#include "vclock.h"

// Placeholder cost of one read (ADC unit setup, two samples, calibration),
// until calibrated against the real board (milestone M4).
#define ADC_READ_COST_NS (60000)

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t *self,
    const mcu_pin_obj_t *pin) {
    if (pin->adc_index == NO_ADC || pin->adc_channel == NO_ADC_CHANNEL) {
        raise_ValueError_invalid_pin();
    }
    common_hal_mcu_pin_claim(pin);
    self->pin = pin;
    // The espressif port disables the pull-up here; GPIO34-39 have none anyway.
    vidix_host_gpio_config((double)vclock_now_ns(), pin->number,
        VIDIX_GPIO_MODE_DISABLED, VIDIX_GPIO_PULL_NONE, 0);
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t *self) {
    return self->pin == NULL;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t *self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    vclock_advance_ns(ADC_READ_COST_NS);
    vclock_poll();
    int32_t voltage = vidix_host_adc_read_mv((double)vclock_now_ns(), self->pin->number);
    return voltage * ((1 << 16) - 1) / 3300;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 3.3f;
}
