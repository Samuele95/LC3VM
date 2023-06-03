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
#include "ui.h"
#include <semaphore.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <wait.h>
#include <sys/utsname.h>


static system_t* system_init(system_t*);
static directory_t* system_volume_init(system_t*);
directory_t* update_volume(system_t*);
static mem_t* system_memory_init(system_t*);
static long check_user_input(char* input);
static void system_fork(system_t* systemt, char* program);
static char* choose_program();
static int sum(const char*, const struct stat*, int);
static int shsid;
static int status;
static system_t* lc3_system;
static char program[PROGRAM_NAME_LENGTH];
static char drive_content[BUFSIZ];


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

void launch_editor() {
    int status;
    char launch_command[BUFSIZ];
    /*
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "failed process generation\n");
        exit(1);
    }

    else if (pid == 0) {
        strcpy(launch_command, EDITOR_COMMAND);
        //strcat(launch_command, " ");
        //strcat(launch_command, "/utils/prova.lc3");
        if (!execl("/bin/sh", "sh", "-c", launch_command))
            perror("Error in launching text editor!");
    }

    else {
        wait(&status);
        mainProcedure();
    }
     */
    strcpy(launch_command, "clear ; ./texor ");
    //strcat(launch_command, lc3_system->system_directory.volume_path);
    if (choose_program() != NULL)
        strcat(strcat(launch_command, lc3_system->system_directory.volume_path), program);
    system(launch_command);
    fflush(stdin);
    fflush(stdout);
    mainProcedure();

}

void launch_program(system_t* systemt) {
/*
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
 */
    if (choose_program() == NULL) return;
    system_fork(systemt, program);
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
        fflush(stdin);
        fflush(stdout);
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
        fprintf(stdout,"Choose among the followings:\n\n""1) Write and compile an LC3 program\n2) Launch a new program\n""3) Read machine status\n"
                       "4) About\n""5) Shutdown\n\n""Insert a number please: ");
        fscanf(stdin, " %c", &command);
        if (command == '1')
            launch_editor();
        if (command == '2')
            launch_program(lc3_system);
        if (command == '3') {
            update_volume(lc3_system); 
            get_system_info(lc3_system);  
        }
        if (command == '4')
            about();
        if (command == '5')
            return lc3_system->running = 0;
        /*
        switch (command) {
            case '1' : launch_editor();
            case '2' : launch_program(lc3_system);
            case '3' : update_volume(lc3_system); 
                       get_system_info(lc3_system);
            case '4' : about();
            case '5' : return lc3_system->running = 0;
            default  : break;
        }
        command = 0;
        fflush(stdin);
        fflush(stdout);
        */
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
    return create_volume(&(system->system_directory));
}

directory_t* update_volume(system_t* system) {
    memset(&(system->system_directory), 0, sizeof(directory_t));
    return system_volume_init(system);
}

mem_t* system_memory_init(system_t* system) {
    return lc3_memory_init(&(system->system_memory));
}

char* choose_program() {
    memset(program,0, PROGRAM_NAME_LENGTH);
    char userinput[INPUT_LENGTH];
    long programindex = -1;
    fprintf(stdout, "Select a program among the followings:\n"
                    "%s\nChoose the program by typing its index, or press any other key to continue: ",
            print_drive_content(lc3_system));
    if ((fscanf(stdin, " %s", userinput) == 0) || ((programindex = check_user_input(userinput)) == -1))
        return NULL;
    fflush(stdin);
    fflush(stdout);
    return strcpy(program,lc3_system->system_directory.dentry_list[programindex].dentry.d_name);
}

void get_system_info(system_t* system) {
    printf("\n\n/****************************************************"
           "***********************************************/\n");
    printf("\nVIRTUAL MACHINE INFO:\nMachine location in host file system: %s\n", get_drive_volume(&system->system_directory));
    printf("\nVOLUME INFO:\nLC-3 Hard Drive used space: %ld bytes\n%s", get_volume_size(&system->system_directory),
           print_drive_content(system));
    printf("\nMEMORY INFO:\nRAM size: %.1f GB\nCPU registers: %d\n", get_memory_size(system), get_cpuregisters_num(system));
    printf("\nPROCESSES INFO:\nExecuted processes: %d\n", get_processes_num(system));
    get_host_info();
    printf("\n/****************************************************"
           "***********************************************/\n\n\n");
}

float get_memory_size(system_t* system) {
    return MEM_16BIT/(8.0f * 1024.0f);
}

int get_cpuregisters_num(system_t* system) {
    return CPU_REG_NUM;
}

int get_processes_num(system_t* system) {
    return system->pid_num;
}

int get_host_info() {
    struct utsname uname_dt;
    if (uname(&uname_dt) < 0) {
        perror("uname");
        exit(EXIT_FAILURE);
    }

    printf("\nHOST INFO\n");
    printf("System name: %s\n", uname_dt.sysname);
    printf("Node name: %s\n", uname_dt.nodename);
    printf("Release: %s\n", uname_dt.release);
    printf("Version: %s\n", uname_dt.version);
    printf("CPU Architecture: %s\n", uname_dt.machine);
    return EXIT_SUCCESS;
}

char* print_drive_content(system_t* system) {
    char buf[BUF_LENGTH];
    if (strlen(drive_content) != 0)
        strcpy(drive_content, "");
    strcat(drive_content, "Directory tree content:\n");
    strcat(drive_content, "(index\t  filename\tfile extension)\n");
    for (int i = 0; i < system->system_directory.n_files; i++) {
        if (strcmp(system->system_directory.dentry_list[i].ext, ".obj") != 0) {
            sprintf(buf, "%d\t%s\t%s\n", i,
                    system->system_directory.dentry_list[i].dentry.d_name,
                    system->system_directory.dentry_list[i].ext);
            strcat(drive_content, buf);
        }
    }
    return drive_content;
}




