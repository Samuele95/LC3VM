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

#include "opcodes.h"
#include <stdlib.h>
#include <stdio.h>

/*******************************************************************************
 ******************** PRIVATE UTILITY FUNCTION DECLARATIONS ********************
 *******************************************************************************/

/**
 * Executes a logical/arithmetic instruction (ADD, AND, NOT), by parsing the
 * given 2-bytes length instruction to execute the operation matching the opcode
 * of the instruction itself. It continues parsing the instruction finding the
 * required operands, saving the first operand in register R0 and the second
 * operand in register R1.
 *
 * Operand parsing is executed through a subroutine which checks whether the
 * content of the "operand fields"(bit fields [0-4] and [6-8] in the instruction must be
 * interpreted as actual operands' values or memory addresses to the location in
 * which the actual value is stored. The choice among those two options is performed
 * through the value of a flag stored inside the instruction itself, at a specific
 * bit position (typically, it is the 5th least significant bit.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 */
static int execute_logical_instruction(uint16_t, mem_t*);

/**
 * Executes a load/store instruction as bt LC-3 ISA specs, by parsing the given 2-bytes
 * length instruction to  execute the operation matching the opcode of the instruction itself.
 * It continues parsing the instruction finding the required operands, saving them in the
 * appropriate registers (R0 and R1), sign-extending the values if required.
 *
 * Operand parsing is executed strictly following the LC-3 ISA specs, through bitmasks and
 * bit-shifting operation which are able to isolate the bit fields in which the operands values
 * (direct values or memory addresses) are stored inside the given instruction.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 */
static int execute_loadstore_instruction(uint16_t instruction, mem_t *memory);

/**
 * Executes a load/store register instruction as bt LC-3 ISA specs, by parsing the given 2-bytes
 * length instruction to  execute the operation matching the opcode of the instruction itself.
 * It continues parsing the instruction finding the required operands, saving them in the
 * appropriate registers (R0 and R1), sign-extending the values if required.
 *
 * Operand parsing is executed strictly following the LC-3 ISA specs, through bitmasks and
 * bit-shifting operation which are able to isolate the bit fields in which the operands values
 * (direct values or memory addresses) are stored inside the given instruction.
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 *
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise;
 */
static int execute_loadstore_register_instruction(uint16_t, mem_t*);

/**
 * Actually executes the given logical instruction, making use of the C language
 * defined logical and arithmetic operations.
 *
 * @param instruction: the 2-bytes long LC-3 ISA AND instruction, parsed through a uint16_t type;
 * @param first_operand: the first operand of the instruction;
 * @param second_operand: the second operand of the instruction;
 *
 * @return the result of the operation as a uint16_t type; it exits the process
 * with an appropriate "exit value" if the opcode is not associated to any logical
 * operation or if the operands given are not in a proper format.
 */
static uint16_t execute_operation(uint16_t instruction, uint16_t first_operand, uint16_t second_operand);

/**
 * Subroutine called to extract an operand for a logical/arithmetic operation from the
 * 2-bytes instruction itself. The flag signals whether the operand to be parsed is an actual
 * integer-valued operand ("immediate operand") which has to be sign-extended or it is the address
 * of the registry in which the actual value is stored ("registry operand")
 *
 * @param uint16_t: the 2-bytes long LC-3 ISA instruction, parsed through a uint16_t type;
 * @param mem_t*: the LC-3 emulated memory, in which the operation takes place;
 * @param uint16_t: flag signalling whether the operand is immediate or not
 *
 * @return the actual value of the operand
 */

static uint16_t compute_operand(uint16_t, mem_t*, uint16_t);

static uint16_t format_value(uint16_t value, int flag, mem_t* memory);

/**
 * Sign-extends the given uint16_t value, shifting for a number of bits equal to
 * the integer argument passed to this function.
 *
 * @param uint16_t the value to be sign_extended;
 * @param int the number of bits to shift in the sign_extending operation
 *
 * @return the sign_extended value passed as uint16_t argument
 */
static uint16_t sign_extend(uint16_t x, int bit_count);

static int trap_getc(mem_t*);

static int trap_out(mem_t*);

static int trap_in(mem_t*);

static int trap_puts(mem_t*);

static int trap_putsp(mem_t*);

static int trap_halt(uint8_t*);


/*******************************************************************************
 *********************** PUBLIC FUNCTION IMPLEMENTATIONS ***********************
 *******************************************************************************/


