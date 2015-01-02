#include <lock.h>
#include <q.h>

struct lentry locks[NLOCKS];

int nextlock;

void linit()
{
int i;
nextlock = NLOCKS - 1;
struct lentry *lptr;
for (i=0 ; i<NLOCKS ; i++) {      /* initialize locks */
	(lptr = &locks[i])->ltype = FREE;
	lptr->lqtail = 1 + (lptr->lqhead = newqueue());
	lptr->lcnt = 0;
	lptr->lowner = -1;
}
}
