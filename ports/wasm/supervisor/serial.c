// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>

#include "py/mpconfig.h"
#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/serial.h"
#include "common-hal/microcontroller/Pin.h"

#include "host.h"
#include "supervisor/serial_rx.h"
#include "vclock.h"

// Same size as the espressif console UART RX buffer.
#define RX_BUF_SIZE (256)

static uint8_t rx_buf[RX_BUF_SIZE];
static uint32_t rx_head;
static uint32_t rx_tail;

static uint32_t rx_count(void) {
    return rx_head - rx_tail;
}

void serial_rx_poll_host(void) {
    while (rx_count() < RX_BUF_SIZE) {
        int32_t c = vidix_host_serial_read();
        if (c < 0) {
            break;
        }
        if (c == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
            continue;
        }
        rx_buf[rx_head++ % RX_BUF_SIZE] = (uint8_t)c;
    }
}

// The real console is UART0 on GPIO1 (TX) and GPIO3 (RX); constructing it
// claims both pins for good, so user code finds them in use.
void port_serial_early_init(void) {
    claim_pin(&pin_GPIO1);
    claim_pin(&pin_GPIO3);
    never_reset_pin_number(1);
    never_reset_pin_number(3);
}

void port_serial_init(void) {
}

bool port_serial_connected(void) {
    return true;
}

char port_serial_read(void) {
    if (rx_count() == 0) {
        return -1;
    }
    return rx_buf[rx_tail++ % RX_BUF_SIZE];
}

// Polling an empty console costs time like any loop iteration on the MCU, so
// busy-wait loops (e.g. mp_hal_stdin_rx_chr() in the REPL) let virtual time
// advance until the host delivers input. Calibrated in milestone M4.
#define EMPTY_POLL_COST_NS (1000)

uint32_t port_serial_bytes_available(void) {
    if (rx_count() == 0) {
        vclock_advance_ns(EMPTY_POLL_COST_NS);
        vclock_poll();
    }
    return rx_count();
}

// The VIDI X console is UART0 at 115200 baud (8N1, 10 bits per byte) through a
// USB-serial chip. Output beyond the 128-byte hardware FIFO blocks the writer
// until the FIFO drains, which is what limits print-heavy loops on the board.
#define UART_BYTE_NS (1000000000ULL * 10 / 115200)
#define UART_FIFO_BYTES (128)

// Virtual time at which the last queued byte has been sent.
static uint64_t uart_tx_done_ns;

void port_serial_write_substring(const char *text, uint32_t length) {
    uint64_t now = vclock_now_ns();
    if (uart_tx_done_ns < now) {
        uart_tx_done_ns = now;
    }
    uart_tx_done_ns += length * UART_BYTE_NS;
    uint64_t fifo_ns = UART_FIFO_BYTES * UART_BYTE_NS;
    if (uart_tx_done_ns > now + fifo_ns) {
        vclock_advance_to_ns(uart_tx_done_ns - fifo_ns);
    }
    vidix_host_serial_write((double)now, text, length);
}
