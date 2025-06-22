# processor_simulator
programming project in Computer Organization course in TAU.
assembler and RISC processor (defined similarly to MIPS, named SIMP) simulator implementation in C in linux environment.

the processor and assembler function separately.
the assembler recieves assembley .asm files as input, and outputs memin.txt the memory of the system with the program loaded from memory 0 onward.

the SIMP processor simulator recieves an input of memin.txt simulating the memory of the system with a program loaded in it.
the processor simulator should execute the program from memory via defined array of registers, and instructions defined by the ISA of the SIMP processor (in the PDF).

the program also simulates hardware interfaces such as:
interrupts via periodic timer, hard disk, button press (simulated via premade input file).
clock
leds
7 segments display
reading and writing to hard disk
writing to monitor

hardware interfaces are simulated via an array of hardware registers controlling inputs, outputs and status
the simulator also parallely outputs each hardware change log to a .txt log file for debugging (hwregtrace.txt, cycles.txt, leds.txt, etc.)

the code is written in visual studio environment, for Linux OS.
ISA in Hebrew and project desctiption is added bellow as PDFs
