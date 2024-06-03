#include "../header/bsp.h"

//-----------------------------------------------------------------------------
//           Calibration configuration
//-----------------------------------------------------------------------------
void CALconfig(void){
    // Clock calibration - 1MHz
    DCOCTL = 0;
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
}

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
  
  // PushButtons Setup
  PBsArrPortSel &= ~PBs;
  PBsArrPortDir &= ~PBs;
  PBsArrIntEdgeSel &= ~(PB0);        // pull-up mode
  PBsArrIntEn |= (PBs);
  PBsArrIntPend &= ~PBs;            // clear pending interrupts

  // Servo Setup
  ServoArrPortSel |= ServoPWMLeg;
  ServoArrPortDir |= ServoPWMLeg;
  ServoArrPort &= ~ServoPWMLeg;

  // Ultrasonic Setup
  USonicArrPortSel |= (USonicTrigLeg+USonicEchoLeg);
  USonicArrPortDir |= USonicTrigLeg;
  USonicArrPortDir &= ~USonicEchoLeg;

  // LDR Setup
  LDRArrPortSel |= LDRmask;
  LDRArrPortDir &= ~LDRmask;

  _BIS_SR(GIE);                     // enable interrupts globally
}                             
//------------------------------------------------------------------------------------- 
//            Timers configuration
//-------------------------------------------------------------------------------------
void TIMER0config(void){
    TA0Cap0Control |= CCIE;
    TA0Cap0Count = TA0CCR0VAL;
    TA0Control = (TA0SourceSel+TA0ClockDiv);
}

void TIMER1config(void){
    TA1Control = TA1SourceSel + TA1ClockDiv;
    TA1Cap0Control = CCIE;
    TA1Cap2Control = OUTMOD_7;
}
//------------------------------------------------------------------------------------- 
//            ADC configuration
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADCControl0 = ADC10SHT_2 + ADC10ON + SREF_0;
	ADCControl1 = ADC10SSEL_3;
}

void LCDconfig(void){
    LCD_EN_SEL(0);
    LCD_RS_SEL(0);
    LCD_RW_SEL(0);
    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
}


void USonicconfig(void){
    TA1Cap1Control = OUTMOD_7 + CCIE;
}
  
void UARTconfig(void){
    UARTArrPortSel = UARTmask;                     // P1.1 = RXD, P1.2=TXD
    UARTArrPortSel2 = UARTmask;                     // P1.1 = RXD, P1.2=TXD
    UARTArrPortDir |= UARTDirmask;
    UARTArrPort &= UARTmask;
    UARTControl1 |= UARTClockSource;                     // CLK = SMCLK
    UARTBaudControl0 = UARTBaudRate0;                           //
    UARTBaudControl1 = UARTBaudRate1;                           //
    UARTModControl = UARTModSelect;               //
    UARTControl1 &= ~UARTResetBit;                     // **Initialize USCI state machine**
    UARTIntEn |= UARTRXIE;                          // Enable USCI_A0 RX interrupt
}
