#include "at_control.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/tmr2.h"
#include "uart_manager.h"
#include "util.h"


/////////////////////////////////////
// private defines
/////////////////////////////////////
#define AT_LENGTH (128+2)

#define NAP_ADDR_LENGTH 4
#define UAP_ADDR_LENGTH 2
#define LAP_ADDR_LENGTH 6
#define FORMATTED_ADDR_LENGTH 14
/////////////////////////////////////

/////////////////////////////////////
// private types
/////////////////////////////////////
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
BUFFER_OBJ atTxBuffer;
BUFFER_OBJ atRxBuffer;


const AT_COMMAND_PART deviceName = {"TRANSPORT", 9};
const AT_COMMAND_PART controllerHostName = {"KURT-PC", 7};
ADDRESS hostAddress;
/////////////////////////////////////

/////////////////////////////////////
// external variables
/////////////////////////////////////
const AT_COMMAND_PART atCommands[] = {
    {"", 0},
    {"+RESET", 6},
    {"+ORGL", 5},
    //AT_COMMAND_ADDR,
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

const AT_COMMAND_PART atCommandDividers[] = {
    {"AT", 2},
    {"?", 1},
    {"=", 1},
    {":", 1},
    {":", 1},
    {",", 1}
};

const AT_COMMAND_PART atStates[] = {
    {"INITIALIZED", 11},
    {"PAIRABLE", 8},
    {"CONNECTED", 9},
    {"DISCONNECTED", 12}
};

const AT_COMMAND_PART btRoles[] = {
    {"0", 1},
    {"1", 1}
};

void init_bluetooth(void) {
    ////////
    // AT+ORGL\r\n
    // AT+UART=9600,1,0\r\n
    // AT+NAME=TRANSPORT
    // AT+INQM=0,10,4
    // AT+CMODE=1
    TMR1_Delay_ms(1000); // Wait for bluetooth to boot before restarting
    init_buffer(&atRxBuffer, AT_LENGTH);
    init_buffer(&atTxBuffer, AT_LENGTH);
    
    AT_PIN_Set();
    AT_COMMAND_PART params;
    TMR1_Delay_ms(100);
    // Set to original settings
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_ORGL_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
    copy_to_buffer(&atTxBuffer, "AT+ORGL\r\n", 9, true);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
    
    // UART with 9600 BAUD, 1 stop bit and 0 parity bits
    params.part = "9600,1,0";
    params.length = 8;
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_UART_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            params);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
    
    // Set the device's friendly visible bluetooth name
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_NAME_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            deviceName);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
    
    // Standard inquiry mode, max 10 bluetooth devices to inquire
    // Timeout in 4*1.28=5.12 seconds
    params.part = "0,10,4";
    params.length = 6;
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_INQM_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            params);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
    
    // Module will connect to any address
    params.part = "1";
    params.length = 1;
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_CMODE_INDEX],
            atCommandDividers[AT_COMMAND_SET_CHAR_INDEX],
            params);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
    
    // Remove all authenticated devices
    create_command(&atTxBuffer, true, atCommands[AT_COMMAND_RMAAD_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(400);
    BLUETOOTH_CLEAR_RX_BUFFER();
}

// Checks if buffer contains an AT State response, if it does, returns true and
// sets the buffers tail to the begining of the response
uint8_t is_state_response(BUFFER_OBJ *buffer) {
    if (compare_strings(buffer->buffer, \
                atCommands[AT_COMMAND_STATE_INDEX].part, \
                atCommands[AT_COMMAND_STATE_INDEX].length) &&
            compare_strings(buffer->buffer + atCommands[AT_COMMAND_STATE_INDEX].length, \
                    atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].part, \
                    atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length)) {
        buffer->tail = buffer->buffer + \
                atCommands[AT_COMMAND_STATE_INDEX].length + \
                atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length;
        return true;
    }
    return false;
}

// Determines which state string
uint8_t extract_state(BUFFER_OBJ *buffer) {
    uint8_t i;
    uint8_t numStates = sizeof atStates / sizeof atStates[0];
    for (i = 0; i < numStates; i++) {
        if (compare_strings(buffer->tail, atStates[i].part, atStates[i].length)) {
            copy_to_buffer(buffer, atStates[i].part, atStates[i].length, true);
            copy_to_buffer(buffer, "\r\n", 2, false);
            send_buffer(buffer, PC_UART_NUM, true);
            
            return i;
        }
    }
    return BT_STATE_INVALID; // Not a valid state
}

