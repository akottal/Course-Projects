/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);

void newlinuxepoch();
int linux_sched();
int round_robin_sched();
void choosing_queue();
int newmqepoch();

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

void newlinuxepoch()
{
	int proc;

	/* Create a new ready list for each epoch */

	q[rdyhead].qnext = rdytail;
	q[rdytail].qprev = rdyhead;

	for(proc = 0; proc < NPROC; proc++) {
		proctab[proc].counter = proctab[proc].pprio + proctab[proc].counter/2;
		if(proctab[proc].pstate == PRREADY)
			insert(proc, rdyhead, (proctab[proc].pprio + proctab[proc].counter));
	}
}

int linux_sched()
{
	register struct	pentry	*optr;	
	register struct	pentry	*nptr;

		/* Put the current process in the ready queue */

		if ((optr= &proctab[currpid])->pstate == PRCURR) {
			optr->pstate = PRREADY;
			if (preempt <= 0) {
				insert(currpid,rdyhead,optr->pprio);
				optr->counter = 0;
			} else {
				insert(currpid,rdyhead,(optr->pprio+optr->counter));
				optr->counter = preempt;
			}
		}

		/* Select next process to run from the ready list*/

		/*
		 * Check for processes having non zero counter value in decreasing
		 * order of goodness.
		 */

		int proc = q[rdytail].qprev;
		while((proc < NPROC) && proctab[proc].counter < 1) {
			proc = q[proc].qprev;
		}
	
		if(q[proc].qkey > 0) {		/* If there are any process with quantum left, let them run */
			nptr = &proctab[ (currpid = dequeue(proc)) ];
		} else {
				/* No more processes in this epoch */
				newlinuxepoch();
				nptr = &proctab[ (currpid = getlast(rdytail)) ];
		}

		nptr->pstate = PRCURR;
		preempt = nptr->counter;

		if(optr == nptr) {
		/* No need to context switch */
			return OK;
		}
		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

        return OK;
}

int round_robin_sched()
{
                register struct pentry *optr;
                register struct pentry *nptr;
		
		if(q[rdy2head].qnext == rdy2tail) {
				choosing_queue();
				return OK;
		} else {
			optr = &proctab[currpid];

			/* Real Time processes go into a separate queue headed by rdy2head and tail at rdy2tail */

	                if (optr->pstate == PRCURR) {
        	                optr->pstate = PRREADY;

				/* Insert the process at the end */
                	       	insert_rr(currpid,rdy2tail,optr->pprio);
              		}

			/* Dequeue the process from the head */
	                nptr = &proctab[ (currpid = getfirst(rdy2head)) ];
        	        nptr->pstate = PRCURR;
  	        	preempt = 100;
                	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                	return OK;
		}
}


void choosing_queue()
{
	int i,index,random[10],choice;

        for(i = 0; i < 7; i++)
        random[i] = 1;		/* 70% for real time queue */
        for(i = 7; i < 10; i++)
        random[i] = 2;		/* 30% for normal queue */

        index = rand() % 10;
        choice = random[index];

        if(choice == 1) {
		if(q[rdy2head].qnext != rdy2tail)
			round_robin_sched();
		else
			linux_sched();
	}
        if(choice == 2) {
		linux_sched();
	}
}

int resched()
{
		register struct pentry *optr;
		register struct pentry *nptr;

		if (getschedclass() == LINUXSCHED)
			return linux_sched();
	        else if (getschedclass() == MULTIQSCHED) {
                	if ( (optr= &proctab[currpid])->ptype == NORMAL ) {
                        	return linux_sched();
                	} else if ( (optr= &proctab[currpid])->ptype == REALTIME )
                        	return round_robin_sched();
		}

                if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
                        (lastkey(rdytail)<optr->pprio)) {
                                return(OK);
                }

                /* force context switch */

                if (optr->pstate == PRCURR) {
                        optr->pstate = PRREADY;
                        insert(currpid,rdyhead,optr->pprio);
                }

                /* remove highest priority process at end of ready list */

                nptr = &proctab[ (currpid = getlast(rdytail)) ];
                nptr->pstate = PRCURR;          /* mark it currently running    */
#ifdef  RTCLOCK
                preempt = QUANTUM;              /* reset preemption counter     */
#endif
 
                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		return OK;
}
