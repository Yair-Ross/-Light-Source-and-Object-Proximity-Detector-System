#ifndef _hal_H_
#define _hal_H_

#include <stdlib.h>
#include "../header/bsp.h"
#include  "../header/app.h"    		// private library - APP layer


extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern enum RXmode RX_mode; // global variable
extern enum TA1mode TA1_mode; // global variable

extern void sysConfig(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
extern void TA0_Start();
extern void TA0_Stop();
extern void TA1_Start();
extern void TA1_Stop();
extern void EnableADC();
extern void DisableADC();
extern void EnableLDR();
extern void DisableLDR();
extern void UART_puts(const char* str);

extern void ServoAngle(const unsigned int angle);
extern long USonicTrigger();
extern unsigned int LDRSample(const unsigned int ldr_num, const unsigned int samples_power);
extern void Flash_WriteFile();
extern void Flash_LoadFiles();
extern void Flash_SendFilesToUI();
extern void Flash_WriteLDR();
extern void Flash_LoadLDR();


extern __interrupt void PBs_handler(void);
extern __interrupt void T1_handler(void);
extern __interrupt void T3_handler(void);
extern __interrupt void T4_handler(void);
extern __interrupt void USCI0RX_ISR(void);
extern __interrupt void USCI0TX_ISR(void);

#endif







