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

#include "fatfs.h"
#include "diskio.h"

#include "ff.h"
#include "monitor.h"
#include "uart.h"

#include "alt_types.h"

#include <altera_up_avalon_audio.h>
#include <altera_up_avalon_audio_and_video_config.h>

/*=========================================================================*/
/*  Constants                                                              */
/*=========================================================================*/
#define MAX_SONGS 20
#define MAX_SONGNAME_SIZE 20
#define DRIVE 0

/*=========================================================================*/
/*  Typedefs                                                               */
/*=========================================================================*/
typedef struct{
	char filename[MAX_SONGNAME_SIZE];
	unsigned long size;
} SongFile;

typedef enum {
	PLAYING,
    NOT_PLAYING
} PlayerState;

typedef enum {
    PLAY_PAUSE,
    STOP,
    BACKWARD,
    FORWARD
} Transtion;

int stateChanged = 0;

/*=========================================================================*/
/*  Globals                                                                */
/*=========================================================================*/
SongFile songs[MAX_SONGS];
PlayerState pstate;
Transition transition;
int currentTrack;
int songCount = 0;

/*=========================================================================*/
/*  Functions                                                              */
/*=========================================================================*/

static void putRc(FRESULT rc) {
    const char *str =
        "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0";
    FRESULT i;

    for (i = 0; i != rc && *str; i++) {
        while (*str++);
    }
    xprintf("rc=%u FR_%s\n", (uint32_t) rc, str);
}

int printTrack(int trackNumber, char *filename) {
	// open the lcd
	FILE *lcd = fopen("/dev/lcd_display", "w");
	if (lcd == NULL)
		return 1; // error opening the lcd

	//xprintf("%d - %s\n", trackNumber, filename);
	fprintf(lcd, "%d - %s", trackNumber, filename);
	fclose(lcd);
}

void init(){
    // Ensure the current track is the first one
	currentTrack = 0;
	printTrack(currentTrack, songs[currentTrack].filename);
	pstate = NOT_PLAYING;
}

void nextSong(){
	if(currentTrack < songCount - 1){
		currentTrack += 1;
	} else {
		currentTrack = 0;
	}

	printTrack(currentTrack, songs[currentTrack].filename);
	pstate = NOT_PLAYING;
}

void prevSong(){
	if(currentTrack > 0){
		currentTrack -= 1;
	} else {
		currentTrack = songCount - 1;
	}

	printTrack(currentTrack, songs[currentTrack].filename);
	pstate = NOT_PLAYING;
}

static void buttonISR(void* context, alt_u32 id) {
	//xprintf("Testing ISR\n");
	unsigned int n = IORD(BUTTON_PIO_BASE, 0);
    // identify which button is pressed
	switch (n) {
        case 0xe:
            transition = FORWARD;
            // TODO: how to recognize button being held
            break;
        case 0xd:
            transition = PLAY_PAUSE;
            break;
        case 0xb:
            transition = STOP;
            break;
        case 0x7:
            transition = BACKWARD;
            // TODO: how to recognize button being held
            break;
	}
    // indicate the state has changed
    stateChanged = 1;
    // clear button ISR
	IOWR(BUTTON_PIO_BASE, 3, 0x0);
}



// Checks a filename if it is a vaild .wav filename
int isWav(const char *filename){
  const char *suffix = ".wav"; // Length of 4
  const size_t suffixlen = 4;

  if(!filename){
    return 0; // filename is null
  }

  size_t namelen = strlen(filename);
  return strncasecmp(filename + namelen - suffixlen, suffix, suffixlen) == 0;
}

// Load all songs in directory into player
int loadSongs(char *dirName){
	FILINFO fileInfo;
	DIR dir;
	int result;

	if(result = f_opendir(&dir, dirName)){
		putRc(result);
		return result;
	}

	while(songCount < MAX_SONGS){
		result = f_readdir(&dir, &fileInfo);
		if ((result != FR_OK) || !fileInfo.fname[0]){
			break;
		}
		// If subdirectory, skip it
		if (fileInfo.fattrib & AM_DIR) {
			continue;
		}

		if(isWav(fileInfo.fname)){
			memcpy(songs[songCount].filename, fileInfo.fname, MAX_SONGNAME_SIZE);
			songs[songCount].size = fileInfo.fsize;
			//xprintf("%dth Track: %s  with size: %ul\n", i, songs[i].filename, songs[i].size);
			songCount += 1;
		}
	}
	return 0;
}

int loadSongsInCurrentDir(){
	return loadSongs("");
}

int main() {
	// Initializations
	FATFS disk;
	FIL songFile;

	// "di 0" -- 'disk initialize' and mounts the drive to the ‘0’ location
	xprintf("rc=%d\n", (uint16_t) disk_initialize((uint8_t) DRIVE));
    // "fi 0" --  'force initialization' of drive ‘0’ location
	putRc(f_mount((uint8_t) DRIVE, &disk));
    // load songs in current direcotry
	putRc(loadSongsInCurrentDir());

	//Clear previous interrupts if any and register ISR
	IOWR(BUTTON_PIO_BASE, 3, 0x0);
	alt_irq_register(BUTTON_PIO_IRQ, (void*) 0, buttonISR);
	IOWR(BUTTON_PIO_BASE, 2, 0xFF);
    
    init();
    // TODO: need to f_open the file
    
	// Main run loop
	while(1) {
        while (!stateChanged) {
            if (pstate == NOT_PLAYING) continue;
            // TODO: code for playing song
        }
        
		switch(transition){
		case PLAY_PAUSE:
            if (pstate == PLAYING) pstate = NOT_PLAYING;
            else pstate = PLAYING;
			break;
		case STOP:
            // TODO: move internal pointer in file to very beginning
            
            pstate = NOT_PLAYING;
			break;
		case BACKWARD:
            if (pstate == NOT_PLAYING) { 
                prevSong();
                // TODO: need to f_open the file before playing
            }
            else {} // pstate == PLAYING
			break;
		case FORWARD:
            if (pstate == NOT_PLAYING) { 
                nextSong();
                // TODO: need to f_open the file before playing
            }
            else {} // pstate == PLAYING
			break;
		}
        
        stateChanged = 0;
	}
}

