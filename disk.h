#ifndef DISK_H
#define DISK_H

#include <stddef.h>


void* disk_open(const char* filename, size_t size);


void disk_close(void* addr, size_t size);

#endif