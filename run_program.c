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

#include "run_program.h"
#include <semaphore.h>

static int load_program();
static int execute_program(mem_t*);
static int system_reset(mem_t*);
static int get_system_memory();
static char* get_program();
static void acquire_lock();
static int release_lock();
static char path[PATH_MAX];
static int shsid;
static system_t* shsystem;
static sem_t* semaphore;

int main() {
    fprintf(stdout,"Waiting to acquire lock on system memory...\n");
    semaphore = sem_open("SEM", O_CREAT, 0666, 0);
    if (run() != 0) {
        perror("Run program procedure aborted");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int run() {
    if (load_program() != 0) {
        perror("Arguments checking failed!");
        return EXIT_FAILURE;
    }

    if (execute_program(&shsystem->system_memory) != 0) {
        perror("Program execution error! Aborting...");
        return EXIT_FAILURE;
    }

    if (system_reset(&shsystem->system_memory) != 0) {
        perror("System reset failure!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int load_program() {
    if (get_system_memory() != 0) {
        perror("System initialization failed!");
        return EXIT_FAILURE;
    }
    if (get_program() == NULL) {
        perror("Can't get the program path!");
        return EXIT_FAILURE;
    }
    if (loadfile_from_path(path, &shsystem->system_memory) != 0) {
        fprintf(stdout, "failed to load image: %s\n", path);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int execute_program(mem_t* lc3_memory) {
    fprintf(stdout, "Program is starting in 5 seconds...\n");
    sleep(5);
    fprintf(stdout, "Executing LC-3 program...\n");
    uint8_t is_running = 1;
    while (is_running) {
        uint16_t instruction = mem_read(lc3_memory->reg[R_PC]++, lc3_memory);
        if (execute_instruction(instruction, lc3_memory, &is_running) != 0) {
            perror("Error in executing instruction!");
            return EXIT_FAILURE;
        }
    }
    fprintf(stdout, "Execution completed!\n");
    print_memory_status(stdout,lc3_memory);
    sleep(2);
    fflush(stdin);
    fflush(stdout);
    return EXIT_SUCCESS;
}

int system_reset(mem_t* lc3_memory) {
    fprintf(stdout, "Zeroeing memory... Please wait\n");
    if (memory_zeroed(lc3_memory) != 0) {
        perror("Can't reset memory structure! Fatal error");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Operation completed!\n");
    return release_lock();
}


int get_system_memory() {
    if ((shsid = shm_open("/lc3", O_RDWR, 0666)) == 1) {
        perror("Cannot access shared system memory!");
        return EXIT_FAILURE;
    }
    if ((shsystem = (system_t *) mmap(0, sizeof(system_t), PROT_READ | PROT_WRITE, MAP_SHARED, shsid, 0)) ==
        MAP_FAILED) {
        perror("Map failed!\n");
        return EXIT_FAILURE;
    }

    acquire_lock();
    if (memory_zeroed(&shsystem->system_memory) != 0) {
        perror("Error in setting system memory to zero");
        return EXIT_FAILURE;
    }
    print_memory_status(stdout,&shsystem->system_memory);
    return EXIT_SUCCESS;
}

char* get_program() {
    FILE* npipe = fopen("lc3pipe", "r");
    strtok(fgets(path, PATH_MAX, npipe), "\n");
    fclose(npipe);
    unlink("lc3pipe");
    return path;
}

void acquire_lock() {
    while (shsystem->condvar != 0)
        sem_wait(semaphore);
    atomic_fetch_add(&shsystem->condvar,1);
    fprintf(stdout, "Lock acquired!\n");
}

int release_lock() {
    sleep(5);
    atomic_fetch_sub(&shsystem->condvar,1);
    return sem_post(semaphore);
}








