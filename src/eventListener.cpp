/*
 * eventListener.cpp
 *
 *  Created on: Jun 24, 2015
 *      Author: timmins
 */


#include "eventListener.h"


EventListener::EventListener(void) {

		    unsigned int msgprio = 0;
		    my_pid = getpid();
		    char msgcontent[MAX_MSG_LEN];
		    int create_queue = 0;
		    time_t currtime;

		    struct mq_attr attr;
		    attr.mq_flags = 0;
		    attr.mq_maxmsg = 10;
		    attr.mq_msgsize = 512;
		    attr.mq_curmsgs = 0;
		    create_queue = 1;

		    msgprio=10;

		    /* opening the queue        --  mq_open() */
		    if (create_queue) {
		        /* mq_open() for creating a new queue (using default attributes) */
		        msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, &attr);
		    } else {
		        /* mq_open() for opening an existing queue */
		        msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
		    }
		    if (msgq_id == (mqd_t)-1) {
		        perror("In mq_open()");
	//	        exit(1);
		    }

		    /* producing the message */
		    currtime = time(NULL);
		    snprintf(msgcontent, MAX_MSG_LEN, "Hello from process %u (at %s).", my_pid, ctime(&currtime));

		    /* sending the message      --  mq_send() */
		    mq_send(msgq_id, msgcontent, strlen(msgcontent)+1, msgprio);

		    /* closing the queue        -- mq_close() */
		    //mq_close(msgq_id);

	}

void *EventListener::run(void) {
	  unsigned int sender;
	  struct mq_attr msgq_attr;
	  int msgsz;
	  char msgcontent[512+1];

	  msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
	    if (msgq_id == (mqd_t)-1) {
	        perror("In mq_open()");
	        exit(1);
	    }

	    /* getting the attributes from the queue        --  mq_getattr() */
	    mq_getattr(msgq_id, &msgq_attr);
	    printf("Queue \"%s\":\n\t- stores at most %ld messages\n\t- large at most %ld bytes each\n\t- currently holds %ld messages\n", MSGQOBJ_NAME, msgq_attr.mq_maxmsg, msgq_attr.mq_msgsize, msgq_attr.mq_curmsgs);

	    /* getting a message */
	    msgsz = mq_receive(msgq_id, msgcontent, 512+1, &sender);
	    if (msgsz == -1) {
	        perror("In mq_receive()");
	        exit(1);
	    }
	    printf("Received message (%d bytes) from %d: %s\n", msgsz, sender, msgcontent);

	    /* closing the queue    --  mq_close() */
	    mq_close(msgq_id);


}
