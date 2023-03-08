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

#include "sysinfo.h"
int sum(const char*, const struct stat*, int);
char drive_content[BUFSIZ];

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
    strcat(drive_content, " (index\t  filename\tfile extension)\n");
    for (int i = 0; i < system->system_directory.n_files; i++) {
        if (strcmp(system->system_directory.dentry_list[i].ext, ".obj") != 0) {
            sprintf(buf, "\t%d\t%s\t%s\n", i,
                    system->system_directory.dentry_list[i].dentry.d_name,
                    system->system_directory.dentry_list[i].ext);
            strcat(drive_content, buf);
        }
    }
    return drive_content;
}







