/*
 * eventListener.h
 *
 *  Created on: Jun 24, 2015
 *      Author: timmins
 */

#ifndef EVENTLISTENER_H_
#define EVENTLISTENER_H_


#include <stdio.h>
#include <string.h>
#include <csignal>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>

#include "thread.h"


/* name of the POSIX object referencing the queue */
#define MSGQOBJ_NAME    "/termSeq"
/* max length of a message (just for this process) */
#define MAX_MSG_LEN     70

class EventListener : public Thread
{
  public:
	EventListener( void ) ;
	~EventListener( void ) {

	}
	void *run(void);
	void setCallback(void *) {

	}			//tell the thread what to call when we have an event to process

  private:
	void * callback;
	mqd_t msgq_id;
	 pid_t my_pid;
};




#endif /* EVENTLISTENER_H_ */
