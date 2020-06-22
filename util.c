#include "util.h"
#include "buffer.h"
#include "uart_manager.h"

BUFFER_OBJ debugBuffer;
// Maximum length of the debug UART
#define DEBUG_LENGTH 128

// Initialise debug functions vary on whether the debug has be turned on
void init_debug(void) {
    init_buffer(&debugBuffer, DEBUG_LENGTH);
}

// Printing debug functions vary on whether the debug has be turned on.
// Just blank files if not used so print statements don't need to be removed 
// when turning off debug
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

/**
 * Check if the givens match up to strLen characters
 * @param toCheck string to check
 * @param str string to compare with
 * @param strLen number of characters to compare
 * @return true if the first strLen characters of toCheck and str match
 */
bool compare_strings(uint8_t *toCheck, char *str, uint8_t strLen) {
    uint8_t i;
    for (i = 0; i < strLen; i++) {
        if (toCheck[i] != str[i]) {
            return false;
        }
    }
    return true;
}