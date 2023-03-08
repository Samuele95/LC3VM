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

#ifndef UNTITLED_FSYSTEM_H
#define UNTITLED_FSYSTEM_H
#define MAX_FILE_LIM 100
#define EXT_SIZE 5
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>

/**
 * Directory entry type, defined through the native UNIX VFS dirent struct.
 * It contains a char array with the extension of the directory entry.
 */

typedef struct d_entry {
    struct dirent dentry;
    char ext[EXT_SIZE];
} d_entry_t;

/**
 * File system volume struct, defined as an array/list of d_entry_t types.
 * It represents the virtual volume, or file system, associated to the
 * operating system of a LC-3 VM. From the host's perspective, it is a
 * directory entry in the local file system.
 *
 * This structs contains information related to the virtual volume, such
 * as the path of the folder in the host file system, the number of files
 * contained in the volume (host's folder) and the total occupied size.
 */

typedef struct directory {
    char volume_path[PATH_MAX];
    d_entry_t dentry_list[MAX_FILE_LIM];
    long volume_occupied_size;
    int n_files;
} directory_t;

/**
 * Initializes the LC-3 system's main volume, defining the contents
 * of the various fields in the directory_t struct.
 *
 * A standard implementation should provide three kinds of operations:
 * - a volume path definition, thus inserting a proper value in the
 *   volume_path field;
 * - the actual drive initialization in the host file system, to which
 *   the VM relies as a "physical" source for its volume and hard drive.
 *   This may be accomplished by simply creating a folder inside the
 *   directory entry in the host corresponding to the volume path;
 * - listing of all the files contained in the folder's mapped volume,
 *   followed by their insertion in the d_entry_t array/list thus
 *   allowing their use by the LC-3 VM
 *
 * The directory_t struct must be previously allocated.
 *
 * @param directory_t* pointer to an initialized and empty directory;
 * @return the same directory filled with the necessary values
 */
directory_t* create_volume(directory_t*);

/**
 * Returns the folder path in the host machine associated to the given
 * directory
 * @param directory_t* pointer to the LC-3 System directory struct;
 * @return the string containing the directory's folder path in the host OS
 */
char* get_drive_volume(directory_t*);

/**
 * Returns the total size (in bytes) of the content of the given directory
 * @param directory_t* pointer to the LC-3 System directory struct;
 * @return the "long" total size of the content of the given directory, or
 *         EXIT_FAILURE whether the value could not be computed
 */
long get_volume_size(directory_t*);



#endif //UNTITLED_FSYSTEM_H
