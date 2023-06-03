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

#ifndef CONDFLAGS_H
#define CONDFLAGS_H

/**
 * Condition flags stored in the R_COND register, which provide information
 * about the most recently executed calculation. This allows programs to check
 * logical conditions. Load instructions (LD, LDI, LDR, and LEA) and operate
 * instructions (ADD, AND, and NOT) each load a result into one of the eight
 * general purpose registers. The condition codes are set, based on whether
 * that result, taken as a 16-bit 2’s complement integer, is negative
 * (N = 1; Z, P = 0), zero (Z = 1; N, P = 0), or positive (P = 1; N, Z = 0).
 * All other LC-3 instructions leave the condition codes unchanged
 */

typedef enum condition_flags {
    FL_POS = 1 << 0,    /* Posv */
    FL_ZRO = 1 << 1,    /* Zero */
    FL_NEG = 1 << 2,    /* Negv */
} condition_flags_t;



#endif //CONDFLAGS_H
