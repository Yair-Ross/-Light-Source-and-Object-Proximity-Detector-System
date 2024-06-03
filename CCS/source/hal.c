#include  "../header/hal.h"     // private library - HAL layer

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){
    CALconfig();
	GPIOconfig();
	TIMER0config();
	TIMER1config();
	USonicconfig();
	ADCconfig();
	LCDconfig();
	lcd_init();
	UARTconfig();
	Flash_LoadLDR();
	Flash_LoadFiles();
}
//---------------------------------------------------------------------
//            Polling based Delay function
//---------------------------------------------------------------------
void delay(unsigned int t){  // t[msec]
	volatile unsigned int i;
	
	for(i=t; i>0; i--);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            Enable ADC
//---------------------------------------------------------------------
void EnableADC(){
    ADCControl0 |= ENC + ADC10SC;
}

//---------------------------------------------------------------------
//            Disable ADC
//---------------------------------------------------------------------
void DisableADC(){
    ADCControl0 &= ~ENC;
}

//---------------------------------------------------------------------
//            Enable LDR
//---------------------------------------------------------------------
void EnableLDR() {
    ADCAE0 |= LDRAnalogmask;
}

//---------------------------------------------------------------------
//            Disable LDR
//---------------------------------------------------------------------
void DisableLDR() {
    ADCAE0 &= ~LDRAnalogmask;
}
 
  //******************************************************************
  // send a command to the LCD
  //******************************************************************
  void lcd_cmd(unsigned char c){

      LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

      if (LCD_MODE == FOURBIT_MODE)
      {
          LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
                  LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
                  LCD_DATA_WRITE &= ~OUTPUT_DATA;
              LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
          lcd_strobe();
      }
      else
      {
          LCD_DATA_WRITE = c;
          lcd_strobe();
      }
  }
  //******************************************************************
  // send data to the LCD
  //******************************************************************
  void lcd_data(unsigned char c){

      LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

      LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_RS(1);
      if (LCD_MODE == FOURBIT_MODE)
      {
              LCD_DATA_WRITE &= ~OUTPUT_DATA;
                  LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
                  LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
                  LCD_DATA_WRITE &= ~OUTPUT_DATA;
          LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
          lcd_strobe();
      }
      else
      {
          LCD_DATA_WRITE = c;
          lcd_strobe();
      }

      LCD_RS(0);
  }
  //******************************************************************
  // write a string of chars to the LCD
  //******************************************************************
  void lcd_puts(const char * s){

      while(*s)
          lcd_data(*s++);
  }
  //******************************************************************
  // initialize the LCD
  //******************************************************************
  void lcd_init(){

      char init_value;

      if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
          else init_value = 0x3F;

      LCD_RS(0);
      LCD_EN(0);
      LCD_RW(0);

      DelayMs(15);
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_DATA_WRITE |= init_value;
      lcd_strobe();
      DelayMs(5);
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_DATA_WRITE |= init_value;
      lcd_strobe();
      DelayUs(200);
          LCD_DATA_WRITE &= ~OUTPUT_DATA;
      LCD_DATA_WRITE |= init_value;
      lcd_strobe();

      if (LCD_MODE == FOURBIT_MODE){
          LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
                  LCD_DATA_WRITE &= ~OUTPUT_DATA;
          LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
          lcd_strobe();
          lcd_cmd(0x28); // Function Set
      }
          else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

      lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
      lcd_cmd(0x1); //Display Clear
      lcd_cmd(0x6); //Entry Mode
      lcd_cmd(0x80); //Initialize DDRAM address to zero
      cursor_off();
  }
  //******************************************************************
  // Delay usec functions
  //******************************************************************
  void DelayUs(unsigned int cnt){

      unsigned char i;
          for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

  }
  //******************************************************************
  // Delay msec functions
  //******************************************************************
  void DelayMs(unsigned int cnt){

      unsigned char i;
          for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

  }
  //******************************************************************
  // lcd strobe functions
  //******************************************************************
  void lcd_strobe(){
    LCD_EN(1);
    asm(" nop");
    asm(" nop");
    LCD_EN(0);
  }

  //******************************************************************
  // Start TA0
  //******************************************************************
  void TA0_Start() {
      TA0_ISRCOUNT = 0;
      TA0Counter = 0;
      TA0Control |= TA0MODECONTROL;
  }

  //******************************************************************
  // Stop TA0
  //******************************************************************
  void TA0_Stop() {
      TA0Control &= ~TA0MODECONTROL;
  }

  //******************************************************************
  // Start TA1
  //******************************************************************
  void TA1_Start() {
      TA1Counter = 0;
      TA1Control |= TA1MODECONTROL;
  }

  //******************************************************************
  // Stop TA1
  //******************************************************************
  void TA1_Stop() {
      TA1Control &= ~TA1MODECONTROL;
      TA1Cap2Count = TA1Cap1Count = 0xFFFF;
  }

  void UART_puts(const char* str) {
      if(*str) {
          UART_str = str;
          UART_str_i = 0;
          UARTIntEn |= UARTTXIE;
      }
  }

  void ServoAngle(const unsigned int angle) {
      unsigned long ccr = angle % 181;
      ccr = (Servo0Angle + ((ccr << 5) * ServoDegreeDelta2)) >> 10;
      TA1_mode = TA1_Servo;
      TA1Cap0Control = CCIE;
      TA1Cap2Control = OUTMOD_7;
      TA1Cap0Count = ServoPWMCycles;
      TA1Cap2Count = ccr;
      TA1_Start();
      enterLPM(lpm_mode);
      TA1_Stop();
  }

  long USonicTrigger() {
      int i;
      long avg = 0;
      TA1_mode = TA1_USonicTrig;
      _delay_cycles(10000);
      TA1Cap2Control = CAP | CCIE | CCIS_1 | CM_3 | SCS;
      TA1Cap1Control = CCIE | OUTMOD_7;
      TA1Cap0Count = 60500;
      TA1Cap1Count = 11; // min 10
      TA1_Start();
      for(i=1 << USonicMeasPower; i; i--) {
          enterLPM(lpm_mode);
          avg += USonicMeas;
      }
      TA1_Stop();
      return avg >> USonicMeasPower;
  }

  unsigned int LDRSample(const unsigned int ldr_num, const unsigned int samples_power) {
      volatile unsigned int samples = 1 << LDRSamples;
      unsigned long avg = 0;
      switch((ldr_num - 1) & 1) {
          case 0:
              ADCControl1 = LDR1ADCINCH + ADC10SSEL_3;
              break;
          case 1:
              ADCControl1 = LDR2ADCINCH + ADC10SSEL_3;
              break;
      }
      for(; samples; samples--) {
          EnableADC();
          avg += ADCMEM;
      }
      DisableADC();
      return avg >> LDRSamples;
  }

  void Flash_WriteLDR() {
        unsigned int* flashPtr = Flash_basePtrLDR;
        volatile unsigned int i;
        FlashControl2 = FlashWRKey + FlashClockSelect + FlashClockDiv; // SMCLK/2
        FlashControl3 = FlashWRKey; // Clear LOCK
        FlashControl1 = FlashWRKey + FlashErase; // Enable erase
        *flashPtr = 0; // Dummy write to erase segment
        while(FlashControl3&BUSY);
        FlashControl1 = FlashWRKey + FlashWRTBit;

        for(i=0; i<LDRArrSize; i++) {
            *flashPtr++ = LDRMeas[0][i];
            *flashPtr++ = LDRMeas[1][i];
        }

        FlashControl1 = FlashWRKey;
        FlashControl3 = FlashWRKey + FlashLock;
    }

  void Flash_LoadLDR() {
      unsigned int* flashPtr = Flash_basePtrLDR;
      volatile unsigned int i;
      if(*flashPtr != 0xFFFF) {
          for(i=0; i<LDRArrSize; i++) {
              LDRMeas[0][i] = *flashPtr++;
              LDRMeas[1][i] = *flashPtr++;
          }
      }
  }

  char* Flash_GetFilePtr(const unsigned int filenum) {
      return (char*)Flash_basePtr + (0x40 * filenum);
  }

  void Flash_WriteFile(char* str) {
      char* flashPtr;
      volatile unsigned int i;
      if(RunningScript < Flash_MAX_Files) {
          flashPtr = Flash_GetFilePtr(RunningScript); // Segment size is 64 bytes (0x40h)
          FlashControl2 = FlashWRKey + FlashClockSelect + FlashClockDiv; // SMCLK/2
          FlashControl3 = FlashWRKey; // Clear LOCK
          FlashControl1 = FlashWRKey + FlashErase; // Enable erase
          *flashPtr = 0; // Dummy write to erase segment
          FlashControl1 = FlashWRKey + FlashWRTBit;

          while(*str != 0xFF) {
              *flashPtr++ = *str++;
          }
          *flashPtr = 0;

          FlashControl1 = FlashWRKey;
          FlashControl3 = FlashWRKey + FlashLock;
      }
      Flash_LoadFiles();
  }

  void Flash_LoadFiles() {
      unsigned int i, j;
      char* flashPtr;
      unsigned int filesize;
      files.filecount = 0;
      for(i=0; i<Flash_MAX_Files; i++) {
          flashPtr = Flash_GetFilePtr(i);
          filesize = 0;
          j = 0;
          if(flashPtr[j++] == ':') {
              filesize = flashPtr[j++];
          }
          if(flashPtr[j++] == ':' && flashPtr[j+filesize] == ':' && filesize > 0 && filesize <= 30) {
              files.filesize[i] = filesize;
              files.filePtr[i] = flashPtr + j;
              files.filename[i] = flashPtr + j + filesize + 1;
              files.filecount++;
          }
          else {
              files.filesize[i] = 0;
          }
      }
      Flash_SendFilesToUI();
  }

  void Flash_SendFilesToUI() {
      volatile unsigned int i,j;
      char* filename;
      j=0;
      uart_msg[j++] = '6';
      for(i=0; i<Flash_MAX_Files; i++) {
          if(files.filesize[i] > 0) {
              filename = files.filename[i];
              uart_msg[j++] = '.';
              uart_msg[j++] = (i+'0');
              uart_msg[j++] = '.';
              while(*filename) {
                  uart_msg[j++] = *filename++;
              }
          }
      }
      if(j > 2) {
          uart_msg[j++] = '\n';
          uart_msg[j] = 0;
          UART_puts(uart_msg);
      }
  }

  //*********************************************************************
  //            Port1 Interrupt Service Rotine
  //*********************************************************************
  #pragma vector=PORT2_VECTOR
    __interrupt void PBs_handler(void){

      delay(debounceVal);
  //---------------------------------------------------------------------
  //            selector of transition between states
  //---------------------------------------------------------------------
      if(PBsArrIntPend & PB0){
          if(state != STATE_LDR_CAL) {
              state = STATE_LDR_CAL;
              state_flag = state_flag2 = 0;
          }
          else {
              LDR_Cal_index += 5;
          }
          PBsArrIntPend &= ~PB0;
      }
  //---------------------------------------------------------------------
  //            Exit from a given LPM
  //---------------------------------------------------------------------
          LPM4_EXIT;

  }

  //*********************************************************************
  //            TIMERA0_VECTOR1 Interrupt Service Rotine
  //*********************************************************************
  #pragma vector=TIMERA0_VECTOR1
    __interrupt void T1_handler(void){
        //---------------------------------------------------------------------
        //            Exit from a given LPM
        //---------------------------------------------------------------------
        if(++TA0_ISRCOUNT >= TA0_MAXISR) {
            TA0_ISRCOUNT = 0;
            LPM4_EXIT;
        }
    }

    //*********************************************************************
        //            TIMERA1_VECTOR1 Interrupt Service Rotine
        //*********************************************************************
        #pragma vector=TIMERA1_VECTOR1
          __interrupt void T3_handler(void){
              static unsigned int ServoCount = 0;
              switch(TA1_mode) {
                  case TA1_Servo:
                      if(++ServoCount > ServoDelay) {
                          ServoCount = 0;
                          LPM4_EXIT;
                      }
                      break;
              }
          }

          //*********************************************************************
                  //            TIMERA1_VECTOR2 Interrupt Service Rotine
                  //*********************************************************************
                  #pragma vector=TIMERA1_VECTOR2
                    __interrupt void T4_handler(void){
                        static unsigned int USonicEchoResults[2];
                        static unsigned int USonicEchoIndex = 0;
                        switch(TA1IntVector) {
                            case 2: // CCR1
                                switch(TA1_mode) {
                                    case TA1_USonicTrig:
                                        TA1_mode = TA1_USonicEcho;
                                        break;
                                }
                                break;
                            case 4: // CCR2
                                if(TA1_mode == TA1_USonicEcho) {
                                    USonicEchoResults[USonicEchoIndex++] = TA1Cap2Count;
                                    if(USonicEchoIndex > 1) {
                                        USonicEchoIndex = 0;
                                        TA1_mode = TA1_USonicTrig;
                                        USonicMeas = USonicEchoResults[1] - USonicEchoResults[0];
                                        LPM4_EXIT;
                                    }
                                break;
                            }
                        }
                    }

  //*********************************************************************
  //            ADC10 Vector Interrupt Service Routine
  //*********************************************************************
  /*#pragma vector = ADC10_VECTOR
  __interrupt void ADC10_ISR (void)
  {
      __bic_SR_register_on_exit(CPUOFF);
  }*/

  //*********************************************************************
  //            UART RX Vector Interrupt Service Routine
  //*********************************************************************
    #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
    #pragma vector=UART_RX_VECTOR
    __interrupt void USCI0RX_ISR(void)
    #elif defined(__GNUC__)
    void __attribute__ ((interrupt(UART_RX_VECTOR))) USCI0RX_ISR (void)
    #else
    #error Compiler not supported!
    #endif
    {
      if(UARTArrIntPend & UARTRXIntPend) {
          char input = UARTRXBuffer;
          if(UART_index == UART_RX_MAX_DATA || (((!UploadingScript && input == '\n') || UploadingScript && input == 0xFF) && UART_index > 0)) {
              char bypass = 0;
              if(!UploadingScript)
                  UART_data[UART_index] = '\0';
              else
                  UART_data[UART_index] = 0xFF;
              UART_index = 0;
              if(RX_mode == RX_command && !UART_data[1]) {
                  switch(UART_data[0]) {
                      case '1':
                          state = STATE_OBJSWEEP;
                          break;
                      case '2':
                          state = STATE_TELEMETER;
                          bypass = 1;
                          TelemeterAngle = 181;
                          RX_mode = RX_data;
                          break;
                      case '3':
                          state = STATE_LDRSWEEP;
                          break;
                      case '4':
                          state = STATE_BOTHSWEEP;
                          break;
                      case '5':
                          state = STATE_SCRIPT;
                          bypass = 1;
                          RX_mode = RX_data;
                          break;
                      case '6':
                          bypass = 1;
                          Flash_SendFilesToUI();
                          break;
                      case '7':
                          bypass = 1;
                          UploadingScript = 1;
                          RX_mode = RX_data;
                          break;
                      case '9':
                          state = STATE_SLEEP;
                          break;
                  }
                  if(!bypass) {
                      state_flag = state_flag2 = 0;
                      __bic_SR_register_on_exit(CPUOFF);
                  }
              }
              else if(RX_mode == RX_data) {
                  RX_mode = RX_command;
                  if(UploadingScript) {
                      UploadingScript = 0;
                      RunningScript = UART_data[0] - '0';
                      Flash_WriteFile(UART_data+1);
                  }
                  else if(state == STATE_TELEMETER) {
                      state_flag = state_flag2 =  0;
                      TelemeterAngle = atoi(UART_data);
                  }
                  else if(state == STATE_SCRIPT) {
                      state_flag = state_flag2 = 0;
                      RunningScript = atoi(UART_data);
                  }
                  __bic_SR_register_on_exit(CPUOFF);
              }
          }
          else if((!UploadingScript && input != '\n') || (UploadingScript && input != 0xFF))
              UART_data[UART_index++] = input;
       }
    }

    //*********************************************************************
    //            UART TX Vector Interrupt Service Routine
    //*********************************************************************
      #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
      #pragma vector=UART_TX_VECTOR
      __interrupt void USCI0TX_ISR(void)
      #elif defined(__GNUC__)
      void __attribute__ ((interrupt(UART_TX_VECTOR))) USCI0TX_ISR (void)
      #else
      #error Compiler not supported!
      #endif
      {
          if(UART_str[UART_str_i])
              UARTTXBuffer = UART_str[UART_str_i++];
          else
              UARTIntEn &= ~UARTTXIE;
      }
