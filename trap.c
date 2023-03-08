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

#include "trap.h"

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
    while (*c) {
        int char1 = (*c) & 0xFF;
        putc(char1, stdout);
        int char2 = (*c) >> 8;
        if (char2) {
            putc(char2, stdout);
            ++c;
        }
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}

int trap_halt(uint8_t* endFlag) {
    fputs("\nHALT\n", stdout);
    fflush(stdout);
    *endFlag = 0;
    return EXIT_SUCCESS;
}

