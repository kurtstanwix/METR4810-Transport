#include "mcc_generated_files/clock.h"
#define FCY _XTAL_FREQ
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "libpic30.h"
#include <stdbool.h>
#include "at_control.h"
#include "uart_manager.h"
#include "util.h"
#include "comms.h"
#include "hardware_PWM.h"


#include "mcc_generated_files/interrupt_manager.h"
#include "ms_timer.h"

#ifdef __USER_DEBUG
#define __USER_CONTROL
#endif

int main(void) {
    // initialize the device
    SYSTEM_Initialize();
    
    // Init motor to zero
    Hardware_PWM_Enable();
    set_speed(0);
    Hardware_PWM_Start(HARDWARE_PWM1_OC_NUM);
    Hardware_PWM_Start(HARDWARE_PWM2_OC_NUM);
    
    init_debug();

    // Put in comms mode
    AT_PIN_Clear();
    
    // Delay so a quick turn on and off won't reset Bluetooth
    MS_TIMER_Delay_ms(4000);

    init_comms();

    BUFFER_OBJ rcvBuffer;
    init_buffer(&rcvBuffer, PACKET_LENGTH - 1); // Length of packet without seq number

    while (1) {
        process_comms();
        if (comms_get_packet(&rcvBuffer)) {
            process_packet(&rcvBuffer);
        }
    }
}