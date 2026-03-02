# Interrupt-Driven Sense-Think-Act System (4⭐ Distinction)

## Overview

This project implements a complete real-time interrupt-driven embedded system using an Arduino Uno. The system demonstrates grouped Pin Change Interrupts (PCI), an External Interrupt, and a Timer interrupt to achieve concurrent event handling and deterministic periodic scheduling.

The design follows a structured Sense–Think–Act architecture and satisfies all 4⭐ requirements for Module 1 (Real-Time Systems).

---

## Learning Objectives Demonstrated

- Implementation of interrupt-driven programming
- Configuration of Pin Change Interrupts (PCI)
- Use of External Interrupt (INT0)
- Timer1 configuration in CTC mode
- Separation of event-based and time-based logic
- Safe interrupt handling using volatile variables
- Minimal ISR design to reduce interrupt latency

---

## Hardware Components

- Arduino Uno
- PIR Motion Sensor
- Push Button (with 10kΩ pull-down resistor)
- HC-SR04 Ultrasonic Sensor
- 4 LEDs (including heartbeat LED)
- 220Ω resistors (for LEDs)
- Breadboard & jumper wires

---

## Pin Configuration

| Component              | Arduino Pin | Interrupt Type              |
|------------------------|------------|-----------------------------|
| PIR Sensor             | D8         | Pin Change Interrupt (PCI) |
| Push Button            | D9         | Pin Change Interrupt (PCI) |
| Ultrasonic TRIG        | D3         | Digital Output             |
| Ultrasonic ECHO        | D2         | External Interrupt (INT0)  |
| LED – PIR              | D5         | Output                     |
| LED – Button           | D6         | Output                     |
| LED – Distance         | D10        | Output                     |
| Heartbeat LED          | D13        | Timer-based                |

---

## System Architecture

### 1️⃣ Pin Change Interrupt (PCINT0_vect)

- Monitors D8 and D9
- Handles PIR motion detection
- Handles push button press/release
- Both inputs are part of Port B (D8–D13 group)

### 2️⃣ External Interrupt (INT0)

- Connected to ultrasonic ECHO pin (D2)
- Configured in CHANGE mode
- Measures pulse duration using `micros()`
- Calculates distance

### 3️⃣ Timer1 Interrupt

- Configured in CTC mode
- 1024 prescaler
- OCR1A = 15624 (1 second interval)
- Toggles heartbeat LED
- Triggers ultrasonic measurement
- Logs uptime in Serial Monitor

---

## Design Principles

- ISRs are minimal and flag-based only
- No delay() inside any ISR
- All processing occurs in the main loop
- Shared variables declared `volatile`
- Clear separation of:
  - Event-driven logic (PCI + External Interrupt)
  - Time-driven logic (Timer1)

This ensures low interrupt latency and reliable concurrent execution.

---

## Sense–Think–Act Flow

**Sense:**
- PIR detects motion
- Button detects user input
- Ultrasonic measures object distance

**Think:**
- Evaluate motion state
- Evaluate button state
- Check if distance < 20cm
- Track system uptime

**Act:**
- Turn LEDs ON/OFF accordingly
- Toggle heartbeat LED
- Display detailed interrupt events in Serial Monitor

---

## Serial Monitor Output Example

```
=== Interrupt Driven System Started ===
[TIMER] 1 Second Tick → Uptime: 1
[PCI] PIR Motion Detected → LED_PIR ON
[PCI] Button Pressed → LED_BUTTON ON
[EXT INT] Distance Measured: 14.2 cm
[ULTRA] Object Close → LED_DISTANCE ON
```

---

## Testing Scenarios

1. Trigger PIR motion sensor  
   → PCI event detected  
   → Motion LED activated  

2. Press and release push button  
   → PCI event detected  
   → Button LED toggled  

3. Place object within 20cm of ultrasonic sensor  
   → External interrupt measures distance  
   → Distance LED activated  

4. Observe heartbeat LED blinking every second  
   → Timer interrupt verified  

---

## Repository Structure

```
SIT315_SEM4/
│
├── TaskM1.cpp
├── README.md
├── Reflection_Report.pdf
├── Circuit_Diagram.png
└── Serial_Output.png
```

---

## How to Run

1. Open Arduino IDE
2. Connect Arduino Uno via USB
3. Upload `TaskM1.cpp`
4. Open Serial Monitor (9600 baud)
5. Test motion, button, and distance events

---

## Key Technical Concepts Applied

- PCICR and PCMSK0 register configuration
- ISR(PCINT0_vect) grouped interrupt handling
- attachInterrupt() for INT0
- Timer1 register configuration (TCCR1A, TCCR1B, OCR1A, TIMSK1)
- Volatile shared variables
- Interrupt latency minimisation

---

## Conclusion

This project demonstrates a complete interrupt-driven real-time embedded system using multiple interrupt mechanisms with safe concurrency handling. The implementation satisfies all 4⭐ requirements by:

- Monitoring multiple pins within the same PCI group
- Integrating Timer1 for periodic scheduling
- Demonstrating concurrent interrupt handling
- Maintaining interrupt-safe programming discipline
