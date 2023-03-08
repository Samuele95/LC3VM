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

#ifndef UNTITLED_OS_H
#define UNTITLED_OS_H
#define SH_COMMAND "x-terminal-emulator -e ./run_program"
#define INPUT_LENGTH 256
#define PROGRAM_NAME_LENGTH 1024
#include <semaphore.h>
#include <ctype.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdatomic.h>
#include <errno.h>
#include "memory.h"
#include "fsystem.h"
#include "run_program.h"
#include "ui.h"

typedef struct system {
    mem_t system_memory;
    directory_t system_directory;
    uint8_t pid_num;
    uint8_t running;
    atomic_int condvar;
} system_t;

system_t* system_boot();

int system_shutdown(system_t* system);

int mainProcedure();

void launch_program(system_t*);

#endif //UNTITLED_OS_H