int execute_instruction(uint16_t instruction, mem_t* lc3_memory, uint8_t* endFlag) {
    switch (opcode(instruction)) {
        case OP_ADD:
            return add(instruction,lc3_memory);
        case OP_AND:
            return and(instruction,lc3_memory);
        case OP_NOT:
            return not(instruction,lc3_memory);
        case OP_BR:
            return br(instruction,lc3_memory);
        case OP_JMP:
            return jump(instruction,lc3_memory);
        case OP_JSR:
            return jsr(instruction,lc3_memory);
        case OP_LD:
            return ld(instruction,lc3_memory);
        case OP_LDI:
            return ld(instruction,lc3_memory);
        case OP_LEA:
            return ld(instruction,lc3_memory);
        case OP_LDR:
            return ldr(instruction,lc3_memory);
        case OP_ST:
            return st(instruction,lc3_memory);
        case OP_STI:
            return st(instruction,lc3_memory);
        case OP_STR:
            return str(instruction,lc3_memory);
        case OP_TRAP:
            return trap(instruction,lc3_memory, endFlag);
        case OP_RES:
        case OP_RTI:
        default:
            perror("Wrong opcode format! Operation aborted.");
            abort();
    }
}

uint16_t opcode(uint16_t instruction) {
    return (instruction >> 12) & 0xF;
}


int add(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_ADD)
        return EXIT_FAILURE;
    return execute_logical_instruction(instruction, memory);
}

int and(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_AND)
        return EXIT_FAILURE;
    return execute_logical_instruction(instruction, memory);
}

int not(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_NOT)
        return EXIT_FAILURE;
    return execute_logical_instruction(instruction, memory);
}

int jump(uint16_t instruction, mem_t* memory) {
    if ((opcode(instruction) != OP_JMP) && (opcode(instruction) != OP_JSR))
        return EXIT_FAILURE;
    uint16_t r1 = (instruction >> 6) & 0x7;
    memory->reg[R_PC] = memory->reg[r1];
    return EXIT_SUCCESS;
}

int jsr(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_JSR)
        return EXIT_FAILURE;
    uint16_t long_flag = (instruction >> 11) & 1;
    memory->reg[R_R7] = memory->reg[R_PC];
    if (long_flag) {
        /* executes JSR operation */
        uint16_t long_pc_offset = sign_extend(instruction & 0x7FF, 11);
        memory->reg[R_PC] += long_pc_offset;
        return EXIT_SUCCESS;
    }
    /* else executes JSRR operation as standard JMP operation */
    return jump(instruction,memory);
}

int br(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_BR)
        return EXIT_FAILURE;
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9);
    uint16_t cond_flag = (instruction >> 9) & 0x7;
    if (cond_flag & memory->reg[R_COND])
        memory->reg[R_PC] += pc_offset;
    return EXIT_SUCCESS;
}


int ld(uint16_t instruction, mem_t* memory) {
    uint16_t op = opcode(instruction);
    if ((op != OP_LD) && (op != OP_LDI) && (op != OP_LEA))
        return EXIT_FAILURE;
    return execute_loadstore_instruction(instruction, memory);
}


int ldr(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_LDR)
        return EXIT_FAILURE;
    return execute_loadstore_register_instruction(instruction,memory);
}


int st(uint16_t instruction, mem_t* memory) {
    if ((opcode(instruction) != OP_ST) && (opcode(instruction) != OP_STI))
        return EXIT_FAILURE;
    return execute_loadstore_instruction(instruction, memory);
}


int str(uint16_t instruction, mem_t* memory) {
    if (opcode(instruction) != OP_STR)
        return EXIT_FAILURE;
    return execute_loadstore_register_instruction(instruction,memory);
}


int trap(uint16_t instruction, mem_t* memory, uint8_t* endFlag) {
    memory->reg[R_R7] = memory->reg[R_PC];
    uint16_t trapcode = instruction & 0xFF;

    if (trapcode == TRAP_GETC) return trap_getc(memory);
    if (trapcode == TRAP_OUT) return trap_out(memory);
    if (trapcode == TRAP_IN) return trap_in(memory);
    if (trapcode == TRAP_PUTS) return trap_puts(memory);
    if (trapcode == TRAP_PUTSP) return trap_putsp(memory);
    if (trapcode == TRAP_HALT) return trap_halt(endFlag);
    return EXIT_FAILURE;
}


/*******************************************************************************
 ********************** PRIVATE FUNCTION IMPLEMENTATIONS ***********************
 *******************************************************************************/


int execute_logical_instruction(uint16_t instruction, mem_t* memory) {
    uint16_t r0 = (instruction >> 9) & 0x7;
    uint16_t r1 = (instruction >> 6) & 0x7;
    uint16_t imm_flag = (instruction >> 5) & 0x1;
    uint16_t operand = compute_operand(instruction, memory, imm_flag);
    memory->reg[r0] = execute_operation(instruction, memory->reg[r1], operand);
    return update_flags(memory, r0);
}


