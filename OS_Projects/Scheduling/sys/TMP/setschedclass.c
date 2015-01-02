#include <lab1.h>

static int scheduling;

void setschedclass(int sched)
{
	scheduling = sched;
}

int getschedclass()
{
        return scheduling;
}
