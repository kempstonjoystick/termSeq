//============================================================================
// Name        : terminal_sequencer.cpp - Name: Foundry, Carthage, Raccoon
// Author      : Tim Shearer
// Version     :
// Copyright   : 
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "termSeq.h"
#include "audioTrack.h"
#include <iostream>
#include <linux/types.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
using namespace std;

#define MUS_PATH "/usr/share/hydrogen/data/drumkits/TR808909/909_clap.wav"
#define MUS_PATH2 "/usr/share/hydrogen/data/drumkits/TR808909/909_rim.wav"
/*
// prototype for our audio callback
// see the implementation for more informatio
void my_audio_callback(void *userdata, Uint8 *stream, int len);

// variable declarations
static Uint8 *audio_pos; // global pointer to the audio buffer to be played
static Uint32 audio_len; // remaining length of the sample we have to play

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {

	if (audio_len ==0)
		return;

	len = ( len > audio_len ? audio_len : len );
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}
*/

int main() {

	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	int audio_rate = 22050;
	Uint16 audio_format = AUDIO_S16SYS;
	int audio_channels = 2;
	int audio_buffers = 4096;

	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}

/*
	SDL_Window *window;                    // Declare a pointer

   // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );

    // Check that the window was successfully made
    if (window == NULL) {
        // In the event that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    // The window is open: enter program loop (see SDL_PollEvent)

    SDL_Delay(3000);  // Pause execution for 3000 milliseconds, for example

    // Close and destroy the window
    SDL_DestroyWindow(window);
*/

	AudioTrack *track[8];

	for (int i=0; i<MAX_TRACKS; i++) {
		track[i] = new AudioTrack();
		track[i]->setTrack(i);
		track[i]->start();
	}

//	Mix_Chunk *sound1 = NULL;
	char *file = MUS_PATH;
	track[0]->setInterval(200000);
	track[0]->setLoops(48);
	track[0]->setSound(file);
	/*if(sound1 == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}
	*/

	char *file2 = MUS_PATH2;
	track[1]->setInterval(400000);
	track[1]->setLoops(24);
	track[1]->setSound(file2);



//	Mix_Chunk *sound2 = NULL;
	/*
	sound2 = Mix_LoadWAV(MUS_PATH2);
	if(sound2 == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}
 	 */

	TRACE(("thread launch done...\n"));
/*
	int channel1, channel2;

	channel1 = Mix_PlayChannel(-1, sound1, 0);
	if(channel1 == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}

	channel2 = Mix_PlayChannel(-1, sound2, 0);
	if(channel1 == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}

	while(Mix_Playing(channel1) ||  Mix_Playing(channel2));

	Mix_FreeChunk(sound1);
	Mix_FreeChunk(sound2);
*/
//	Mix_CloseAudio();


/*
	TRACE(("Initializing libSDL\n"));

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        fprintf(stderr,
                "\nUnable to initialize SDL:  %s\n",
                SDL_GetError()
               );
        return 1;
    }
    // local variables
	static Uint32 wav_length; // length of our sample
	static Uint8 *wav_buffer; // buffer containing our audio file
	static SDL_AudioSpec wav_spec; // the specs of our piece of music

	// Load the WAV
	// the specs, length and buffer of our wav are filled
	if( SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL ){
		printf("Failed to load %s\n",MUS_PATH);
	  return 1;
	}
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length

	printf("File ready\n");

	// Open the audio device
	if ( SDL_OpenAudio(&wav_spec, NULL) < 0 ){
	  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
	  exit(-1);
	}

	// Start playing
	printf("Starting to play\n");
	SDL_PauseAudio(0);

	// wait until we're don't playing
	while ( audio_len > 0 ) {
		SDL_Delay(100);
	}

	printf("Playing done\n");

	// shut everything down
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);

    atexit(SDL_Quit);

*/

    SDL_Delay(8000);

	for (int i=0; i<MAX_TRACKS; i++) {
		track[i]->detach();
		track[i]->join();
		delete track[i];
	}

    SDL_Quit();


	return 0;
}
