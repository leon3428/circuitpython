// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "extmod/vfs_fat.h"
#include "lib/oofatfs/ff.h"
#include "supervisor/filesystem.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/reload.h"

#include "host.h"
#include "supervisor/vidix_files.h"

void vidix_files_pending(void) {
    if (autoreload_is_enabled()) {
        autoreload_trigger();
    }
}

// Create every parent directory of path. Existing ones are fine.
static void make_parents(FATFS *fatfs, char *path) {
    for (char *p = path + 1; *p != '\0'; p++) {
        if (*p == '/') {
            *p = '\0';
            f_mkdir(fatfs, path);
            *p = '/';
        }
    }
}

void vidix_files_apply(void) {
    fs_user_mount_t *vfs = filesystem_circuitpy();
    if (vfs == NULL) {
        return;
    }
    FATFS *fatfs = &vfs->fatfs;
    char path[256];
    int32_t length;
    while ((length = vidix_host_next_file(path, sizeof(path))) >= 0) {
        uint8_t *data = port_malloc(length > 0 ? length : 1, false);
        vidix_host_file_data(data, length);
        make_parents(fatfs, path);
        FIL fp;
        if (f_open(fatfs, &fp, path, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
            UINT written;
            f_write(&fp, data, length, &written);
            f_close(&fp);
        }
        port_free(data);
    }
    filesystem_flush();
}
