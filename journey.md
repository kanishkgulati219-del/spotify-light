# Light Project — File-by-File Guide

This document explains each file in the **led_blink** STM32F407VETx project. The firmware does bare-metal clock setup, GPIO (debug LED + UART + PWM pins), UART1, RGB LED PWM via TIM3, and a timer-driven lighting fade layer.

---

## Application source (`Src/`)

### `main.c`
**Role:** Program entry and system bring-up.

- **Clock setup (no HAL):** Enables HSI (16 MHz), configures PLL (PLLM=16, PLLN=336, PLLP=4 → 84 MHz SYSCLK), sets AHB/APB1/APB2 prescalers, and configures Flash wait states. SYSCLK source is switched to PLL. PLL output follows **F<sub>OUT</sub> = (F<sub>IN</sub> / M) × N / P** (input divided by M, multiplied by N, divided by P).
- **Initialization order:** `gpio_init()` → `uart1_init()` → `pwm_init()` → `lighting_init()`.
- **Demo sequence:** Sets an RGB color (999,999,999), updates lighting, does a short delay, then starts a fade to red over ~10 s. Main loop toggles the debug LED with a delay (and can optionally send UART).
- **Notes in code:** USART1 is on APB2 for higher baud; prescalers are power-of-2 (silicon behavior).

### `gpio.c` / `Inc/gpio.h`
**Role:** GPIO configuration and debug LED control.

- **gpio.h:** Declares `gpio_init()`, `gpio_debug_led_on()`, `gpio_debug_led_off()`, `gpio_debug_led_toggle()`.
- **gpio.c:** Uses direct register access (no HAL).
  - Enables GPIOA and GPIOC on AHB1.
  - **Debug LED:** PA6 as push-pull output, default off; toggle via ODR.
  - **UART1:** PA9 (TX), PA10 (RX) as alternate function AF7, high speed, PA10 pull-up.
  - **TIM3 PWM:** PC6 (CH1), PC7 (CH2), PC8 (CH3) as AF2, push-pull, high speed.

### `uart.c` / `Inc/uart.h`
**Role:** USART1 for serial output.

-**Disclaimer:** Only transmitter is enabled in this code.

- **uart.h:** Declares `uart1_init()`, `uart1_writeChar()`, `uart1_writeString()`, `uart1_writeHex()`.
- **uart.c:** Enables USART1 on APB2, sets baud (mantissa 45, fraction 9 for 16× oversampling), 8 data bits, 1 stop bit, transmitter on. `uart1_writeChar()` polls TXE and writes to the data register (and toggles the debug LED as a visible debug side effect).

### `pwm.c` / `Inc/pwm.h`
**Role:** TIM3 PWM for RGB LED (three channels).

- **pwm.h:** Defines channel constants `red`(1), `green`(2), `blue`(3). Declares `pwm_init()`, `pwm_set_duty()`, `pwm_get_duty()`, `pwm_set_frequency()`, `pwm_update()`, `pwm_enable()`, `pwm_disable()`.
- **pwm.c:** Enables TIM3 on APB1. Configures PSC/ARR for ~1 kHz PWM (e.g. PSC=83, ARR=999), edge-aligned up-counting, ARR preload. CH1–CH3 in PWM mode 1 with preload; outputs enabled, active high. Duty is set via CCR1/CCR2/CCR3; `pwm_update()` triggers a register update event.

### `led_pwm.c` / `Inc/led_pwm.h`
**Role:** LED driver layer on top of PWM (color + global brightness).

- **led_pwm.h:** Declares `led_set_color(r,g,b)`, `led_set_brightness(level)`, `led_update()`.
- **led_pwm.c:** Keeps requested R/G/B (0–999) and a global brightness (0–1000). `led_update()` scales R/G/B by brightness, applies inversion (999 − value) for common-anode style LEDs, and calls `pwm_set_duty()` + `pwm_update()`.

### `lighting.c` / `Inc/lighting.h`
**Role:** High-level lighting API with optional time-based fade.

