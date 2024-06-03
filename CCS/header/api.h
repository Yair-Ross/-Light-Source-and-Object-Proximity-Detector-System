#ifndef _api_H_
#define _api_H_

#include  "../header/hal.h"     // private library - HAL layer

extern void counter(unsigned int x, int count_mode);
extern void ldr(void);
unsigned int read_operand(char* src, unsigned int* src_index);
extern void sweep(unsigned int angle1, unsigned int angle2, unsigned char sweep_mode);
extern void telemeter(unsigned int input, const unsigned int scan_mode);
extern void run_file(const unsigned int filenum);
extern void rra(unsigned char x);
extern void set_delay(const unsigned int input);
extern void sleep(void);


#endif
