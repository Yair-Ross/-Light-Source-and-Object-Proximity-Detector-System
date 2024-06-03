#include  "../header/api.h"    		// private library - API layer
#include  "../header/hal.h"     // private library - HAL layer

char state_flag = 0;
char state_flag2 = 0;
unsigned int TA0_ISRCOUNT = 0;
unsigned int TA0_MAXISR = 0;
char UART_data[UART_RX_MAX_DATA+1] = {0};
unsigned int UART_index = 0;
const char* UART_str = 0;
unsigned int UART_str_i = 0;
char uart_msg[50] = {0};
unsigned long USonicMeas = 0;
unsigned int LDR_Cal_index = 0;
unsigned int LDRMeas[2][LDRArrSize];
file_struct files;
unsigned int ServoDelay = ServoDelayOneTime;
unsigned int TelemeterAngle = 999;
unsigned int RunningScript = 0;
unsigned int UploadingScript = 0;

void sleep(void) { // Sleep
    disable_interrupts();
    state_flag = 1;
    lcd_clear();
    lcd_puts("Sleeping...");
    lcd_resetcursor2();
    lcd_puts("(Use GUI)");
    enable_interrupts();
    enterLPM(lpm_mode);
}

void telemeter(unsigned int input, const unsigned int scan_mode) { // Telemeter
    state_flag = scan_mode ? 0 : 1;
    volatile unsigned int i;
    ServoDelay = ServoDelayOneTime;
    if(TelemeterAngle != 999) {
        input = TelemeterAngle;
        TelemeterAngle = 999;
    }
    ServoAngle(input);
    long distance;
    do {
        distance = USonicTrigger();
        i=0;
        uart_msg[i++] = '2';
        uart_msg[i++] = '.';
        ltoa(input, uart_msg+i, 16);
        i += input > 15 ? 2 : 1;
        uart_msg[i++] = '.';
        ltoa(distance, uart_msg+i, 16);
        for(i=4; i<10 && uart_msg[i]; i++);
        uart_msg[i++] = '\n';
        uart_msg[i] = 0;
        UART_puts(uart_msg);
    } while(state_flag);
}

void sweep(unsigned int angle1, unsigned int angle2, unsigned char sweep_mode) { // Object / Light Detector System
    if(sweep_mode == Sweep_Objects || sweep_mode == Sweep_Lights || sweep_mode == Sweep_Both)
        state_flag = 1;
    long distance;
    volatile unsigned int i;
    volatile unsigned long angle = 0;
    unsigned int LDR1Meas, LDR2Meas;
    volatile unsigned int LDR1Meas_index, LDR2Meas_index;
    unsigned int Distance1, Distance2;
    unsigned int LDR1Min, LDR1Max, LDR2Min, LDR2Max;
    const int add = ServoSweepDelta;
    unsigned char cmd;
    ServoDelay = ServoDelayOneTime;
    angle1 %= 181;
    angle2 %= 181;
    unsigned int max_angle = angle1 > angle2 ? angle1 : angle2;
    switch(sweep_mode) {
        case Sweep_Objects:
            cmd = '1';
            break;
        case Sweep_Lights:
            cmd = '3';
            break;
        case Sweep_Both:
            cmd = '4';
            break;
    }
    for(angle = (angle1 > angle2 ? angle2 : angle1); state_flag && angle<=max_angle; angle += add) {
        ServoAngle(angle);
        ServoDelay = ServoDelaySweep;
        i=0;
        uart_msg[i++] = cmd;
        uart_msg[i++] = '.';
        ltoa(angle, uart_msg+i, 16);
        i += angle > 15 ? 2 : 1;
        if(sweep_mode == Sweep_Objects || sweep_mode == Sweep_Both) {
            distance = USonicTrigger();
            uart_msg[i++] = '.';
            ltoa(distance, uart_msg+i, 16);
            for(i=4; i<10 && uart_msg[i]; i++);
        }

        if(sweep_mode == Sweep_Lights || sweep_mode == Sweep_Both) {
            LDR1Meas = LDRSample(1, LDRSamples);
            LDR2Meas = LDRSample(2, LDRSamples);
            for(LDR1Meas_index = 0; LDR1Meas_index < LDRArrSize && LDRMeas[0][LDR1Meas_index] < LDR1Meas; LDR1Meas_index++);
            for(LDR2Meas_index = 0; LDR2Meas_index < LDRArrSize && LDRMeas[1][LDR2Meas_index] < LDR2Meas; LDR2Meas_index++);
            if((LDR1Meas_index < LDRArrSize && LDR2Meas_index < LDRArrSize) && (LDR1Meas_index-LDR2Meas_index <= LDRDelta || LDR2Meas_index-LDR1Meas_index <= LDRDelta))
            {
                LDR1Min = LDR1Meas_index ? LDRMeas[0][LDR1Meas_index-1] : 0;
                LDR1Max = LDRMeas[0][LDR1Meas_index];
                LDR2Min = LDR2Meas_index ? LDRMeas[1][LDR2Meas_index-1] : 0;
                LDR2Max = LDRMeas[1][LDR2Meas_index];
                Distance1 = (LDR1Meas - LDR1Min) / (LDR1Min + LDR1Max) + LDR1Meas_index;
                Distance2 = (LDR2Meas - LDR2Min) / (LDR2Min + LDR2Max) + LDR2Meas_index;
            }
            else {
                Distance1 = Distance2 = 0;
            }

            uart_msg[i++] = '.';
            ltoa((Distance1+Distance2)>>1, uart_msg+i, 16);
            for(i=i; i<20 && uart_msg[i]; i++);
        }

        uart_msg[i++] = '\n';
        uart_msg[i] = 0;
        UART_puts(uart_msg);
    }
    if(state_flag && state != STATE_SCRIPT) {
        state = STATE_SLEEP;
        state_flag = 0;
    }
}

