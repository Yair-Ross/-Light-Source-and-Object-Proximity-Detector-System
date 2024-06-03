/*
 * Written by Arad Meyouhas and Yair Ross.
 * Based on Hanan Ribo's basecode.
 */

#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
enum RXmode RX_mode;
enum TA1mode TA1_mode;

/*
 * Connections:
 * P1.0 -> LDR1
 * P1.1-P1.2 -> Reserved for TX/RX
 * P1.3 -> LDR2
 * P1.4-P1.7 -> LCD Data D4-D7
 * P2.0 -> PB0
 * P2.2 -> Ultrasonic Trig
 * P2.3 -> LCD Control Bit E
 * P2.4 -> AMP3
 * AMP3 -> Servo PWM
 * P2.5 -> Ultrasonic Echo
 * P2.6-P2.7 -> LCD Control Bits RS/RW
 * Total: 9 Jumpers + 5 Jumpers from sensors
 */

void main(void){
  state = STATE_SLEEP;  // start in idle state on RESET
  lpm_mode = mode0;
  RX_mode = RX_command;
  TA1_mode = TA1_Servo;
  sysConfig();

  while(1){
	switch(state){
	  case STATE_SLEEP: // Sleep
	    sleep();
		break;

	  case STATE_OBJSWEEP: // Object Detector System
	      sweep(0, 180, Sweep_Objects);
	      break;

	  case STATE_TELEMETER: // Telemeter
	      telemeter(0, 0);
	      break;

	  case STATE_LDRSWEEP: // Light Sources Detector System
	      sweep(0, 180, Sweep_Lights);
	      break;

	  case STATE_BOTHSWEEP: // Object / Light Detector System
	      sweep(0, 180, Sweep_Both);
	      break;

	  case STATE_SCRIPT: // Script Mode
	      run_file(RunningScript);
	      break;


	  case STATE_LDR_CAL: // LDR Calibration
	      ldr();
	      break;

	  default:
        state = STATE_SLEEP;
        break;
	}
  }
}
