/* Lab 2 for ECE224
 * Group 6
 * Richard Shi and Fanzhe Meng
 *
 * Audio Player Module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "alt_types.h"
#include <altera_up_avalon_audio.h>
#include <altera_up_avalon_audio_and_video_config.h>

#include "fatfs.h"
#include "diskio.h"

#include "audioplayer.h"

uint8_t Buff[8192] __attribute__ ((aligned(4)));  /* Working buffer */


//TODO:
FIL songFile;
FRESULT result;


alt_up_audio_dev *audio_dev;
uint32_t bytesToRead, bytesBeenRead;
long totalBytesToRead;
uint8_t l_buf[2], r_buf[2];


// Song player
int setupPlayer(){

}

int playSong(char *songFile, unsigned long size){

}

int exitPlayer(){

}

// Loop for Audio Player
void play(void){
	xprintf("TODO: PLAY SONG");
}

