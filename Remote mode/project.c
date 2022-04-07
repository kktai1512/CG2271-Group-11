#include "MKL25Z4.h"
#include "motors.h"
#include "uart.h"
#include "led.h"
#include "Buzzer.h"
#include "RTE_Components.h"
#include "cmsis_os2.h"
#include "ultrasonic.h"
osSemaphoreId_t selfDriveSem, selfDriveSquare, motorSem;
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
void moveRefactored(uint8_t rx_data) {
	int isMovingTemp = 1;
	switch(rx_data) {
		case 0b0000:
			stop();
			isMovingTemp = 0;
			break;
		case 0b0001:
			right(90);
			break;
		case 0b0010:
			left(90);
			break;
		case 0b0011:
			forward(50);
			break;
		case 0b0100:
			backward(50);
			break;
		case 0b101:
			//swerve
			left(100);
			rearForward(40);
			break;
		case 0b110:
			//swerve
			right(100);
			rearForward(40);
			break;	
		
		case 0b1000:
			move(0b1000);
		
			
	}
	isMoving = isMovingTemp;
}
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

  void controlThread() {
      for(;;) {  
        if (uartData.data == 7) {
            isEnd = 1;
        }
				//if self-drive
        if (uartData.data == 8) {
            isSelfDrive = 1;
            osSemaphoreRelease(selfDriveSem);
        }
				osMessageQueuePut(motorMessage, &uartData, NULL, osWaitForever);
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

int volatile stop_flag = 0;
int volatile counter = 0;
void ultrasonicThread() {
	for(;;){
				osSemaphoreAcquire(selfDriveSem, osWaitForever);
        //only once
//			if (stop_flag == 1) {
//					left(90);
//					delay(800);
//					stop();
//				
//					forward(50);
//					osDelay(100);
//					stop();
//				
//					right(90);
//					osDelay(1500);
//					stop();
//				
//					stop_flag = 0;
//				counter++;
//					
//			}
//			if (stop_flag == 0) {
//			if (counter < 5){
				if (stop_flag == 0) {
					pulse();
					if (distance >= 30 && distance <= 210) {
							uartData.data = 0;
							osDelay(2000);
							stop_flag = 1;
							uartData.data = 9;
							
					}
					
					 else if (distance >= 210) { //move forward
							uartData.data = 0b1111;
							osSemaphoreRelease(selfDriveSem);
							
					}
				} 
				
					 
//				if(stop_flag == 1 && counter < 5) {
//						stop();
//						move(0b011);
//						osDelay(20000);
//						counter++;
//						stop();
//						stop_flag = 0;
//				}
				
        osDelay(0x150);}
    
}

//void selfDriveThread() {
//	for(;;){
//				osSemaphoreAcquire(selfDriveSquare, osWaitForever);
//				//curve
//				
//				left(80);
//				osDelay(1000);
//				stop();
//		
//				forward(50);
//				osDelay(800);
//				stop();
//		
//				right(80);
//				osDelay(2000);
//				stop();
//				
//				counter ++;
//				stop_flag = 0;
//	}
//}


  int main(void) {
	
	InitLED();
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 4);   
	
	uint8_t rx_data ;

	initUART2(BAUD_RATE);
	initMotorPWM();
	initPWM();
	initTimer();
		initUltrasonic();
	
	osKernelInitialize();
	osThreadNew (controlThread,NULL,NULL);
	osThreadNew (greenLedThread,NULL,NULL);
	osThreadNew (redLedThread,NULL,NULL);
	osThreadNew (motorThread,NULL,NULL);
	osThreadNew (buzzerThread,NULL,NULL);
  osThreadNew (ultrasonicThread, NULL, NULL);
	//osThreadNew (selfDriveThread, NULL, NULL);
	
	
	motorMessage = osMessageQueueNew(1,sizeof(myDataPacket), NULL);
	selfDriveSem = osSemaphoreNew(1, 0, NULL);
	selfDriveSquare = osSemaphoreNew(1, 0, NULL);
	motorSem = osSemaphoreNew(1, 0, NULL);
	
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