# Adaptive Cruise Control (FreeRTOS)

Real-time adaptive cruise control simulation built on **FreeRTOS**. Periodic tasks are triggered by **software timers and semaphores**, and the system communicates over two independent serial channels (COM0 and COM1) while using an LED bar and a seven-segment display as the user interface. The project was developed in a simulated embedded environment (Visual Studio, x86), with simulated serial, LED bar and seven-segment peripherals.

<p align="center">
  <img src="Tempomat komanda.gif" width="600" alt="Cruise control commands demo"/>
</p>

---

## Features

- Automatic and manual operating modes
- Monitoring of current speed
- Monitoring of minimum and maximum measured distance
- Distance threshold warning (default threshold: 150 cm, changeable at runtime)
- LED bar used as both input and output interface
- Seven-segment display for status information
- Serial communication over two channels (COM0 for the sensor, COM1 for commands and status)

---

## System Architecture

FreeRTOS **software timers** release the periodic tasks at different time intervals, and **semaphores** synchronize the timer callbacks with the tasks that do the work. This keeps the timing logic separate from the communication and processing logic.

### Tasks

| Task | Functionality |
|---|---|
| `Prijem_podataka_sa_senzora` | Receives sensor data (COM0) |
| `Slanje_podataka_kanal1` | Sends status data to COM1 |
| `Prijem_podataka_kanal1` | Receives commands from COM1 |
| `LED_Bar_Read1` | Reads the input LED bar |
| `LED_Bar_Write` | Writes the output LED bar |
| `Display_Task` | Controls the seven-segment display |
| `Automatski_Rezim` | Automatic mode: processes data and controls speed |

---

## How It Works

### Sensor input (COM0)

The sensor is simulated by typing a distance value on COM0, followed by Enter.

- Allowed range: 0 to 999 cm
- The program calculates the **average of the last 10 values**
- The first input LED selects whether the **minimum** or **maximum** sensor value is shown

### Status and commands (COM1)

COM1 reports whether cruise control is on or off and whether the average distance is greater than the threshold. Two commands can be sent:

| Command | Effect |
|---|---|
| `TEMPOMAT_xxx` | Sets the cruise control speed to `xxx`, or `TEMPOMAT_OFF` to disable cruise control |
| `PRAG_xxx` | Sets a new distance threshold (default: 150) |

If cruise control is enabled, the speed increases automatically and tries to reach the set cruise speed. If it is disabled, the speed decreases.

### LED bars

| Bar | LED | Function |
|---|---|---|
| Input | 1st | Selects minimum or maximum sensor distance for the display |
| Input | 2nd | Turns cruise control on or off |
| Input | Last 3 | Simulate the three car pedals; activating any of them turns cruise control off |
| Output | | Blinks every second when the average distance is below the threshold |

### Seven-segment display

Ten digits show the system state:

| Digits | Content |
|---|---|
| 1–3 | Cruise control speed (999 when cruise control is off) |
| 4 | Always 0 |
| 5–7 | Current speed |
| 8–10 | Minimum or maximum sensor value |

---

## Demo

<p align="center">
  <img src="Simulacija senzora.gif" width="600" alt="Sensor simulation on COM0"/>
</p>

<p align="center"><b>Sensor simulation (COM0)</b></p>

<p align="center">
  <img src="Tempomat komanda.gif" width="600" alt="Cruise control commands on COM1"/>
</p>

<p align="center"><b>Cruise control commands (COM1)</b></p>

<p align="center">
  <img src="LED komande.gif" width="600" alt="LED bar example"/>
</p>

<p align="center"><b>LED bar example</b></p>

---

## Running the Project

**Requirements:** Visual Studio and the whole repository downloaded. The peripheral simulators are in the `Peripherals` folder.

1. Open Command Prompt for each peripheral, go to the folder with the peripheral software and start it with its arguments:
   - `LED_bars bY` (other color combinations such as `rG` also work; uppercase and lowercase letters matter)
   - `Seg7_Mux 10`
   - `AdvUniCom 0`
   - `AdvUniCom 1`
2. In Visual Studio set the build configuration to **x86**.
3. Start the program with **Local Windows Debugger**.

---

## Skills Demonstrated

- Real-time task design with FreeRTOS (tasks, software timers, semaphores)
- Periodic task scheduling and synchronization
- Serial communication protocol handling (command parsing)
- State logic with multiple inputs (modes, thresholds, safety inputs)
- Working with simulated peripherals (UART, LED bar, seven-segment display)