#include "MKL25Z4.h"
#include "motors.h"
#include "uart.h"
#include "led.h"
#include "Buzzer.h"
#include "RTE_Components.h"
#include "cmsis_os2.h"
#include CMSIS_device_header

volatile uint8_t rx_interruptData;
volatile myDataPacket uartData = {1,0} ;

//message queue 
osMessageQueueId_t redMessage, greenMessage, motorMessage, buzzerMessage, controlMessage;

void UART2_IRQHandler() {
	
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_interruptData = UART2->D;
		uartData.data = UART2->D;
	}

}

volatile unsigned int counter=0;


#define MSG_COUNT 1
 //type struct for message data structure

//typedef struct
// { 
//	 uint8_t cmd;
//	 uint8_t data;
// }myDataPacket;

enum color {red,blue,green};

#define RED_LED			18  //portb pin 18
#define GREEN_LED		19	//portb pin 19
#define BLUE_LED 		1		//port d pin 1
#define SW_POS			6
#define MASK(x) 		(1<< (x))




static void delay(volatile uint32_t nof) {
	while(nof!=0) {
		__asm("NOP");
		nof--;
	}
}



//motor control should be in a message
//led requirements
//when moving in any direction then the green ledmust be running 
//the rear red led must be flashing continously at 250ms
//when robot is stationary then all the green led must be on
//the rear led must be flashing at a 500ms
//buzzer requirements 
	//the robot continous song tune from the start of the challenge till the end 
	//once the challenge run is finished then play another tune to end

/*----------------------------------------------------------------------------
 * Application led_red thread
 *---------------------------------------------------------------------------*/

void redLedThread() {
	myDataPacket myRxData;
  for (;;) {
		osMessageQueueGet(redMessage, &myRxData, NULL, osWaitForever);
		if(myRxData.cmd == 1) {
				//stopped state 
				if(myRxData.data == 0) {
					toggleRedLED();
					osDelay(250);
				}
				//end state
				else if(myRxData.data == 10) {
					toggleRedLED(0);
				}
				else {
					toggleRedLED();
					osDelay(500);
				}
		} 
	
	}
}

/*----------------------------------------------------------------------------
 * Application green led thread
 *---------------------------------------------------------------------------*/

void greenLedThread() {
		myDataPacket myRxData;
  for (;;) {
		osMessageQueueGet(greenMessage, &myRxData, NULL, osWaitForever);
		if(myRxData.cmd == 1) {
				//stopped state 
				if(myRxData.data == 0) {
					setGreenLED(1);
				}
				//end state
				else if(myRxData.data == 10) {
					setGreenLED(0);
				}
				else {
					shiftGreenLED();
					osDelay(500);
				}
		} 
	
	}
}

/*----------------------------------------------------------------------------
 * Application motor thread
 *---------------------------------------------------------------------------*/

void motorThread() {
		myDataPacket myRxData;
  for (;;) {
		osMessageQueueGet(motorMessage, &myRxData, NULL, osWaitForever);
		if(myRxData.cmd == 1) {
				move(myRxData.data);
		} 
	
	}
}

//continously play tune from start to end
//unique tone once challenge has neded
void buzzerThread() {
	myDataPacket myRxData;
	for(;;) {
		osMessageQueueGet(buzzerMessage, &myRxData,NULL,osWaitForever);
		if(myRxData.cmd == 1) {
			if(myRxData.data == 10) //stopped state 
			{
				play_song(score2Size, score_2, notes);
			}
			else {
				play_song(score1Size,score,notes);
			}
		}
		}
}

void controlThread() {

	for(;;) {
		//osMessageQueueGet (controlMessage, &uartData, NULL, osWaitForever);
		osMessageQueuePut (motorMessage, &uartData, NULL, 0);
		osMessageQueuePut (redMessage, &uartData, NULL, 0);
		osMessageQueuePut (greenMessage, &uartData, NULL, 0);
		osMessageQueuePut (buzzerMessage, &uartData,NULL,0);
	}
}




int main(void) {
	
	InitLED();
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 4);   
	
	uint8_t rx_data ;
	//= 0x69
	SystemCoreClockUpdate();
	initUART2(BAUD_RATE);
	initMotorPWM();
	initPWM();
	
	osKernelInitialize();
	osThreadNew (greenLedThread,NULL,NULL);
	osThreadNew (redLedThread,NULL,NULL);
	osThreadNew (motorThread,NULL,NULL);
	osThreadNew (controlThread,NULL,NULL);
	osThreadNew (buzzerThread,NULL,NULL);
	
	
	redMessage = osMessageQueueNew(MSG_COUNT,sizeof(myDataPacket), NULL);
	greenMessage = osMessageQueueNew(MSG_COUNT,sizeof(myDataPacket), NULL);
	motorMessage = osMessageQueueNew(MSG_COUNT,sizeof(myDataPacket), NULL);
	buzzerMessage = osMessageQueueNew(MSG_COUNT,sizeof(myDataPacket), NULL);
	controlMessage = osMessageQueueNew(MSG_COUNT,sizeof(myDataPacket), NULL);
	
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

