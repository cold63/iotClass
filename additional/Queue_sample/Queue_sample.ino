#include "main_queue.h"

int isQueueEmpty()
{
    if(front_v == rear_v) return 1;
    else return FALSE;    
}

int enqueue(int d)
{
    if(rear_v >= MAXQUEUE)
        return FALSE;
    else{
        queue[++rear_v] = d;
        return TRUE;
    }
}

int dequeue()
{
    if(isQueueEmpty())
        return -1;
    else
        return queue[++front_v];
}

void setup()
{
	Serial.begin(115200);


	int data[6] = {1,2,3,4,5,6};
    int i;
    Serial.println("save data");

    for(i = 0; i < 6; i++){
        enqueue(data[i]);
        Serial.print(i);
        Serial.print(":");
        Serial.println(data[i]);
    }

    Serial.println("data out:");
    while(!isQueueEmpty()){
        Serial.println(dequeue());
    }

    Serial.println();    
}

void loop()
{

}
