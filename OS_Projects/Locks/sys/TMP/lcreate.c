/* lcreate.c - lcreate, newlock */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL lcreate()
{
        STATWORD ps;
        int ldes;

        disable(ps);
        if ( (ldes = newlock()) == SYSERR ) {
                restore(ps);
                return(SYSERR);
        }
        /* lqhead and lqtail were initialized at system startup */
        restore(ps);
        return(ldes);
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
        int id;
        int i;

        for (i=0 ; i<NLOCKS ; i++) {
                id = nextlock--;
                if (nextlock < 0)
                        nextlock = NLOCKS-1;
                if (locks[id].ltype == FREE || locks[id].ltype == DELETED) {
                        return id;
                }
        }
        return(SYSERR);
}
