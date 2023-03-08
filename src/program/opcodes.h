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

#ifndef UNTITLED_OPCODES_H
#define UNTITLED_OPCODES_H
#include "trap.h"

typedef enum opcodes {
    OP_BR ,    /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
} opcodes_t;

/**
 * Executes the given instruction as loaded in the LC-3 System memory.
 * An uint8_t value, signaling the possible end of the program, is given. An
 * HALT instruction or any other instruction which forces to stop program
 * execution must update the flag.
 *
 * In a default implementation, this function is simply an opcode checker of
 * the given instruction, redirecting to the appropriate function call which
 * implements the actual instruction to be executed (ex: the add instruction).
 *
 * Due to the large number of possible instruction, it is advised to implement
 * this function through a comprehensive "switch" statement.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 * @param uint16_t*: pointer the flag signaling the end of the running program;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 */
int execute_instruction(uint16_t, mem_t*, uint8_t*);

/**
 * Parses the OPCODE of the given instruction, extracting it from the 4 most
 * significant bits of the binary representation of the instruction itself.
 * Being the LC-3 ISA a big-endian system, the opcode is placed on the left
 * side of the series of bits.
 *
 * In a standard implementation, the result of this function can be computed
 * simply through a C language "bit shift" operation, as follows:
 *
 * (instruction >> 12) & 0xF;
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA instruction, parsed through a uint16_t type;
 *
 * @return the opcode of the instruction, extended to a uint16_t type
 */
uint16_t opcode(uint16_t instruction);


