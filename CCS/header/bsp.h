#ifndef _bsp_H_
#define _bsp_H_

//---------------------------------------------------------------------
#if defined (__MSP430G2553__)
#include  "../header/bsp_msp430x2xx.h"    // private library - BSP layer

#else
#error "Unsupported MCU"

#endif
//---------------------------------------------------------------------

#endif
