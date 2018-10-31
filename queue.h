#ifndef QUEUE_H_
#define QUEUE_H_
#define MAXLEN 400
#define BUF_SIZE 15


typedef struct qNode* queuePtr;

typedef struct qNode
{  
    int logic_id;
	long jobId;
	char * job;
	queuePtr next;
}queueNode;

typedef struct pool {
    int num_of_jobs;
	queuePtr pendingQueue;
	queuePtr runningQueue;
	queuePtr finishingQueue;
}pool;



queuePtr AddItem (queuePtr head, long JobId, char* job,int logic_id);//Adds item to queue
char *ReturnQueue(queuePtr head);//Returns as a string the selected queue in a printable format
char *ReturnLogic_ids(queuePtr head);//Returns as a string the selected queue in a printable format
queuePtr DeleteItem(queuePtr *p, int JobId);//deletes item with id JobId
int SearchItem(queuePtr *p, int logic_id);//deletes item with id JobId
int ReturnFirstId(queuePtr head);//Returns the jobId of the first element of the queue
int CountItems (queuePtr head);//Counts # items in queue
char *ReturnDeleteFirstItem(queuePtr *head);//Returns the first item as a String "$JobId $job" and deletes it from the queue
#endif /* QUEUE_H_ */