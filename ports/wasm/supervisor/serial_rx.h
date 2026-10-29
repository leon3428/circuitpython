// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

// Pull received bytes from the host into the console RX buffer. Acts like a
// UART RX interrupt: the interrupt character schedules KeyboardInterrupt.
void serial_rx_poll_host(void);