- **lighting.h:** Declares `lighting_init()`, `lighting_set_color()`, `lighting_set_brightness()`, `lighting_update()`, `lighting_fade_to()`, `lighting_is_fading()`, `lighting_stop_fade()`.
- **lighting.c:** Holds current and target R/G/B and step counters. Uses a 10 ms period timer (TIM4) and a callback (`lighting_fade_update`) to step current color toward target each tick. `lighting_init()` registers that callback and sets up TIM4. `lighting_fade_to(r,g,b,time_ms)` computes per-step deltas and starts the timer; when steps are done, it stops the timer and sets final color.

### `timer.c` / `Inc/timer.h`
**Role:** TIM4 as a periodic interrupt source for the lighting fade.

- **timer.h:** Declares `tim4_init()`, `tim4_start()`, `tim4_stop()`, `set_timer_callback()`.
- **timer.c:** Enables TIM4 on APB1, sets PSC/ARR for 10 ms period (e.g. PSC=83, ARR=9999), enables update interrupt and NVIC. On each TIM4 update interrupt it clears the flag and calls the registered callback (here: `lighting_fade_update`). Start/stop control the counter; `set_timer_callback()` stores the function pointer. Uses `stm32f407xx.h` for `NVIC_EnableIRQ(TIM4_IRQn)`.

### `syscalls.c`
**Role:** Minimal C library “system call” stubs for newlib (no OS).

Provides weak/empty implementations so that standard C (e.g. `printf`, `malloc`-related, file I/O) does not pull in undefined symbols. Implements `_getpid`, `_kill`, `_exit`, `_read`/`_write` (optionally wired to `__io_getchar`/`__io_putchar` for UART), `_close`, `_fstat`, `_isatty`, `_lseek`, `_open`, `_wait`, `_unlink`, `_times`, `_stat`, `_link`, `_fork`, `_execve`. Allows the project to link with newlib without an RTOS.

### `sysmem.c`
**Role:** Heap for `malloc` / newlib.

Implements `_sbrk()` so that newlib can grow the heap. Heap starts at the `_end` symbol (from the linker script) and must not grow past `_estack − _Min_Stack_Size`. Returns `(void*)-1` and sets `errno = ENOMEM` when the request would exceed that limit.

---

## Startup and linker scripts

### `Startup/startup_stm32f407vetx.s`
**Role:** Reset and interrupt vector table for Cortex-M4 (GCC/ARM).

- Sets initial stack pointer from `_estack`, then jumps to `Reset_Handler`.
- `Reset_Handler` sets SP, calls `SystemInit`, copies `.data` from flash to RAM, zeroes `.bss`, then calls `main()` (via C library startup).
- Defines the vector table (exceptions + interrupts) with weak aliases to `Default_Handler`; only the implemented handlers (e.g. `TIM4_IRQHandler`) are overridden. Required for the MCU to boot and for interrupts to work.

### `STM32F407VETX_FLASH.ld`
**Role:** Main linker script for running from flash.

- Defines memory regions: FLASH (0x08000000, 512 KB), RAM (0x20000000, 128 KB), CCMRAM (0x10000000, 64 KB).
- Sets `_estack`, `_Min_Heap_Size`, `_Min_Stack_Size`.
- Places `.isr_vector` and `.text` in FLASH, `.data`/`.bss` in RAM, stack at top of RAM, and optional CCMRAM usage. Produces the final memory layout used by `_sbrk()` and the startup code.

### `STM32F407VETX_RAM.ld`
**Role:** Linker script variant for running from RAM (e.g. debugging).

- Same MCU and similar sections, but with execution address in RAM instead of flash. Used when you want to load and run the program from RAM (e.g. faster download during development).

---

## Project and IDE configuration

### `.project`
**Role:** Eclipse/STM32CubeIDE project descriptor.

- Project name: `led_blink`.
- Build: CDT managed build with genmake and ScannerConfigBuilder.
- Natures: STM32 MCU project, C nature, managed makefile, single CPU, etc. Tells the IDE this is an STM32 C project and how to build it.

