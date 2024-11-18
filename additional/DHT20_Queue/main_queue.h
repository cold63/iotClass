#ifndef _MAIN_QUEUE_H_
#define _MAIN_QUEUE_H_
#include <Arduino.h>

#define MAXQUEUE 100

typedef struct _logData
{
    unsigned int idx;
    float TempVale;
    float HumidityValue;
}logData;


int isQueueEmpty(void);
int enqueue(logData d);
int dequeue(logData *value);


#endif