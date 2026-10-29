// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// busio.UART is not simulated (CIRCUITPY_BUSIO_UART = 0), so none of this is
// reachable from Python.

#include "py/runtime.h"
#include "shared-bindings/busio/UART.h"

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx,
    const mcu_pin_obj_t *rts, const mcu_pin_obj_t *cts,
    const mcu_pin_obj_t *rs485_dir, bool rs485_invert,
    uint32_t baudrate, uint8_t bits, busio_uart_parity_t parity, uint8_t stop,
    mp_float_t timeout, uint16_t receiver_buffer_size, byte *receiver_buffer,
    bool sigint_enabled) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return true;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
}

size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    return 0;
}

size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    return 0;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return 0;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return 0;
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return 0;
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    return false;
}

void common_hal_busio_uart_never_reset(busio_uart_obj_t *self) {
}
