#include "main_queue.h"
#include "Arduino.h"

int front_v = -1;
int rear_v =-1;
logData Syslog[MAXQUEUE];

int isQueueEmpty(void)
{
    if(front_v == rear_v) {
     front_v = rear_v = -1;
     return 1;
     }
    else {return 0;    }
}

int enqueue(logData d)
{
    if(rear_v >= MAXQUEUE)
        return 0;
    else
    {
        int x = ++rear_v;
        Syslog[x].idx = d.idx;
        Syslog[x].HumidityValue = d.HumidityValue;
        Syslog[x].TempVale = d.TempVale;
        return 1;
    }
}

int dequeue(logData *value)
{
    if(isQueueEmpty())
    {
        
        return -1;
    }
    else
    {
        int x = ++front_v;
        value->idx  =Syslog[x].idx;
        value->HumidityValue = Syslog[x].HumidityValue;
        value->TempVale = Syslog[x].TempVale;
        return 1;
    }
        
}