// Returns the state of the bluetooth module
// atModeOnExit: if true, leaves the module in AT mode
uint8_t get_bt_state(bool atModeOnExit) {
    uint8_t rxStatus;
    while (true) {
        copy_to_buffer(&atTxBuffer,
                atCommandDividers[AT_COMMAND_PREFIX_INDEX].part,
                atCommandDividers[AT_COMMAND_PREFIX_INDEX].length,
                true);
        copy_to_buffer(&atTxBuffer,
                atCommands[AT_COMMAND_STATE_INDEX].part,
                atCommands[AT_COMMAND_STATE_INDEX].length,
                false);
        copy_to_buffer(&atTxBuffer,
                "\r\n",
                2,
                false);
        BLUETOOTH_CLEAR_RX_BUFFER();
        AT_PIN_Set();
        TMR1_Delay_ms(100);
        
        //send_buffer(&atTxBuffer, PC_UART_NUM, false);
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
        atRxBuffer.tail = atRxBuffer.buffer;
        rxStatus = read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 100);
        if (rxStatus == UART_READ_LINE_NOTHING) {
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

// Reset the bluetooth module and exit AT mode
void reset_bluetooth(void) {
    AT_PIN_Set();
    create_reset(&atTxBuffer);
    send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
    TMR1_Delay_ms(100);
    AT_PIN_Clear();
    BLUETOOTH_CLEAR_RX_BUFFER();
}

bool get_connected_device_name(void) {
    AT_PIN_Clear();
    TMR1_Delay_ms(100);
    AT_PIN_Set();
    TMR1_Delay_ms(100);
    uint8_t attempts = 0;
    while (true) {
        AT_PIN_Set();
        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_MRAD_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX],
                atCommands[AT_COMMAND_BLANK_INDEX]);
        
        BLUETOOTH_CLEAR_RX_BUFFER();
        atRxBuffer.tail = atRxBuffer.buffer;
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
        read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 400);
        if (!compare_strings(atRxBuffer.buffer,
                atCommands[AT_COMMAND_MRAD_INDEX].part,
                atCommands[AT_COMMAND_MRAD_INDEX].length)) {
            atRxBuffer.tail = atRxBuffer.buffer;
            read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 400);
            if (!compare_strings(atRxBuffer.buffer,
                    atCommands[AT_COMMAND_MRAD_INDEX].part,
                    atCommands[AT_COMMAND_MRAD_INDEX].length)) {
                if (++attempts == 10) {
                    return false;
                }
                continue; // No MRAD response, try again
            }
        }
        // Got an MRAD response, make sure the address is valid
        extract_address(&hostAddress, atRxBuffer.buffer + \
                atCommands[AT_COMMAND_MRAD_INDEX].length + \
                atCommandDividers[AT_COMMAND_RESPONSE_CHAR_INDEX].length);
        if (!is_valid_address(hostAddress)) {
            if (++attempts == 10) {
                return false;
            }
            continue;
        }
        BLUETOOTH_CLEAR_RX_BUFFER();
        break;
    }
    attempts = 0;
    format_address(&hostAddress, AT_COMMAND_PARAM_SEPERATOR_CHAR_INDEX);
    while (true) {
        AT_PIN_Set();
        AT_COMMAND_PART tempAddr = {hostAddress.formatted, FORMATTED_ADDR_LENGTH};
        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_RNAME_INDEX],
                atCommandDividers[AT_COMMAND_QUERY_CHAR_INDEX],
                tempAddr);
        
        atRxBuffer.tail = atRxBuffer.buffer;
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
        uint8_t status = read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 2000);
        if (!compare_strings(atRxBuffer.buffer,
                atCommands[AT_COMMAND_RNAME_INDEX].part,
                atCommands[AT_COMMAND_RNAME_INDEX].length)) {
            atRxBuffer.tail = atRxBuffer.buffer;
            read_line_to_buffer(&atRxBuffer, BLUETOOTH_UART_NUM, 2000);
            if (!compare_strings(atRxBuffer.buffer,
                    atCommands[AT_COMMAND_RNAME_INDEX].part,
                    atCommands[AT_COMMAND_RNAME_INDEX].length)) {
                if (++attempts == 10) {
                    return false;
                }
                continue; // No RNAME response, try again
            }
        }
        BLUETOOTH_CLEAR_RX_BUFFER();
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
    //AT_PIN_Clear();
}


