// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

// ADC units as in ESP-IDF's adc_unit_t, plus NO_ADC.
#define ADC_UNIT_1     (0)
#define ADC_UNIT_2     (1)
#define NO_ADC         (3)
#define NO_ADC_CHANNEL (0x3f)

typedef struct {
    mp_obj_base_t base;
    uint8_t number;
    uint8_t adc_index : 2;
    uint8_t adc_channel : 6;
} mcu_pin_obj_t;

// GPIOs of the ESP32-WROVER module on the VIDI X. 6-11 are wired to the
// flash and 16-17 to the PSRAM, so they are not available.
extern const mcu_pin_obj_t pin_GPIO0;
extern const mcu_pin_obj_t pin_GPIO1;
extern const mcu_pin_obj_t pin_GPIO2;
extern const mcu_pin_obj_t pin_GPIO3;
extern const mcu_pin_obj_t pin_GPIO4;
extern const mcu_pin_obj_t pin_GPIO5;
extern const mcu_pin_obj_t pin_GPIO12;
extern const mcu_pin_obj_t pin_GPIO13;
extern const mcu_pin_obj_t pin_GPIO14;
extern const mcu_pin_obj_t pin_GPIO15;
extern const mcu_pin_obj_t pin_GPIO18;
extern const mcu_pin_obj_t pin_GPIO19;
extern const mcu_pin_obj_t pin_GPIO21;
extern const mcu_pin_obj_t pin_GPIO22;
extern const mcu_pin_obj_t pin_GPIO23;
extern const mcu_pin_obj_t pin_GPIO25;
extern const mcu_pin_obj_t pin_GPIO26;
extern const mcu_pin_obj_t pin_GPIO27;
extern const mcu_pin_obj_t pin_GPIO32;
extern const mcu_pin_obj_t pin_GPIO33;
extern const mcu_pin_obj_t pin_GPIO34;
extern const mcu_pin_obj_t pin_GPIO35;
extern const mcu_pin_obj_t pin_GPIO36;
extern const mcu_pin_obj_t pin_GPIO37;
extern const mcu_pin_obj_t pin_GPIO38;
extern const mcu_pin_obj_t pin_GPIO39;

void reset_all_pins(void);
void reset_pin_number(uint8_t pin_number);
void never_reset_pin_number(uint8_t pin_number);
void claim_pin(const mcu_pin_obj_t *pin);
bool pin_number_is_free(uint8_t pin_number);
const mcu_pin_obj_t *mcu_get_pin_by_number(int number);