void run_file(const unsigned int filenum) { // Script Mode
    state_flag2 = 1;
    char* flashPtr;
    unsigned int i = 0;
    unsigned int opcode, operand1, operand2;
    if(filenum < Flash_MAX_Files && files.filesize[filenum] > 0) {
        flashPtr = files.filePtr[filenum];
        set_delay(50);
        while(state_flag2 && i < files.filesize[filenum]) {
            opcode = flashPtr[i++];
            switch(opcode) {
                case 1:
                    operand1 = read_operand(flashPtr, &i);
                    counter(operand1, 1);
                    break;
                case 2:
                    operand1 = read_operand(flashPtr, &i);
                    counter(operand1, -1);
                    break;
                case 3:
                    operand1 = read_operand(flashPtr, &i);
                    rra(operand1);
                    break;
                case 4:
                    operand1 = read_operand(flashPtr, &i);
                    set_delay(operand1);
                    break;
                case 5:
                    lcd_clear();
                    break;
                case 6:
                    operand1 = read_operand(flashPtr, &i);
                    telemeter(operand1, 1);
                    break;
                case 7:
                    operand1 = read_operand(flashPtr, &i);
                    operand2 = read_operand(flashPtr, &i);
                    sweep(operand1, operand2, Sweep_Objects);
                    break;
                case 8:
                    state = STATE_SLEEP;
                    break;
            }
            if(state == STATE_SLEEP)
                break;
        }
    }
    if(state_flag2) {
        state_flag2 = 0;
        state = STATE_SLEEP;
    }
}

unsigned int read_operand(char* src, unsigned int* src_index) {
    return src[(*src_index)++];
}

void counter(unsigned int x, int count_mode) { // Counter onto LCD
    state_flag = 1;
    volatile unsigned long counter, counter2;
    char str[6];
    counter = (count_mode >= 0 ? 0 : x);
    counter2 = x;
    count_mode = (count_mode >= 0 ? 1 : -1);

    disable_interrupts();
    TA0_Start();
    lcd_clear();

    do {
        ltoa(counter, str, 10);

        if(count_mode == -1 && (counter == 99 || counter == 9)) {
            lcd_clear();
        }
        disable_interrupts();
        lcd_resetcursor();
        lcd_puts(str);
        enable_interrupts();
        counter += count_mode;
        enterLPM(lpm_mode);
    } while(state_flag && counter2--);

    TA0_Stop();
    lcd_clear();
    enable_interrupts();
}

void rra(unsigned char x) { // RRA Key onto LCD
    volatile unsigned int i, j;
    disable_interrupts();
    TA0_Start();
    lcd_clear();
    enable_interrupts();
    j = 0;
    i = 16;
    do {
        lcd_data(x);
        enterLPM(lpm_mode);
        lcd_cursor_left();
        lcd_data(' ');
        if(!--i && !j) {
            i = 16;
            j = 1;
            lcd_resetcursor2();
        }
    } while(i);

    TA0_Stop();
    lcd_clear();
}

