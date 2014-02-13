/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS books - available as PDF or paperback  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public 
    License and the FreeRTOS license exception along with FreeRTOS; if not it 
    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 * To check the operation of the memory allocator the check task also 
 * dynamically creates a task before delaying, and deletes it again when it 
 * wakes.  If memory cannot be allocated for the new task the call to xTaskCreate
 * will fail and an error is signalled.  The dynamically created task itself
 * allocates and frees memory just to give the allocator a bit more exercise.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <bbconsole.h>

/*-----------------------------------------------------------*/

/* Priorities for the demo application tasks. */
#define mainLED_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainCOM_TEST_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 4 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 0 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )

/* The rate at which the on board LED will toggle when there is/is not an 
error. */
#define mainNO_ERROR_FLASH_PERIOD	( ( portTickType ) 3000 / portTICK_RATE_MS  )
#define mainERROR_FLASH_PERIOD		( ( portTickType ) 500 / portTICK_RATE_MS  )
#define mainON_BOARD_LED_BIT		PIN21
/* Constants used by the vMemCheckTask() task. */
#define mainCOUNT_INITIAL_VALUE		( ( unsigned long ) 0 )
#define mainNO_TASK					( 0 )

/* The size of the memory blocks allocated by the vMemCheckTask() task. */
#define mainMEM_CHECK_SIZE_1		( ( size_t ) 51 )
#define mainMEM_CHECK_SIZE_2		( ( size_t ) 52 )
#define mainMEM_CHECK_SIZE_3		( ( size_t ) 151 )

/* This is for Serial Task */
#define mainCOM_TEST_BAUD_RATE		( ( unsigned long ) 115200 )
#define mainCOM_TEST_LED			( 1 )
/*-------------------------------------------------------------------------------------------*/



  /*-------------------------------------------------------------------------------------------*/

/*
 * The Beagleboard has 2 LEDS available on the GPIO module
 * I will use LED0 to express errors on tasks
 */

//void prvToggleOnBoardLED( void );

/*
 * Configure the processor of use with the BeagleBoard
 * Currently I assume that U-boot has done all the work for us
 */
static void prvSetupHardware( void );

/*
 * Checks that all the demo application tasks are still executing without error
 * - as described at the top of the file.
 */
//static long prvCheckOtherTasksAreStillRunning( unsigned long ulMemCheckTaskCount );

/*
 * The task that executes at the highest priority and calls
 * prvCheckOtherTasksAreStillRunning(). See the description at the top
 * of the file
 */
//static void vErrorChecks ( void *pvParameters );

/*
 * Dynamically created and deleteted during each cycle of the vErrorChecks()
 * task. This is done to check the operation of the memory allocator.
 * See the top of vErrorChecks for more details
 */
//static void vMemCheckTask( void *pvParameters );

void HelloWorld1(void * pvParameters);
void HelloWorld2(void * pvParameters);
void FlashLED1(void * pvParameters);
void FlashLED2(void * pvParameters);
void Scheduler(void * pvParameters);
void real_time(void * pvParameters);
void dummy();
void critical(void *pvParameters);
void idle(void *pvParameters);


extern int getNextTime();
extern int getExpireTime();
extern void vTimerReset(xTaskHandle);
extern void vTimerStop(xTaskHandle);



	xTaskHandle handles[10] ={0};

int EXECUTED=0;
int MAX_DELAY=2000;
/*-----------------------------------------------------------*/

/*
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	xTaskHandle hdl1,hdl2,hdl3 = 0;
	int c = 'p';

	/* Setup the hardware for use with the Beableboard. */
	prvSetupHardware();


	memdump_32(0x4020ffc0, 64);

	printf("\nIn Main, please press a key\n");
	c = getc();
	printf("Your input %c\n", c);



