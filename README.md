# STM32F407VG Bare-Metal Round-Robin Task Scheduler

A custom preemptive round-robin task scheduler built from scratch on the **STM32F407VG** (ARM Cortex-M4) — no RTOS library, pure bare-metal C.

---

## What It Does

Runs 4 user tasks + 1 idle task concurrently, each blinking an onboard LED at a different frequency, using cooperative blocking delays and preemptive context switching.

| Task   | LED    | Blink Period |
|--------|--------|-------------|
| Task 1 | Green  | 1000 ms     |
| Task 2 | Orange | 500 ms      |
| Task 3 | Blue   | 250 ms      |
| Task 4 | Red    | 125 ms      |

---

## How It Works

### Stack Architecture
- **MSP (Main Stack Pointer)** — used by the scheduler/exception handlers
- **PSP (Process Stack Pointer)** — each task gets its own dedicated stack

```
RAM Layout:
┌─────────────────┐ 0x20020000  ← Top of RAM
│  Scheduler Stack│ (1KB)
├─────────────────┤
│  Task 1 Stack   │ (1KB)
├─────────────────┤
│  Task 2 Stack   │ (1KB)
├─────────────────┤
│  Task 3 Stack   │ (1KB)
├─────────────────┤
│  Task 4 Stack   │ (1KB)
├─────────────────┤
│  Idle Stack     │ (1KB)
└─────────────────┘
```

### Context Switch Flow
```
SysTick (every 1ms)
    │
    ├─ update_global_tick_count()
    ├─ unblock_tasks()         ← unblocks tasks whose delay expired
    └─ Pend PendSV
            │
        PendSV_Handler (naked)
            ├─ SAVE:    MRS R0, PSP → STMDB R0!, {R4-R11} → save_psp_value()
            ├─ SELECT:  update_next_task()  ← round-robin with idle fallback
            └─ RESTORE: get_psp_value() → LDMIA R0!, {R4-R11} → MSR PSP, R0
```

### Task States
```
READY ──── task_delay() ──→ BLOCKED
  ↑                              │
  └──── tick expires ────────────┘
```

### Task Control Block (TCB)
```c
typedef struct {
    uint32_t psp_value;      // saved stack pointer
    uint32_t block_count;    // tick count to unblock at
    uint8_t  current_state;  // READY or BLOCKED
    void (*task_handlers)(void); // function pointer to task
} TCB_t;
```

---

## Key Concepts Demonstrated

- **ARM Cortex-M4 exception model** — SysTick + PendSV combination
- **Dual-stack operation** — MSP for kernel, PSP per task
- **Naked functions** — hand-written assembly for PendSV and stack init
- **Dummy stack frames** — pre-loading xPSR, PC, LR so first context restore works
- **EXC_RETURN = 0xFFFFFFFD** — return to Thread mode using PSP
- **CONTROL register** — switching SP from MSP to PSP at startup
- **Interrupt-safe task_delay** — disables interrupts during state change

---

## Hardware

- **Board:** STM32F407VG Discovery (STM32F407VGTx)
- **Clock:** 16 MHz HSI (SysTick configured for 1ms tick)
- **IDE:** STM32CubeIDE
- **Debugger:** ST-Link V2 (onboard)

---

## Project Structure

```
├── Src/
│   ├── main.c          # Scheduler, tasks, SysTick, PendSV
│   ├── led.c           # LED GPIO init and control
│   └── syscalls.c      # printf retargeting (UART/SWO)
├── Inc/
│   ├── main.h          # Stack addresses, macros, defines
│   └── led.h           # LED pin definitions
├── Startup/
│   └── startup_stm32f407vgtx.s  # Vector table, Reset_Handler
├── STM32F407VGTX_FLASH.ld       # Linker script (Flash)
└── STM32F407VGTX_RAM.ld         # Linker script (RAM)
```

---

## Build & Flash

1. Clone the repo
   ```bash
   git clone https://github.com/JAYAVARDAN145/stm32f407-task-scheduler.git
   ```
2. Open in **STM32CubeIDE** → `File > Open Projects from File System`
3. Build: `Project > Build All` (Ctrl+B)
4. Flash: `Run > Debug` with board connected via USB

---

## Key Macros (main.h)

```c
#define MAX_TASKS           5
#define TICK_HZ             1000          // 1ms SysTick
#define SYSTICK_TIM_CLK     16000000      // 16MHz HSI

#define SCHED_STACK_START   0x20020000
#define T1_STACK_START      0x2001F000
#define T2_STACK_START      0x2001E000
#define T3_STACK_START      0x2001D000
#define T4_STACK_START      0x2001C000
#define IDLE_STACK_START    0x2001B000

#define TASK_READY_STATE    0x00
#define TASK_BLOCKED_STATE  0xFF
#define DUMMY_XPSR          0x01000000
```

---

## Author

**Jayavardan** — Embedded Systems Enthusiast  
[GitHub](https://github.com/JAYAVARDAN145) • [LinkedIn]([https://www.linkedin.com/in/YOUR-LINKEDIN-HERE](https://www.linkedin.com/in/jolapuram-jayavardan-120331218/))

---

## License

MIT License — free to use, modify, and distribute.
