#ifndef BUFFER_H
#define	BUFFER_H

#include <stdint.h>

#define MAX_BUFFER_SIZE (64+2)

typedef struct _BUFFER_OBJ {
    uint8_t buffer[MAX_BUFFER_SIZE];
    uint8_t *tail;
    uint8_t size;
} BUFFER_OBJ;

void init_buffer(BUFFER_OBJ *buffer, uint8_t size);

void copy_to_buffer(BUFFER_OBJ *buffer, char *toCopy, uint8_t numBytes, uint8_t overwrite);

#endif

