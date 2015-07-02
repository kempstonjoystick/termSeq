/*
 * audioTrack.h
 *
 *  Created on: Jun 17, 2015
 *      Author: timmins
 */

#ifndef AUDIOTRACK_H_
#define AUDIOTRACK_H_

#include <stdio.h>
#include <map>
#include <string>
#include <csignal>
#include <stdlib.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "thread.h"
#include "debug.h"

using namespace std;



//NEXT fix timing, this one all varies depending on the length of the track.

#define MAX_TRACKS 3
#define MAX_LOOPS 40


class AudioTrack : public Thread
{
  public:
	AudioTrack( int idx );	//initialize with track index
	~AudioTrack( void ) {
		//free the MixChunk
		if(sound != NULL)
			Mix_FreeChunk(sound);
		if(trackData)
			free(trackData);
	}

	void setTrackNo(int16_t track) { trackNo = track; }

	//not sure what I had in mind for this.
	void setLoops(int16_t lps) { loops = lps; }

	//temporary, for testing only
	/*
	void setBeat(int16_t b) {
		trackData[b*pulses] = 1;
		beatsConfigured=true;
	}*/

	//only outputs first 96 intervals, need to correct this
	void dumpLoopData(int loopId) {
		if(trackData == NULL) {
			printf("Track %d not initialized\n", trackNo);
		}
		for (int i = 0 ; i < 96; i++) {
			printf("Track data for track %d", trackNo);
			printf("	pulse %02d = %d\n",i, trackData[loopId][i]);
		}
	}

	//returns the number of intervals in each bar
	int16_t pulsesPerBar(void){
		return (beats * pulses);
	}

	//beats per bar, pulses per beat (should be 96)
	void setLength(int16_t beatz, int16_t pulses_per_beat) {
		//TODO limit beats/pulses here
		beats = beatz;
		pulses = pulses_per_beat;
		trackData = (unsigned char **) malloc( MAX_LOOPS * pulsesPerBar() * sizeof(unsigned char) );
		memset(trackData, 0, MAX_LOOPS * pulsesPerBar()*sizeof(unsigned char));

//		printf("Track %d set total trackDataSize to %d bytes\n", trackNo,
//				pulsesPerBar()*sizeof(unsigned char) );
	}

	//called by main, tells us to wake up and play something (or not)
	void trigger(int global_ival, int global_lval);

	//TODO - make this private
	void setSound(char *file) {
		sound=Mix_LoadWAV(file);
		if(sound == NULL) {
			fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
		}
		else {
			printf("Track %d loaded wav file %s\n", trackNo, file);
			fileConfigured = true;
		}
	}

	typedef int (AudioTrack::*CommandFunc)(int, char **);
	//command mapping - text command against the class member function.
	//populate the mapping in the constructor.
	std::map <string, CommandFunc> commandMap;

	//General-purpose validation checker, use this before calling runCommand
	int validateCommand(int argc, char **argv);

	//issue the command here, this will figure out what method to call from the commandMap
	int runCommand(int argc, char **argv);

	//command set, as interpreted from the commandline
	int commandSetBeat( int argc, char **argv );
	int commandSetFile( int argc, char **argv );


	//Thread run loop
    void *run(void);

  private:
    uint16_t beats;				//beats per bar
    int16_t pulses; 			//pulses per beat
    unsigned char **trackData;	//start off just using intervals, we can bump the dimensions later, i.e.
    							//***trackData (which would be trackdata[][].
    							//****trackData (trackdata[][][])

    pthread_mutex_t data_mutex;

    pthread_cond_t trigger_cv;
    pthread_mutex_t trigger_mutex;
    //variables
    bool fileConfigured;
    bool beatsConfigured;
    Mix_Chunk* sound;		//the sound assigned to this track
    int16_t trackNo;		//this is the track/channel number, 0 through MAX_TRACKS-1
    int16_t loops;		//number of iterations
    int channel;

//    int16_t volume;
    int32_t interval;		//which pulse are we on
    int32_t loop;			//which loop are we on

};


#endif /* AUDIOTRACK_H_ */
