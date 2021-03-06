#ifndef TMR_H
#define	TMR_H

#include <xc.h>
#include "util.h"

// Defines to modify and read TMR registers, allowing for switching between timers
// by changing a single value

#define TMR_TMR_REGISTER(TMR_NUM) PREPROCESSOR_STITCH(TMR, TMR_NUM)
#define TMR_PR_REGISTER(TMR_NUM) PREPROCESSOR_STITCH(PR, TMR_NUM)
#define TMR_TxCON_REGISTER(TMR_NUM) PREPROCESSOR_STITCH(T, PREPROCESSOR_STITCH(TMR_NUM, CON))
#define TMR_TxCONbits(TMR_NUM) PREPROCESSOR_STITCH(TMR_TxCON_REGISTER(TMR_NUM), bits)
#define TMR_TxIF(TMR_NUM) PREPROCESSOR_STITCH(_T, PREPROCESSOR_STITCH(TMR_NUM, IF))
#define TMR_TxIE(TMR_NUM) PREPROCESSOR_STITCH(_T, PREPROCESSOR_STITCH(TMR_NUM, IE))
#define TMR_TxInterrupt(TMR_NUM) PREPROCESSOR_STITCH(_T, PREPROCESSOR_STITCH(TMR_NUM, Interrupt))
#define TMR_TxIP(TMR_NUM) PREPROCESSOR_STITCH(_T, PREPROCESSOR_STITCH(TMR_NUM, IP))

#endif	// TMR_H

