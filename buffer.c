#include "buffer.h"

/**
 * Initialises a buffer with set size
 * @param buffer buffer object to initialise
 * @param size number of bytes to be in buffer, limited to MAX_BUFFER
 */
void init_buffer(BUFFER_OBJ *buffer, uint8_t size) {
    buffer->tail = buffer->buffer;
    buffer->size = size;
    if (buffer->size > MAX_BUFFER_SIZE) {
        buffer->size = MAX_BUFFER_SIZE;
    }
}

/**
 * Fills the buffer with the given string
 * @param buffer buffer to copy string into
 * @param toCopy string to copy into buffer
 * @param numBytes number of bytes in the string to copy
 * @param overwrite set true to overwrite previous contents of buffer, false to append
 */
void copy_to_buffer(BUFFER_OBJ *buffer, char *toCopy, uint8_t numBytes, uint8_t overwrite) {
    if (overwrite) {
        buffer->tail = buffer->buffer;
    }
    // Don't copy in more than the buffer's size
    uint8_t startPos = buffer->tail - buffer->buffer;
    if (startPos + numBytes > buffer->size) {
        numBytes = buffer->size - startPos;
    }
    // Fill the buffer with bytes
    uint8_t i;
    for (i = startPos; i < startPos + numBytes; i++) {
        buffer->buffer[i] = toCopy[i - startPos];
    }
    buffer->tail += numBytes;
}
