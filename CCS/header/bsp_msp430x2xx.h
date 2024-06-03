#ifndef _bsp_msp_H_
#define _bsp_msp_H_

#include  <msp430g2553.h>          // MSP430x2xx

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD
#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif


#define   debounceVal       5000
#define   LEDs_SHOW_RATE    0xFFFF  // 62_5ms

// States abstraction
#define STATE_OBJSWEEP  state1
#define STATE_TELEMETER state2
#define STATE_LDRSWEEP  state3
#define STATE_BOTHSWEEP state4
#define STATE_SCRIPT    state5
#define STATE_LDR_CAL   state6
#define STATE_SLEEP     state9

// LDR abstraction
#define LDRArrPort     P1IN
#define LDRArrPortSel  P1SEL
#define LDRArrPortDir  P1DIR
#define LDR1           0x01
#define LDR1Analog     0x01
#define LDR1ADCINCH    INCH_0
#define LDR2           0x08
#define LDR2Analog     0x08
#define LDR2ADCINCH    INCH_3
#define LDRmask        (LDR1+LDR2)
#define LDRAnalogmask  (LDR1Analog+LDR2Analog)
#define LDRSamples     10
#define LDRDelta       1
#define LDRArrSize     50


// PushButtons abstraction
#define PBsArrPort	        P2IN
#define PBsArrIntPend	    P2IFG
#define PBsArrIntEn	        P2IE
#define PBsArrIntEdgeSel    P2IES
#define PBsArrPortSel       P2SEL
#define PBsArrPortDir       P2DIR
#define PB0                 0x01
#define PBs                 PB0

// Timer_A0 abstraction
#define TA0Control        TA0CTL
#define TA0Counter        TA0R
#define TA0Cap0Control    TA0CCTL0
#define TA0Cap0Count      TA0CCR0
#define TA0Cap1Control    TA0CCTL1
#define TA0Cap1Count      TA0CCR1
#define TA0Cap2Control    TA0CCTL2
#define TA0Cap2Count      TA0CCR2
#define TA0IntVector      TA0IV
#define TA0SourceSel      TASSEL_2 // SMCLK
#define TA0ClockDiv       ID_3
#define TIMERA0_VECTOR1   TIMER0_A0_VECTOR
#define TIMERA0_VECTOR2   TIMER0_A1_VECTOR

#define TA0CCR0VAL         62900 // 0.5 sec
#define TA0MODECONTROL     MC_1 // Up to CCR0

// Timer_A1 abstraction
#define TA1Control        TA1CTL
#define TA1Counter        TA1R
#define TA1Cap0Control    TA1CCTL0
#define TA1Cap0Count      TA1CCR0
#define TA1Cap1Control    TA1CCTL1
#define TA1Cap1Count      TA1CCR1
#define TA1Cap2Control    TA1CCTL2
#define TA1Cap2Count      TA1CCR2
#define TA1IntVector      TA1IV
#define TA1SourceSel      TASSEL_2 // SMCLK
#define TA1ClockDiv       ID_0
#define TA1IntEnable      TAIE
#define TIMERA1_VECTOR1   TIMER1_A0_VECTOR
#define TIMERA1_VECTOR2   TIMER1_A1_VECTOR

#define TA1CCR0VAL         62900 // 0.5 sec
#define TA1MODECONTROL     MC_1 // Up to CCR0

// Servo Motor abstraction
#define ServoArrPort      P2OUT
#define ServoArrPortSel   P2SEL
#define ServoArrPortDir   P2DIR
#define ServoPWMLeg       0x10
#define ServoPWMCycles    25160
#define Servo0Angle       491520 // 480 in Q20.10
#define Servo180Angle     2264 // 2.5 / 25 * 2500
#define ServoDegreeDelta  1784 // Servo180Angle - Servo0Angle
#define ServoDegreeDelta2  317 // (ServoDegreeDelta / 180) in Q10.5
#define Servo9DegreeJump  101 // or 100
#define ServoDelayOneTime 40
#define ServoDelaySweep   4
#define ServoSweepDelta   3

// Ultrasonic Sensor abstraction
#define USonicTrigArrPort P2OUT
#define USonicEchoArrPort P2IN
#define USonicArrPortSel  P2SEL
#define USonicArrPortDir  P2DIR
#define USonicTrigLeg     0x04
#define USonicEchoLeg     0x20
#define USonicMeasPower   2

// Flash abstraction
#define FlashControl1     FCTL1
#define FlashControl2     FCTL2
#define FlashControl3     FCTL3
#define FlashWRKey        FWKEY
#define FlashRDKey        FRKEY
#define FlashClockSelect  FSSEL1
#define FlashClockDiv     FN0
#define FlashWRTBit       WRT
#define FlashLock         LOCK
#define FlashErase        ERASE
#define Flash_basePtr     0x1000
#define Flash_basePtrLDR  0xE800
#define Flash_MAX_Files   3


