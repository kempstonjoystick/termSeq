
#include "audioTrack.h"

#include <sys/time.h>
#include <iostream>

using namespace std;


AudioTrack::AudioTrack( void ) : trackData( NULL),
		beats(0), pulses(0), configured(false), sound( NULL ), trackNo( -1 ),
		interval(-1), loops( 0 ), channel( 0 ) {

	printf("Track %d initialized...\n", trackNo);
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cv, NULL);

}

void *AudioTrack::run(void) {
	struct timeval start_time;
    struct timeval end_time;

	gettimeofday(&start_time, NULL);

	int local_interval = -1;
    	while ( 1 ) {
            pthread_mutex_lock(&mutex);
loop:
                // wait as long as there is no data available and a shutdown
                // has not beeen signalled
                pthread_cond_wait(&cv, &mutex);
                if(interval == local_interval)  {
                	printf("Track %d spurious wake\n", trackNo);
                	goto loop;
                }
                local_interval = interval;
                pthread_mutex_unlock(&mutex);

                if(configured) {
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
//						printf("Track %d - interval %d Play done\n", local_interval, trackNo);
                	}
				}
				else {
					printf("No sound defined, not playing\n");
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

	pthread_mutex_lock(&mutex);
//	printf("Track %d triggered\n", trackNo);
	interval = global_ival++;
	pthread_cond_signal(&cv);
	pthread_mutex_unlock(&mutex);

}