/**
 *   Register mode (Mode bit 1):
 *   15          Dest    Src1   Mode       Src2  0
 *   |-------------------------------------------|
 *   | 0 0 0 1 | D D D | A A A | 0 | 0 0 | B B B |
 *   |-------------------------------------------|
 *
 *   Immediate mode (Mode bit 1):
 *   15          Dest    Src1  Mode  Immediate   0
 *   |-------------------------------------------|
 *   | 0 0 0 1 | D D D | A A A | 1 | I I I I I   |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA ADD operation, updating the VM memory according to the operation.
 * If Mode bit (bit[5]) is 0, the second source operand is obtained from SR2, otherwise
 * the second source operand is obtained by sign-extending the imm5 field to 16 bits.
 * In both cases, the second source operand is added to the contents of SR1 and the
 * result stored in DR. The condition codes are set, based on whether the result is
 * negative, zero, or positive.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA ADD instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int add(uint16_t, mem_t*);

/**
 *   Register mode (Mode bit 0):
 *   15          Dest    Src1   Mode       Src2  0
 *   |-------------------------------------------|
 *   | 0 1 0 1 | D D D | A A A | 0 | 0 0 | B B B |
 *   |-------------------------------------------|
 *
 *   Immediate mode (Mode bit 1):
 *   15          Dest    Src1  Mode  Immediate   0
 *   |-------------------------------------------|
 *   | 0 1 0 1 | D D D | A A A | 1 | I I I I I   |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA AND operation, updating the VM memory according to the operation.
 * If Mode bit (bit[5]) is 0, the second source operand is obtained from SR2, otherwise
 * the second source operand is obtained by sign-extending the imm5 field to 16 bits.
 * In either case, the second source operand and the contents of SR1 are bit-wise ANDed,
 * and the result stored in DR. The condition codes are set, based on whether the binary
 * value produced, taken as a 2’s complement integer, is negative, zero, or positive
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA AND instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int and(uint16_t, mem_t*);

/**
 *   Instruction Format:
 *   15          Dest    Src    Mode             0
 *   |-------------------------------------------|
 *   | 1 0 0 1 | D D D | S S S | 1 | 1 1 1 1 1   |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA NOT operation, updating the VM memory according to the operation.
 * The bit-wise complement of the contents of SR is stored in DR. The condition codes are
 * set, based on whether the binary value produced, taken as a 2’s complement integer, is
 * negative, zero, or positive
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA NOT instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int not(uint16_t, mem_t*);

/**
 *   Instruction Format:
 *   JMP mode:
 *   15                 Register                 0
 *   |-------------------------------------------|
 *   | 1 1 0 0 | 0 0 0 | R R R | 0 0 0 0 0 0     |
 *   |-------------------------------------------|
 *   R R R = 3-bit base register
 *
 *   RET mode:
 *   15                                          0
 *   |-------------------------------------------|
 *   | 1 1 0 0 | 0 0 0 | 1 1 1 | 0 0 0 0 0 0     |
 *   |-------------------------------------------|
 *   NOTE: RET always loads R7
 *
 * Executes an LC-3 ISA JMP operation, updating the VM memory according to the operation.
 * The program unconditionally jumps to the location specified by the contents of the base
 * register. Bits [8:6] identify the base register. The RET instruction is a special case
 * of the JMP instruction. The PC is loaded with the contents of R7, which contains the
 * linkage back to the instruction following the subroutine call instruction.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA JMP instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int jump(uint16_t, mem_t*);

/**
 *   Instruction Format:
 *   JSR mode:
 *   15              PCOffset11                  0
 *   |-------------------------------------------|
 *   | 0 1 0 0 | 1 | P P P | P P P | P P P | P P |
 *   |-------------------------------------------|
 *   P P P P P P P P P P P = PCOffset11
 *
 *   JSRR mode:
 *   15                   Register               0
 *   |-------------------------------------------|
 *   | 0 1 0 0 | 0 | 0 0 | R R R | 0 0 0 0 0 0   |
 *   |-------------------------------------------|
 *   R R R = 3-bit base register
 *
 * Executes an LC-3 ISA JSR operation, updating the VM memory according to the operation.
 * First, the incremented PC is saved in R7. This is the linkage back to the calling routine.
 * Then the PC is loaded with the address of the first instruction of the subroutine, causing
 * an unconditional jump to that address. The address of the subroutine is obtained from the
 * base register (if bit [11] is 0), or the address is computed by sign-extending bits [10:0]
 * and adding this value to the incremented PC (if bit [11] is 1).
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA JSR instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int jsr(uint16_t, mem_t*);

/**
 *  Instruction Format:
    15          Flags   PCOffset9               0
    |-------------------------------------------|
    | 0 0 0 0 | N Z P | P P P P P P P P P       |
    |-------------------------------------------|
    N = Negative Flag (BRN)
    Z = Zero Flag (BRZ)
    P = Positive Flag (BRP)

 * Executes an LC-3 ISA BR operation, updating the VM memory according to the operation.
 * The condition codes specified by the state of Flags bits (bits[11:9] are tested. If bit [11] is
 * set, N is tested; if bit [11] is clear, N is not tested. If bit [10] is set, Z is tested, etc.
 * If any of the condition codes tested is set, the program branches to the location specified
 * by adding the sign-extended PCOffset9 field to the incremented PC.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA BR instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int br(uint16_t, mem_t*);

/**
 *   LD mode:
 *   15          Dest   PCOffset9                0
 *   |-------------------------------------------|
 *   | 0 0 1 0 | D D D | P P P P P P P P P       |
 *   |-------------------------------------------|
 *
 *   LDI mode:
 *   15          Dest   PCOffset9                0
 *   |-------------------------------------------|
 *   | 1 0 1 0 | D D D | P P P P P P P P P       |
 *   |-------------------------------------------|
 *
 *   LEA mode:
 *   15          Dest   PCOffset9                0
 *   |-------------------------------------------|
 *   | 1 1 1 0 | D D D | P P P P P P P P P       |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA LD, LDI or LEA operation, updating the VM memory according to the operation.
 * An address is computed by sign-extending PCOffset9 bits (bits[8:0]) to 16 bits and adding this
 * value to the incremented PC.
 * For a LEA operation, this address itself is loaded into Dest.
 * For a LD operation, the contents of memory at this address are loaded into Dest.
 * For a LDI operation,the contents of memory at this address is the address of the data to be
 * loaded into DR.
 * The condition codes are set, based on whether the value loaded is negative, zero, or positive
 * @param uint16_t: the 2-bytes long LC-3 ISA LD instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int ld(uint16_t, mem_t*);

/**
 *   Instruction Format:
 *   15          Dest   Base     Offset6         0
 *   |-------------------------------------------|
 *   | 0 1 1 0 | D D D | B B B | O O O O O O     |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA LDR operation, updating the VM memory according to the operation.
 * An address is computed by sign-extending bits [5:0] to 16 bits and adding this value to
 * the contents of the register specified by bits [8:6]. The contents of memory at this
 * address are loaded into DR. The condition codes are set, based on whether the value loaded
 * is negative, zero, or positive
 * @param uint16_t: the 2-bytes long LC-3 ISA LDR instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int ldr(uint16_t, mem_t*);

/**
 *   ST mode:
 *   15          Src    PCOffset9                0
 *   |-------------------------------------------|
 *   | 0 0 1 1 | S S S | P P P P P P P P P       |
 *   |-------------------------------------------|
 *
 *   STI mode:
 *   15          Src    PCOffset9                0
 *   |-------------------------------------------|
 *   | 1 0 1 1 | S S S | P P P P P P P P P       |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA ST or STI operation, updating the VM memory according to the operation.
 * For both operations, the contents of the register specified by Src are stored in the memory
 * location whose address is computed by sign-extending PCOffset9 bits (bits[8:0]) to 16 bits
 * and adding this value to the incremented PC.
 * For ST operation, the content of this memory address is directly updated with the contents of
 * Src;
 * For STI operation, what is in memory at this address is the address of the location to which
 * the data in Src is stored, and for this reason the operation is "indirect".
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA ST instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int st(uint16_t, mem_t*);

/**
 *   Instruction Format:
 *   15          Src    Base     Offset6         0
 *   |-------------------------------------------|
 *   | 0 1 1 1 | S S S | B B B | O O O O O O     |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA STR operation, updating the VM memory according to the operation.
 * The contents of the register specified by Src are stored in the memory location whose
 * address is computed by sign-extending Offset6 bits (bits[5:0]) to 16 bits and adding
 * this value to the contents of the register specified by Base bits (bits[8:6]).
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA STR instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int str(uint16_t, mem_t*);

/**
 *   Instruction Format:
 *   15         11        7  Trapvect8           0
 *   |-------------------------------------------|
 *   | 1 1 1 1 | 0 0 0 0 | P P P P P P P P       |
 *   |-------------------------------------------|
 *
 * Executes an LC-3 ISA TRAP instruction operation, as an event signaled from/to the VM.
 * First R7 is loaded with the incremented PC. (This enables a return to the instruction
 * physically following the TRAP instruction in the original program after the service
 * routine has completed execution.) Then the PC is loaded with the starting address of
 * the system call specified by Trapvector8. The starting address is contained in the
 * memory location whose address is obtained by zero-extending trapvector8 to 16 bits.
 *
 * The actual implementation of this function may be just a reference to other more specific
 * TRAP instructions, among which this function could choose the most appropriate one according
 * to the opcode value. An uint8_t flag is given as argument to this function. It can be used to
 * signal the end of the program execution by an appropriate TRAP (in the LC-3 ISA, this TRAP is
 * the HALT operation).
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA ST instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 * @param uint16_t: the flag signaling the end of the running program;
 *
 * @return: EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 **/
int trap(uint16_t, mem_t*, uint8_t*);

#endif //UNTITLED_OPCODES_H
