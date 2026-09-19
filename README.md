# A Mini x86 Operating System

A mini x86 operating system built from scratch in C and assembly. The kernel handles console output, keyboard input, interrupts, a round-robin process scheduler, system calls, and memory paging, all driven by a small custom command shell.

## ✨ Features

- **Console**: text-mode display with color support (`CHAR_COLOR`)
- **Keyboard**: low-level keyboard driver (AZERTY), character-by-character input
- **Interrupts (IRQ)**: generic interrupt handling, timer handler, keyboard handler
- **Timer**: periodic interrupt used for preemption and `sleep`
- **Processes**:
  - process table, creation (`fork`) / termination (`kill`/`exit`)
  - context switching (`ctx_sw.S`)
  - fair round-robin scheduling
  - sleeping (`sleep`) with wake-up driven by the timer
- **System calls** (software interrupt `int 0x80`): `write`, `getpid`, `exit`, `fork`, `sleep`, `kill`, `shutdown`
- **Memory**: kernel-side allocation (`kheap`, `sbrk`, `malloc`) and **paging** (x86 page tables, virtual memory implementation)
- **Mini Shell**: command-line interpreter with history, running as a user process

## 🖥️ The Mini Shell

The shell runs as a process (`miniShell`) on top of the kernel and supports the following commands:

| Command          | Description                                             |
|------------------|-----------------------------------------------------------|
| `help`           | Show this help screen                                     |
| `ps`             | List running processes                                    |
| `fork <name>`    | Create a new process from the available ones (see `ps`)   |
| `kill <pid>`     | Kill the process with the given PID                       |
| `sleep <secs>`   | Put the current process to sleep (in seconds)              |
| `echo <message>` | Print a message                                            |
| `history`        | Show command history                                       |
| `history -c`     | Clear command history                                       |
| `clear`          | Clear the screen                                            |
| `exit` / `shutdown` | Power off the system and quit the mini shell             |

## 📁 Project layout

```
.
├── boot/         # Low-level entry point (crt0.S), linker script
├── kernel/       # Kernel core: console, irq, timer, keyboard, processes,
│                 # scheduling, syscalls, paging, memory, panic...
├── bin/          # User-space processes (mini shell, test processes, idle)
├── lib/          # Custom standard library (printf, malloc, string, ...)
├── include/      # Headers (n7OS/, custom libc)
└── build/        # Shared build configuration
```

## 🚀 Building and running

### Requirements

- **GCC** (x86 toolchain)

  ```bash
  sudo apt-get install build-essential
  ```

- **QEMU** to run the kernel

  ```bash
  sudo apt-get install qemu-system-x86
  ```

- **GDB** for debugging

  ```bash
  sudo apt-get install gdb
  ```

> [!NOTE]
> Commands above are for Debian/Ubuntu.

### Build

```bash
make
```

Produces `kernel.bin`.

### Run

```bash
make run
```

Boots the kernel in QEMU.

### Debug

```bash
make dbg-qemu   # GDB attached to QEMU
```

### Clean

```bash
make clean
```