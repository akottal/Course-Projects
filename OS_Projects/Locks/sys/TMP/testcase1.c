/* testcase1.c - Lock and release */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lock.h>

void halt();
void check(int num,long args,...);
int prA, prB, prC,prD,prE;
int lock1,lock2,lock3;
int j;

void proc(char c);
void proc1(char c);
void proc2(char c);
void proc3(char c);
void proc4(char c);

int count=0;

struct pentry *ptr;

#define LOOP	50


/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */

int main()
{	
	int i;
	kprintf("\n\nHello World, Xinu lives\n\n");
	lock1=lcreate();
	resume(prA = create(proc, 2000, 55, "proc A", 1, 'A'));
	resume(prB = create(proc, 2000, 40, "proc B", 1, 'B'));
	resume(prC = create(proc2, 2000, 90, "proc C", 1, 'C'));
	resume(prD = create(proc3, 2000, 44, "proc d", 1, 'D'));
	resume(prE = create(proc4, 2000, 54, "proc e", 1, 'E'));
	while (count++ < LOOP) {
		kprintf("M");
		for (i = 0; i < 10000000; i++);
		for (i = 0; i < 10000000; i++);
	}
	sleep(5);
	return 0;
}

void proc(char c) 
{
int i;
lock(lock1,WRITE,20);
for(i=0;i<50;i++) {
	kprintf("%c",c);
	sleep(1);
}
releaseall(1,lock1);
}

void proc1(char c)
{
int i;
lock(lock1,READ,10);
for(i=0;i<50;i++){
	kprintf("%c",c);
	sleep(1);
}
releaseall(1,lock1);
}

void proc2(char c)
{
int i;
lock(lock1,READ,10);
for(i=0;i<150;i++){
	kprintf("%c",c);
	sleep(1);
}
releaseall(1,lock1);
}

void proc3(char c)
{
int i;
lock(lock1,READ,10);
for(i=0;i<150;i++){
	kprintf("%c",c);
	sleep(1);
}
releaseall(1,lock1);
}

void proc4(char c)
{
int i;
lock(lock1,READ,10);
for(i=0;i<150;i++){
	kprintf("%c",c);
	sleep(1);
}
releaseall(1,lock1);
}
