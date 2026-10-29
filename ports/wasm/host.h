// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Functions implemented by the JavaScript host (js/host_library.js). The host
// owns the simulated board: serial terminal, inputs, time pacing and files.

#pragma once

#include <stdint.h>

// Flags returned by vidix_host_sync() and vidix_host_idle().
#define VIDIX_HOST_SERIAL_RX     (1 << 0)
#define VIDIX_HOST_FILES_PENDING (1 << 1)

// Lifecycle events reported to the host.
#define VIDIX_EVENT_RESET_PORT   (1)
// code.py finished ("Code done running." has been printed).
#define VIDIX_EVENT_CODE_DONE    (2)

// Let the host apply input events up to now_ns and pace against wall time.
uint32_t vidix_host_sync(double now_ns);

// Sleep until wake_ns (Infinity for no deadline) or until the host has an
// earlier event. Returns the virtual time to resume at, which is >= now_ns.
double vidix_host_idle(double now_ns, double wake_ns);

void vidix_host_serial_write(double now_ns, const char *text, uint32_t length);
// Next received serial byte, or -1 if none.
int32_t vidix_host_serial_read(void);

// Pending file writes into CIRCUITPY. vidix_host_next_file() copies the path
// (NUL-terminated) into path and returns the data length, or -1 when there are
// no more files. vidix_host_file_data() then copies the data.
int32_t vidix_host_next_file(char *path, uint32_t path_size);
void vidix_host_file_data(uint8_t *data, uint32_t length);

// GPIO pad state as configured by the MCU. The host resolves the level seen on
// a pin from this and the board's circuitry (buttons, pull-ups, LED).
#define VIDIX_GPIO_MODE_DISABLED   (0)
#define VIDIX_GPIO_MODE_INPUT      (1)
#define VIDIX_GPIO_MODE_OUTPUT     (2)
#define VIDIX_GPIO_MODE_OPEN_DRAIN (3)
// DAC output (GPIO25/26); level is the 8-bit DAC code.
#define VIDIX_GPIO_MODE_DAC        (4)

#define VIDIX_GPIO_PULL_NONE (0)
#define VIDIX_GPIO_PULL_UP   (1)
#define VIDIX_GPIO_PULL_DOWN (2)

void vidix_host_gpio_config(double now_ns, uint32_t pin, uint32_t mode, uint32_t pull, uint32_t level);
// Logic level (0 or 1) on the pin at now_ns.
uint32_t vidix_host_gpio_read(double now_ns, uint32_t pin);

// Calibrated ADC reading of the pin at now_ns in millivolts, as returned by
// ESP-IDF's adc_cali_raw_to_voltage() at 11 dB attenuation.
int32_t vidix_host_adc_read_mv(double now_ns, uint32_t pin);

// The ILI9341 panel. display identifies the busdisplay driving it.
// The init sequence uses busdisplay's format (command, length | 0x80 for a
// delay, data, [delay]).
void vidix_host_display_init(double now_ns, uint32_t display, const uint8_t *init_sequence, uint32_t length);
// RGB565 pixels (big endian, row-major) for the inclusive column/row address
// window, as written to the panel's RAM.
void vidix_host_display_write(double now_ns, uint32_t display, uint32_t x1, uint32_t y1,
    uint32_t x2, uint32_t y2, const uint8_t *pixels, uint32_t length);
// A refresh pass of the display has finished.
void vidix_host_display_refreshed(double now_ns, uint32_t display);

void vidix_host_event(uint32_t kind);

// Hard reset of the simulated MCU. Does not return.
void vidix_host_reset(void);
