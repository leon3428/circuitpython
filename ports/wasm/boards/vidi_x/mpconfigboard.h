// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Copied from ports/espressif/boards/vidi_x. Deviations are marked.

#pragma once

#define MICROPY_HW_BOARD_NAME       "VIDI X V1.1"
#define MICROPY_HW_MCU_NAME         "ESP32"

#define MICROPY_HW_LED_STATUS (&pin_GPIO2)

#if CIRCUITPY_BUSIO_SPI
#define CIRCUITPY_BOARD_SPI         (1)
#define CIRCUITPY_BOARD_SPI_PIN     {{.clock = &pin_GPIO18, .mosi = &pin_GPIO23, .miso = &pin_GPIO19}}
#endif

#if CIRCUITPY_BUSIO_I2C
#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO32, .sda = &pin_GPIO33}}
#endif

#define CIRCUITPY_BOOT_BUTTON       (&pin_GPIO0)

// Explanation of how a user got into safe mode
#define BOARD_USER_SAFE_MODE_ACTION MP_ERROR_TEXT("You pressed the VOLUME button at start up.")

// Deviation: the real board's console is a UART on GPIO1/GPIO3 through a
// USB-serial chip. Here the console goes to the host directly
// (CIRCUITPY_PORT_SERIAL), so CIRCUITPY_CONSOLE_UART_TX/RX are not defined.
