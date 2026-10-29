# VIDI X simulator port: CircuitPython compiled to WebAssembly with Emscripten.
# Hardware is provided by a JavaScript host (see js/host_library.js).
#
# The goal is the same Python environment as ports/espressif BOARD=vidi_x:
# a full build, with only the modules whose hardware is not simulated (yet)
# turned off. Compare with:
#   make -C ports/espressif BOARD=vidi_x print-CFLAGS

# Match the espressif port's language-level settings.
LONGINT_IMPL = MPZ
INTERNAL_LIBM = 0
CIRCUITPY_FULL_BUILD = 1

CIRCUITPY_BUILD_EXTENSIONS = mjs

# The filesystem lives in a RAM-backed block device owned by the port.
INTERNAL_FLASH_FILESYSTEM = 1

CIRCUITPY_MESSAGE_COMPRESSION_LEVEL = 1

# Serial goes to the JS host instead of USB/UART.
CIRCUITPY_USB_DEVICE = 0
CIRCUITPY_PORT_SERIAL = 1

# Same as the espressif port.
CIRCUITPY_SYNTHIO_MAX_CHANNELS = 12

# --- Simulated hardware not implemented yet ---------------------------------
# busio: SPI and I2C are simulated. The ESP32 build also has UART, which is
# not simulated yet.
CIRCUITPY_BUSIO_UART = 0
# No parallel LCD on the VIDI X.
CIRCUITPY_PARALLELDISPLAYBUS = 0
# Speaker, neopixels, touch and the remaining peripherals.
CIRCUITPY_ANALOGBUFIO = 0
CIRCUITPY_AUDIOBUSIO = 0
CIRCUITPY_AUDIOIO = 0
CIRCUITPY_COUNTIO = 0
CIRCUITPY_FREQUENCYIO = 0
CIRCUITPY_NEOPIXEL_WRITE = 0
CIRCUITPY_ONEWIREIO = 0
CIRCUITPY_PS2IO = 0
CIRCUITPY_PULSEIO = 0
CIRCUITPY_PWMIO = 0
CIRCUITPY_ROTARYIO = 0
CIRCUITPY_ROTARYIO_SOFTENCODER = 0
CIRCUITPY_TOUCHIO = 0
CIRCUITPY_SDCARDIO = 0
CIRCUITPY_CANIO = 0
# Not available on the ESP32 build either.
CIRCUITPY_I2CTARGET = 0
# Power, clocks and persistence.
CIRCUITPY_ALARM = 0
CIRCUITPY_NVM = 0
CIRCUITPY_RTC = 0
CIRCUITPY_WATCHDOG = 0
CIRCUITPY_SETTABLE_PROCESSOR_FREQUENCY = 0

# --- Not planned: wireless and ESP-IDF specifics -----------------------------
CIRCUITPY_BLEIO = 0
CIRCUITPY_BLEIO_HCI = 0
CIRCUITPY_ESPCAMERA = 0
CIRCUITPY_ESPIDF = 0
CIRCUITPY_ESPNOW = 0
CIRCUITPY_ESPULP = 0
CIRCUITPY_IPADDRESS = 0
CIRCUITPY_MDNS = 0
CIRCUITPY_SOCKETPOOL = 0
CIRCUITPY_SSL = 0
CIRCUITPY_WEB_WORKFLOW = 0
CIRCUITPY_WIFI = 0
CIRCUITPY_DUALBANK = 0
CIRCUITPY_MEMORYMAP = 0
CIRCUITPY_MAX3421E = 0
CIRCUITPY_USB_HOST = 0
CIRCUITPY_PYUSB = 0

# --- Software modules needing library submodules, not fetched yet -----------
CIRCUITPY_HASHLIB = 0
CIRCUITPY_AUDIOMP3 = 0
CIRCUITPY_QRIO = 0

# Derived from the modules above, as on the ESP32 build: gifio, jpegio,
# bitmaptools etc. follow displayio; audiocore, synthio etc. follow audio output.
