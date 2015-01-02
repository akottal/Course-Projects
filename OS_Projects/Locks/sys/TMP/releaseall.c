/* releaseall.c */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

int remove_and_return_node(int proc)
{
	struct  qent    *mptr;          /* pointer to q entry for item  */
        mptr = &q[proc];
        q[mptr->qprev].qnext = mptr->qnext;
        q[mptr->qnext].qprev = mptr->qprev;
        return(proc);
}

int releaselock(int ldes)
{
	STATWORD ps;

	struct lentry *lptr;

	if (isbadlock(ldes) || (lptr= &locks[ldes])->ltype == FREE || (lptr= &locks[ldes])->ltype == DELETED) {
                restore(ps);
                return(SYSERR);
        }

	(&proctab[currpid])->plock[ldes] = NOT_USED_BY_PROC;
	lptr->lcnt--;
	if(lptr->lcnt == 0) {

		/* Take out the highest waiting process from the end of the list */

		int temp = lptr->lqtail;
		int eq_first = -1;
		int eq_last = -1;
		int count = 0;
		while(q[temp].qprev != EMPTY) {
			int prev_index = q[temp].qprev;
			if(q[temp].qkey > q[prev_index].qkey)
			break;
			else if(q[temp].qkey == q[prev_index].qkey) {
				if(count == 0)
				eq_first = temp;
				eq_last = prev_index;
				count++;
			}
			temp = prev_index;
		}
		if (eq_first == -1) {
			lptr->lowner = q[temp].qnext;
			ready(remove_and_return_node(temp), RESCHYES);	
		} else {
			int max_waiting_proc = eq_first;
			int max_waiting_proc2 = eq_first;
			unsigned long temp_time;
			unsigned long max_waiting_time = ctr1000 - (&proctab[eq_first])->waiting_start_time;
			unsigned long max_waiting_time2 = ctr1000 - (&proctab[eq_first])->waiting_start_time;
			temp = eq_first;
			while(temp != eq_last) {
				temp_time = ctr1000 - (&proctab[temp])->waiting_start_time;
				if(temp_time > max_waiting_time) {
					if(temp_time > max_waiting_time) {
						max_waiting_time2 = max_waiting_time;
						max_waiting_proc2 = max_waiting_proc;
						max_waiting_time = temp_time;
						max_waiting_proc = temp;
					} else if(max_waiting_time > temp_time && temp_time > max_waiting_time2) {
						max_waiting_time2 = temp_time;
						max_waiting_proc2 = temp;
					} else if(max_waiting_time2 > temp_time);
				}
				temp = q[temp].qprev;
			}
			if((max_waiting_time - max_waiting_time2) < 1000 && (&proctab[max_waiting_proc2])->plock_type[ldes] == WRITE) {
				lptr->lowner = q[max_waiting_proc2].qnext;
				ready(remove_and_return_node(max_waiting_proc2), RESCHYES);
			} else {
				lptr->lowner = q[max_waiting_proc].qnext;
				ready(remove_and_return_node(max_waiting_proc), RESCHYES);
			}
		}
	}
	restore(ps);
	return(OK);
}

/*------------------------------------------------------------------------
 * releaseall - release one or more locks
 *------------------------------------------------------------------------
 */
SYSCALL releaseall(int numlocks, int args)
{
	STATWORD ps;

	struct pentry *pptr;

	disable(ps);

	int nargs = numlocks;
	int i,n,ids[nargs];
	int *a;

	a = (int *)&args + nargs - 1;

	/* The args are stored in ids[1], ids[2],..., ids[nargs] */

	for(; nargs > 0; nargs--)
		ids[nargs] = *a--;

	n = 0;
	for(i = 1; i <= nargs; i++) {
		if((pptr = &proctab[currpid])->plock[ids[i]] == USED_BY_PROC) {
			n++;
			releaselock(ids[i]);
		}
	}

	if(n == nargs)
		return OK;
	else
		return SYSERR;
/*
	for(i = 1; i <= numlocks; i++)
	kprintf("%d\n",ids[i]);
*/

}

