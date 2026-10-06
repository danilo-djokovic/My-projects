# OhmSprint – Hardware Project

## Competition Task

The objective of the competition was to design a measurement system in which the input consisted of a **7 V AC signal and the current flowing through the measured load**.

The measured signals were required to be acquired and processed using the **ADM90E26** measurement IC. The remaining parts of the circuit, including the power supply, communication interfaces, microcontroller and supporting circuitry, were left to the competitor's choice.

In addition to designing the hardware, the competition required the measured data to be **processed and presented in a suitable way**. The implementation and presentation of the measured data were left to the designer's discretion.

### Original Design

The following images show the schematic provided as part of the original design.

<p align="center">
  <img src="Pictures/KiCaad_measuring_unit.png" width="48%">
  <img src="Pictures/KiCaad_mcu.png" width="48%">
</p>

<p align="center">
  <img src="Pictures/KiCaad_power_supply.png" width="48%">
</p>

### Original PCB

The following images show the original PCB design.

<p align="center">
  <img src="KiCaad_both_layers.png" width="48%">
  <img src="KiCaad_3d.png" width="48%">
</p>

---

## Altium Designer Redesign

The original design was redesigned in **Altium Designer** primarily to address issues identified in the original PCB, particularly **routing problems and errors in the power distribution**.

While redesigning the board, a similar overall component placement was intentionally maintained in order to remain consistent with the original design and preserve its general architecture. The focus was therefore placed on improving the implementation rather than completely changing the original component arrangement.

### Main Improvements

- Recreated and corrected the schematic in Altium Designer
- Redesigned and corrected component footprints
- Created and corrected 3D models for the components
- Improved overall PCB routing
- Improved power routing and power distribution
- Improved the use of GND planes to provide more appropriate current return paths
- Removed unnecessary test points
- Improved GND via stitching
- Improved differential-pair routing
- Corrected and optimized differential trace routing

---

## Redesigned Schematic

The complete schematic was recreated and corrected in **Altium Designer**.

<p align="center">
  <img src="Pictures/Measurment_unit.png" width="48%">
  <img src="Pictures/MCU_and_connections.png" width="48%">
</p>

<p align="center">
  <img src="Pictures/Power_supply.png" width="48%">
</p>

---

## PCB Layout

The PCB layout was redesigned with particular attention to power distribution, current return paths, differential routing and overall routing quality.

<p align="center">
  <img src="Pictures/Top-layer.png" width="48%">
  <img src="Pictures/Bottom_layer.png" width="48%">
</p>

<p align="center">
  <img src="Pictures/Both_layers.png" width="48%">
  <img src="Pictures/3D_PCB.png" width="48%">
</p>

## Tools & Technologies

The project involved the use of multiple hardware design tools and technologies throughout the design process.

### Design Tools

- **Altium Designer** – schematic capture, PCB design, component placement, routing, design-rule configuration and 3D PCB verification
- **KiCad** – schematic/PCB reference and supporting PCB design work

### Hardware Design

- Schematic analysis and redesign
- Component selection and evaluation
- Component footprint creation and modification
- PCB component placement
- Power distribution and power routing
- Ground-plane design
- Ground via stitching
- Differential-pair routing
- PCB trace-width and clearance considerations
- PCB layer and stack-up considerations

### Communication Interfaces

- **SPI** communication between the measurement circuitry and microcontroller
- **USB** communication for data transfer and interfacing
- Digital interface and signal routing considerations

### Measurement & Data Acquisition

- AC voltage measurement
- Current measurement
- Signal acquisition using the **ADM90E26**
- Digital processing and transfer of measured data
- Processing and presentation of measurement results

---

## Project Status

**Completed**

The competition project has been completed, including both the **original design analysis** and the **redesigned schematic and PCB implementation**.