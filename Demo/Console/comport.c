/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <bbconsole.h>

#ifdef CFG_PRINTF
#ifdef CFG_NS16550_SERIAL

#include <ns16550.h>

#if CONFIG_CONS_INDEX == 1
extern NS16550_t console = (NS16550_t) CFG_NS16550_COM1;
#elif CONFIG_CONS_INDEX == 2
extern NS16550_t console = (NS16550_t) CFG_NS16550_COM2;
#elif CONFIG_CONS_INDEX == 3
extern NS16550_t console = (NS16550_t) CFG_NS16550_COM3;
#elif CONFIG_CONS_INDEX == 4
extern NS16550_t console = (NS16550_t) CFG_NS16550_COM4;
#else
#error no valid console defined
#endif

static int calc_divisor (void)
{
	return (CFG_NS16550_CLK / 16 / CONFIG_BAUDRATE);
}

int serial_init (void)
{
	int clock_divisor = calc_divisor();

	NS16550_init(console, clock_divisor);

	return (0);
}

void serial_putc(const char c)
{
	if (c == '\n')
		NS16550_putc(console, '\r');

	NS16550_putc(console, c);
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}


int serial_getc(void)
{
	return NS16550_getc(console);
}

int serial_tstc(void)
{
	return NS16550_tstc(console);
}

void serial_setbrg (void)
{
	int clock_divisor;

    clock_divisor = calc_divisor();
	NS16550_reinit(console, clock_divisor);
}

#endif
#endif