void set_delay(const unsigned int input) { // Set Delay - units of 10ms
    volatile unsigned int i;
    for(i=0; i<8; i++) {
        if((input >> i) <= 50) {
            TA0CCR0 = (TA0CCR0VAL  / 50) * (input >> i);
            TA0_MAXISR = (1 << i);
            break;
        }
    }
}

void ldr(void){ // LDR Calibration
    state_flag = 1;
    LDR_Cal_index = 4;
    volatile unsigned int i, i2;
    volatile unsigned int j;
    unsigned long Vout;
    volatile unsigned long Vavg1, Vavg2;
    const unsigned long Vmax = 10600; // 3.3 in Q10.5
    const char msg1[] = "0.00 / 0.00 [v]";
    const char msg2[] = "LDR CAL: 00 [cm]";
    char last_value1[4] = {0};
    char last_value2[4] = {0};
    char digit;
    unsigned int LDR_index = 0;
    unsigned int CAL_distance = 0;
    disable_interrupts();
    lcd_clear();
    lcd_puts(msg1);
    lcd_resetcursor2();
    lcd_puts(msg2);
    EnableLDR();
    while(state_flag){
        if(LDR_index != LDR_Cal_index) {
            if(LDR_index) {
                for(i=0; i<2; i++) {
                    unsigned int distanceDelta = LDRMeas[i][LDR_index];
                    if(LDR_index > 4) {
                        distanceDelta -= LDRMeas[i][LDR_index-5];
                        if(distanceDelta > LDRMeas[i][LDR_index] || distanceDelta < 5) {
                            LDR_Cal_index -= 5;
                            break;
                        }
                    }
                    distanceDelta /= 5;
                    for(j=LDR_index-4; j<LDR_index; j++) {
                        LDRMeas[i][j] = distanceDelta;
                        if(j)
                            LDRMeas[i][j] += LDRMeas[i][j-1];
                    }
                }
            }
            if((LDR_index = LDR_Cal_index) >= LDRArrSize)
                break;
        }
        Vavg1 = Vavg2 = 0;
        enable_interrupts();
        LDRMeas[0][LDR_index] = LDRSample(1, LDRSamples);
        LDRMeas[1][LDR_index] = LDRSample(2, LDRSamples);
        if(state_flag) {
            Vavg1 = LDRMeas[0][LDR_index] << 5;
            Vavg2 = LDRMeas[1][LDR_index] << 5;
            Vout = (Vmax * Vavg1) >> 20;
            i2 = 0;
            disable_interrupts();
            if(LDR_index + 1 != CAL_distance) {
                if(CAL_distance == 4) {

                }
                CAL_distance = LDR_index + 1;
                if(CAL_distance < 10) {
                    lcd_goto2(10);
                    lcd_data(CAL_distance + '0');
                }
                else {
                    char sCalDistance[3] = {0};
                    lcd_goto2(9);
                    ltoa((long)CAL_distance, sCalDistance, 10);
                    lcd_puts(sCalDistance);
                }
            }
            lcd_goto(0);
            for(i = 100; i >= 1; i /= 10) {
                digit = Vout / i + '0';
                Vout %= i;
                if(digit != last_value1[i2]) {
                    last_value1[i2] = digit;
                    lcd_data(digit);
                }
                else {
                    lcd_cursor_right();
                }
                if(i2++ == 0)
                    lcd_cursor_right();
            }

            Vout = (Vmax * Vavg2) >> 20;
            i2 = 0;
            lcd_goto(7);
            disable_interrupts();
            for(i = 100; i >= 1; i /= 10) {
                digit = Vout / i + '0';
                Vout %= i;
                if(digit != last_value2[i2]) {
                    last_value2[i2] = digit;
                    lcd_data(digit);
                }
                else {
                    lcd_cursor_right();
                }
                if(i2++ == 0)
                    lcd_cursor_right();
            }
        }
    }

    disable_interrupts();
    if(state_flag) {
        state_flag = 0;
        state = STATE_SLEEP;
    }
    DisableADC();
    DisableLDR();
    lcd_clear();
    Flash_WriteLDR();
    enable_interrupts();
}
