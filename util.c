#include "util.h"

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