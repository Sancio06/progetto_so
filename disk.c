#include "disk.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>


void* disk_open(const char* filename, size_t size) {
    int fd = open(filename, O_RDWR | O_CREAT, 0666);
    if (fd < 0) return NULL;

    int new_file = (lseek(fd, 0, SEEK_END) == 0);

    ftruncate(fd, size);

    void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) return NULL;

    if (new_file) {
        memset(addr, 0, size);
    }

    close(fd);
    return addr;
}
void disk_close(void* addr, size_t size) {
    munmap(addr, size);
}