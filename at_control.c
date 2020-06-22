#include "at_control.h"
#include "ms_timer.h"
#include "uart_manager.h"
#include "util.h"

/////////////////////////////////////
// private defines
/////////////////////////////////////
#define AT_LENGTH (128+2) // bytes + \r\n

// Lengths of the Bluetooth address parts
#define NAP_ADDR_LENGTH 4
#define UAP_ADDR_LENGTH 2
#define LAP_ADDR_LENGTH 6
#define FORMATTED_ADDR_LENGTH 14 // Sum of above plus seperators
/////////////////////////////////////

/////////////////////////////////////
// private types
/////////////////////////////////////

/* Describes a MAC address as used by HC-05. Each part is represented by a
 * hexdecimal digit of lengths defined above.
 */
typedef struct _ADDRESS {
    uint8_t nap[NAP_ADDR_LENGTH];
    uint8_t uap[UAP_ADDR_LENGTH];
    uint8_t lap[LAP_ADDR_LENGTH];
    char formatted[FORMATTED_ADDR_LENGTH];
} ADDRESS;
/////////////////////////////////////

/////////////////////////////////////
// private function declarations
/////////////////////////////////////
void format_address(ADDRESS *address, uint8_t seperatorIndex);
void extract_address(ADDRESS *address, uint8_t *addrStr);
void create_reset(BUFFER_OBJ *buffer);
void create_command(BUFFER_OBJ *buffer, bool prefix, AT_COMMAND_PART command,
        AT_COMMAND_PART seperator, AT_COMMAND_PART param);
bool is_valid_address(ADDRESS address);
/////////////////////////////////////


/////////////////////////////////////
// private variables
/////////////////////////////////////
BUFFER_OBJ atTxBuffer; // Buffer to be used for sending
BUFFER_OBJ atRxBuffer; // Buffer to be used for receiving

// This device's friendly Bluetooth name
const AT_COMMAND_PART deviceName = {"TRANSPORT", 9};
// Host/Controller's friendly Bluetooth name
const AT_COMMAND_PART controllerHostName = {"CONTROLLER", 10};
// Host/Controller's Bluetooth MAC address
ADDRESS controllerHostAddr;
/////////////////////////////////////

/////////////////////////////////////
// external variables
/////////////////////////////////////
/* AT commands recognised by HC-05
 */
const AT_COMMAND_PART atCommands[] = {
    {"", 0},
    {"+RESET", 6},
    {"+ORGL", 5},
    {"+NAME", 5},
    {"+RNAME", 6},
    {"+ROLE", 5},
    {"+INQM", 5},
    {"+PSWD", 5},
    {"+UART", 5},
    {"+CMODE", 6},
    {"+RMAAD", 6},
    {"+MRAD", 5},
    {"+STATE", 6},
    {"+DISC", 5}
};

/* AT commands seperators
 */
const AT_COMMAND_PART atCommandDividers[] = {
    {"AT", 2}, // Command prefix
    {"?", 1}, // Query command
    {"=", 1}, // Set command
    {":", 1}, // Query response
    {":", 1}, // MAC address seperator
    {",", 1} // Parameter seperator
};

/* States the HC-05 can be in, and responses to a AT+STATE? command
 */
const AT_COMMAND_PART atStates[] = {
    {"INITIALIZED", 11},
    {"PAIRABLE", 8},
    {"CONNECTED", 9},
    {"DISCONNECTED", 12}
};

/* Different roles HC-05 can be in, either Slave or Master (or hybrid but only
 * relevant for debugging)
 */
const AT_COMMAND_PART btRoles[] = {
    {"0", 1}, // 
    {"1", 1}
};

/**
 * Initialise the HC-05 into a state ready to receive a connection from the
 * Controller. Will clear any paired devices and reset the friendly name.
 */
void init_bluetooth(void) {
    ////////
    // Send these commands:
    // AT+ORGL
    // AT+UART=9600,1,0
    // AT+NAME=TRANSPORT
    // AT+INQM=0,10,4
    // AT+CMODE=1
    ////////
    MS_TIMER_Delay_ms(1000); // Wait for bluetooth to boot before restarting
    init_buffer(&atRxBuffer, AT_LENGTH);
    init_buffer(&atTxBuffer, AT_LENGTH);

    AT_PIN_Set();
    AT_COMMAND_PART params;
    MS_TIMER_Delay_ms(100);
    // Set to original manufacturer settings
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_ORGL_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
    copy_to_buffer(&atTxBuffer, "AT+ORGL\r\n", 9, true);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();

    // UART with 9600 BAUD, 1 stop bit and 0 parity bits
    params.part = "9600,1,0";
    params.length = 8;
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_UART_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            params);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();

    // Set the device's friendly visible bluetooth name
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_NAME_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            deviceName);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();

    // Standard inquiry mode, max 10 bluetooth devices to inquire
    // Timeout in 4*1.28=5.12 seconds
    params.part = "0,10,4";
    params.length = 6;
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_INQM_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            params);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();

    // Module will connect to any address
    params.part = "1";
    params.length = 1;
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_CMODE_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            params);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();

    // Remove all authenticated devices
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_RMAAD_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
}

