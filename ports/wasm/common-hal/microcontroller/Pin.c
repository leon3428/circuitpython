// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "host.h"
#include "vclock.h"

// Claim tracking mirrors the espressif port: a pin is in use from construction
// until deinit or VM reset, unless marked never-reset (e.g. the display bus).
static uint64_t in_use;
static uint64_t never_reset;

#define PIN_BIT(n) (((uint64_t)1) << (n))

// Return a pin to its power-on state. The VIDI X board code pulls GPIO2 (the
// blue LED) down instead of up; see espressif_board_reset_pin_number().
static void reset_pad(uint8_t pin_number) {
    uint32_t pull = pin_number == 2 ? VIDIX_GPIO_PULL_DOWN : VIDIX_GPIO_PULL_NONE;
    vidix_host_gpio_config((double)vclock_now_ns(), pin_number, VIDIX_GPIO_MODE_DISABLED, pull, 0);
}

void reset_all_pins(void) {
    for (uint8_t i = 0; i < 40; i++) {
        if ((in_use & PIN_BIT(i)) != 0 && (never_reset & PIN_BIT(i)) == 0) {
            reset_pad(i);
        }
    }
    in_use &= never_reset;
}

void never_reset_pin_number(uint8_t pin_number) {
    never_reset |= PIN_BIT(pin_number);
}

void reset_pin_number(uint8_t pin_number) {
    never_reset &= ~PIN_BIT(pin_number);
    in_use &= ~PIN_BIT(pin_number);
    reset_pad(pin_number);
}

void common_hal_never_reset_pin(const mcu_pin_obj_t *pin) {
    never_reset_pin_number(pin->number);
}

void common_hal_reset_pin(const mcu_pin_obj_t *pin) {
    if (pin == NULL) {
        return;
    }
    reset_pin_number(pin->number);
}

void claim_pin(const mcu_pin_obj_t *pin) {
    in_use |= PIN_BIT(pin->number);
}

bool pin_number_is_free(uint8_t pin_number) {
    return (in_use & PIN_BIT(pin_number)) == 0;
}

bool common_hal_mcu_pin_is_free(const mcu_pin_obj_t *pin) {
    return pin_number_is_free(pin->number);
}

uint8_t common_hal_mcu_pin_number(const mcu_pin_obj_t *pin) {
    return pin->number;
}

void common_hal_mcu_pin_claim(const mcu_pin_obj_t *pin) {
    claim_pin(pin);
}

void common_hal_mcu_pin_reset_number(uint8_t pin_no) {
    reset_pin_number(pin_no);
}

// ADC assignments from ports/espressif/peripherals/esp32/pins.c.
#define PIN(p_number, p_adc_index, p_adc_channel) \
    const mcu_pin_obj_t pin_GPIO##p_number = { \
        { &mcu_pin_type }, \
        .number = p_number, \
        .adc_index = p_adc_index, \
        .adc_channel = p_adc_channel, \
    }

PIN(0, ADC_UNIT_2, 1);
PIN(1, NO_ADC, NO_ADC_CHANNEL);
PIN(2, ADC_UNIT_2, 2);
PIN(3, NO_ADC, NO_ADC_CHANNEL);
PIN(4, ADC_UNIT_2, 0);
PIN(5, NO_ADC, NO_ADC_CHANNEL);
PIN(12, ADC_UNIT_2, 5);
PIN(13, ADC_UNIT_2, 4);
PIN(14, ADC_UNIT_2, 6);
PIN(15, ADC_UNIT_2, 3);
PIN(18, NO_ADC, NO_ADC_CHANNEL);
PIN(19, NO_ADC, NO_ADC_CHANNEL);
PIN(21, NO_ADC, NO_ADC_CHANNEL);
PIN(22, NO_ADC, NO_ADC_CHANNEL);
PIN(23, NO_ADC, NO_ADC_CHANNEL);
PIN(25, ADC_UNIT_2, 8);
PIN(26, ADC_UNIT_2, 9);
PIN(27, ADC_UNIT_2, 7);
PIN(32, ADC_UNIT_1, 4);
PIN(33, ADC_UNIT_1, 5);
PIN(34, ADC_UNIT_1, 6);
PIN(35, ADC_UNIT_1, 7);
PIN(36, ADC_UNIT_1, 0);
PIN(37, ADC_UNIT_1, 1);
PIN(38, ADC_UNIT_1, 2);
PIN(39, ADC_UNIT_1, 3);
