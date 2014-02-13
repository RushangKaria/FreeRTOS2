/*
 * (C) Copyright 2004 Texas Instruments
 *
 * Based on the following file:
 *  linux/lib/vsprintf.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 */


#include "FreeRTOS.h"
#include <stdarg.h>
#include <bbconsole.h>
#include <ns16550.h>
#include <semphr.h>

#define FCRVAL (FCR_FIFO_EN | FCR_RXSR | FCR_TXSR)
#define LCRVAL LCR_8N1	
#define MSG_SIZE 50
#define QUEUE_SIZE configLOG_BUFFER_QUEUE_SIZE
int HANDLE=1;

char *BUFFER[QUEUE_SIZE];
int FIRST=0;
int LAST=0;
extern NS16550_t console; 
int VALID=1;
//xSemaphore x;
//char abc[200];
/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */


static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */

#define do_div(n,base) ({ \
int __res; \
__res = ((unsigned long) n) % (unsigned) base; \
n = ((unsigned long) n) / (unsigned) base; \
__res; })

static char * number(char * str, long num, int base, int size, int precision, int type)
{

	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (type & LEFT)
		type &= ~ZEROPAD;

	if (base < 2 || base > 36)
		return 0;

	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;

	if (type & SIGN)
	{
		if (num < 0)
		{
			sign = '-';
			num = -num;
			size--;
		}
		else if (type & PLUS)
		{
			sign = '+';
			size--;
		}
		else if (type & SPACE)
		{
			sign = ' ';
			size--;
		}
	}

	if (type & SPECIAL)
	{
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}

	i = 0;
	if (num == 0)
	{
		tmp[i++]='0';
	}
	else
	{
		while (num != 0)
			tmp[i++] = digits[do_div(num,base)];
	}

	if (i > precision)
		precision = i;

	size -= precision;

	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			*str++ = ' ';

	if (sign)
		*str++ = sign;

	if (type & SPECIAL)
	{
		if (base==8)
			*str++ = '0';
		else if (base==16)
		{
			*str++ = '0';
			*str++ = digits[33];
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			*str++ = c;

	while (i < precision--)
		*str++ = '0';

	while (i-- > 0)
		*str++ = tmp[i];

	while (size-- > 0)
		*str++ = ' ';

    return str;
}


static int vsprintk_1(char *buf, const char *fmt, va_list args)
{
	int len;
	unsigned long num;
	int i, base;
	char * str;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
			qualifier = *fmt;
			++fmt;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = ' ';
			*str++ = (unsigned char) va_arg(args, int);
			while (--field_width > 0)
				*str++ = ' ';
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					*str++ = ' ';
			for (i = 0; i < len; ++i)
				*str++ = *s++;
			while (len < field_width--)
				*str++ = ' ';
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;


		case 'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		case '%':
			*str++ = '%';
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			*str++ = '%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN)
			num = va_arg(args, int);
		else
			num = va_arg(args, unsigned int);
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = '\0';
	return str-buf;
}

void printk (const char *fmt, ...)
{
int i;

if(HANDLE==1)		//activate the UART atleast once
{
NS16550_init(console,CFG_NS16550_CLK / 16 / CONFIG_BAUDRATE);
	NS16550_disable(console);
(*(REG32(MPU_INTC + INTCPS_CONTROL))) = 0x1;	//clear the interrupt flag
HANDLE=0;
}


int LOG_LEVEL=*(fmt+0);	//get the log level of the message

	if(LOG_LEVEL>*(configLOG_LEVEL+0))	//log level checking condition...if not satisfied immediately return
	return;

	if(FIRST==LAST)
	{
	FIRST=0;
	LAST=0;
	}

	if(LAST>=QUEUE_SIZE)	//ring buffer logic
	{
	LAST=0;
	}
	char printer[100];
	va_list args;
	
	*fmt++;	//point to String (reqd since string is of type LOG+string) Therefore add 1 to index to get start of the original string
	va_start (args, fmt);
	i = vsprintk_1 (printer, fmt, args);
	BUFFER[LAST++]=printer;
	va_end (args);

	if(VALID==1)		//enable the interrupts again since the line was printed... now is the time to print the new line
	NS16550_enable(console);

}

 

void vUART_ISR_Handler(void) // This is the Interrupt Service Routine
{ 
		
int i,j;
VALID=0;
	
	if(*(BUFFER[FIRST])!='\0')
	{	
		if(*(BUFFER[FIRST])!='\n')
		{
		console->thr=*(BUFFER[FIRST])++;	//this disables interrupts
		console->ier=0x02;			//since we are not done printing re-enable them
		}
		else	//to print new line logic
		{
		console->thr='\r';
		console->ier=0x02;	//writing to thr disables the interrupts
		console->thr='\n';
		*(BUFFER[FIRST])++;		
		console->ier=0x02;
		}

	}
	else
	{
	FIRST++;
	NS16550_disable(console);	//disable interrupts once a line is printed
 	(*(REG32(MPU_INTC + INTCPS_CONTROL))) = 0x1;	//signal line low
	VALID=1;	//signal that a line was printed and the next line can be printed...cannot do it here since it will corrupt the ring
	}
	


}
  