/**
 * Checks if buffer contains an AT State response, if it does, returns true and
 * sets the buffers tail to the beginning of the response.
 * @param buffer : Buffer to with data to check if it contains an AT+STATE
 * response.
 * @return true if buffer contains a response to an AT+STATE command, false
 * otherwise.
 */
uint8_t is_state_response(BUFFER_OBJ *buffer) {
    if (compare_strings(buffer->buffer, \
                atCommands[AT_COMMAND_STATE_INDEX].part, \
                atCommands[AT_COMMAND_STATE_INDEX].length) &&
            compare_strings(buffer->buffer + atCommands[AT_COMMAND_STATE_INDEX].length, \
                    atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].part, \
                    atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length)) {

        // Set tail position to the start of the state response
        buffer->tail = buffer->buffer + \
                atCommands[AT_COMMAND_STATE_INDEX].length + \
                atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length;
        return true;
    }
    return false;
}

/**
 * Determines which state is contained in the buffer
 * @param buffer : buffer with tail pointing to start of state string
 * @return true if buffer contains a valid state
 */
uint8_t extract_state(BUFFER_OBJ *buffer) {
    uint8_t i;
    // 32 bit divide by 16 bit instruction as much quicker than compiled divide
    uint8_t numStates = __builtin_divsd(sizeof atStates, sizeof atStates[0]);
    for (i = 0; i < numStates; i++) {
        if (compare_strings(buffer->tail, atStates[i].part, atStates[i].length)) {

            // print the state
            print_debug(atStates[i].part, atStates[i].length, true);

            return i;
        }
    }
    return BT_STATE_INVALID; // Not a valid state
}

/**
 * Queries and returns the state code of the bluetooth module.
 * @param atModeOnExit : if true, leaves the module in AT command mode on return
 * @return The state code the module is in
 */
uint8_t get_bt_state(bool atModeOnExit) {
    uint8_t rxStatus;
    while (true) {
        // Create a state query command
        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_STATE_INDEX],
                atCommandDividers[AT_COMMAND_QUERY_CHAR_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX]);
        BLUETOOTH_CLEAR_RX_BUFFER();
        AT_PIN_Set();
        MS_TIMER_Delay_ms(100);

        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
        atRxBuffer.tail = atRxBuffer.buffer; // Reset Rx buffer
        rxStatus = read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 100);
        if (rxStatus == UART_READ_LINE_NOTHING) {
            // Didn't get a response, possibly in comms mode; cycle pin and try
            // again
            AT_PIN_Clear();
            continue;
        }
        BLUETOOTH_CLEAR_RX_BUFFER();
        if (is_state_response(&atRxBuffer)) {
            if (!atModeOnExit) {
                AT_PIN_Clear();
            }
            return extract_state(&atRxBuffer);
        }
    }
}

/**
 * Reset the bluetooth module and exit AT mode
 */
void reset_bluetooth(void) {
    AT_PIN_Set();
    create_reset(&atTxBuffer);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    MS_TIMER_Delay_ms(100);
    AT_PIN_Clear();
    BLUETOOTH_CLEAR_RX_BUFFER();
}

/**
 * Get the friendly Bluetooth name of the connected device
 * @return friendly Bluetooth name of the connected device
 */