void wait_for_connection(void) {
    while (true) {
        init_bluetooth();
        while (!BT_IS_CONNECTED()) {
            AT_PIN_Clear();
            //get_bt_state(false);
            TMR1_Delay_ms(100);
        }
        get_connected_device_name();
        if (compare_strings(atRxBuffer.buffer, controllerHostName.part, controllerHostName.length)) {
            break;
        }
        copy_to_buffer(&atTxBuffer, "Host Name Incorrect: ", 21, true);
        copy_to_buffer(&atTxBuffer, atRxBuffer.buffer,
                atRxBuffer.tail - atRxBuffer.buffer, false);
        send_buffer(&atTxBuffer, PC_UART_NUM, true);
        create_command(&atTxBuffer, true, atCommands[AT_COMMAND_DISC_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
        send_buffer(&atTxBuffer, BLUETOOTH_UART_NUM, true);
        TMR1_Delay_ms(100);
        AT_PIN_Clear();
        //TMR1_Delay_ms(400);
        BLUETOOTH_CLEAR_RX_BUFFER();
        //init_bluetooth();
        //reset_bluetooth();
    }
    AT_PIN_Clear();
    BLUETOOTH_CLEAR_RX_BUFFER();
    /*uint8_t state = BT_STATE_INVALID;
    while (state != BT_STATE_CONNECTED_INDEX) {
        state = get_bt_state(false);
        TMR1_Delay_ms(200);
        BLUETOOTH_CLEAR_RX_BUFFER();
    }
    AT_PIN_Clear();
    TMR1_Delay_ms(100);*/
}



/////////////////////////////////////
// private function implementations
/////////////////////////////////////

bool is_valid_address(ADDRESS address) {
    uint8_t i;
    for (i = 0; i < NAP_ADDR_LENGTH; i++) {
        if (address.nap[i] != '0') {
            return true;
        }
    }
    return false; // Upper 4 bytes are 
}

void format_address(ADDRESS *address, uint8_t seperatorIndex) {
    uint8_t i;
    uint8_t offset = 0;
    for (i = 0; i < 4; i++) {
        address->formatted[i] = address->nap[i];
    }
    address->formatted[i++] = atCommandDividers[seperatorIndex].part[0];
    offset += i;
    for (i = 0; i < 2; i++) {
        address->formatted[offset + i] = address->uap[i];
    }
    address->formatted[offset + i++] = atCommandDividers[seperatorIndex].part[0];
    offset += i;
    for (i = 0; i < 6; i++) {
        address->formatted[offset + i] = address->lap[i];
    }
}

void extract_address(ADDRESS *address, uint8_t *addrStr) {
    uint8_t addrSeperator = atCommandDividers[AT_COMMAND_ADDR_SEPERATOR_CHAR_INDEX].part[0];
    uint8_t paramSeperator = atCommandDividers[AT_COMMAND_PARAM_SEPERATOR_CHAR_INDEX].part[0];
    //int8_t i;
    //for (i = 0; i < FORMATTED_ADDR_LENGTH; i++) {
    //    address->formatted[i] = '0';
    //}
    //i--;
    uint8_t *pos = addrStr;
    while (*pos != '\r') {
        pos++;
    }
    pos--;
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
    /*
    
    //pos--;
    
    
    uint8_t start = 0;
    
    while (addrStr[i] != addrSeperator) {
        address->nap[i] = addrStr[i];
        i++;
    }
    // Only 2 bytes in upper, pre-pad with two "0"'s
    if (i == 2) {
        address->nap[2] = address->nap[0];
        address->nap[3] = address->nap[1];
        address->nap[0] = '0';
        address->nap[1] = '0';
    }
    i++;
    start = i;
    while (addrStr[i] != addrSeperator) {
        address->uap[i - start] = addrStr[i];
        i++;
    }
    i++;
    start = i;
    while (!(addrStr[i] == '\r' || addrStr[i] == paramSeperator)) {
        address->lap[i - start] = addrStr[i];
        i++;
    }*/
}

// Creates an AT reset message as per HC-05 documentation
void create_reset(BUFFER_OBJ *buffer) {
    create_command(buffer, true, atCommands[AT_COMMAND_RESET_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX],
            atCommands[AT_COMMAND_BLANK_INDEX]);
}

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