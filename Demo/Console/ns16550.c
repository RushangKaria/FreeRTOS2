/*
 * This file is licensed under
 * the terms of the GNU General Public License version 2.  This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 *  
 * COM1 NS16550 support
 * originally from linux source (arch/ppc/boot/ns16550.c)
 * modified to use CFG_ISA_MEM and new defines
 */
#include "FreeRTOS.h"
#include <bbconsole.h>

#ifdef CFG_PRINTF
#ifdef CFG_NS16550

#include <ns16550.h>
#include <semphr.h>

#define LCRVAL LCR_8N1					/* 8 data, 1 stop, no parity */
#define MCRVAL (MCR_DTR | MCR_RTS)			/* RTS/DTR */
#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)	/* Clear & enable FIFOs */


#define LONG_TIME 0xffff
#define TICKS_TO_WAIT    10

int handler = 0;
//xSemaphoreHandle xSemaphore = NULL;

void NS16550_init (NS16550_t com_port, int baud_divisor)
{
//printf("HERES");
 //vSemaphoreCreateBinary(xSemaphore);

	//com_port->ier = 0x00;
          com_port->ier = 0x00;
#ifdef CONFIG_OMAP
	com_port->mdr1 = 0x7;   /* mode select reset TL16C750*/
#endif
	com_port->lcr = LCR_BKSE | LCRVAL;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;
#if defined(CONFIG_OMAP)
	com_port->mdr1 = 0;	/* select uart mode */
#endif
}

void NS16550_reinit (NS16550_t com_port, int baud_divisor)
{
//printf("HERE");
	//com_port->ier = 0x00;
	com_port->ier = 0x02;
        com_port->lcr = LCR_BKSE;
	com_port->dll = baud_divisor & 0xff;
	com_port->dlm = (baud_divisor >> 8) & 0xff;
	com_port->lcr = LCRVAL;
	com_port->mcr = MCRVAL;
	com_port->fcr = FCRVAL;
}

void NS16550_enable(NS16550_t com_port)		//enable all the comments to run on qemu
{
	//printf("enable");
	com_port->ier = 0x02;
	//com_port->fcr = FCRVAL;
}
void NS16550_disable(NS16550_t com_port)
{
char c;
	//printf("disable");
	com_port->ier = 0x00;
	//c=com_port->fcr;	
	//com_port->fcr=0x00;

}
void NS16550_putc (NS16550_t com_port, char c)
{

	while((com_port->lsr & LSR_THRE) == 0);
	com_port->thr = c;
   

}

char NS16550_getc (NS16550_t com_port)
{
       
	while ((com_port->lsr & LSR_DR) == 0);
	
return (com_port->rbr);
}

int NS16550_tstc (NS16550_t com_port)
{
	return ((com_port->lsr & LSR_DR) != 0);
}

/* Timer ISR */
/*
void vTimerISR( void * pvParameters )
{
static unsigned portCHAR ucLocalTickCount = 0;
static signed portBASE_TYPE xHigherPriorityTaskWoken; 
 
    /* A timer tick has occurred. */
    
    
    
    /* Is it time for vATask() to run? */
  /*  xHigherPriorityTaskWoken = pdFALSE;
    ucLocalTickCount++;
    if( ucLocalTickCount >= TICKS_TO_WAIT )
    {
        /* Unblock the task by releasing the semaphore. */
    /*    xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );
        
        /* Reset the count so we release the semaphore again in 10 ticks time. */
      /*  ucLocalTickCount = 0;
    }
    
    /* If xHigherPriorityTaskWoken was set to true you
    we should yield.  The actual macro used here is 
    port specific. */
    /*portYIELD_FROM_ISR();
}
*/
/*
void getHandle(NS16550_t com_port){
  
  if(!((com_port->lsr & LSR_THRE) == 0)){
   handle = 1;
  }
}
*/
#endif
#endif
