#ifndef _app_H_
#define _app_H_

typedef struct file_struct {
    unsigned int filecount;
    char* filename[Flash_MAX_Files];
    char* filePtr[Flash_MAX_Files];
    unsigned int filesize[Flash_MAX_Files];
} file_struct;

enum FSMstate{state1,state2,state3,state4, state5, state6, state7, state8, state9}; // global variable
enum SYSmode{mode0,mode1,mode2,mode3,mode4}; // global variable
enum RXmode{RX_command, RX_data, RX_none};
enum TA1mode{TA1_Servo, TA1_USonicTrig, TA1_USonicEcho};
enum sweepMode{Sweep_Objects, Sweep_Lights, Sweep_Both};
extern char state_flag;
extern char state_flag2;
extern char UART_data[UART_RX_MAX_DATA+1];
extern unsigned int UART_index;
extern unsigned int TA0_ISRCOUNT, TA0_MAXISR;
extern const char* UART_str;
extern unsigned int UART_str_i;
extern unsigned int USonicState;
extern unsigned long USonicMeas;
extern unsigned int LDRMeas[2][LDRArrSize];
extern unsigned int LDR_Cal_index;
extern char uart_msg[50];
extern file_struct files;
extern unsigned int ServoDelay;
extern unsigned int TelemeterAngle;
extern unsigned int RunningScript;
extern unsigned int UploadingScript;


#endif







