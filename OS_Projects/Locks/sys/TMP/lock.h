#define NLOCKS	50

#define FREE 	0
#define READ	1
#define WRITE	2

#define USED_BY_PROC		111
#define NOT_USED_BY_PROC	222

struct lentry {
	int ltype;	/* Either READ, WRITE, FREE or DELETED */
	int lcnt;	/* This get increased when someone tries to acquire the lock */
	int lowner;	/* Current owner of the lock */
	int lqhead;	/* Head of wait queue */
	int lqtail;	/* Tail of wait queue */
};

extern struct lentry locks[];

extern int nextlock;

extern unsigned long ctr1000;

#define isbadlock(s)     (s<0 || s>=NLOCKS)

void linit();
