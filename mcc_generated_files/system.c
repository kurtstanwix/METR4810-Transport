#include "pin_manager.h"
#include "clock.h"
#include "system.h"
#include "interrupt_manager.h"
#include "../uart_manager.h"
#include "../hardware_PWM.h"
//#include "../software_PWM.h"
#include "../util.h"
//#include "../two_timers_PWM.h"
//#include "../one_timer_PWM.h"
#include "traps.h"
#include "../ms_timer.h"

void SYSTEM_Initialize(void) {
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART_Initialise();
    Hardware_PWM_Initialise();
    MS_TIMER_Initialize();
#ifdef __USER_DEBUG
    init_debug();
#endif
}