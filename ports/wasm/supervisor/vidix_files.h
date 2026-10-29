// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#pragma once

// The host has new files for CIRCUITPY. Behaves like saving over USB: triggers
// an autoreload when it is enabled.
void vidix_files_pending(void);

// Write all pending host files into CIRCUITPY. Only call while no VM runs.
void vidix_files_apply(void);
