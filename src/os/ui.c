/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 Samuele Stronati, Università degli Studi di Camerino.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Contributors:
 *     Samuele Stronati - initial API and implementation
 *******************************************************************************/

#include "ui.h"
#include <stdio.h>
#include <unistd.h>


void mainmenu() {
    fprintf(stdout,"LC-3 Operating System loaded\n");
    fprintf(stdout, "\n\n\n"
                    " __________________________________________________________\n"
                    "|         _     ____     _____  __     ____  __            |\n"
                    "|        | |   / ___|   |___ /  \\ \\   / /  \\/  |           |\n"
                    "|        | |  | |   _____ |_ \\   \\ \\ / /| |\\/| |           |\n"
                    "|        | |__| |__|_____|__) |   \\ V / | |  | |           |\n"
                    "|        |_____\\____|   |____/     \\_/  |_|  |_|           |\n"
                    "|                                                          |\n"
                    "|__________________________________________________________|\t\n\n\n"
                    "Welcome to the LC-3 Virtual Machine!\n\n"
                    "This process emulates an LC-3 ISA Architecture, and it can \n"
                    "be used to effectively launch any program written using the \n"
                    "LC-3 Assembly language.\n");
}


void about() {
    fprintf(stdout, "\n\n\n"
                    "/*******************************************************************************\n"
                    " * MIT License\n"
                    " * \n"
                    " * Copyright (c) 2023 Samuele Stronati, Università degli Studi di Camerino.\n"
                    " *\n"
                    " * Permission is hereby granted, free of charge, to any person obtaining a copy\n"
                    " * of this software and associated documentation files (the \"Software\"), to deal\n"
                    " * in the Software without restriction, including without limitation the rights\n"
                    " * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
                    " * copies of the Software, and to permit persons to whom the Software is\n"
                    " * furnished to do so, subject to the following conditions:\n"
                    " *\n"
                    " * The above copyright notice and this permission notice shall be included in all\n"
                    " * copies or substantial portions of the Software.\n"
                    " *\n"
                    " * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
                    " * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
                    " * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
                    " * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
                    " * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
                    " * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
                    " * SOFTWARE.\n"
                    " *\n"
                    " * Contributors:\n"
                    " *     Samuele Stronati - initial API and implementation\n"
                    " *******************************************************************************/\n"
                    "\n"
                    "\n"
                    "This projects aims to reproduce a virtual LC-3 computer.\n"
                    "\n"
                    "\n"
                    "The choice of the LC-3 ISA Architecture came from the vast availability of docs \n"
                    "and resources in the open source community, together with the rather simplicity\n"
                    "of the overall architecture. Both properties made the choice ideal for the author's\n"
                    "purpose, which was all about getting a deeper understanding of how an actual \n"
                    "virtual machine is actually built on the host system, in all its flavours from a\n"
                    "basic interpreter as the Java Virtual Machine to the most complexes one as a \n"
                    "QEMU/KVM instance.\n"
                    "\n"
                    "As an example, here is a sample description of an ADD operation in the LC-3 ISA\n"
                    "Architecture, in both Register mode and Immediate mode:\n"
                    "\n"
                    "    Register mode (Mode bit 0):\n"
                    "    \n"
                    "    15          Dest    Src1   Mode       Src2  0\n"
                    "    |-------------------------------------------|\n"
                    "    | 0 0 0 1 | D D D | A A A | 0 | 0 0 | B B B |\n"
                    "    |-------------------------------------------|\n"
                    "   \n"
                    "    D D D = 3-bit Destination Register\n"
                    "    A A A = 3-bit Source 1 Register\n"
                    "    B B B = 3-bit Source 2 Register\n"
                    "    \n"
                    "    Immediate mode (Mode bit 1):\n"
                    "    \n"
                    "    15          Dest    Src1  Mode  Immediate   0\n"
                    "    |-------------------------------------------|\n"
                    "    | 0 0 0 1 | D D D | A A A | 1 | I I I I I   |\n"
                    "    |-------------------------------------------|\n"
                    "    \n"
                    "    D D D = 3-bit Destination Register\n"
                    "    A A A = 3-bit Source 1 Register\n"
                    "    I I I I I = 5-bit Immediate Value Two's Complement Integer\n"
                    "    \n"
                    "    NOTE: The immediate value must be sign extended\n"
                    "\n"
                    "\n"
                    "In the project repository, you can find a .pdf document describing the whole \n"
                    "LC-3 ISA Architecture. It is stored inside the \"resources\" folder.\n"
                    "\n"
                    "INSTRUCTIONS VIRTUALIZATION AND IMPLEMENTATION:\n"
                    "Every instruction has been encoded through an \"uint16_t\", since it is \n"
                    "guaranteed to be a 16-bit long unsigned integer value, regardless of the host\n"
                    "machine in which the VM is being run. Similarly, the instruction fields are\n"
                    "parsed through bit shifting operators, and storing the result of the shifting\n"
                    "operation in a \"uint8_t\" field, which is the smaller data type for which\n"
                    "we can be sure it is going to represent a byte-long unsigned integer value.\n"
                    "The host machine takes care to zero-extend the 3 bit-parsed REGISTER, the\n"
                    "4-bit parsed OPCODE or the 5-bit parsed \"immediate value two's complement \n"
                    "integer\" value into the bigger uint8_t type.  \n"
                    "\n"
                    "MEMORY VIRTUALIZATION AND IMPLEMENTATION:\n"
                    "The LC-3 System memory hardware has been emulated through an user defined\n"
                    "struct. This struct is composed of an array of uint16_t type, whose length \n"
                    "is defined through the macro CPU_REG_NUM and is corresponding to the actual \n"
                    "number of registers in a real LC-3 System memory hardware. The registry number\n"
                    "address can be seen as an offset in this array. The RAM memory is also emulated \n"
                    "through an array of uint16_t type, whose length is computed through the macro \n"
                    "MEM_16BIT, set to the maximum size of the LC-3 System memory hardware, corresponding \n"
                    "to 65536 bits or 8,0 GB.\n"
                    "\n"
                    "CPU registers have been defined through an enumeration. This enumeration is \n"
                    "defined with constants whose numerical values are equal to the binary addresses \n"
                    "of the registers in a real LC-3 System. Those enum values can be used as offset \n"
                    "in an array of \"registers\" type, in order to simulate a registry memory access \n"
                    "from an instruction.\n"
                    "\n"
                    "\n"
                    "TODO:\n"
                    "The following functionalities still need to be implemented or fixed, hopefully\n"
                    "in a future update:\n"
                    "\n"
                    "- Sending user input to the LC-3 Program launched in the separate shell;\n"
                    "- Reliability upgrade of the memory locking mechanism, which actually can't\n"
                    "  support an high amount of processes waiting in the POSIX semaphore queue;\n"
                    "- SMP architecture implementation, as an alternative system boot option, in\n"
                    "  order to allow parallel execution of the LC-3 Programs. \n"
                    "- Socket-based data exchange between VM instances.\n\n\n");
}

int clear() {
    if (execl("/bin/sh", "sh", "-c", "clear", (char *) NULL) < 0) {
        perror("Error in cleaning terminal!\n");
        return -1;
    }
    return 0;
}
