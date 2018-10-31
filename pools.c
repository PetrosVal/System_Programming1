#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <time.h>

#include "queue.h"


#define COMMAND 1
#define INTLENGTH 4
#define MAX_ARGS 8



const char *getUserName()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw)
  {
    return pw->pw_name;
  }

  return "";
}


char* str_date()
{
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char* s;
    s=malloc(30*sizeof(char));
    strcpy(s, asctime(tm));

    return s;
}


void JobDone();
int jobs_pool;
pool Pool; 
int N;
char pid_directory[40]; 
char jobid[5],linux_pid[5],pid_date[30],pid_stdout[10],pid_stderr[10],pid_arxeio_1[60],pid_arxeio_2[60],pid_arxeio[60];
int fp,ft;
int pool1fd;



int main(int argc, char *argv[]){
	const char s[2] = " ";
	char *token;
	char *temptext;
	char *filename;
	char command;
	int bytesread;
	int filesize,st;
	int templength, templength2;
	int pool2fd;
	int pool1bytes, pool2bytes;
	int bytesleft;
	int msgsize;
	int i,jobpid;
	char* text;
	int nwrite;
	char str[300],xronos[10];
	char querybuf;
	N=0; //Proccess id 
	int logic_id;
	char *argvs[MAX_ARGS];
	char* tok;
	int status;
	DIR* dir_pid;
	

	//signal(SIGINT, SIG_IGN);

	signal(SIGCHLD, JobDone); //Install signal handler for when a process is finished


	struct stat statbuf;

	jobs_pool=atoi(argv[1]);

	//printf("Jobspool: %d\n",jobs_pool);

	Pool.num_of_jobs=0;
    Pool.pendingQueue=NULL;
    Pool.runningQueue=NULL;
    Pool.finishingQueue=NULL;



	if ( (pool2fd = open("/tmp/sdi1300017/_poolfifo2", O_RDONLY | O_NONBLOCK)) < 0) {
			perror("fifo open error");
			exit(1);
	}

	if ( (pool1fd = open("/tmp/sdi1300017/_poolfifo1", O_WRONLY)) < 0) {
		    perror("fifo open error");
			exit(1);
	 }

			for (;;) {

				if ( (pool2bytes = read(pool2fd, &querybuf, COMMAND)) == 1) {
					if ( (pool1fd = open("/tmp/sdi1300017/_poolfifo1", O_WRONLY)) < 0){
						perror("poolfifo2 open problem");
						exit(3);
					}
					//printf("coord entolipool:%c\n",querybuf);

					if (querybuf == 'a') {
						if ( (read(pool2fd,&msgsize , INTLENGTH)) == 4) {
							// printf("message from boardpost received = %d\n", channelid);
						}
					  //printf("Message sizepool: %d\n",msgsize);


						text = malloc(msgsize + 1);

						

						bytesleft = msgsize;
						while ((pool1bytes = read(pool2fd, text, msgsize)) > bytesleft) {
							if (pool1bytes > 0) {
								text += pool1bytes;
								bytesleft -= pool1bytes;
							}
						}
						text += pool1bytes;
						*text = '\0';
						text -= msgsize;

						//	printf("Textpool: %s \n",text);

						/*if (write(pool1fd, &validch, INTLENGTH) == -1) {
								perror("Error in Writing (server channel id)");
								exit(2);
						}*/
                          

							if (CountItems(Pool.runningQueue) < jobs_pool) //If I am permitted to execute
						{
							i=0;						
							tok=strtok(text,s);
							while (tok !=NULL)
							{
								
								argvs[i] = (char*) malloc(strlen(tok) + 1);
								strcpy(argvs[i], tok);
								i++;
								tok=strtok(NULL,s);
							}							
							argvs[i] = NULL;

                             ++N;     
                           

							if ((jobpid = fork()) < 0) //Create job to be executed
							{
								perror(
										"Error while creating job: ");
								exit(1);
							}
							else if (jobpid == 0)
							{
								//fclose(stdout); //make sure process cant print
								//fclose(stderr); //make sure process cant print
                                strcpy(pid_directory,"/tmp/sdi1300017/jms_sdi");
								strcat(pid_directory,getUserName());
								strcat(pid_directory,"_");
								sprintf(jobid,"%d",N);
								strcat(pid_directory,jobid);
								strcat(pid_directory,"_");
								sprintf(linux_pid,"%d",getpid());
								strcat(pid_directory,linux_pid);
								//strcat(pid_directory,"_");
								//strcat(pid_directory,str_date());
								//strcat(pid_directory,"/");

								//printf("Pid directory:%s\n",pid_directory);

								 st=mkdir(pid_directory,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

								if ((dir_pid = opendir(pid_directory)) == NULL) {
		                             fprintf(stderr, "cannot open or create %s\n",pid_directory);
		                             exit(1);
	                             }

	                             strncpy(pid_arxeio,pid_directory,40);

	                            // printf("pid_arxeio:%s\n",pid_arxeio);

	                             strncpy(pid_arxeio_1,pid_arxeio,40);
	                             strcat(pid_arxeio_1,"/");
	                             strcpy(pid_stdout,"stdout_");
	                             sprintf(jobid,"%d",N);
	                             strcat(pid_stdout,jobid);
	                             strcat(pid_arxeio_1,pid_stdout);
	                            // printf("Prwto arxeio:%s\n",pid_arxeio_1);

	                              
	                             strncpy(pid_arxeio_2,pid_arxeio,40);
	                             strcat(pid_arxeio_2,"/");
	                             strcpy(pid_stderr,"stderr_");
	                             sprintf(jobid,"%d",N);
	                             strcat(pid_stderr,jobid);
	                             strcat(pid_arxeio_2,pid_stderr);
	                             //printf("Deutero arxeio:%s\n",pid_arxeio_2);
								

								fp=open(pid_arxeio_1,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
								ft=open(pid_arxeio_2,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);


							    dup2(fp,1);
								dup2(ft,2);

								close(fp);
								close(ft);								
								
								if (execvp(argvs[0], argvs) < 0)
								{ /* execute the command  */
									printf("*** ERROR: exec failed\n");
									exit(1);
								}							
							}
							else{

								//waitpid(jobpid,&status,0); 
							

								if (write(pool1fd,&N,INTLENGTH) == -1) {
								perror("Error in Writing (pool to coord)");
								exit(2);
						        }      
						    
						        if (write(pool1fd,&jobpid,INTLENGTH) == -1) {
								perror("Error in Writing (pool to coord)");
								exit(2);
						        }           
						       
								Pool.runningQueue = AddItem(Pool.runningQueue, jobpid,
										text,N); //add to currently running queue
								char* running=ReturnQueue(Pool.runningQueue);
								printf("Running:%s \n",running);
							}
						}
						else //If due to small jobs_pool i can't run the command add it to pending queue to
						{	//be executed later
							++N;
							if (write(pool1fd,&N,INTLENGTH) == -1) {
								perror("Error in Writing (pool to coord)");
								exit(2);
						     }      
						    
						    if (write(pool1fd,&jobpid,INTLENGTH) == -1) {
								perror("Error in Writing (pool to coord)");
								exit(2);
						     }           
							Pool.pendingQueue = AddItem(Pool.pendingQueue,0,text,N);
							char* pending = ReturnQueue(Pool.pendingQueue);
							printf("Pending :%s \n",pending);
						}
						
					}
					else if(querybuf=='g'){

                        queuePtr q1;
						if ( (read(pool2fd,&logic_id , INTLENGTH)) == 4) {
							 //printf("message from coord received = %d\n", logic_id);
						}

						printf("I am going to suspend process with logic_id:%d\n",logic_id);
						//kill(logic_id,SIGKILL);
						q1=DeleteItem(&Pool.runningQueue,logic_id);//remove from running queue
						Pool.pendingQueue=AddItem(Pool.pendingQueue,q1->jobId,q1->job,logic_id);


					  if (write(pool1fd,&logic_id,INTLENGTH) == -1) {
						 perror("Error in Writing (pool to coord)");
						 exit(2);
					   }   				       

					}
					else if(querybuf=='j'){

                        queuePtr q1;
						if ( (read(pool2fd,&logic_id , INTLENGTH)) == 4) {
							 //printf("message from coord received = %d\n", logic_id);
						}

						printf("I am going to resume process with logic_id:%d\n",logic_id);
						//kill(logic_id,SIGKILL);
						q1=DeleteItem(&Pool.pendingQueue,logic_id);//remove from running queue
						Pool.runningQueue=AddItem(Pool.runningQueue,q1->jobId,q1->job,logic_id);

					  if (write(pool1fd,&logic_id,INTLENGTH) == -1) {
						 perror("Error in Writing (pool to coord)");
						 exit(2);
					   }   				       

					}
					else if(querybuf=='b'){


						if ( (read(pool2fd,&logic_id , INTLENGTH)) == 4) {
							 //printf("message from coord received = %d\n", logic_id);
						}

						//printf("I am going to status process with logic_id:%d\n",logic_id);

						if(SearchItem(&Pool.runningQueue,logic_id))

							command='r';

						else if(SearchItem(&Pool.pendingQueue,logic_id))

							command='s';
						else if (SearchItem(&Pool.finishingQueue,logic_id))
							command='f';

						if (write(pool1fd,&command,1) == -1) {
						 perror("Error in Writing (pool to coord)");
						 exit(2);
					   }   				   			    

					  if (write(pool1fd,&logic_id,INTLENGTH) == -1) {
						 perror("Error in Writing (pool to coord)");
						 exit(2);
					   }   	

					  
					}
					else if(querybuf=='c'){

						//printf("I am going to status all process \n");
						int mikos_1,mikos_2,mikos_3;
						char* results_1,*results_2,*results_3;
						results_1 = ReturnQueue(Pool.runningQueue);
						results_2 = ReturnQueue(Pool.pendingQueue);
						results_3 = ReturnQueue(Pool.finishingQueue);
						mikos_1=strlen(results_1);
						mikos_2=strlen(results_2);
						mikos_3=strlen(results_3);


						if (write(pool1fd,&mikos_1,INTLENGTH) == -1) {
						     perror("Error in Writing (pool to coord)");
						     exit(2);
					    } 

					    if (write(pool1fd,&mikos_2,INTLENGTH) == -1) {
						     perror("Error in Writing (pool to coord)");
						     exit(2);
					    }   	

					    if (write(pool1fd,&mikos_3,INTLENGTH) == -1) {
						     perror("Error in Writing (pool to coord)");
						     exit(2);
					    }   	  	  			    
  	  			    

					    if (write(pool1fd,results_1,mikos_1) == -1) {
						   perror("Error in Writing (pool to coord)");
						   exit(2);
					    }

					     if (write(pool1fd,results_2,mikos_2) == -1) {
						   perror("Error in Writing (pool to coord)");
						   exit(2);
					    } 

					    if (write(pool1fd,results_3,mikos_3) == -1) {
						   perror("Error in Writing (pool to coord)");
						   exit(2);
					    }   		     	  		     	
				  
					}
					else if(querybuf=='f'){

						//printf("I am going to status all process \n");
						int mikos_1;
						char* results_1;
						results_1 = ReturnQueue(Pool.finishingQueue);				
						mikos_1=strlen(results_1);		

						if (write(pool1fd,&mikos_1,INTLENGTH) == -1) {
						     perror("Error in Writing (pool to coord)");
						     exit(2);
					    } 
					  	    

					    if (write(pool1fd,results_1,mikos_1) == -1) {
						   perror("Error in Writing (pool to coord)");
						   exit(2);
					    }
		   				  
					}
                    else if(querybuf=='d'){
          
					 //printf("I am going to show active jobs\n");
					 char* results;//running  processes will be stored here in a printable format
					 results = ReturnLogic_ids(Pool.runningQueue);
					 int mikos =strlen(results);

                     // printf("mikos_pools:%d\n",mikos);
					  if (write(pool1fd,&mikos,INTLENGTH) == -1) {
						 perror("Error in Writing (pool to coord)");
						 exit(2);
					   }   		
                       // printf("results_pools:%s \n",results);
					   if (write(pool1fd,results,mikos) == -1) {
						 perror("Error in Writing (pool to coord)");
						 exit(2);
					   }   	
					}
					else if(querybuf=='i'){
						int garbage;
						queuePtr temp;
						while ((garbage = ReturnFirstId(Pool.runningQueue)) != 0)//Kill all processes running
						{
							printf("We delete(runningQueue) :%d\n", garbage);
							//kill(garbage, SIGKILL);
							temp=DeleteItem(&Pool.runningQueue, garbage);
						}
						while ((garbage = ReturnFirstId(Pool.pendingQueue)) != 0)//Kill all processes running
						{
							printf("We delete(pendingQueue) :%d\n", garbage);
							//kill(garbage, SIGKILL);
							temp=DeleteItem(&Pool.pendingQueue, garbage);
						}
						free(Pool.runningQueue);
						free(Pool.pendingQueue);	//free pendingQueue
						exit(0);
					}
			    }	
		    }	
}	


void JobDone() //When a process is terminated!
{
	queuePtr q1;
	int status;
	pid_t pid;
	pid = waitpid(WAIT_ANY, &status, WNOHANG); //get status of finished processs
	//printf("Pid_jobdone:%d\n",pid);
	//printf("N_job_done:%d\n",N);
	q1=DeleteItem(&Pool.runningQueue,N); //delete finished job from running queue
	Pool.finishingQueue=AddItem(Pool.finishingQueue,q1->jobId,q1->job,N);
	//char* finishing = ReturnQueue(Pool.finishingQueue);
	//printf("finishing :%s \n",finishing);
	signal(SIGCHLD, JobDone);               //reinstall handler
	if (CountItems(Pool.pendingQueue) > 0 && CountItems(Pool.runningQueue) < jobs_pool) //If there are items pending
	{		//and if I am have space to execute the with the current job_pools
		//Once again we do fork exec
		pid_t jobpid;
		char *argvs[MAX_ARGS];
		int i = 0;
		char *tok = strtok(ReturnDeleteFirstItem(&Pool.pendingQueue), " ");
		//printf("tok_jobdone:%s\n",tok);
		do
		{
			argvs[i] = (char*) malloc(strlen(tok) + 1);
			strcpy(argvs[i], tok);
			i++;
		} while ((tok = strtok(NULL, " ")));
		argvs[i] = NULL;
		//printf("Entoli_jobdone:%s\n",argvs[0]);
		//printf("orisma_jobdone:%s\n",argvs[1]);
		if ((jobpid = fork()) < 0)
		{
			perror("Error while creating jobExecutorServer: ");
			exit(1);
		}
		else if (jobpid == 0)
		{
			//fclose(stdout);
			//fclose(stderr);
			 strcpy(pid_directory,"/tmp/sdi1300017/jms_sdi");
			 strcat(pid_directory,getUserName());
			 strcat(pid_directory,"_");
			 sprintf(jobid,"%d",N);
			 strcat(pid_directory,jobid);
			 strcat(pid_directory,"_");
			 sprintf(linux_pid,"%d",getpid());
			 strcat(pid_directory,linux_pid);

			 strncpy(pid_arxeio,pid_directory,40);

	         // printf("pid_arxeio:%s\n",pid_arxeio);

	         strncpy(pid_arxeio_1,pid_arxeio,40);
	         strcat(pid_arxeio_1,"/");
	         strcpy(pid_stdout,"stdout_");
	         sprintf(jobid,"%d",N);
	         strcat(pid_stdout,jobid);
	         strcat(pid_arxeio_1,pid_stdout);
	       // printf("Prwto arxeio:%s\n",pid_arxeio_1);

	                              
	         strncpy(pid_arxeio_2,pid_arxeio,40);
	         strcat(pid_arxeio_2,"/");
	         strcpy(pid_stderr,"stderr_");
	         sprintf(jobid,"%d",N);
	         strcat(pid_stderr,jobid);
	         strcat(pid_arxeio_2,pid_stderr);
	       //printf("Deutero arxeio:%s\n",pid_arxeio_2);

	         fp=open(pid_arxeio_1,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	         ft=open(pid_arxeio_2,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);


		     dup2(fp,1);
			 dup2(ft,2);

		     close(fp);
			 close(ft);								


			if (execvp(argvs[0], argvs) < 0)
			{  //execute the command 
				printf("*** ERROR: exec failed\n");
				exit(1);
			}
			exit(1);
		}
		else{

			if ( (pool1fd = open("/tmp/sdi1300017/_poolfifo1", O_WRONLY)) < 0) {
		           perror("fifo open error");
		           exit(1);
            }


			if (write(pool1fd,&N,INTLENGTH) == -1) {
					perror("Error in Writing (pool to coord)");
					exit(2);
			}      
						    
		    if (write(pool1fd,&jobpid,INTLENGTH) == -1) {
					perror("Error in Writing (pool to coord)");
					exit(2);
		    }           
						       
			Pool.runningQueue = AddItem(Pool.runningQueue, jobpid,argvs[0],N); //add to currently running queue
			char* running=ReturnQueue(Pool.runningQueue);
			printf("Running:%s \n",running);
		}	
	}
}






