#include "MKL25Z4.h"
#include "motors.h"
#include "uart.h"
#include "led.h"
#include "Buzzer.h"
#include "RTE_Components.h"
#include "cmsis_os2.h"
#include "ultrasonic.h"

#include CMSIS_device_header

/*----------------------------------------------------------------------------
 * UARTDATA
 *---------------------------------------------------------------------------*/
volatile uint8_t rx_interruptData;
volatile myDataPacket uartData = {0,0} ;
void UART2_IRQHandler() {
	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_interruptData = UART2->D;
		uartData.data = UART2->D;
	}
}

int isMoving = 0;
int isEnd = 0;
int isSelfDrive = 0;
/*----------------------------------------------------------------------------
 * Application led_red thread
 *---------------------------------------------------------------------------*/

void redLedThread() {
  for (;;) {
        if(isMoving) {
            toggleRedLED();
            osDelay(500);
				}
        else {
            toggleRedLED();
            osDelay(250);
        }
	}
    }


/*----------------------------------------------------------------------------
 * Application green_led thread
 *---------------------------------------------------------------------------*/

void greenLedThread() {
  for (;;) {
        if(isMoving){
            shiftGreenLED();
            osDelay(500);
				}
        else {
            setGreenLED(1);
        }
	}
}

/*----------------------------------------------------------------------------
 * Application buzzer thread
 *---------------------------------------------------------------------------*/
void buzzerThread() {
	int note_number = 0;
	int stopped = 0;
	for(;;) {
        if(isEnd) //stopped state 
        {	
            if (stopped == 0) {
                note_number = 0;
                stopped = 1;
            }    
            play_note(note_number, score2Size,score_2, notes);
            note_number ++;
        }
        else {
            play_note(note_number, score1Size,score, notes);
            note_number ++;
        }
		}
}


/*----------------------------------------------------------------------------
 * Application motor thread
 *---------------------------------------------------------------------------*/
osMessageQueueId_t motorMessage;
void motorThread() {
	myDataPacket myMotorRx;
	
  for (;;) {
		osMessageQueueGet(motorMessage, &myMotorRx, NULL, osWaitForever);
		//only stop state and end state is stop
		if (myMotorRx.data == 0 | myMotorRx.data == 7) {
				isMoving = 0;
    }
		else {
				isMoving = 1;
    }
		move(myMotorRx.data);
  }
}
/*----------------------------------------------------------------------------
 * Application control thread
 *---------------------------------------------------------------------------*/
osSemaphoreId_t selfDriveSem, selfDriveSquare;
  void controlThread() {
      for(;;) {  
        myDataPacket myControlData;
        myControlData.data = uartData.data;
        myControlData.cmd = uartData.cmd;
        if (uartData.data == 7) {
            isEnd = 1;
        }
				//if self-drive
        if (uartData.data == 8) {
            isSelfDrive = 1;
            osSemaphoreRelease(selfDriveSem);
        }
				//if not selfdriving, motor gets data from control thread
				if (uartData.data != 8) {
						osMessageQueuePut(motorMessage, &myControlData, NULL, osWaitForever);
				}
      }
  }

/*----------------------------------------------------------------------------
 * Ultrasonic thread
 *---------------------------------------------------------------------------*/
	
volatile float distance = 1000.0;
volatile int active = 0;
void TPM2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	
	if (TPM2_C0SC & TPM_CnSC_CHF_MASK) {
		TPM2_C0SC |= TPM_CnSC_CHF(1); // If CHF = channel is 1 when an event occur- cleared by writing 1 
		if (active) {
			active = 0;
			float multiplier = (SPEED_OF_SOUND * PRESCALER)/48000000;
			distance = ((TPM2_C0V) * multiplier)/2;
			resetTimer();
		}
		else {
			active = 1;
			resetTimer();
		}
	}
	else if (TPM2->SC & TPM_SC_TOF_MASK) {
		TPM2->SC |= TPM_SC_TOF(1); // Checking for overflow so that it resets when hit and counter = 1 cycle.
		if (active) {
			active = 0;
			distance = 1000;
			resetTimer();
		}
	}
}

int stop_flag = 0;
void ultrasonicThread() {
	for(;;){
				osSemaphoreAcquire(selfDriveSem, osWaitForever);
				stop_flag = 999;
        //only once
        if (stop_flag != 1){
        pulse();
        if (distance >= 30 && distance <= 250) {
           stop_flag = 1;
            osSemaphoreRelease(selfDriveSquare);
		}
        osDelay(0x150);
        }
}
	}

void selfDriveThread() {
    for(;;) {
				myDataPacket selfDriveRx;
				osSemaphoreAcquire(selfDriveSquare, osWaitForever);
				selfDriveRx.data = 3;
        osMessageQueuePut(motorMessage, &selfDriveRx, NULL, osWaitForever);
    }
}

  int main(void) {
	
	InitLED();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 4);   
	
	uint8_t rx_data ;

	initUART2(BAUD_RATE);
	initMotorPWM();
	initPWM();
	
	osKernelInitialize();
	osThreadNew (controlThread,NULL,NULL);
	osThreadNew (greenLedThread,NULL,NULL);
	osThreadNew (redLedThread,NULL,NULL);
	osThreadNew (motorThread,NULL,NULL);
	osThreadNew (buzzerThread,NULL,NULL);
  osThreadNew (ultrasonicThread, NULL, NULL);
	osThreadNew (selfDriveThread, NULL, NULL);
	
	
	motorMessage = osMessageQueueNew(1,sizeof(myDataPacket), NULL);
	
	selfDriveSem = osSemaphoreNew(1, 0, NULL);
	selfDriveSquare = osSemaphoreNew(1, 0, NULL);
	
	osKernelStart();
	for(;;) {
		
	}
	
	/*
	while(1) {
		rx_data = UART2_Receive_Poll();
		move(rx_data);
		delay(0x80000);
	}
	*/
}