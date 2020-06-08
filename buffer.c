#include "buffer.h"

void init_buffer(BUFFER_OBJ *buffer, uint8_t size) {
    buffer->tail = buffer->buffer;
    buffer->size = size;
    if (buffer->size > MAX_BUFFER_SIZE) {
        buffer->size = MAX_BUFFER_SIZE;
    }
}

// Fill the buffer with the given string
void copy_to_buffer(BUFFER_OBJ *buffer, char *toCopy, uint8_t numBytes, uint8_t overwrite) {
    if (overwrite) {
        buffer->tail = buffer->buffer;
    }
    // Don't copy in more than the buffer's size
    uint8_t startPos = buffer->tail - buffer->buffer;
    if (startPos + numBytes > buffer->size) {
        numBytes = buffer->size - startPos;
    }
    uint8_t i;
    for (i = startPos; i < startPos + numBytes; i++) {
        buffer->buffer[i] = toCopy[i - startPos];
    }
    buffer->tail += numBytes;
}
