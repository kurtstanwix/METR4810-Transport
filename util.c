#include "util.h"
#include "buffer.h"
#include "uart_manager.h"

BUFFER_OBJ debugBuffer;
#define DEBUG_LENGTH 128

void init_debug(void) {
    init_buffer(&debugBuffer, DEBUG_LENGTH);
}

#ifdef PC_UART_NUM
#ifdef __USER_DEBUG
void print_debug(char *toPrint, uint8_t length, bool newLine) {
    copy_to_buffer(&debugBuffer, toPrint, length, true);
    if (newLine) {
        copy_to_buffer(&debugBuffer, "\r\n", 2, false);
    }
    send_buffer(&debugBuffer, PC_UART_NUM, true);
}
#else
void print_debug(char *toPrint, uint8_t length, bool newLine) {
    return;
}
#endif
#else
void print_debug(char *toPrint, uint8_t length, bool newLine) {
    return;
}
#endif

// Check if the given array contains the specified command
bool compare_strings(uint8_t *toCheck, char *command, uint8_t commandLength) {
    uint8_t i;
    for (i = 0; i < commandLength; i++) {
        if (toCheck[i] != command[i]) {
            return false;
        }
    }
    return true;
}