### `.cproject`
**Role:** CDT C/C++ build configuration.

- Defines **Debug** and **Release** configurations with MCU ARM GCC toolchain, MCU `STM32F407VETx`, FPU/float ABI, and include paths (e.g. `../Inc`, CMSIS Core/Device).
- Compiler: defines `DEBUG` (Debug only), `STM32`, `STM32F4`, `STM32F407VETx`; include paths point to `Inc` and STM32Cube CMSIS.
- Linker: uses `STM32F407VETX_FLASH.ld`.
- Source folders: `Src`, `Startup`, `Inc` (Inc often for includes only). This is what STM32CubeIDE uses to build the `.elf`.

### `.settings/`
**Role:** Eclipse workspace preferences for this project.

- **language.settings.xml:** Language/parser settings for the editor.
- **org.eclipse.core.resources.prefs:** Resource/encoding options.
- **com.st.stm32cube.ide.mcu.sfrview.prefs:** SFR view options for the MCU.

### `CMakePresets.json`
**Role:** CMake presets for alternative (e.g. VSCode/CLI) builds.

- Defines presets (e.g. default, debug, release) that use a Ninja generator and a GCC ARM toolchain file (`cmake/gcc-arm-none-eabi.cmake`). Used when building with CMake instead of Eclipse’s managed build.

### `.vscode/`
**Role:** VSCode/Cursor integration for building and debugging the same MCU.

- **c_cpp_properties.json:** C/C++ IntelliSense configuration; uses `ms-vscode.cmake-tools` as the configuration provider so include paths and defines come from CMake.
- **launch.json:** Cortex-Debug configs for STM32: “Build & Debug” and “Attach” via ST-Link (SWD), device `STM32F407VETx`, GDB/server paths from STM32 Cube CLT config, run to `main`, SVD for peripheral view.
- **tasks.json:** Tasks for flashing with STM32_Programmer_CLI (SWD, reset, start), CMake clean rebuild, and listing interfaces. “Build + Flash” depends on CMake clean rebuild then flash.
- **extensions.json:** Recommends extensions: C/C++, CMake, Arm assembly, linker script, hex editor, map files, Cortex-Debug and related debug/RTOS views.

---

## Build output (typical)

- **Debug/** (or **Release/**): Contains `led_blink.elf`, `led_blink.list`, `led_blink.map`, `makefile`, `objects.list`, `objects.mk`, `sources.mk`, and object/dependency files (e.g. `main.o`, `gpio.o`, …) under `Debug/Src/` and `Debug/Startup/`. Generated by the IDE or by Make/CMake; not version-controlled as source.

---

## Optional / placeholder

### `Inc/board_config.h`
**Role:** Intended for board-specific configuration (e.g. pin or feature macros). Currently empty; can be included where board options are needed.

### `led_blink Debug.launch`
**Role:** Eclipse launch configuration for “Debug” (build + flash + debug). Duplicates or complements what is in `.vscode/launch.json` for the Eclipse workflow.

---

## Data flow summary

1. **main.c** configures system clock, then initializes GPIO, UART, PWM (TIM3), and lighting (TIM4 + callback).
2. **lighting** layer sets RGB and can run a timed fade; each 10 ms tick it calls **led_pwm** to update R/G/B and brightness.
3. **led_pwm** converts color + brightness to duty cycles and calls **pwm** to set TIM3 CCRs and trigger an update.
4. **pwm** drives TIM3 channels on PC6/PC7/PC8; **gpio** has already set those pins to TIM3 AF.
5. **timer** (TIM4) fires every 10 ms and invokes the lighting fade callback.
6. **uart** is available for debug output; **gpio** also drives the debug LED (e.g. toggled from main or UART write).

Together, these files form a bare-metal STM32F407 application with RGB PWM LED control, optional fade, debug LED, and UART, with both STM32CubeIDE and VSCode/Cortex-Debug support.