bool get_connected_device_name(void) {
    AT_PIN_Clear(); // Clear then set to ensure in AT mode
    MS_TIMER_Delay_ms(100);
    AT_PIN_Set();
    MS_TIMER_Delay_ms(100);

    uint8_t attempts = 0; // Get address may fail, keep track of how many tries
    while (true) {
        // Command to get the most recent connected device address, MRAD
        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_MRAD_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX]);

        BLUETOOTH_CLEAR_RX_BUFFER(); // To ensure only the upcoming response is read
        atRxBuffer.tail = atRxBuffer.buffer;
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true); // Send address request
        read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 400); // Hopefully is an MRAD response
        if (!compare_strings(atRxBuffer.buffer,
                atCommands[AT_COMMAND_MRAD_INDEX].part,
                atCommands[AT_COMMAND_MRAD_INDEX].length)) {
            // Not an MRAD response (or nothing received), try read another line
            // Two reads are necessary as the module will respond with the MRAD message,
            // but also with an "OK" message which has the chance of coming first
            atRxBuffer.tail = atRxBuffer.buffer;
            read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 400);
            if (!compare_strings(atRxBuffer.buffer,
                    atCommands[AT_COMMAND_MRAD_INDEX].part,
                    atCommands[AT_COMMAND_MRAD_INDEX].length)) {
                // Second read was also not an MRAD, failed attempt
                if (++attempts == 10) {
                    // If 10 attempts, safe to assume a connection didn't actually occur
                    return false;
                }
                continue; // No MRAD response, try again
            }
        }
        // Got an MRAD response, make sure the address is valid
        extract_address(&controllerHostAddr, atRxBuffer.buffer + \
                atCommands[AT_COMMAND_MRAD_INDEX].length + \
                atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length);
        if (!is_valid_address(controllerHostAddr)) {
            // Was a valid MRAD response, but address provided was invalid
            if (++attempts == 10) {
                return false;
            }
            continue;
        }
        BLUETOOTH_CLEAR_RX_BUFFER();
        break;
    }
    attempts = 0; // Get name may fail, reset to keep track of how many tries
    // Reformats received address into a valid argument format
    format_address(&controllerHostAddr, AT_COMMAND_PARAM_SEPERATOR_CHAR_INDEX);
    while (true) {
        // Turns the address into a command argument
        AT_COMMAND_PART tempAddr = {controllerHostAddr.formatted, FORMATTED_ADDR_LENGTH};
        // Command to get the friendly name of a device, RNAME
        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_RNAME_INDEX],
                atCommandDividers[AT_COMMAND_QUERY_CHAR_INDEX],
                tempAddr);

        atRxBuffer.tail = atRxBuffer.buffer;
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true); // Send name request
        // Gets the response, with a large timeout as requesting a device name can
        // take a while
        uint8_t status = read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 2000);
        if (!compare_strings(atRxBuffer.buffer,
                atCommands[AT_COMMAND_RNAME_INDEX].part,
                atCommands[AT_COMMAND_RNAME_INDEX].length)) {
            // Not a valid RNAME response, try again as a valid response can be
            // followed by an "OK" message which sometimes gets received first
            atRxBuffer.tail = atRxBuffer.buffer;
            read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 2000);
            if (!compare_strings(atRxBuffer.buffer,
                    atCommands[AT_COMMAND_RNAME_INDEX].part,
                    atCommands[AT_COMMAND_RNAME_INDEX].length)) {
                // Second read failed
                if (++attempts == 10) {
                    return false;
                }
                continue; // No RNAME response, try again
            }
        }
        BLUETOOTH_CLEAR_RX_BUFFER(); // In case "OK" message hasn't been cleared
        break;
    }
    // Remove the RNAME message header so atRxBuffer just contains the device name
    copy_to_buffer(&atRxBuffer,
            atRxBuffer.buffer + atCommands[AT_COMMAND_RNAME_INDEX].length + \
            atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length,
            atRxBuffer.tail -
            (atRxBuffer.buffer + atCommands[AT_COMMAND_RNAME_INDEX].length + \
            atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length),
            true);
    return true;
}

/**
 * Wait for a Controller System to connect to this device. Will wait indefinitely
 * until a connection is made to a device with the correct host name
 */
void wait_for_connection(void) {
    while (true) {
        STATUS_LED_SetPin(PIN_STATE_OFF);
        init_bluetooth();
        STATUS_LED_SetPin(PIN_STATE_ON);
        while (!BT_IS_CONNECTED()) { // Pin will be high when connected
            AT_PIN_Clear();
            MS_TIMER_Delay_ms(100);
        }
        // The host need only start with the host name, allowing for a suffix at the end
        // if needed
        if (get_connected_device_name() &&
                compare_strings(atRxBuffer.buffer, controllerHostName.part, controllerHostName.length)) {
            break;
        }
        // Host name did not match, disconnect with this device and reinitialise
        print_debug("Host Name Incorrect: ", 21, false);
        print_debug(atRxBuffer.buffer, atRxBuffer.tail - atRxBuffer.buffer, false);
        atRxBuffer.tail = atRxBuffer.buffer;

        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_DISC_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX]);
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
        MS_TIMER_Delay_ms(100);
        AT_PIN_Clear();
        BLUETOOTH_CLEAR_RX_BUFFER();
    }
    AT_PIN_Clear();
    MS_TIMER_Delay_ms(20);
    BLUETOOTH_CLEAR_RX_BUFFER();
}

