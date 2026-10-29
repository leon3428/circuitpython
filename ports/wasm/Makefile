# This file is part of the CircuitPython project: https://circuitpython.org
#
# SPDX-License-Identifier: MIT

# CircuitPython compiled to WebAssembly for the VIDI X simulator.
# Requires Emscripten (emcc on PATH). Build with: make BOARD=vidi_x

include ../../py/circuitpy_mkenv.mk

CROSS_COMPILE =
CC = emcc
CPP = emcc -E
AR = emar
SIZE = true

INC += \
	-I. \
	-I../.. \
	-I../lib/mp-readline \
	-I../shared/timeutils \
	-Iboards/$(BOARD) \
	-Iboards/ \
	-I$(BUILD)

# GCC-only flags from circuitpy_defns.mk that clang does not accept.
BASE_CFLAGS := $(filter-out -fsingle-precision-constant -Wimplicit-fallthrough=2 --param max-inline-insns-single=500,$(BASE_CFLAGS))

# Python semantics must match the board, so keep -O2 rather than -Os for speed.
COPT ?= -O2

# GC roots: --spill-pointers moves live pointers from wasm locals onto the
# shadow stack, which the GC scans conservatively like a real C stack. The pass
# finds the stack pointer global by name, so names are kept (--profiling-funcs).

# NLR uses setjmp/longjmp, implemented with Wasm exception handling.
WASM_EH = -sSUPPORT_LONGJMP=wasm

DISABLE_WARNINGS = -Wno-cast-align -Wno-unknown-warning-option -Wno-gnu-label-as-value
CFLAGS += $(INC) -Wall -std=gnu11 -fshort-enums $(BASE_CFLAGS) $(CFLAGS_MOD) $(COPT) $(DISABLE_WARNINGS) -Werror=missing-prototypes $(WASM_EH)

SRC_C += \
	boards/$(BOARD)/board.c \
	boards/$(BOARD)/pins.c \
	background.c \
	mphalport.c \
	vclock.c \
	display.c \
	supervisor/vidix_files.c \

SRC_COMMON_HAL_EXPANDED = $(addprefix shared-bindings/, $(SRC_COMMON_HAL)) \
                          $(addprefix shared-bindings/, $(SRC_BINDINGS_ENUMS)) \
                          $(addprefix common-hal/, $(SRC_COMMON_HAL))

SRC_SHARED_MODULE_EXPANDED = $(addprefix shared-bindings/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE)) \
                             $(addprefix shared-module/, $(SRC_SHARED_MODULE_INTERNAL))

# A few modules have files both in common-hal/ and shared-module/; $(sort)
# removes the duplicates.
SRC_COMMON_HAL_SHARED_MODULE_EXPANDED = $(sort $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED))

OBJ = $(PY_O) $(SUPERVISOR_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_COMMON_HAL_SHARED_MODULE_EXPANDED:.c=.o))
ifeq ($(INTERNAL_LIBM),1)
OBJ += $(addprefix $(BUILD)/, $(SRC_LIBM:.c=.o))
endif
OBJ += $(addprefix $(BUILD)/, $(SRC_CIRCUITPY_COMMON:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MOD:.c=.o))

SRC_QSTR += $(SRC_C) $(SRC_SUPERVISOR) $(SRC_COMMON_HAL_EXPANDED) $(SRC_SHARED_MODULE_EXPANDED) $(SRC_CIRCUITPY_COMMON)

LDFLAGS_WASM = \
	$(COPT) $(WASM_EH) \
	-sMODULARIZE -sEXPORT_ES6 -sEXPORT_NAME=createVidixModule \
	-sENVIRONMENT=node,web,worker \
	-sINVOKE_RUN=0 -sEXIT_RUNTIME=0 \
	-sSTACK_SIZE=1MB -sINITIAL_MEMORY=32MB -sALLOW_MEMORY_GROWTH=0 \
	-sBINARYEN_EXTRA_PASSES=--spill-pointers \
	--profiling-funcs \
	-sEXPORTED_FUNCTIONS=_main,_vidix_flash_ptr,_vidix_flash_size \
	-sEXPORTED_RUNTIME_METHODS=HEAPU8 \
	--js-library js/host_library.js

all: $(BUILD)/firmware.mjs

ifeq ($(VALID_BOARD),)
$(BUILD)/firmware.mjs: invalid-board
else
$(BUILD)/firmware.mjs: $(OBJ) js/host_library.js
	$(STEPECHO) "LINK $@"
	$(Q)echo $(OBJ) > $(BUILD)/firmware.objs
	$(Q)$(CC) -o $@ $(LDFLAGS_WASM) $(LDFLAGS_EXTRA) @$(BUILD)/firmware.objs
endif

include $(TOP)/py/mkrules.mk
