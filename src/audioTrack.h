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
#include <errno.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "thread.h"
#include "debug.h"

using namespace std;



//NEXT fix timing, this one all varies depending on the length of the track.

#define MAX_TRACKS 3
#define MAX_LOOPS 40

enum pulse_type_e {
	BEAT_EMPTY = 0,
	BEAT_FILL,			//not a note type, more of a notification that a note is sustaining
	BEAT_SEMIBREVE,
	BEAT_MINIM,
	BEAT_CROTCHET,
	BEAT_QUAVER,
	BEAT_SEMIQUAVER,
	BEAT_DEMISEMIQUAVER,
	BEAT_HEMIDEMISEMIQUAVER
};

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
	int setLength(int16_t beatz, int16_t pulses_per_beat) {
		//TODO limit beats/pulses here
		beats = beatz;
		pulses = pulses_per_beat;

		if(trackData) {
			printf("Structure already configured\n");
			return -1;
		}
		//allocate the appropriate space for our beat data

		trackData = (unsigned char **) malloc( MAX_LOOPS * sizeof(unsigned char) );
		if(trackData <= 0) {
			printf("Malloc failed: %s\n", strerror(errno));
			return -1;
		}
		for(int i = 0 ; i < MAX_LOOPS ; i++) {
			trackData[i] = (unsigned char *) malloc( pulsesPerBar() * sizeof(unsigned char) );
			if(trackData[i] <= 0) {
				printf("Malloc failed: %s\n", strerror(errno));
				return -1;
			}

			printf("Doing memset %d\n", i);
			for (int j = 0 ; j < pulsesPerBar() ; j++ ) {
				trackData[i][j] = 0;
			}
			//memset(trackData[i], 0, pulsesPerBar()*sizeof(unsigned char));
		}

		printf("PulsesPerBar = %d\n\\n", pulsesPerBar());


		//set the beat length
		semibreve = 4 * pulses;
		minim = 2 * pulses;
		crotchet = 1 * pulses;
		quaver = crotchet >> 1;
		semiquaver = crotchet >> 2;
		demisemiquaver = crotchet >> 3;
		hemidemisemiquaver = crotchet >> 4;

		trackData[0][0] = 0x55;
		trackData[0][1] = 0x55;

		TRACE(("Length configured for track %d\n", trackNo));

		lengthConfigured = true;

//		printf("Track %d set total trackDataSize to %d bytes\n", trackNo,
//				pulsesPerBar()*sizeof(unsigned char) );
		return 0;
	}

	//called by main, tells us to wake up and play something (or not)
	int trigger(int global_ival, int global_lval);

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
    unsigned char **trackData;	//trackData[loop][interval]

    pthread_mutex_t data_mutex;

    pthread_cond_t trigger_cv;
    pthread_mutex_t trigger_mutex;
    //variables
    bool fileConfigured;
    bool beatsConfigured;
    bool lengthConfigured;

    Mix_Chunk* sound;		//the sound assigned to this track
    int16_t trackNo;		//this is the track/channel number, 0 through MAX_TRACKS-1
    int16_t loops;		//number of iterations
    int channel;

//    int16_t volume;
    int32_t interval;		//which pulse are we on
    int32_t loop;			//which loop are we on

	int semibreve;				//TODO - handle triplets
	int minim;
	int crotchet;
	int quaver;
	int semiquaver;
	int demisemiquaver;
	int hemidemisemiquaver;

};


#endif /* AUDIOTRACK_H_ */
