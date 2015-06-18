/*
 * audioTrack.h
 *
 *  Created on: Jun 17, 2015
 *      Author: timmins
 */

#ifndef AUDIOTRACK_H_
#define AUDIOTRACK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "thread.h"


//NEXT fix timing, this one all varies depending on the length of the track.

#define MAX_TRACKS 2

class AudioTrack : public Thread
{
  public:
	AudioTrack( void ) : configured(false), sound( NULL ), trackNo( -1 ), loops( 0 ), channel( 0 )  // option 1.
	{
	}
	~AudioTrack( void ) {
		//free the MixChunk
		if(sound != NULL)
			Mix_FreeChunk(sound);
	}
	void setTrack(int16_t track) { trackNo = track; }
	void setLoops(int16_t lps) { loops = lps; }
	void setInterval(float ival) { interval = ival; }
	void setSound(char *file) {
		sound=Mix_LoadWAV(file);
		if(sound == NULL) {
			fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
		}
		else {
			printf("Track %d loaded wav file %s\n", trackNo, file);
			configured = true;
		}
	}
    void *run() {
        for (int i = 0; i < loops; i++) {
        	printf("Track %d playing - iteration %d\n", trackNo, i+1);
        	if(configured) {
        		//play sound here
        		channel = Mix_PlayChannel(trackNo, sound, 0);
        		if(channel == -1) {
					fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
				}
        		printf("Track %d - Play done\n", trackNo);
        	}
        	else {
        		printf("No sound defined, not playing\n");
        	}
            usleep(interval);
        }
        printf("thread done %lu\n", (long unsigned int)self());
        return NULL;
    }
  private:

    //variables
    bool configured;
    Mix_Chunk* sound;		//the sound assigned to this track
    int16_t trackNo;		//this is the track/channel number, 0 through MAX_TRACKS-1
    int16_t loops;		//number of iterations
    int channel;
//    int16_t volume;
    float interval;		//this will be some enum, describes how many intervals in a bar for this track

};






#endif /* AUDIOTRACK_H_ */