int execute_loadstore_instruction(uint16_t instruction, mem_t* memory) {
    uint16_t op = opcode(instruction);
    uint16_t r0 = (instruction >> 9) & 0x7;
    uint16_t pc_offset = sign_extend(instruction & 0x1FF, 9);
    uint16_t memory_addr = memory->reg[R_PC] + pc_offset;

    if (op == OP_LEA) memory->reg[r0] = memory_addr;
    if (op == OP_LD)  memory->reg[r0] = mem_read(memory_addr,memory);
    if (op == OP_LDI) memory->reg[r0] = mem_read(mem_read(memory_addr,memory),memory);
    if (op == OP_ST)  mem_write(memory_addr, memory->reg[r0], memory);
    if (op == OP_STI) mem_write(mem_read(memory_addr, memory), memory->reg[r0], memory);
    return update_flags(memory,r0);
}

int execute_loadstore_register_instruction(uint16_t instruction, mem_t* memory) {
    uint16_t r0 = (instruction >> 9) & 0x7;
    uint16_t r1 = (instruction >> 6) & 0x7;
    uint16_t offset = sign_extend(instruction & 0x3F, 6);
    if (opcode(instruction) == OP_LDR) {
        memory->reg[r0] = mem_read(memory->reg[r1] + offset, memory);
        return update_flags(memory,r0);
    }
    if (opcode(instruction) == OP_STR)
        return mem_write(memory->reg[r1] + offset, memory->reg[r0], memory);
    return EXIT_FAILURE;
}



uint16_t compute_operand(uint16_t instruction, mem_t* memory, uint16_t flag) {
    if (flag)
        return sign_extend(instruction & 0x1F, 5);
    uint16_t r2 = instruction & 0x7;
    return memory->reg[r2];
}

uint16_t execute_operation(uint16_t instruction, uint16_t first_operand, uint16_t second_operand) {
    uint16_t op = opcode(instruction);
    if (op == OP_ADD)
        return first_operand + second_operand;
    if (op == OP_AND)
        return first_operand & second_operand;
    if (op == OP_NOT && second_operand == 0xFF)
        return first_operand ^ second_operand;
    perror("Not a logical operation!");
    exit(EXIT_FAILURE);
}


uint16_t format_value(uint16_t value, int flag, mem_t* memory) {
    if (flag > 2 || flag < 0) {
        perror("Invalid flag!");
        exit(EXIT_FAILURE);
    }
    if (flag == 0)
        return value;
    format_value(mem_read(value, memory), flag-1, memory);
}

uint16_t sign_extend(uint16_t x, int bit_count) {
    if ((x >> (bit_count-1)) & 1)
        x |= (0xFFFF << bit_count);
    return x;
}

/*******************************************************************************
 ********************** TRAP INSTRUCTIONS IMPLEMENTATION ***********************
 *******************************************************************************/
 
 
 int trap_getc(mem_t* memory) {
    /* read a single ASCII char */
    memory->reg[R_R0] = (uint16_t) getchar();
    update_flags(memory,R_R0);
    return EXIT_SUCCESS;
}

int trap_out(mem_t* memory) {
    putc((char) memory->reg[R_R0], stdout);
    return fflush(stdout);
}

int trap_in(mem_t* memory) {
    fprintf(stdout,"Enter a character: ");
    int c = getc(stdin);
    putc(c, stdout);
    fflush(stdout);
    memory->reg[R_R0] = (uint16_t) c;
    update_flags(memory, R_R0);
    return EXIT_SUCCESS;
}

int trap_puts(mem_t* memory) {
    uint16_t* c = memory->memory + memory->reg[R_R0];
    while (*c) {
        putc((char) *c, stdout);
        ++c;
    }
    return fflush(stdout);
}

int trap_putsp(mem_t* memory) {
    uint16_t* c = memory->memory + memory->reg[R_R0];
    int backtrackchar;	/* last char of previous line */
    int lookaheadchar;
    while (*c) {
        int char1 = (*c) & 0xFF;
        int char2 = (*c) >> 8;
        lookaheadchar = *(c+1) & 0xFF;
        if ((*c == 0x6E5C)) 
            fputs("\n", stdout);   /*0x5C6E is the hexadecimal representation of a raw "\n" string. Execute line feed*/
        else if ((backtrackchar == 0x5C) && (char1 == 0x6E)) {
            fputs("\n", stdout);
            putc(char2, stdout);
        }
        else if (char2 == 0x5C && lookaheadchar == 0x6E) 
            putc(char1,stdout);
        else {
            putc(char1, stdout); 
            putc(char2, stdout);        
        }  
    backtrackchar = char2;
    ++c;
    }
    fflush(stdout);
    return EXIT_SUCCESS;
}

int trap_halt(uint8_t* endFlag) {
    fputs("\n\n\nHALT\n\n\n", stdout);
    fflush(stdout);
    *endFlag = 0;
    return EXIT_SUCCESS;
}

