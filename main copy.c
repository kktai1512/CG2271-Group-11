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
volatile myDataPacket uartData = {1,0} ;
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

  void controlThread() {
      for(;;) {  
        if (uartData.data == 7) {
            isEnd = 1;
        }
				//if self-drive
        if (uartData.data == 8) {
            isSelfDrive = 1;
        }
				//if not selfdriving, motor gets data from control thread
				if (uartData.data != 8) {
						osMessageQueuePut(motorMessage, &uartData, NULL, osWaitForever);
				}
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
	
	motorMessage = osMessageQueueNew(1,sizeof(myDataPacket), NULL);
	
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