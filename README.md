# My Projects

Personal and course projects in **PCB design, power electronics and embedded systems**. Focus: hardware design (schematic capture, PCB layout, power integrity, mixed-signal measurement) with embedded firmware to bring the boards to life. Looking for junior and internship hardware design engineer positions.

---

## Hardware Design Projects

| Project | Description | Key technologies |
|---|---|---|
| [Buck-Boost Converter](./Buck%20-%20Boost) | 4-layer buck-boost converter, 3–30 V input, 1–22 V I2C-programmable output, up to 3 A | TPS55289, Altium Designer, I2C, Arduino Nano |
| [OhmSprint Competition Board](./OhmSprint%20competition) | AC voltage and current measurement board with LCD, redesigned from KiCad to Altium Designer to fix routing and power distribution issues | ATM90E26, STM32G431, SPI, USB, KiCad, Altium Designer |
| [Smart Greenhouse Automation](./Applications%20of%20Sensors%20and%20Actuators) | Team project: automated greenhouse with a custom 2-layer PCB, 12 V / 5 V power rails, sensors, motor drivers and RTC scheduling | Arduino Nano, BME280, A4988, L9110, DS1307 |
| [UART–RS232 Board](./Course%20Project%28EKVP%29) | 4-layer RS-232 communication board with 24 V input, analyzed for DC drop and current distribution | ADM3202, STM32F042, Ansys SIwave |
| [CH32V006K8U6 Dev Board](./CH32V006K8U6%20Tutorial) | Compact RISC-V microcontroller board with USB-C on a 2-layer PCB (tutorial-based practice project) | CH32V006K8U6, USB-C |

## Embedded Systems Projects

| Project | Description | Key technologies |
|---|---|---|
| [Applied Electronics](./Applied%20Electronics) | Home security system with PIR, MQ3 and light sensors, buzzer, servo and touch-screen GLCD, built around a state machine | dsPIC30F4013, C, MPLAB X, UART |
| [Adaptive Cruise Control (ACC)](./Adaptive%20Cruise%20Control%20%28ACC%29) | Adaptive cruise control simulation with timer-driven FreeRTOS tasks, serial commands, LED bar and seven-segment interface | FreeRTOS, C, UART |

---

## Skills

**PCB and hardware design**
Schematic capture, PCB layout, multi-layer stackup (4 layers), component placement, power distribution and routing, ground planes and GND via stitching, differential-pair routing, footprint and 3D model creation, design review and redesign of existing boards

**Power electronics**
Buck-boost converter design, I2C-controlled DC-DC conversion, 12 V / 5 V / 3.3 V power rail design

**Simulation and analysis**
DC voltage drop and plane current analysis (Ansys SIwave), circuit simulation (MicroCap)

**Measurement, sensors and actuators**
AC voltage and current measurement, energy metering IC (ATM90E26), analog front-end design, environmental and light sensors, motor, stepper and servo drivers

**Embedded and interfaces**
C, C++, FreeRTOS, I2C, SPI, UART, RS-232, USB, SWD

**Tools**
Altium Designer, KiCad, Ansys SIwave, MicroCap, Arduino IDE, MPLAB X IDE

**Platforms**
Arduino Nano, ESP32, dsPIC30F, STM32 (hardware design), CH32V006