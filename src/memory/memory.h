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

#ifndef UNTITLED_MEMORY_H
#define UNTITLED_MEMORY_H
#define MEM_16BIT (1 << 16)
#define PC_START 0x3000
#define CPU_REG_NUM 9
#include <stdint.h>
#include <stdio.h>

/**
 * Registers mapped in the LC-3 ISA System. This enumeration is defined with constants
 * whose numerical values are equal to the binary addresses of the registers in a real
 * LC-3 System. Those enum values can be used as offset in an array of "registers" type,
 * in order to simulate a registry memory access from an instruction.
 */

typedef enum registers{
    R_R0,       /* Bin addr: 0000 */
    R_R1,       /* Bin addr: 0001 */
    R_R2,       /* Bin addr: 0010 */
    R_R3,       /* Bin addr: 0011 */
    R_R4,       /* Bin addr: 0100 */
    R_R5,       /* Bin addr: 0101 */
    R_R6,       /* Bin addr: 0110 */
    R_R7,       /* Bin addr: 0111 */
    R_PC,
    R_COND,
    R_COUNT
} registers_t;



/**
 * A memory struct emulating a LC-3 System memory hardware. This struct
 * is composed of an array of uint16_t type, whose length is defined
 * through the macro CPU_REG_NUM and is corresponding to the actual number
 * of registers in a real LC-3 System memory hardware. The registry number
 * address can be seen as an offset in this array. The RAM memory is also
 * emulated through an array of uint16_t type, whose length is computed
 * through the macro MEM_16BIT, set to the maximum size of the LC-3 System
 * memory hardware, corresponding to 65536 bits or 8,0 GB.
 */

typedef struct mem {
    uint16_t reg[R_COUNT];
    uint16_t memory[MEM_16BIT];
} mem_t;


/**
 * Returns an initialized LC-3 Memory struct, by filling the fields of the
 * empty mem_t struct pointed by the given argument.
 * Some implementations may initialize the struct prior to this function call.
 * If that is the case, this function simply initializes the fields which have
 * not been initialized yet.
 * @param mem_t* pointer to a memory struct;
 * @return the initialized memory struct
 */
mem_t* lc3_memory_init(mem_t*);

/**
 * Sets to zero the LC-3 Memory struct pointed to by the given argument.
 * The memory struct must not be null. If it is null, the function immediately
 * returns with value EXIT_FAILURE to avoid possible segmentation faults.
 * @param mem_t*: pointer to a memory struct;
 *
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE if the
 *         argument is NULL
 */
int memory_zeroed(mem_t*);

/**
 * Sets the condition flags defined in condflags.h, as a result to certain
 * instructions. Check the LC-3 ISA specs for more info
 * @param mem_t*: pointer to a memory struct;
 * @param uint16_t: address of a registry whose value must be check and evaluated
 *                  to perform the flags' update.
 *
 */
int update_flags(mem_t*, uint16_t);

/**
 * Writes instruction or data, encoded as an uint16_t type, in the memory address
 * whose offset from the start of the memory array is defined by the uint16_t "val"
 * argument.
 * @param address: memory address/offset in which the instruction or data must be written
 * @param val: the actual value to write into memory. It may be an instruction or some data;
 * @param memory: the LC-3 system memory
 * @return the uint16_t instruction or data written in memory
 */
int mem_write(uint16_t address, uint16_t val, mem_t* memory);

/**
 * Returns the uint16_t content (instruction or data) from the memory location identified
 * through the offset value "address"
 * @param address: the address, as offset from the pointer of the memory array, of the memory location
 *                where the data has to be read from
 * @param memory: the LC-3 system memory.
 * @return the uint16_t instruction or data read from memory
 */
uint16_t mem_read(uint16_t address, mem_t* memory);

/**
 * Loads the given FILE stream into memory. The FILE must be an .obj file representing
 * a LC-3 program. The FILE stream is read line by line, and every instruction is saved
 * in a corresponding address in the memory struct.
 *
 * A standard implementation of the LC-3 ISA architecture should read the first line of the
 * program, in which the "origin" field contains the memory address value from which the
 * program must be stored. In a little-endian host architecture, every instruction in the
 * program must be swapped to a big-endian structure, which is the one supported by the actual
 * LC-3 computer
 *
 * @param file: the file to be read;
 * @param memory: the LC-3 memory;
 */
int loadfile(FILE* file, mem_t* memory);

/**
 * Opens a FILE stream from the given path in the host machine, and loads the stream into memory
 * as explained for the loadfile method
 * @param image_path: LC-3 program file path in the host machine;
 * @param memory: LC-3 system memory;
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE if an error occurred into
 *         opening the stream from the path, in loading the stream into memory or if the file
 *         path was null
 */
int loadfile_from_path(const char* image_path, mem_t* memory);

/**
 * Prints the memory status on the given FILE stream. The process exits with
 * status EXIT_FAILURE if an error occurred while accessing the FILE stream or
 * the memory structure.
 *
 * The status is comprehensive of the number of CPU registers and the total
 * RAM size;
 *
 * @param FILE*: pointer to the output file stream;
 * @param memory: the LC-3 memory;
 */
void print_memory_status(FILE*, mem_t*);

#endif //UNTITLED_MEMORY_H
