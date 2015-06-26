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

	typedef int (*MessageFunc)(char *);  // Defines a function pointer type pointing to a void function which doesn't take any parameter.
    MessageFunc messageCallback;  //  Actually defines a member variable of this type.


  private:
	mqd_t msgq_id;
	int * callback;
	 pid_t my_pid;
};




#endif /* EVENTLISTENER_H_ */
