// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "py/obj.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "supervisor/filesystem.h"
#include "supervisor/port.h"
#include "supervisor/shared/safe_mode.h"

void common_hal_mcu_delay_us(uint32_t delay) {
    mp_hal_delay_us(delay);
}

// There are no real interrupts: host callbacks only run at well-defined points.
// Keep the nesting count so mismatched calls are still caught.
static volatile uint32_t nesting_count = 0;

void common_hal_mcu_disable_interrupts(void) {
    nesting_count++;
}

void common_hal_mcu_enable_interrupts(void) {
    if (nesting_count == 0) {
        reset_into_safe_mode(SAFE_MODE_INTERRUPT_ERROR);
    }
    nesting_count--;
}

static bool next_reset_to_bootloader = false;

void common_hal_mcu_on_next_reset(mcu_runmode_t runmode) {
    switch (runmode) {
        case RUNMODE_UF2:
        case RUNMODE_BOOTLOADER:
            next_reset_to_bootloader = true;
            break;
        case RUNMODE_SAFE_MODE:
            safe_mode_on_next_reset(SAFE_MODE_PROGRAMMATIC);
            break;
        default:
            break;
    }
}

void common_hal_mcu_reset(void) {
    filesystem_flush();
    if (next_reset_to_bootloader) {
        reset_to_bootloader();
    } else {
        reset_cpu();
    }
}

const mcu_processor_obj_t common_hal_mcu_processor_obj = {
    .base = {
        .type = &mcu_processor_type,
    },
};

const mp_rom_map_elem_t mcu_pin_global_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_GPIO0), MP_ROM_PTR(&pin_GPIO0) },
    { MP_ROM_QSTR(MP_QSTR_GPIO1), MP_ROM_PTR(&pin_GPIO1) },
    { MP_ROM_QSTR(MP_QSTR_GPIO2), MP_ROM_PTR(&pin_GPIO2) },
    { MP_ROM_QSTR(MP_QSTR_GPIO3), MP_ROM_PTR(&pin_GPIO3) },
    { MP_ROM_QSTR(MP_QSTR_GPIO4), MP_ROM_PTR(&pin_GPIO4) },
    { MP_ROM_QSTR(MP_QSTR_GPIO5), MP_ROM_PTR(&pin_GPIO5) },
    { MP_ROM_QSTR(MP_QSTR_GPIO12), MP_ROM_PTR(&pin_GPIO12) },
    { MP_ROM_QSTR(MP_QSTR_GPIO13), MP_ROM_PTR(&pin_GPIO13) },
    { MP_ROM_QSTR(MP_QSTR_GPIO14), MP_ROM_PTR(&pin_GPIO14) },
    { MP_ROM_QSTR(MP_QSTR_GPIO15), MP_ROM_PTR(&pin_GPIO15) },
    { MP_ROM_QSTR(MP_QSTR_GPIO18), MP_ROM_PTR(&pin_GPIO18) },
    { MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_PTR(&pin_GPIO19) },
    { MP_ROM_QSTR(MP_QSTR_GPIO21), MP_ROM_PTR(&pin_GPIO21) },
    { MP_ROM_QSTR(MP_QSTR_GPIO22), MP_ROM_PTR(&pin_GPIO22) },
    { MP_ROM_QSTR(MP_QSTR_GPIO23), MP_ROM_PTR(&pin_GPIO23) },
    { MP_ROM_QSTR(MP_QSTR_GPIO25), MP_ROM_PTR(&pin_GPIO25) },
    { MP_ROM_QSTR(MP_QSTR_GPIO26), MP_ROM_PTR(&pin_GPIO26) },
    { MP_ROM_QSTR(MP_QSTR_GPIO27), MP_ROM_PTR(&pin_GPIO27) },
    { MP_ROM_QSTR(MP_QSTR_GPIO32), MP_ROM_PTR(&pin_GPIO32) },
    { MP_ROM_QSTR(MP_QSTR_GPIO33), MP_ROM_PTR(&pin_GPIO33) },
    { MP_ROM_QSTR(MP_QSTR_GPIO34), MP_ROM_PTR(&pin_GPIO34) },
    { MP_ROM_QSTR(MP_QSTR_GPIO35), MP_ROM_PTR(&pin_GPIO35) },
    { MP_ROM_QSTR(MP_QSTR_GPIO36), MP_ROM_PTR(&pin_GPIO36) },
    { MP_ROM_QSTR(MP_QSTR_GPIO37), MP_ROM_PTR(&pin_GPIO37) },
    { MP_ROM_QSTR(MP_QSTR_GPIO38), MP_ROM_PTR(&pin_GPIO38) },
    { MP_ROM_QSTR(MP_QSTR_GPIO39), MP_ROM_PTR(&pin_GPIO39) },
};
MP_DEFINE_CONST_DICT(mcu_pin_globals, mcu_pin_global_dict_table);

const mcu_pin_obj_t *mcu_get_pin_by_number(int number) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(mcu_pin_global_dict_table); i++) {
        mcu_pin_obj_t *obj = MP_OBJ_TO_PTR(mcu_pin_global_dict_table[i].value);
        if (obj->number == number) {
            return obj;
        }
    }
    return NULL;
}
