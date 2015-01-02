/* lock.c */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int check_for_high_pri_waiting_writers(struct lentry *lptr, int pri, int ldes)
{
	int temp = lptr->lqhead;
	while(temp != lptr->lqtail) {
		if((&proctab[temp])->plock_type[ldes] == WRITE) {
			if(q[temp].qkey > pri)
				return 1;
		}
		temp = q[temp].qnext;
	}
	return 0;
}

/*------------------------------------------------------------------------
 * lock  --  try to acquire the lock, if not possible, wait.
 *------------------------------------------------------------------------
 */
SYSCALL lock(int ldes1, int type, int priority)
{
        STATWORD ps;

        struct  lentry  *lptr;
        struct  pentry  *pptr;

        disable(ps);
        if (isbadlock(ldes1) || (lptr= &locks[ldes1])->ltype == DELETED) {
                restore(ps);
                return(SYSERR);
        }

	lptr = &locks[ldes1];
	
	if (type == WRITE) {
		if (lptr->lcnt == 0) {
			lptr->ltype = WRITE;
			lptr->lcnt++;
			lptr->lowner = currpid;
			(pptr = &proctab[currpid])->plock[ldes1] = USED_BY_PROC;
			pptr->plock_type[ldes1] = WRITE;
		} else if (lptr->lcnt > 0) {
/* Check this case */
			(pptr = &proctab[currpid])->pstate = PRWAIT;
			pptr->waiting_start_time = ctr1000;
			insert(currpid, lptr->lqtail, priority);
			resched();
		}
	}

	if (type == READ) {
		if (lptr->lcnt == 0) {
			lptr->ltype = READ;
			lptr->lcnt++;
			lptr->lowner = currpid;
			(pptr = &proctab[currpid])->plock[ldes1] = USED_BY_PROC;
			pptr->plock_type[ldes1] = READ;
		} if(lptr->lcnt > 0) {
			if (lptr->ltype == WRITE || check_for_high_pri_waiting_writers(lptr, priority, ldes1) == 1) {
				(pptr = &proctab[currpid])->pstate = PRWAIT;
				pptr->waiting_start_time = ctr1000;
				insert(currpid, lptr->lqtail, priority);
				resched();
			} else
				lptr->lcnt++;
        	}
	}
	restore(ps);
	return(OK);
}
