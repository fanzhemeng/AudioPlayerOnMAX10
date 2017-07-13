/* Lab 2 for ECE224
 * Group 6
 * Richard Shi and Fanzhe Meng
 */

/*=========================================================================*/
/*  Includes                                                               */
/*=========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <system.h>
#include <sys/alt_alarm.h>
#include <io.h>

#include "ff.h"
#include "monitor.h"
#include "uart.h"

#include "audioplayer.h"

/*=========================================================================*/
/*  Globals                                                                */
/*=========================================================================*/

PlayerState pstate = NOT_PLAYING;
ButtonState bstate = NONE;
bool stateChanged = false;

/*=========================================================================*/
/*  Functions                                                              */
/*=========================================================================*/

static void buttonPressed(void* context, alt_u32 id) {
	//xprintf("Testing ISR\n");
	int i;

	// Delay read due to button oscillation
	for(i = 0; i < 100; i++);
	unsigned int n = IORD(BUTTON_PIO_BASE, 0);

    // identify which button is pressed
	switch (n) {
        case 0b1110: //0xe - Forward button
            bstate = FORWARD;
            break;
        case 0b1101: //0xd - Play Button
        	bstate = PLAY_PAUSE;
            break;
        case 0b1011: //0xb - Stop Button
        	bstate = STOP;
            break;
        case 0b0111: //0x7 - Reverse Button
        	bstate = BACKWARD;
            break;
        case 0b1111: //0xf - No button pressed
        	bstate = NONE;
        	break;
        default:
        	// Do nothing if invalid button combination
        	break;
	}

    stateChanged = true;     			// indicate the state has changed
	IOWR(BUTTON_PIO_BASE, 3, 0x0);     	// clear button ISR
}

//Clear previous interrupts if any and register ISR
static void setupInterrupts(void){
	IOWR(BUTTON_PIO_BASE, 3, 0x0);
	alt_irq_register(BUTTON_PIO_IRQ, (void*) 0, buttonPressed);
	IOWR(BUTTON_PIO_BASE, 2, 0xFF);
}

int main(int argc, char* argv[]){
	SongFile *currentSong;

	// Setup menu
	initializeDisk();
	loadDirectory("");

	// Enable buttons
	setupInterrupts();

	// Main loop
	// Controls main program flow
	while(true){
		currentSong = menu();
		playSong(currentSong->filename, currentSong->size);
	}
}
