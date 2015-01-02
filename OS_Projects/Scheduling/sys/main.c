#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <sched.h>
#include <stdio.h>
#include <math.h>
#include <lab1.h>


#define LOOP	50

int prA, prB, prC, prD, prE, prF, prG, prH, prI;
int proc(char c);

int main() {
	int i;
	int count = 0;
	//char buf[8];

    for(i=0;i<10;i++){
    	srand(i);
    	printf("%d\n", rand() );
    }

   	setschedclass(LINUXSCHED);
	resume(prA = create((int *)proc, 2000, 5, "proc A", 1, 'A'));
        resume(prB = create((int *)proc, 2000, 50, "proc B", 1, 'B'));
        resume(prC = create((int *)proc, 2000, 90, "proc C", 1, 'C'));
/*
	resume(prE = createReal((int *)proc, 2000, 90, "proc E", 1, 'E'));
	resume(prF = createReal((int *)proc, 2000, 90, "proc F", 1, 'F'));
        resume(prG = createReal((int *)proc, 2000, 90, "proc G", 1, 'G'));
        resume(prH = create((int *)proc, 2000, 50, "proc B", 1, 'H'));
        resume(prI = create((int *)proc, 2000, 90, "proc C", 1, 'I'));
*/
	while (count++ < LOOP) {
		kprintf("M");

		for (i = 0; i < 10000000; i++);
	}

}

int proc(char c) {
	int i;
	int count = 0;

	while (count++ < LOOP) {
		kprintf("%c", c);
		for (i = 0; i < 10000000; i++);
	}

}
