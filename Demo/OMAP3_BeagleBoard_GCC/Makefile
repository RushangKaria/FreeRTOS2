#/*
#    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.
#
#    ***************************************************************************
#    *                                                                         *
#    * If you are:                                                             *
#    *                                                                         *
#    *    + New to FreeRTOS,                                                   *
#    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
#    *    + Looking for basic training,                                        *
#    *    + Wanting to improve your FreeRTOS skills and productivity           *
#    *                                                                         *
#    * then take a look at the FreeRTOS books - available as PDF or paperback  *
#    *                                                                         *
#    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
#    *                  http://www.FreeRTOS.org/Documentation                  *
#    *                                                                         *
#    * A pdf reference manual is also available.  Both are usually delivered   *
#    * to your inbox within 20 minutes to two hours when purchased between 8am *
#    * and 8pm GMT (although please allow up to 24 hours in case of            *
#    * exceptional circumstances).  Thank you for your support!                *
#    *                                                                         *
#    ***************************************************************************
#
#    This file is part of the FreeRTOS distribution.
#
#    FreeRTOS is free software; you can redistribute it and/or modify it under
#    the terms of the GNU General Public License (version 2) as published by the
#    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
#    ***NOTE*** The exception to the GPL is included to allow you to distribute
#    a combined work that includes FreeRTOS without being obliged to provide the
#    source code for proprietary components outside of the FreeRTOS kernel.
#    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
#    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#    more details. You should have received a copy of the GNU General Public 
#    License and the FreeRTOS license exception along with FreeRTOS; if not it 
#    can be viewed here: http://www.freertos.org/a00114.html and also obtained 
#    by writing to Richard Barry, contact details for whom are available on the
#    FreeRTOS WEB site.
#
#    1 tab == 4 spaces!
#
#    http://www.FreeRTOS.org - Documentation, latest information, license and
#    contact details.
#
#    http://www.SafeRTOS.com - A version that is certified for use in safety
#    critical systems.
#
#    http://www.OpenRTOS.com - Commercial support, development, porting,
#    licensing and training services.
#*/

USE_THUMB_MODE = NO
# use 1 or 0 only
DEBUG = -ggdb3
OPTIM = -O0
RUN_MODE = RUN_FROM_RAM
LDSCRIPT = omap3-ram.ld
TEXT_BASE = 0x80300000

# use this to print debug messages for task interleaving
DBGMSG = NO

CROSS_COMPILE=arm-none-eabi-

CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
ARCH = $(CROSS_COMPILE)ar
CRT0 = boot.s
WARNINGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wbad-function-cast -Wcast-align -Wsign-compare \
		   -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wunused

#
# CFLAGS common to both the THUMB and ARM mode builds

CFLAGS = $(WARNINGS) -D DEBUG -D $(RUN_MODE) -D GCC_OMAP3_BeagleBoard -I. -I../../Source/include \
		-I../Console/include -I../Common/include $(DEBUG) -mcpu=cortex-a8 -T$(LDSCRIPT) -Ttext $(TEXT_BASE) \
	     $(OPTIM) -fomit-frame-pointer -fno-strict-aliasing

ifeq ($(USE_THUMB_MODE),YES)
	CFLAGS += -mthumb-interwork -D THUMB_INTERWORK
	THUMB_FLAGS=-mthumb
endif

LINKER_FLAGS = -Xlinker -o$(TARGET).elf -Xlinker -M -Xlinker -Map=$(TARGET).map

RTOS_SOURCE_DIR =  ../../Source
DEMO_SOURCE_DIR =  ../Common/Minimal
CONSOLE_SRC_DIR =  ../Console
SAMPLE_TASKS_DIR = ../SampleTasks

#
# Source files that can be built to THUMB mode.
#

THUMB_SRC = \
main.c \
$(CONSOLE_SRC_DIR)/printk.c \
$(RTOS_SOURCE_DIR)/timers.c \
$(RTOS_SOURCE_DIR)/tasks.c \
$(RTOS_SOURCE_DIR)/queue.c \
$(RTOS_SOURCE_DIR)/list.c \
$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c \
$(RTOS_SOURCE_DIR)/portable/GCC/OMAP3_BeagleBoard/port.c \
$(CONSOLE_SRC_DIR)/comport.c \
$(CONSOLE_SRC_DIR)/ns16550.c \
$(CONSOLE_SRC_DIR)/printf.c \
$(SAMPLE_TASKS_DIR)/SampleTasks.c

#
# Source files that must be built to ARM mode.
#
ARM_SRC = $(RTOS_SOURCE_DIR)/portable/GCC/OMAP3_BeagleBoard/portISR.c \


#
# Define all object files.
#
ARM_OBJ = $(ARM_SRC:.c=.o)
THUMB_OBJ = $(THUMB_SRC:.c=.o)

ifeq ($(USE_THUMB_MODE),YES)
	TARGET=rtosdemo-t
else
	TARGET=rtosdemo
endif

$(TARGET).bin : $(TARGET).elf
	$(OBJCOPY) $(TARGET).elf -O binary $(TARGET).bin

$(TARGET).hex : $(TARGET).elf
	$(OBJCOPY) $(TARGET).elf -O ihex $(TARGET).hex

$(TARGET).elf : $(ARM_OBJ) $(THUMB_OBJ) $(CRT0) Makefile
	$(CC) $(CFLAGS) $(ARM_OBJ) $(THUMB_OBJ) -nostartfiles $(CRT0) $(LINKER_FLAGS)
	$(OBJDUMP) -d $@ > $@.disasm

$(THUMB_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) $< -o $@
	$(CC) -c $(THUMB_FLAGS) $(CFLAGS) -S $< -o $@.S

$(ARM_OBJ) : %.o : %.c $(LDSCRIPT) Makefile
	$(CC) -c $(CFLAGS) $< -o $@
	$(CC) -c $(CFLAGS) -S $< -o $@.S

clean :
	rm *.bin *.elf *.map *.o *.o.S
	rm ../Console/*.o
	rm $(RTOS_SOURCE_DIR)/*.o
	rm $(RTOS_SOURCE_DIR)/*.S
	rm $(RTOS_SOURCE_DIR)/portable/GCC/OMAP3_BeagleBoard/*.S
	rm $(RTOS_SOURCE_DIR)/portable/GCC/OMAP3_BeagleBoard/*.o	
	rm $(SAMPLE_TASKS_DIR)/*.o
	rm $(SAMPLE_TASKS_DIR)/*.o.S
	rm $(RTOS_SOURCE_DIR)/portable/MemMang/*.o
	rm $(RTOS_SOURCE_DIR)/portable/MemMang/*.S
	
