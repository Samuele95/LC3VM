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

#include "fsystem.h"
#include <stdio.h>
#include <ftw.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

static char cwd[PATH_MAX];
static long total;

/*******************************************************************************
 ******************** PRIVATE UTILITY FUNCTION DECLARATIONS ********************
 *******************************************************************************/

/**
 * Defines a path in the host file system for the given directory, which gets mapped
 * to it.
 * @param directory_t* pointer to an initialized directory;
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE otherwise
 */
static int set_drive_volume(directory_t*);

/**
 * Initialize the VM volume by creating a directory entry in the host file system,
 * at the path to which the given directory is mapped.
 * @param directory_t* pointer to an initialized directory;
 * @return EXIT_SUCCESS if the operation is successful, EXIT_FAILURE if an error
 *         occurred or the folder already exists
 */
static int mount_drive_volume(directory_t*);

/**
 * Gets the path to the current working directory in the host file system.
 * @return the string containing the path to the current working directory
 *         in the host file system.
 */
static char* get_working_dir();

/**
 * Gets the extension from the filename passed as string.
 *
 * The maximum length of the file extension string is defined through
 * the macro EXT_SIZE in the fsystem.h file
 * @return the string containing the extension of the given file
 */
static char* get_ext(const char*);

/**
 * Utility function to sum the size of the series of file in the volume path.
 */
static int sum(const char *fpath, const struct stat *sb, int typeflag);


/*******************************************************************************
 *********************** PUBLIC FUNCTION IMPLEMENTATIONS ***********************
 *******************************************************************************/

directory_t* create_volume(directory_t* directory) {
    fprintf(stdout,"Initializing system main partition...\n");
    set_drive_volume(directory);
    mount_drive_volume(directory);
    set_drive_content(directory);
    fprintf(stdout,"Directory initialization complete!\n");
    return directory;
}


char* get_drive_volume(directory_t* directory) {
    return directory->volume_path;
}


long get_volume_size(directory_t* directory) {
    if (total != 0x00)
        total = 0x00;
    if (ftw(get_drive_volume(directory), &sum, 1)) {
        perror("ftw");
        return EXIT_FAILURE;
    }
    return total;
}

/*******************************************************************************
 ********************** PRIVATE FUNCTION IMPLEMENTATIONS ***********************
 *******************************************************************************/

int set_drive_volume(directory_t* directory) {
    if (directory == NULL) {
        perror("A directory has not been defined yet!");
        return EXIT_FAILURE;
    }
    strcat(strcpy(directory->volume_path, get_working_dir(directory)), RELATIVE_VOL_PATH);
    return EXIT_SUCCESS;
}

int mount_drive_volume(directory_t* directory) {
    fprintf(stdout, "Mounting new volume...\n");
    if (!mkdir(get_drive_volume(directory), 0777) && errno == EEXIST) {
        //printf("Volume already existing in path: %s\n", get_drive_volume(directory));
        return EXIT_FAILURE;
    }
    fprintf(stdout, "System storage volume in path: %s\n", get_drive_volume(directory));
    return EXIT_SUCCESS;
}

int set_drive_content(directory_t* directory) {
    DIR* stream;
    struct dirent *d_entry;
    int i = 0;
    if ((stream = opendir(get_drive_volume(directory)))) {
        fprintf(stdout, "LC-3 Programs stored in volume: ");
        do {
            if (d_entry->d_type == DT_REG) {
                directory->dentry_list[i].dentry = *d_entry;
                //TODO: NAME BOUND CHECK
                strcpy(directory->dentry_list[i++].ext, get_ext(d_entry->d_name));
                directory->n_files++;
            }
        } while ((d_entry = readdir(stream)) != NULL);
    }
    return closedir(stream);
}

char* get_working_dir() {
    if (strlen(cwd) > PATH_MAX) {
        perror("Path length can't be longer than OS defined paths!");
        exit(EXIT_FAILURE);
    }
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Current working directory: %s\n", cwd);
        exit(EXIT_FAILURE);
    }
    return cwd;
}


char* get_ext(const char* filename) {
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return ++dot;
}


int sum(const char *fpath, const struct stat *sb, int typeflag) {
    total += sb->st_size;
    return EXIT_SUCCESS;
}
