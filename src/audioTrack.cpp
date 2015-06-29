
#include "audioTrack.h"

#include <sys/time.h>
#include <iostream>

AudioTrack::AudioTrack( void ) :
		beats(0), pulses(0), trackData( NULL), fileConfigured(false), beatsConfigured(false),
		sound( NULL ), trackNo( -1 ), loops( 0 ), channel( 0 ), interval(-1) {

	printf("Track %d initialized...\n", trackNo);
	pthread_mutex_init(&trigger_mutex, NULL);
	pthread_cond_init(&trigger_cv, NULL);

	//initialize our command interface

	commandMap.insert( std::make_pair( "beat", &AudioTrack::commandSetBeat ));
	commandMap.insert( std::make_pair( "file", &AudioTrack::commandSetFile ));
//	fmap.insert( std::make_pair( "g", &AudioTrack::g ));
}

//CommandFunction AudioTrack::commandSetBeat {

//}

void *AudioTrack::run(void) {
	struct timeval start_time;
    struct timeval end_time;

	gettimeofday(&start_time, NULL);

	int local_interval = -1;
    	while ( 1 ) {
            pthread_mutex_lock(&trigger_mutex);
loop:
                // wait as long as there is no data available and a shutdown
                // has not beeen signalled
                pthread_cond_wait(&trigger_cv, &trigger_mutex);
                if(interval == local_interval)  {
                	printf("Track %d spurious wake\n", trackNo);
                	goto loop;
                }
                local_interval = interval;
                pthread_mutex_unlock(&trigger_mutex);

				//play sound here
				if(trackData[local_interval]) {
					gettimeofday(&end_time, NULL);

					float duration = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) * 1E-6;

					if(trackNo == 0)
						cout << "Track duration between triggers: " << duration << "s" << endl;
					start_time = end_time;

					channel = Mix_PlayChannel(trackNo, sound, 0);
					if(channel == -1) {
						fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
					}
				}

                goto loop;

    	}
    		/*
    	}
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
        */
}

//TODO hand this an or'ed loop, interval, pulse
void AudioTrack::trigger(int global_ival) {

	//don't do anything until we have something to play!
	if(!fileConfigured || !beatsConfigured)
		return;

	pthread_mutex_lock(&trigger_mutex);
//	printf("Track %d triggered\n", trackNo);
	interval = global_ival++;
	pthread_cond_signal(&trigger_cv);
	pthread_mutex_unlock(&trigger_mutex);

}

int AudioTrack::validateCommand(int argc, char **argv) {

	map<string, CommandFunc>::iterator it;

	if(!argc)
		return -1;

	printf("validating command %s\n", argv[0]);
	it = commandMap.find(argv[0]);
	if (it == commandMap.end()) {
		printf("Command %s not supported\n", argv[0]);
		return -1;
	}

	//TODO, further verification of command argument length

	return 0;
}

int AudioTrack::runCommand(int argc, char **argv) {

	map<string, CommandFunc>::const_iterator it;

	if(!argc)
		return -1;

	printf("processing command %s\n", argv[0]);
	it = commandMap.find(argv[0]);
	if (it == commandMap.end()) {
		printf("Command %s not supported\n", argv[0]);
		return -1;
	}

	(this->*(it->second))(argc-1, &argv[1]);

	//TODO, further verification of command argument length

	return 0;
}

//Our command interface starts here
int AudioTrack::commandSetFile( int argc, char **argv ) {
	printf("Called the %s command!\n", __func__);
	//lock the mutex
	//adjust some stuff
	//unlock the mutex
	//return

	if(argc)
		setSound(argv[0]);

	return 0;
}

//Our command interface starts here
int AudioTrack::commandSetBeat( int argc, char **argv ) {
	printf("Called the %s command!\n", __func__);
	//lock the mutex
	//adjust some stuff
	//unlock the mutex
	//return

	beatsConfigured=true;

	return 0;
}

