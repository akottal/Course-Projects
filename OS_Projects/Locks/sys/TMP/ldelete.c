/* sdelete.c - sdelete */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
SYSCALL ldelete(int ldes)
{
        STATWORD ps;
        int     pid;
	int 	i,j;
        struct  lentry  *lptr;
	struct	pentry 	*pptr;

        disable(ps);
        if (isbadlock(ldes) || locks[ldes].ltype == FREE || locks[ldes].ltype == DELETED) {
                restore(ps);
                return(SYSERR);
        }
	for(i = 0; i < NPROC; i++) {
        	pptr=&proctab[i];
		for(j = 0; j < NLOCKS; j++) {
			if(pptr->plock[j] == USED_BY_PROC)	
				pptr->plock[j] = NOT_USED_BY_PROC;
		}
        }
        lptr = &locks[ldes];
        lptr->ltype = DELETED;
        if (nonempty(lptr->lqhead)) {
                while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		{
			ready(pid,RESCHNO);
		}
                resched();
        }
        restore(ps);
        return(OK);
}

