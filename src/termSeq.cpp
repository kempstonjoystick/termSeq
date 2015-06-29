//============================================================================
// Name        : terminal_sequencer.cpp - Name: Foundry, Carthage, Raccoon
// Author      : Tim Shearer
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "termSeq.h"
#include "audioTrack.h"
#include "eventListener.h"

#include <iostream>
#include <csignal>
#include <linux/types.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <sys/time.h>
#include <iostream>
#include <pthread.h>

using namespace std;

#define MUS_PATH "/usr/share/hydrogen/data/drumkits/TR808909/909_clap.wav"
#define MUS_PATH2 "/usr/share/hydrogen/data/drumkits/TR808909/808_sd.wav"
#define MUS_PATH3 "/usr/share/hydrogen/data/drumkits/TR808909/909_bd.wav"

enum messageHandlerError {
	MSGHDLR_NOERR=0,
	MSGHDLR_TOOFEWARGS,
	MSGHDLR_INVALIDARG1,
	MSGHDLR_INVALIDARG2,
	MSGHDLR_INVALIDSUBCOMMAND
};
//TODO const char array which describes each failure above.


//This class is intended to be threadsafe, so calls from message_handler and the main thread
//shouldnt cause any problems.
AudioTrack *track[8];

int message_handler(int argc, char **argv) {
	char * pEnd;

	//check 1 - argument count
	if(argc < 2)
		return MSGHDLR_TOOFEWARGS;

	//validate first command - should be track and track number
	if(strcmp(argv[0], "track") != 0)
		return MSGHDLR_INVALIDARG1;

	//validate the track index
	int trackIdx = strtol(argv[1], &pEnd, 10);
	if( (trackIdx <=0) || (trackIdx > MAX_TRACKS) )
		return MSGHDLR_INVALIDARG2;

	//Use the track index, call a next level validation:
	if (track[trackIdx-1]->validateCommand(argc-2, &argv[2]) < 0)
		return MSGHDLR_INVALIDSUBCOMMAND;

	//call the track method to process command
	track[trackIdx-1]->runCommand(argc-2, &argv[2]);

	return MSGHDLR_TOOFEWARGS;
}

int main() {

	EventListener myEvents;
	myEvents.messageCallback = message_handler;

	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 1024;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}

	for (int i=0; i<MAX_TRACKS; i++) {
		track[i] = new AudioTrack();
		track[i]->setTrack(i);
		track[i]->setLength(4, 96);
	}

//	Mix_Chunk *sound1 = NULL;
	char *file = MUS_PATH;
	track[0]->setLoops(48);
	track[0]->setSound(file);
	track[0]->start();
	/*if(sound1 == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}
	*/

	char *file2 = MUS_PATH2;
	track[1]->setLoops(24);
	track[1]->setSound(file2);
	track[1]->start();


	char *file3 = MUS_PATH3;
		track[1]->setLoops(24);
		track[1]->setSound(file3);
		track[1]->start();


//	Mix_Chunk *sound2 = NULL;
	/*
	sound2 = Mix_LoadWAV(MUS_PATH2);
	if(sound2 == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}
 	 */

	TRACE(("thread launch done...\n"));


	//experimental setting of beat data
	track[0]->setBeat(1);
	track[0]->setBeat(3);

	track[2]->setBeat(1);
	//track[2]->setBeat(3);

	track[1]->setBeat(0);
	track[1]->setBeat(1);
	track[1]->setBeat(2);
	track[1]->setBeat(3);

	myEvents.start();

	//need to wait for ready signal
	usleep(5000);

	struct timeval start_time;
    struct timeval end_time;

	//for( int i = 0 ; i < 1 ; i++) {
	for( int i = 0 ; i < 4 ; i++) {
		for(int j = 0 ; j < 96*4 ; j++) {
			gettimeofday(&start_time, NULL);
//			printf("main thread waking others\n");
			track[0]->trigger(j);
			track[1]->trigger(j);
			track[2]->trigger(j);
			SDL_Delay(5);
			 gettimeofday(&end_time, NULL);

		//	    float duration = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1E-6;

		//	    cout << "duration: " << duration << "s" << endl;
		}
	}



	for (int i=0; i<MAX_TRACKS; i++) {
		track[i]->detach();
		track[i]->join();
		delete track[i];
	}

    SDL_Quit();


	return 0;
}
