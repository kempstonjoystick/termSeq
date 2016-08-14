#include "audioTrack.h"

#include <sys/time.h>
#include <iostream>

AudioTrack::AudioTrack(int idx) :
		beats(0), pulses(0), trackData( NULL), fileConfigured(false), beatsConfigured(
				false), lengthConfigured(false), sound( NULL), loops(0), channel(
				0), interval(-1), loop(-1), semibreve(0), minim(0), crotchet(0), quaver(
				0), semiquaver(0), demisemiquaver(0), hemidemisemiquaver(0) {

	//TODO check index is <= MAX_TRACKS
	trackNo = idx;
	printf("Track %d initialized...\n", trackNo);

	pthread_mutex_init(&data_mutex, NULL);

	pthread_mutex_init(&trigger_mutex, NULL);
	pthread_cond_init(&trigger_cv, NULL);

	//initialize our command interface

	commandMap.insert(std::make_pair("beat", &AudioTrack::commandSetBeat));
	commandMap.insert(std::make_pair("file", &AudioTrack::commandSetFile));
//	fmap.insert( std::make_pair( "g", &AudioTrack::g ));
}

//CommandFunction AudioTrack::commandSetBeat {

//}

void *AudioTrack::run(void) {
	struct timeval start_time;
	struct timeval end_time;

	gettimeofday(&start_time, NULL);

	int local_interval = -1;
	int local_loop = -1;
	while (1) {
		pthread_mutex_lock(&trigger_mutex);
		loop:
		// wait as long as there is no data available and a shutdown
		// has not beeen signalled
		pthread_cond_wait(&trigger_cv, &trigger_mutex);
		if (interval == local_interval) {
			printf("Track %d spurious wake\n", trackNo);
			goto loop;
		}
		local_interval = interval;
		local_loop = loop;
		pthread_mutex_unlock(&trigger_mutex);

		//play sound here
		if ((trackData[local_loop][local_interval] != BEAT_FILL)
				&& (trackData[local_loop][local_interval] != BEAT_EMPTY)) {
			//printf("Track %d playing interval %d\n", trackNo, local_interval);
		//	gettimeofday(&end_time, NULL);

		//	float duration = (end_time.tv_sec - start_time.tv_sec)
		//			+ (end_time.tv_usec - start_time.tv_usec) * 1E-6;

		//	if (trackNo == 0)
		//		cout << "Track duration between triggers: " << duration << "s"
		//				<< endl;
		//	start_time = end_time;

			channel = Mix_PlayChannel(trackNo, sound, 0);
			if (channel == -1) {
				fprintf(stderr, "Unable to play WAV file: %s\n",
						Mix_GetError());
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
int AudioTrack::trigger(int global_ival, int global_lval) {

	//don't do anything until we have something to play!
	if (fileConfigured && beatsConfigured && lengthConfigured) {
		pthread_mutex_lock(&trigger_mutex);
		//printf("Track %d triggered\n", trackNo);
		interval = global_ival;
		loop = global_lval;
		pthread_cond_signal(&trigger_cv);
		pthread_mutex_unlock(&trigger_mutex);
	}
	if(trackData[global_lval][global_ival])
		return 1;

	return 0;
}

int AudioTrack::validateCommand(int argc, char **argv) {

	map<string, CommandFunc>::iterator it;

	if (!argc)
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

	if (!argc)
		return -1;

	printf("processing command %s\n", argv[0]);
	it = commandMap.find(argv[0]);
	if (it == commandMap.end()) {
		printf("Command %s not supported\n", argv[0]);
		return -1;
	}

	(this->*(it->second))(argc - 1, &argv[1]);

	//TODO, further verification of command argument length

	return 0;
}

//Our command interface starts here

/* commandSetFile
 * Args: path/filename
 */
int AudioTrack::commandSetFile(int argc, char **argv) {
	printf("Called the %s command!\n", __func__);
	//lock the mutex
	pthread_mutex_lock(&data_mutex);

	//adjust some stuff
	if (argc)
		setSound(argv[0]);

	//unlock the mutex
	pthread_mutex_unlock(&data_mutex);

	return 0;
}

/* commandSetBeat
 * Args:
 * examples: loop 1 crotchet 1 (offset quaver)
 *           loop 1 fill crotchet
 *           loop 1 clear crotchet \
 */
int AudioTrack::commandSetBeat(int argc, char **argv) {
	enum {
		BEAT_SET, BEAT_CLEAR, BEAT_FILL,
	} beat_setting;

	printf("Called the %s command!\n", __func__);

	if (!(fileConfigured && lengthConfigured)) {
		TRACE(("Please configure the structure and track file first\n"));
		return -1;
	}

	//lock the mutex
	pthread_mutex_lock(&data_mutex);
	int i;
	int offset=0;
	char * pEnd;
	int theloop = 1;
	int thebeat = 1; //beat in bar, defaults to 1
	beat_setting = BEAT_SET;
	pulse_type_e beat_type = BEAT_EMPTY;
	int length = 0;

	//adjust some stuff
	for (i = 0; i < argc; i++) {

		if (strcmp(argv[i], "loop") == 0) {
			if (argc > i + 1) {
				theloop = strtol(argv[i + 1], &pEnd, 10);
				TRACE(("Configuring beat for loop %d\n", theloop));
				//TODO Limit loops between 1 and MAX_LOOPS
			}
			i++;	//double increment this time
		}

		else if (strcmp(argv[i], "offset") == 0) {
				if (argc > i + 1) {
					offset = strtol(argv[i + 1], &pEnd, 10);
					printf("Set offset to %d\n", offset);
				}
		}

		else if (strcmp(argv[i], "clear") == 0) {
			TRACE(("Clear\n"));
			beat_setting = BEAT_CLEAR;
		} else if (strcmp(argv[i], "fill") == 0) {
			TRACE(("Fill\n"));
			beat_setting = BEAT_FILL;
		}

		if (strcmp(argv[i], "semibreve") == 0) {
			beat_type = BEAT_SEMIBREVE;
			length = semibreve;
		}
		else if (strcmp(argv[i], "minim") == 0) {
			beat_type = BEAT_MINIM;
			length = minim;
		}
		else if (strcmp(argv[i], "crotchet") == 0) {
			beat_type = BEAT_CROTCHET;
			length = crotchet;
		}
		else if (strcmp(argv[i], "quaver") == 0) {
			beat_type = BEAT_QUAVER;
			length = quaver;
		}
		else if (strcmp(argv[i], "semiquaver") == 0) {
			beat_type = BEAT_SEMIQUAVER;
			length = semiquaver;
		}
		else if (strcmp(argv[i], "demisemiqauver") == 0) {
			beat_type = BEAT_DEMISEMIQUAVER;
			length = demisemiquaver;
		}
		else if (strcmp(argv[i], "hemidemisemiquaver") == 0) {
			beat_type = BEAT_HEMIDEMISEMIQUAVER;
			length = hemidemisemiquaver;
		}

		printf("Length is %d\n");

		if(length != BEAT_EMPTY) {	//if we've set the length
			if (argc > i + 1) {
				thebeat = strtol(argv[i + 1], &pEnd, 10);
				TRACE(("Configuring beat %d, the note length is %d\n", thebeat, length));
				thebeat--;						//index is zero-based so adjust
				int j = thebeat * pulses;			//get starting pulse
				if(offset) {
					if ((offset * length) < pulses) {
						j = j + (offset * length);
						printf("j is %d\n", j);
					}
					else {
						printf("offset too long - ignoring\n");
						return -1;
					}
				}

				if (beat_setting == BEAT_SET) {
					TRACE(("Setting loop %d pulse %d\n", theloop, j));
					trackData[theloop][j++] = beat_type;
					//for (; j < (thebeat * 96) + pulses; j++) {
						//	TRACE(("Setting pulse %d\n", j));
						//	trackData[theloop][j] = BEAT_FILL;
					//}
				} else if (beat_setting == BEAT_CLEAR) {
					for (int k = j; k < (j + length); k++) {
						TRACE(("Clearing loop %d pulse %d\n", theloop, k));
						trackData[theloop][k] = BEAT_EMPTY;
					}
				} else if (beat_setting == BEAT_FILL) {
					//TODO - handle the filling throughout the bar
					TRACE(("Filling loop %d pulse %d\n", theloop, j));
					for ( ; j < pulsesPerBar(); j++) {
						if ((j % length) == 0) {
							TRACE(("Setting pulse %d\n", j));
							trackData[theloop][j] = beat_type;
						}
					}
				}
			}
		}

	}

	//need to think about how to handle this. Need to check if an entire loop is erased
	beatsConfigured = true;

	//unlock the mutex
	pthread_mutex_unlock(&data_mutex);

//	dumpLoopData(0);


	return 0;
}

