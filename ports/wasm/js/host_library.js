// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

// Emscripten JS library implementing host.h. Every call is forwarded to the
// `vidixHost` object passed to the module factory, so the browser worker and
// the Node CLI can provide their own hosts.

addToLibrary({
    vidix_host_sync: (nowNs) => Module.vidixHost.sync(nowNs),

    vidix_host_idle: (nowNs, wakeNs) => Module.vidixHost.idle(nowNs, wakeNs),

    vidix_host_serial_write: (nowNs, text, length) =>
        Module.vidixHost.serialWrite(nowNs, HEAPU8.slice(text, text + length)),

    vidix_host_serial_read: () => Module.vidixHost.serialRead(),

    vidix_host_next_file__deps: ['$stringToUTF8'],
    vidix_host_next_file: (path, pathSize) => {
        const file = Module.vidixHost.nextFile();
        if (!file) {
            return -1;
        }
        stringToUTF8(file.path, path, pathSize);
        Module.vidixPendingFileData = file.data;
        return file.data.length;
    },

    vidix_host_file_data: (data, length) => {
        HEAPU8.set(Module.vidixPendingFileData.subarray(0, length), data);
        Module.vidixPendingFileData = null;
    },

    vidix_host_gpio_config: (nowNs, pin, mode, pull, level) =>
        Module.vidixHost.gpioConfig(nowNs, pin, mode, pull, level),

    vidix_host_gpio_read: (nowNs, pin) => Module.vidixHost.gpioRead(nowNs, pin),

    vidix_host_adc_read_mv: (nowNs, pin) => Module.vidixHost.adcReadMv(nowNs, pin),

    vidix_host_display_init: (nowNs, display, initSequence, length) =>
        Module.vidixHost.displayInit(nowNs, display, HEAPU8.slice(initSequence, initSequence + length)),

    vidix_host_display_write: (nowNs, display, x1, y1, x2, y2, pixels, length) =>
        Module.vidixHost.displayWrite(nowNs, display, x1, y1, x2, y2, HEAPU8.subarray(pixels, pixels + length)),

    vidix_host_display_refreshed: (nowNs, display) => Module.vidixHost.displayRefreshed(nowNs, display),

    vidix_host_event: (kind) => Module.vidixHost.event(kind),

    vidix_host_reset: () => Module.vidixHost.reset(),
});