/////////////////////////////////////
// private function implementations
/////////////////////////////////////

/**
 * Determines if the address is a valid Bluetooth address
 * @param address the address to check
 * @return true if address is a valid Bluetooth address
 */
bool is_valid_address(ADDRESS address) {
    uint8_t i;
    for (i = 0; i < NAP_ADDR_LENGTH; i++) {
        if (address.nap[i] != '0') {
            return true;
        }
    }
    return false; // Upper 4 bytes are not 0 
}

/**
 * Format the address into a string
 * @param address address to format
 * @param seperatorIndex the seperator to use between address parts
 */
void format_address(ADDRESS *address, uint8_t seperatorIndex) {
    uint8_t i;
    uint8_t offset = 0;
    // Fill upper bytes
    for (i = 0; i < 4; i++) {
        address->formatted[i] = address->nap[i];
    }
    address->formatted[i++] = atCommandDividers[seperatorIndex].part[0];
    offset += i;
    // Fill middle bytes
    for (i = 0; i < 2; i++) {
        address->formatted[offset + i] = address->uap[i];
    }
    address->formatted[offset + i++] = atCommandDividers[seperatorIndex].part[0];
    offset += i;
    // Fill lower bytes
    for (i = 0; i < 6; i++) {
        address->formatted[offset + i] = address->lap[i];
    }
}

/**
 * Extract the Bluetooth address from a string
 * @param address destination for extracted address
 * @param addrStr string to extract the address from
 */
void extract_address(ADDRESS *address, uint8_t *addrStr) {
    uint8_t addrSeperator = atCommandDividers[AT_COMMAND_ADDR_SEPERATOR_CHAR_INDEX].part[0];
    uint8_t paramSeperator = atCommandDividers[AT_COMMAND_PARAM_SEPERATOR_CHAR_INDEX].part[0];
    
    // Find the end of the string
    uint8_t *pos = addrStr;
    while (*pos != '\r') {
        pos++;
    }
    pos--;
    // Get the lower bytes, padding with zero if necessary
    uint8_t j = LAP_ADDR_LENGTH;
    while (j > 0) {
        if (pos == (addrStr - 1)) {
            address->lap[--j] = '0';
        } else {
            if (*pos == addrSeperator || *pos == paramSeperator) {
                pos--;
                continue;
            }
            address->lap[--j] = *(pos--);
        }
    }
    // Get the middle bytes, padding with zero if necessary
    j = UAP_ADDR_LENGTH;
    while (j > 0) {
        if (pos == (addrStr - 1)) {
            address->uap[--j] = '0';
        } else {
            if (*pos == addrSeperator || *pos == paramSeperator) {
                pos--;
                continue;
            }
            address->uap[--j] = *(pos--);
        }
    }
    // Get the upper bytes, padding with zero if necessary
    j = NAP_ADDR_LENGTH;
    while (j > 0) {
        if (pos == (addrStr - 1)) {
            address->nap[--j] = '0';
        } else {
            if (*pos == addrSeperator || *pos == paramSeperator) {
                pos--;
                continue;
            }
            address->nap[--j] = *(pos--);
        }
    }
}

/**
 * Creates an AT reset message as per HC-05 documentation
 * @param buffer buffer to store the command
 */
void create_reset(BUFFER_OBJ *buffer) {
    create_command(buffer, true, atCommands[AT_COMMAND_RESET_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
}

/**
 * Create a given AT command as per HC-05 documentation
 * @param buffer buffer to store the command
 * @param prefix whether to use the AT+ prefix
 * @param command the command to create
 * @param seperator the separator between command and parameters
 * @param param the parameters to use
 */
void create_command(BUFFER_OBJ *buffer, bool prefix, AT_COMMAND_PART command,
        AT_COMMAND_PART seperator, AT_COMMAND_PART param) {
    if (prefix) {
        copy_to_buffer(buffer,
                atCommandDividers[AT_COMMAND_PREFIX_INDEX].part,
                atCommandDividers[AT_COMMAND_PREFIX_INDEX].length,
                true);
    }
    copy_to_buffer(buffer, command.part, command.length, !prefix);
    copy_to_buffer(buffer, seperator.part, seperator.length, false);
    copy_to_buffer(buffer, param.part, param.length, false);
    copy_to_buffer(buffer, "\r\n", 2, false);
}