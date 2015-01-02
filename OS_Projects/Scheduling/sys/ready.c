/* ready.c - ready */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

/*------------------------------------------------------------------------
 * ready  --  make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
int ready(int pid, int resch)
{
	register struct	pentry	*pptr;

	if (isbadpid(pid))
		return(SYSERR);
	pptr = &proctab[pid];
	pptr->pstate = PRREADY;
	if(pptr->ptype == NORMAL) 
		insert(pid,rdyhead,pptr->pprio + pptr->counter);
	else if(pptr->ptype == REALTIME)
		insert_rr(pid,rdytail,pptr->pprio);
	if (resch)
		resched();
	return(OK);
}
