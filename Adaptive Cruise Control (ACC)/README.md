# Adaptive cruise control

This project demonstrates a timer-driven task scheduling system using FreeRTOS. The application implements periodic serial communication over two independent channels (COM0 and COM1) by using software timers and semaphores to trigger tasks at different time intervals.

The system is used for adaptive cruise control management and the transmission of related information.

The implementation illustrates how FreeRTOS timers can be used to control task execution timing, while semaphores ensure synchronization between timer callbacks and communication tasks.

The implementation was developed in a simulated embedded environment using modules for serial communication, a seven-segment display, and LED control.

## Main functionalities

- Automatic and manual operating modes
- Control of window raising/lowering level
- Monitoring of current, average, minimum, and maximum speed
- Speed limit detection
- LED bar used as input and output interface
- 7-segment display for status indication
- Serial communication (COM0 & COM1)

### Tasks

| Task | Functionality |
|-----|------|
| `Prijem_podataka_sa_senzora` | Receiving data from sensor |
| `Slanje_podataka_kanal1` | Send data to COM1 |
| `Prijem_podataka_kanal1` | Receiving data from COM1 |
| `LED_Bar_Read1` | Reads input LED bar |
| `LED_Bar_Write` | Writes output LED bar |
| `Display_Task` | 7-seg display control |
| `Automatski_Rezim` | Data processing |

## Instructions for testing

To run the program, it is necessary to have Visual Studio installed and to download the entire GitHub repository.

All required peripheral devices must be configured. The necessary peripheral software can be found in the Peripherals folder.

For each peripheral, the following steps are required:

Open Command Prompt.
Create a directory.
Set the path to the folder containing the corresponding peripheral software.
Then enter the software name along with the appropriate arguments:
- LED_bars : bY (Other color combinations can also be used, e.g. rG. It is important to respect uppercase and lowercase letters.)
- Seg7_Mux 10
- AdvUniCom 0
- AdvUniCom 1

Finally, start the program by clicking the Local Windows Debugger button in Visual Studio, and make sure that the build configuration is set to x86 on the left side of the button.

## PROGRAM
On COM0, insert the sensor value and click Enter to send and simulate the sensor input:
- The allowed range is from 0 to 999 cm.
- The program calculates the average of the last 10 values, and by activating the 1st input LED on the LED bar, we can choose whether to display the minimum or maximum sensor value.
- Each message must end with Enter.

On COM1, the following information is displayed:
- Whether cruise control is on or off
- Whether the average distance is greater than the threshold

On COM1, we can also send two commands:
- TEMPOMAT_xxx — xxx represents the speed that can be set, or OFF can be entered to disable cruise control
- PRAG_xxx — used to set a new threshold value (the default threshold is 150)

Two LED bars are used: one for input and one for output:
Input bar:
- Turning the 1st LED on/off selects whether the minimum or maximum sensor distance is displayed, as mentioned earlier.
- The 2nd LED turns cruise control on/off.
- The last 3 LEDs simulate the three pedals in a car. Activating any of them turns cruise control off.

Output bar:
- Blinks every second if the average sensor distance is smaller than the threshold.

If cruise control is enabled, the speed will automatically increase and attempt to reach the cruise control speed set with the TEMPOMAT_xxx command. If cruise control is disabled, the speed will decrease.

Ten seven-segment displays are used to show information:
- The first 3 digits display the cruise control speed (if cruise control is off, 999 is displayed).
- The 4th digit is always 0.
- The next 3 digits display the current speed.
- The last 3 digits display the minimum or maximum sensor value.

<p align="center">
  <img src="Simulacija senzora.gif" width="600"/>
</p>

<p align="center"><b>Sensor simulation (COM0)</b></p>

---

<p align="center">
  <img src="Tempomat komanda.gif" width="600"/>
</p>

<p align="center"><b>Cruise control commands</b></p>

---

<p align="center">
  <img src="LED komande.gif" width="600"/>
</p>

<p align="center"><b>LED example</b></p>
