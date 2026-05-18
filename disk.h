#ifndef DISK_H
#define DISK_H

#include <stddef.h>

// apriamo o creiamo  un file e lo mappiamo  in memoria
void* disk_open(const char* filename, size_t size);

// chiude il mapping
void disk_close(void* addr, size_t size);

#endif