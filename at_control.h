#ifndef AT_CONTROL_H
#define	AT_CONTROL_H

#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"
#include "mcc_generated_files/pin_manager.h"

#define AT_PIN_Set() IO_RB8_SetPin(1)
#define AT_PIN_Clear() IO_RB8_SetPin(0)
#define BT_IS_CONNECTED() IO_RB9_GetValue()


typedef struct _AT_COMMAND {
    char *part;
    uint8_t length;
} AT_COMMAND_PART;

#define AT_COMMAND_BLANK_INDEX 0
#define AT_COMMAND_RESET_INDEX 1
#define AT_COMMAND_ORGL_INDEX 2
#define AT_COMMAND_NAME_INDEX 3
#define AT_COMMAND_RNAME_INDEX 4
#define AT_COMMAND_ROLE_INDEX 5
#define AT_COMMAND_INQM_INDEX 6
#define AT_COMMAND_PSWD_INDEX 7
#define AT_COMMAND_UART_INDEX 8
#define AT_COMMAND_CMODE_INDEX 9
#define AT_COMMAND_RMAAD_INDEX 10
#define AT_COMMAND_MRAD_INDEX 11
#define AT_COMMAND_STATE_INDEX 12
#define AT_COMMAND_DISC_INDEX 13
extern const AT_COMMAND_PART atCommands[];

#define AT_COMMAND_PREFIX_INDEX 0
#define AT_COMMAND_QUERY_CHAR_INDEX 1
#define AT_COMMAND_SET_CHAR_INDEX 2
#define AT_COMMAND_RESPONSE_CHAR_INDEX 3
#define AT_COMMAND_ADDR_SEPERATOR_CHAR_INDEX 4
#define AT_COMMAND_PARAM_SEPERATOR_CHAR_INDEX 5
extern const AT_COMMAND_PART atCommandDividers[];

#define BT_STATE_INITIALISED_INDEX 0
#define BT_STATE_PAIRABLE_INDEX 1
#define BT_STATE_CONNECTED_INDEX 2
#define BT_STATE_DISCONNECTED_INDEX 3
#define BT_STATE_INVALID 0xFF
extern const AT_COMMAND_PART atStates[];

// Possible Bluetooth Roles
#define BT_ROLE_SLAVE_INDEX 0
#define BT_ROLE_MASTER_INDEX 1
extern const AT_COMMAND_PART btRoles[];

void init_bluetooth(void);

uint8_t is_state_response(BUFFER_OBJ *buffer);

uint8_t extract_state(BUFFER_OBJ *buffer);

uint8_t get_bt_state(bool atModeOnExit);

uint8_t get_bt_state(bool atModeOnExit);

void reset_bluetooth(void);

void wait_for_connection(void);

#endif

