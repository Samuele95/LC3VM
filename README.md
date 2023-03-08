<a href="https://github.com/Samuele95/LC3VM"><img align="right" src="https://camo.githubusercontent.com/38ef81f8aca64bb9a64448d0d70f1308ef5341ab/68747470733a2f2f73332e616d617a6f6e6177732e636f6d2f6769746875622f726962626f6e732f666f726b6d655f72696768745f6461726b626c75655f3132313632312e706e67" alt="Fork me on GitHub" data-canonical-src="https://s3.amazonaws.com/github/ribbons/forkme_right_darkblue_121621.png"></a>

# LC3VM
Implementation of a basic LC3 Virtual Machine, with a basic Operating System, based on https://www.jmeiners.com/lc3-vm/.
The program is divided into two main components: a OS-like process and a program-execution environment.

## Before starting
This program has been implemented through Linux system calls and POSIX synchronization API. Therefore, it actually can 
run in an environment where those functionalities are available. Additionally, it makes use of the Debian default call
"x-terminal-emulator" to launch a shell for the program-execution environment. The shell environment is set through 
the macro SH_COMMAND in the vm/os.h file. If you wish to use another kind of terminal emulator, just change the value 
associated to the macro.

LC-3 programs must have been already compiled into object files.

## Install - build
CMake is used as a build system. The CMakelist.txt file has been given to compile the program.

## Brief description
The main process of the program is a basic OS, which gives you an user interface with all the necessary information to
use the program efficiently. At the start, some basic system details are given, as the location of the virtualized 
"system volume" from which LC3 programs (object files) can be picked up for execution.

From the main menu, you can press "1" to choose a program to launch among the ones stored in the system volume. The system
volume is a folder created by the program itself in the current execution path of the program, and it is labeled as "utils".
From the host-OS, you can put LC3 object files straight inside this folder, allowing the Virtual Machine to index them 
and the user to pick them up for execution. Programs may be chosen just typing the index associated to the object file, which
is shown on screen. Once the program has been chosen, a separate shell is launched which runs in background. The shell shows
the program execution, but in the meanwhile the main process (the OS) continue it's execution and the user can interact with it.

The system is implemented through a virtual memory hardware, which is shared among all the programs which are launched.
Just one process at the time is granted the access to the system memory, therefore if more programs are launched a waiting queue
is instantiated. This mechanism makes use of the POSIX semaphore api, to grant correct access to the critical section.

Typing "2" from the main menu, the OS shows the user all the relevant information about the system, like for example the number
of processes launched from the system boot as the location of the system volume or the total amount of hard-drive space used by
the virtual machine. The amount of virtual memory granted to the program is about 8.0 GB.

Typing "4" from the main menu, the system shuts down, clearing the terminal and freeing all resources.


## About the LC-3 ISA 
The choice of the LC-3 ISA Architecture came from the vast availability of docs 
and resources in the open source community, together with the rather simplicity
of the overall architecture. Both properties made the choice ideal for the author's
purpose, which was all about getting a deeper understanding of how an actual 
virtual machine is actually built on the host system, in all its flavours from a
basic interpreter as the Java Virtual Machine to the most complexes one as a 
QEMU/KVM instance.

As an example, here is a sample description of an ADD operation in the LC-3 ISA
Architecture, in both Register mode and Immediate mode:

    Register mode (Mode bit 0):
    
    15          Dest    Src1   Mode       Src2  0
    |-------------------------------------------|
    | 0 0 0 1 | D D D | A A A | 0 | 0 0 | B B B |
    |-------------------------------------------|
   
    D D D = 3-bit Destination Register
    A A A = 3-bit Source 1 Register
    B B B = 3-bit Source 2 Register
    
    Immediate mode (Mode bit 1):
    
    15          Dest    Src1  Mode  Immediate   0
    |-------------------------------------------|
    | 0 0 0 1 | D D D | A A A | 1 | I I I I I   |
    |-------------------------------------------|
    
    D D D = 3-bit Destination Register
    A A A = 3-bit Source 1 Register
    I I I I I = 5-bit Immediate Value Two's Complement Integer
    
    NOTE: The immediate value must be sign extended


In the project repository, you can find a .pdf document describing the whole 
LC-3 ISA Architecture. It is stored inside the "resources" folder.

## INSTRUCTIONS VIRTUALIZATION AND IMPLEMENTATION:
Every instruction has been encoded through an "uint16_t", since it is 
guaranteed to be a 16-bit long unsigned integer value, regardless of the host
machine in which the VM is being run. Similarly, the instruction fields are
parsed through bit shifting operators, and storing the result of the shifting
operation in a "uint8_t" field, which is the smaller data type for which
we can be sure it is going to represent a byte-long unsigned integer value.
The host machine takes care to zero-extend the 3 bit-parsed REGISTER, the
4-bit parsed OPCODE or the 5-bit parsed "immediate value two's complement 
integer" value into the bigger uint8_t type.  

## MEMORY VIRTUALIZATION AND IMPLEMENTATION:
The LC-3 System memory hardware has been emulated through an user defined
struct. This struct is composed of an array of uint16_t type, whose length 
is defined through the macro CPU_REG_NUM and is corresponding to the actual 
number of registers in a real LC-3 System memory hardware. The registry number
address can be seen as an offset in this array. The RAM memory is also emulated 
through an array of uint16_t type, whose length is computed through the macro 
MEM_16BIT, set to the maximum size of the LC-3 System memory hardware, corresponding 
to 65536 bits or 8,0 GB.

CPU registers have been defined through an enumeration. This enumeration is 
defined with constants whose numerical values are equal to the binary addresses 
of the registers in a real LC-3 System. Those enum values can be used as offset 
in an array of "registers" type, in order to simulate a registry memory access 
from an instruction.


## TODO:
The following functionalities still need to be implemented or fixed, hopefully
in a future update:

- Sending user input to the LC-3 Program launched in the separate shell;
- Reliability upgrade of the memory locking mechanism, which actually can't
  support an high amount of processes waiting in the POSIX semaphore queue;
- SMP architecture implementation, as an alternative system boot option, in
  order to allow parallel execution of the LC-3 Programs. 
- Socket-based data exchange between VM instances.
