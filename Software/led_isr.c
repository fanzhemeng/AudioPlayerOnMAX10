/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include "sys/alt_irq.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"

static void led_isr(void* context, alt_u32 id)
{
	  unsigned int buttons = IORD(BUTTON_PIO_BASE, 0);
	  buttons = buttons ^ 0xf;
	  unsigned int leds = 0xf & buttons;
	  IOWR(LED_PIO_BASE, 0, leds);

	  IOWR(BUTTON_PIO_BASE, 3, 0x0);
}

int background()
{
	int j;
	int x = 0;
	int grainsize = 4;
	int g_taskProcessed = 0;
	for(j = 0; j < grainsize; j++){
		g_taskProcessed++;
	}
	return x;
}

int main()
{
  printf("Hello from Nios II! I am using an isr\n");

  //Clear previous interrupts if any
  IOWR(BUTTON_PIO_BASE, 3, 0x0);

  // Register ISR
  alt_irq_register(BUTTON_PIO_IRQ, (void*)0, led_isr);

  // Enable ISRs
  IOWR(BUTTON_PIO_BASE, 2, 0xf);	//Enable all 4 buttons

  while (1) {
	  //Waiting
  }

  return 0;
}
