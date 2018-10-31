#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"


queuePtr AddItem(queuePtr p, long JobId, char* job,int logic_id)
{
	int cond;
	if (p == NULL)
	{
		p = malloc(sizeof(queueNode));
		p->job = malloc((strlen(job) + 1) * sizeof(char));
		strcpy(p->job, job);
		p->jobId = JobId;
		p->logic_id=logic_id;
		p->next = NULL;
	}
	else
		p->next = AddItem(p->next, JobId, job,logic_id);
	return p;
}
queuePtr DeleteItem(queuePtr *p, int JobId)
/* Delete element v from list, if it exists
 */
{
	queuePtr tempqueue,orig;
	while ((*p) != NULL)
		if ((*p)->logic_id == JobId)
		{
			tempqueue = *p;
			orig=tempqueue;
			*p = (*p)->next;
			free(tempqueue);
			tempqueue=NULL;
			return orig;
		}
		else
			p = &((*p)->next);
}

int SearchItem(queuePtr *p, int logic_id){

	queuePtr tempqueue;
	while ((*p) != NULL)
		if ((*p)->logic_id == logic_id)
		{
			tempqueue = *p;
			*p = (*p)->next;
			return 1;
		}
		else
			p = &((*p)->next);
	return 0;

}


int CountItems(queuePtr head)
{
	int i = 0;
	queuePtr p = NULL;
	for (p = head; p != NULL; p = p->next)
		i++;
	return i;
}

char *ReturnQueue(queuePtr head)
{
	char *reString = (char*)malloc(sizeof(char)*MAXLEN);
	reString[0] = '\0';
	queuePtr tempqueue = head;
	char buf[BUF_SIZE],buff[BUF_SIZE];
	while (tempqueue != NULL)
	{
		snprintf(buf, BUF_SIZE, "%ld", (long)tempqueue->jobId);
		strcat(reString,buf);
		strcat(reString," ");
		snprintf(buff, BUF_SIZE, "%d", tempqueue->logic_id);
		strcat(reString,buff);
        strcat(reString," ");
		strcat(reString,tempqueue->job);
		strcat(reString,"\n");
		tempqueue = tempqueue->next;
	}
	return reString;
}

char *ReturnLogic_ids(queuePtr head){
	char *reString = (char*)malloc(sizeof(char)*MAXLEN);
	reString[0] = '\0';
	queuePtr tempqueue = head;
	char buf[BUF_SIZE];
	while (tempqueue != NULL)
	{
		snprintf(buf, BUF_SIZE, "%d",tempqueue->logic_id);
		strcat(reString,buf);
		strcat(reString," ");
		tempqueue = tempqueue->next;
	}
	return reString;
}

int ReturnFirstId(queuePtr head)
{
	queuePtr tempqueue = head;
	if (tempqueue!=NULL)
	{
		return tempqueue->logic_id;
	}
	else
		return 0;
}
char *ReturnDeleteFirstItem(queuePtr *head)
{
	char *reString = (char*)malloc(sizeof(char)*MAXLEN);
	reString[0] = '\0';
	queuePtr tempqueue = *head;
	if (tempqueue != NULL)
	{
		strcat(reString,tempqueue->job);
		DeleteItem(head,tempqueue->logic_id);
	}
	return reString;

}

