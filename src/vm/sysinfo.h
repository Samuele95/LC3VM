/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 Samuele Stronati, Universit√† degli Studi di Camerino.
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

#ifndef UNTITLED_SYSINFO_H
#define UNTITLED_SYSINFO_H
#define BUF_LENGTH 1024
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
#include <linux/limits.h>
#include <sys/utsname.h>
#include "os.h"


void get_system_info(system_t*);

float get_memory_size(system_t*);

int get_cpuregisters_num(system_t*);

int get_processes_num(system_t*);

int get_host_info();

char* print_drive_content(system_t*);

long get_volume_size(directory_t*);

#endif //UNTITLED_SYSINFO_H
