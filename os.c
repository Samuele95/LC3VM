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


#include "os.h"
#include "sysinfo.h"


static system_t* system_init(system_t*);
static directory_t* system_volume_init(system_t*);
static mem_t* system_memory_init(system_t*);
static long check_user_input(char* input);
static void system_fork(system_t* systemt, char* program);
static int shsid;
static int status;
static system_t* lc3_system;

int main() {

    sem_t* semaphore;
    semaphore = sem_open("SEM", O_CREAT, 0666, 0);
    lc3_system = system_boot();
    atomic_init(&lc3_system->condvar, 0);
    mainmenu();

    mainProcedure();
    sem_close(semaphore);
    return system_shutdown(lc3_system);

}

void launch_program(system_t* systemt) {
    char pro[PROGRAM_NAME_LENGTH];
    char userinput[INPUT_LENGTH];
    long programindex = -1;
    fprintf(stdout, "Select a program you want to execute among the following (write the index number):\n"
                    "%s\nChoose the program by typing its index, or press any other key to return to main menu: ",
                    print_drive_content(lc3_system));
    if ((fscanf(stdin, " %s", userinput) == 0) || ((programindex = check_user_input(userinput)) == -1))
        return;
    fflush(stdin);
    fflush(stdout);
    system_fork(systemt, strcpy(pro,lc3_system->system_directory.dentry_list[programindex].dentry.d_name));
}

void system_fork(system_t* systemt, char* program) {
    pid_t pid = vfork();
    if (pid < 0) {
        fprintf(stderr, "failed process generation\n");
        exit(1);
    }

    else if (pid == 0) {
        if (!execl("/bin/sh", "sh", "-c", SH_COMMAND))
            perror("Error in launching program!");
    }

    else {
        systemt->pid_num++;
        FILE* npipe;
        if (!(mkfifo("lc3pipe",0777))) {
            perror("Warning in opening pipe");
        }
        if ((npipe= fopen("lc3pipe", "w+")) == NULL){
            printf("Something went wrong!\n");
            return;
        }
        fprintf(npipe, "%s%s\n", systemt->system_directory.volume_path, program);
        fflush(npipe);
        mainProcedure();
    }
}

long check_user_input(char* input) {
    long inpindex;
    for (int i = 0; i < strlen(input); i++) {
        if (isalpha(input[i]))
            return -1;
    }
    if ((inpindex = strtol(input, NULL, 10)) < 0 || inpindex > lc3_system->system_directory.n_files-1)
        return -1;
    return inpindex;
}

int mainProcedure() {
    char command;
    while (lc3_system->running) {
        fprintf(stdout,"Choose among the followings:\n\n""1) Launch a new program\n""2) Read machine status\n"
                       "3) About\n""4) Shutdown\n\n""Insert a number please: ");
        fscanf(stdin, " %c", &command);
        if (command == '1')
            launch_program(lc3_system);
        if (command == '2')
            get_system_info(lc3_system);
        if (command == '3')
            about();
        if (command == '4')
            return lc3_system->running = 0;
    }
    return EXIT_FAILURE;
}


system_t* system_boot() {
    shsid = shm_open("/lc3", O_CREAT | O_RDWR, 0666);
    ftruncate(shsid, sizeof(system_t));
    if ((lc3_system = (system_t*) mmap(0, sizeof(system_t), PROT_READ | PROT_WRITE, MAP_SHARED, shsid, 0)) == MAP_FAILED) {
        printf("Map failed\n");
        exit(EXIT_FAILURE);
    }
    return system_init(lc3_system);
}

int system_shutdown(system_t* system) {
    pid_t lastpid = wait(&status);
    if (WIFEXITED(status))
        fprintf(stderr, "\n\n[%d]\tProcess %d exited with status %d\n",
                (int) getpid(), lastpid, WEXITSTATUS(status));
    shm_unlink("/lc3");
    free(system);
    system = NULL;
    return clear();
}


system_t* system_init(system_t* system) {
    fprintf(stdout,"Booting system...\n");
    if (!(system = calloc(1, sizeof(system_t)))) {
        perror("LC-3 System Boot failure!\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"Bootloader starting...\n");
    system_volume_init(system);
    system_memory_init(system);
    system->running = 1;
    return system;
}

directory_t* system_volume_init(system_t* system) {
    create_volume(&(system->system_directory));
}

mem_t* system_memory_init(system_t* system) {
    lc3_memory_init(&(system->system_memory));
}




