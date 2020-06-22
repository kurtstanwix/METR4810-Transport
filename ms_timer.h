#ifndef MS_TIMER_H
#define	MS_TIMER_H

#include <xc.h>
#include "util.h"

// Millisecond value type
typedef uint32_t ms_time_t;
// Microsecond value type
typedef uint32_t us_time_t;

#define MS_TIMER_TMR 1
// The timer's interrupt priority register bits
#define MS_TIMER_TxIP PREPROCESSOR_STITCH(_T, PREPROCESSOR_STITCH(MS_TIMER_TMR, IP))

void MS_TIMER_Initialize(void);
void MS_TIMER_Delay_ms(uint16_t ms);
ms_time_t MS_TIMER_get_time_ms(void);
uint16_t MS_TIMER_get_TMR(void);
us_time_t MS_TIMER_get_time_us(void);

#endif	// MS_TIMER_H

