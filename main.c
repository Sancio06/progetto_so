#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "disk.h"
#include "fs.h"

#define MAX_CMD 128

int main() {

    size_t size = sizeof(FileSystem);

    void* disk = disk_open("myfs.bin", size);
    if (!disk) return 1;

    printf("MiniFS shell avviata. Scrivi 'exit' per uscire.\n");

    fs_init(disk);

    char line[MAX_CMD];

    while (1) {

        printf("\n> ");
        fflush(stdout);

        if (!fgets(line, MAX_CMD, stdin))
            break;

        // rimuove newline
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0)
            break;

        // parsing semplice: comando + arg1 + arg2
        char cmd[32], arg1[64], arg2[64];
        arg1[0] = arg2[0] = 0;

        int n = sscanf(line, "%s %s %s", cmd, arg1, arg2);


        if (strcmp(cmd, "mkdir") == 0 && n >= 2)
            fs_mkdir(arg1);

        else if (strcmp(cmd, "touch") == 0 && n >= 2)
            fs_touch(arg1);

        else if (strcmp(cmd, "ls") == 0)
                fs_ls(n >= 2 ? arg1 : NULL);

        else if (strcmp(cmd, "cd") == 0 && n >= 2)
            fs_cd(arg1);

        else if (strcmp(cmd, "cat") == 0 && n >= 2)
            fs_cat(arg1);

        else if (strcmp(cmd, "rm") == 0 && n >= 2)
            fs_rm(arg1);

        else if (strcmp(cmd, "append") == 0 && n >= 3)
            fs_append(arg1, arg2);

        else
            printf("Comando non valido\n");
    }

    disk_close(disk, size);

    return 0;
}