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
#include "memory.h"


mem_t* lc3_memory_init(mem_t* lc3_memory) {
    if (lc3_memory == NULL) {
        perror("The given memory set can't be initialized because it is NULL!");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"Initializing system memory...\nInitializing CPU registers...\nInitializing RAM...\n");
    lc3_memory->reg[R_COND] = FL_ZRO;
    lc3_memory->reg[R_PC] = PC_START;
    fprintf(stdout,
            "Memory initialization complete!\nInitialized %d CPU registers\nInitialized %d bits of RAM...\n",
            CPU_REG_NUM,
            MEM_16BIT);
    return lc3_memory;
}

int memory_zeroed(mem_t* memory_set) {
    if (memory_set == NULL) {
        perror("The given memory set can't be zeroed because it is NULL!");
        return EXIT_FAILURE;
    }
    memset(memory_set, 0, sizeof(*memory_set));
    return EXIT_SUCCESS;
}

int update_flags(mem_t* memory, uint16_t r) {
    if (memory == NULL) {
        perror("Cannot update flags on a NULL memory reference!");
        return EXIT_FAILURE;
    }
    if (memory->reg[r] == 0)
        memory->reg[R_COND] = FL_ZRO;
    else if (memory->reg[r] >> 15)
        memory->reg[R_COND] = FL_NEG;
    else
        memory->reg[R_COND] = FL_POS;
    return EXIT_SUCCESS;
}

int mem_write(uint16_t address, uint16_t val, mem_t* memory) {
    if (memory == NULL) {
        perror("Cannot write on a NULL memory reference!");
        return EXIT_FAILURE;
    }
    memory->memory[address] = val;
    return EXIT_SUCCESS;
}

uint16_t mem_read(uint16_t address, mem_t* memory) {
    if (memory == NULL) {
        perror("Cannot read from on a NULL memory reference!");
        exit(EXIT_FAILURE);
    }
    return memory->memory[address];
}


int loadfile(FILE* file, mem_t* memory) {
    if (memory == NULL) {
        perror("Can't load a file in an unitialized memory!");
        return EXIT_FAILURE;
    }
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = bswap_16(origin);

    uint16_t* p = memory->memory + origin;
    size_t read = fread(p, sizeof(uint16_t), MEM_16BIT - origin, file);

    while(read-- > 0) {
        *p = bswap_16(*p);
        ++p;
    }
    return EXIT_SUCCESS;
}



int loadfile_from_path(const char* image_path, mem_t* memory) {
    if (image_path == NULL) {
        perror("The given file path is not valid");
        return EXIT_FAILURE;
    }
    FILE* file;
    if ((file = fopen(image_path, "rb")) == NULL) {
        perror("Error in opening the file!");
        return EXIT_FAILURE;
    }
    loadfile(file, memory);
    return fclose(file);
}

void print_memory_status(FILE* stream, mem_t* memory) {
    if ((stream == NULL) || (memory == NULL)) {
        perror("Can't print the memory status without a valid memory reference or an output stream!");
        exit(EXIT_FAILURE);
    }
    fprintf(stream, "LC-3 CPU REGISTRY CONTENT:\n");
    for(int i = 0; i < (CPU_REG_NUM-1); i++)
        fprintf(stream, "R%d = %d\n", i, memory->reg[i]);
    fprintf(stream, "R_PC = %d\n", memory->reg[R_PC]);
}