int i;
	xTaskCreate(critical, ( signed char * ) "CRITICAL PRIORITY",256, (void *) NULL, 5, &hdl1);
	xTaskCreate(Scheduler, ( signed char * ) "Scheduler",256, (void *) NULL, 3, &hdl2);
	xTaskCreate(idle, ( signed char * ) "idle",256, (void *) NULL, 1, &hdl3);

	for(i=0;i<10;i++)
	xTaskCreateRT(real_time,(signed char *)i,256,i,2,(i+1)*1000,&handles[i]);	//create the real time tasks and schedule
	


	if(hdl2 == 0 || hdl1 ==0 || hdl3 == 0)	//i assume task creation always succeeds
	{
		printf("Task creation failed\n");
		return -1;
	}


	/* Now all the tasks have been stared - start the scheduler.
	 * NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	 * Te processor MUST be in supervisor mode when vTaskStartScheduler is called.
	 * The demo applications included in the the FreeRTOS.og download swith to supervisor
	 * mode prior to main being called. If you are not using one of these demo application
	 * projects then ensure Supervisor mode is used here */
	/* Should never reach here! */

	printf("Start FreeRTOS ...\n");

	vTaskStartScheduler();
	return 0;
}

void Scheduler(void *pvParameters)	//dont use printf/ printk in this function... extremely dangerous
{
static int count=0;
int x;
int task_id;
	while(1)
	{
		if(EXECUTED<10)	//scheduler logic...dont put printk here...dangling pointers issue will arise and system will halt...uses a lot of internal work
		{
		x=getNextTime();
				//printk(KERN_ALERT "Next timer expires in %d\n",getExpireTime());
				//printf("Next timer expires for task %d\n",x);

		if(x!=80)			//error code --- no active timers
		vTaskPrioritySet(handles[x],4);	//else schedule
		EXECUTED++;			//keep track of how many RT task scheduled
		}
		else
		{
			for(x=0;x<10;x++)
			vTaskSuspend(handles[x]);		//suspend the tasks so that idle tasks get some execution time

		vTaskDelay(MAX_DELAY);			//suspend till next time frame


			for(x=0;x<10;x++)		//wake up RT tasks and reset their timers
			{
			vTimerReset(handles[x]);
			vTaskResume(handles[x]);
			}
		EXECUTED=0;			//reset their status

		}

	}


}

void critical(void *pvParameters)
{
static int count=0;

	while(1)
	{
		printk(KERN_WARNING "This will not get printed\n");	//to demonstrate log levels
		printk(KERN_ALERT "CRITICAL TASK\n");
		vTaskDelay(1000);	//uncomment this to see how the tasks are being prempted when a critical task comes
	}
	vTaskSuspend(NULL);	//will never get executed...just placed to avoid RAM error
}

void idle(void *pvParameters)
{
	while(1)
	{
		printk(KERN_ALERT "IDLE TASK\n");
		vTaskDelay(900);
	}
}
void real_time(void * pvParameters)
{
int x=pvParameters;

int i;
	while(1)
	{
	i=0;
		//if(x<6)
		printk(KERN_ALERT "REAL TIME TASK %d\n",x);

		while(i++<1000000);	//do some computation that ends before deadline

		vTimerStop(NULL);
		vTaskPrioritySet(NULL,2);	//go back to the scheduler
	}
}

void dummy()	//call back function for timers...not really required...can be set to NULL with little modification in the timer module
{ 
static int i=1;	
	while(1)
	{
	//printf("Stopping time %d\n",i);
	//xTimerStop(timer_handles[i++],0);
	return;
	}
return ;
}



/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{

	/* Initialize GPIOs */
	/* GPIO5: 31,30,29,28,22,21,15,14,13,12
	 * GPIO6: 23,10,08,02,01 */
	(*(REG32(GPIO5_BASE+GPIO_OE))) = ~(PIN31|PIN30|PIN29|PIN28|PIN22|PIN21|PIN15|PIN14|PIN13|PIN12);
	(*(REG32(GPIO6_BASE+GPIO_OE))) = ~(PIN23|PIN10|PIN8|PIN2|PIN1);

	/* Switch off the leds */
	(*(REG32(GPIO5_BASE+GPIO_CLEARDATAOUT))) = PIN22|PIN21;
}


/*---------------------------------------------------------*/
