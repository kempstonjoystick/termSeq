/*
 * audioTrack.h
 *
 *  Created on: Jun 17, 2015
 *      Author: timmins
 */

#ifndef AUDIOTRACK_H_
#define AUDIOTRACK_H_

#include <stdio.h>
#include <csignal>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "thread.h"


//NEXT fix timing, this one all varies depending on the length of the track.

#define MAX_TRACKS 3

class AudioTrack : public Thread
{
  public:
	AudioTrack( void );
	~AudioTrack( void ) {
		//free the MixChunk
		if(sound != NULL)
			Mix_FreeChunk(sound);
		if(trackData)
			free(trackData);
	}
	void setTrack(int16_t track) { trackNo = track; }
	void setLoops(int16_t lps) { loops = lps; }
	void setBeat(int16_t b) {
		trackData[b*pulses] = 1;
	}
	void dumpTrackData(void) {
		if(trackData == NULL) {
			printf("Track %d not initialized\n", trackNo);
		}
		for (int i = 0 ; i < 96; i++) {
			printf("Track data for track %d", trackNo);
			printf("	pulse %02d = %d\n",i, trackData[i]);
		}
	}

	int16_t pulsesPerBar(void){
		return (beats * pulses);
	}
	void setLength(int16_t beatz, int16_t pulses_per_beat) {
		//TODO limit beats/pulses here
		beats = beatz;
		pulses = pulses_per_beat;
		trackData = (unsigned char *) malloc( pulsesPerBar() * sizeof(unsigned char) );
		memset(trackData, 0, pulsesPerBar()*sizeof(unsigned char));

//		printf("Track %d set total trackDataSize to %d bytes\n", trackNo,
//				pulsesPerBar()*sizeof(unsigned char) );
	}

	void trigger(int global_ival);
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
    void *run(void);

  private:
    uint16_t beats;				//beats per bar
    int16_t pulses; 			//pulses per beat
    unsigned char *trackData;	//start off just using intervals, we can bump the dimensions later, i.e.
    							//***trackData (which would be trackdata[][].
    							//****trackData (trackdata[][][])
    pthread_cond_t cv;
    pthread_mutex_t mutex;
    //variables
    bool configured;
    Mix_Chunk* sound;		//the sound assigned to this track
    int16_t trackNo;		//this is the track/channel number, 0 through MAX_TRACKS-1
    int16_t loops;		//number of iterations
    int channel;

//    int16_t volume;
    int32_t interval;		//this will be some enum, describes how many intervals in a bar for this track

};






#endif /* AUDIOTRACK_H_ */