// ADC10 abstraction
#define ADCControl0       ADC10CTL0
#define ADCControl1       ADC10CTL1
#define ADCAE0            ADC10AE0
#define ADCMEM            ADC10MEM

// UART abstraction
#define UARTArrPort       P1OUT
#define UARTArrPortSel    P1SEL
#define UARTArrPortSel2   P1SEL2
#define UARTArrPortDir    P1DIR
#define UARTControl0      UCA0CTL0
#define UARTControl1      UCA0CTL1
#define UARTBaudControl0  UCA0BR0
#define UARTBaudControl1  UCA0BR1
#define UARTModControl    UCA0MCTL
#define UARTIntEn         IE2
#define UARTmask          BIT1+BIT2
#define UARTDirmask       BIT1+BIT2
#define UARTClockSource   UCSSEL_2
#define UARTBaudRate0     104
#define UARTBaudRate1     0
#define UARTModSelect     UCBRS0
#define UARTResetBit      UCSWRST
#define UARTRXIE          UCA0RXIE
#define UARTTXIE          UCA0TXIE
#define UART_RX_VECTOR    USCIAB0RX_VECTOR
#define UART_TX_VECTOR    USCIAB0TX_VECTOR
#define UARTRXBuffer      UCA0RXBUF
#define UARTTXBuffer      UCA0TXBUF
#define UARTArrIntPend    IFG2
#define UARTTXIntPend     UCA0TXIFG
#define UARTRXIntPend     UCA0RXIFG

#define UART_RX_MAX_DATA  64


// LCD abstraction
#define LCDArrPort        P1OUT
#define LCDArrPortSel     P1SEL
#define LCDArrPortDir     P1DIR
#define LCDControl        P2OUT
#define LCDControlSel     P2SEL
#define LCDControlDir     P2DIR
#define _LCD_EN           0x08
#define _LCD_RS           0x40
#define _LCD_RW           0x80
#define LCD_EN(a)   (!a ? (LCDControl &= ~_LCD_EN) : (LCDControl |= _LCD_EN)) // P1.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (LCDControlDir &= ~_LCD_EN) : (LCDControlDir |= _LCD_EN)) // P1.5 pin direction
#define LCD_EN_SEL(a)   (!a ? (LCDControlSel &= ~_LCD_EN) : (LCDControlSel |= _LCD_EN)) // P1.5 pin selection
#define LCD_RS(a)   (!a ? (LCDControl &= ~_LCD_RS) : (LCDControl |= _LCD_RS)) // P1.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (LCDControlDir &= ~_LCD_RS) : (LCDControlDir |= _LCD_RS)) // P1.6 pin direction
#define LCD_RS_SEL(a)   (!a ? (LCDControlSel &= ~_LCD_RS) : (LCDControlSel |= _LCD_RS)) // P1.6 pin selection
#define LCD_RW(a)   (!a ? (LCDControl &= ~_LCD_RW) : (LCDControl |= _LCD_RW)) // P1.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (LCDControlDir &= ~_LCD_RW) : (LCDControlDir |= _LCD_RW)) // P1.7 pin direction
#define LCD_RW_SEL(a)   (!a ? (LCDControlSel &= ~_LCD_RW) : (LCDControlSel |= _LCD_RW)) // P1.7 pin selection

#define LCD_DATA_OFFSET   0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset
#define LCD_DATA_WRITE    P1OUT
#define LCD_DATA_DIR      P1DIR
#define LCD_DATA_READ     P1IN
#define FOURBIT_MODE      0x0
#define EIGHTBIT_MODE     0x1
#define LCD_MODE          FOURBIT_MODE
#define OUTPUT_PIN        1
#define INPUT_PIN         0
#define OUTPUT_DATA       (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA        0x00
#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)           lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()             lcd_cmd(0x01) // clears all chars from the screen
#define lcd_putchar(x)          lcd_data(x)
#define lcd_goto(x)             lcd_cmd(0x80+(x))
#define lcd_goto2(x)            lcd_cmd(0xC0+(x))
#define lcd_cursor_right()      lcd_cmd(0x14) // moves cursor one step right
#define lcd_cursor_left()       lcd_cmd(0x10) // moves cursor one step left
#define lcd_display_shift()     lcd_cmd(0x1C)
#define lcd_resetcursor()       lcd_cmd(0x02) // cursor gets set to the beginning of the first line
#define lcd_resetcursor2()      lcd_cmd(0xC0) // cursor gets set to the beginning of the second line
#define cursor_off()            lcd_cmd(0x0C) // hides cursor from screen
#define cursor_on()             lcd_cmd(0x0F) // shows cursor on screen
#define lcd_function_set()      lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line()          lcd_cmd(0xC0)


extern void CALconfig(void);
extern void GPIOconfig(void);
extern void TIMER0config(void);
extern void TIMER1config(void);
extern void ADCconfig(void);
extern void LCDconfig(void);
extern void USonicconfig(void);
extern void UARTconfig(void);

#endif



