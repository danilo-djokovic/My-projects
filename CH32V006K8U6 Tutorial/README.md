# CH32V006K8U6 Development Board

Compact development board for the **WCH CH32V006K8U6**, a 32-bit RISC-V microcontroller. The board has a USB-C connector, low power consumption and a small footprint on a 2-layer PCB. It was built as a **practice project following the [Curious Scientist](https://curiousscientist.tech/) tutorial**, to practice the full flow from schematic capture to PCB layout for a new microcontroller family.

<p align="center">
  <img src="CH32 3D.png" width="500" alt="3D view of the CH32V006K8U6 board">
</p>

---

## Features

- CH32V006K8U6 RISC-V microcontroller
- USB-C connector
- Small board size
- Low power consumption
- Through-hole pads for communication interfaces
- 2-layer PCB

---

## About This Project

The goal of this project was to learn how a complete microcontroller board is put together: reading the datasheet and reference design, drawing the schematic, placing components and routing a compact 2-layer board with a USB-C connector. The design follows the Curious Scientist tutorial, which made it a good starting point before moving on to my own, more complex designs such as the [Buck-Boost converter](../Buck%20-%20Boost) and the [OhmSprint measurement board](../OhmSprint%20competition).

---

## Schematic

<p align="center">
  <img src="CH32 sch.png" width="820" alt="Schematic of the CH32V006K8U6 board">
</p>

---

## PCB

<table>
  <tr>
    <td align="center">
      <img src="CH32 3D.png" width="400"><br>
      <b>3D view</b>
    </td>
    <td align="center">
      <img src="CH32 layers.png" width="400"><br>
      <b>Top and bottom layer</b>
    </td>
  </tr>
</table>

---

## Skills Practiced

| Area | Details |
|---|---|
| Schematic capture | Microcontroller power, USB-C connection, communication breakout |
| PCB layout | Compact component placement and routing on a 2-layer board |
| Microcontroller hardware | Working from a datasheet and reference design for a new MCU family |

---

## Credits

Based on the CH32V006K8U6 board tutorial by the [Curious Scientist](https://curiousscientist.tech/) channel.