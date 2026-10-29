// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include <math.h>
#include <string.h>

#include "common-hal/microcontroller/Processor.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/microcontroller/ResetReason.h"

float common_hal_mcu_processor_get_temperature(void) {
    return NAN;
}

float common_hal_mcu_processor_get_voltage(void) {
    return NAN;
}

uint32_t common_hal_mcu_processor_get_frequency(void) {
    return 240000000;
}

void common_hal_mcu_processor_set_frequency(mcu_processor_obj_t *self, uint32_t frequency) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_mcu_processor_get_uid(uint8_t raw_id[]) {
    // A fixed, plausible ESP32 MAC address.
    static const uint8_t mac[COMMON_HAL_MCU_PROCESSOR_UID_LENGTH] = {0x24, 0x0a, 0xc4, 0x00, 0x00, 0x01};
    memcpy(raw_id, mac, sizeof(mac));
}

mcu_reset_reason_t common_hal_mcu_processor_get_reset_reason(void) {
    return RESET_REASON_POWER_ON;
}
