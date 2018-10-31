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
#include <libgen.h>

#define COMMAND 1
#define INTLENGTH 4

int main(int argc, char *argv[]){
	const char s[2] = " ";
	char *token;
	char *temptext;
	char* query;
	char* consolefifo1;
	char* consolefifo2;
	char command;
	char* text;
	int fd,msgsize;
	int bytesread;
	int tempbuf,logic_id,linux_pid;
	int templength, templength2;
	int cons1fd, cons2fd;
	int cons1bytes, cons2bytes;
	int bytesleft;
	int i,lsize,mikos_1,mikos_2,mikos_3;
	char* text1,*text2,*text3;
	int nwrite;
	char str[300];
	FILE* datafile;

	//signal(SIGINT, SIG_IGN);
    consolefifo1=malloc(30*sizeof(char));
    consolefifo2=malloc(30*sizeof(char));
	struct stat statbuf;

	if (argc < 4) {
		printf("Usage: Provide all arguments \n");
		exit(1);
	}

    for (i = 1; i < argc; i = i+2) {
		if (!strcmp(argv[i], "-w")) {

			strcpy(consolefifo1,argv[i+1]);

		} else if (!strcmp(argv[i], "-r")) {
          
          strcpy(consolefifo2,argv[i+1]);
			
			
		} else if (!strcmp(argv[i],"-o"))

		  datafile=fopen(argv[i+1],"rb");
		
	}
    

	if ( (cons1fd = open(consolefifo1, O_WRONLY)) < 0) {
			perror("fifo open error");
			exit(1);
	}
	if ( (cons2fd = open(consolefifo2, O_RDONLY | O_NONBLOCK)) < 0) {
			perror("fifo open error");
			exit(1);
	}

 
	while (fgets(str, sizeof(str), stdin) != NULL) {
		if (str[strlen(str) - 1] == '\n') {
      str[strlen(str) - 1] = '\0';
		}
		token = strtok(str, s);
       
		if (strcmp("submit", token) == 0) {
			command = 'a';
			
			//printf("Command:%c \n",command);
			token = strtok(NULL, "");
			//printf("Token: %s \n",token);
			templength = strlen(token);
			//printf("mikos: %d \n",templength);
			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}
			if ((nwrite = write(cons1fd,&templength, INTLENGTH)) == -1) {
					perror("Error in Writing (length of query)");
					exit(2);
			}

			if ((nwrite = write(cons1fd,token, templength)) == -1) {
					perror("Error in Writing (query)");
					exit(2);
			}

			sleep(5);

			if (read(cons2fd,&logic_id,INTLENGTH) == 4){

				 printf("message from coord received logic_id = %d\n", logic_id);
			}

			if ( (read(cons2fd, &linux_pid, INTLENGTH)) == 4) {

			     printf("message from coord received linux_pid = %d\n", linux_pid);

			}			
			
		}	

		else if (strcmp("suspend", token) == 0) {
			command='g';
			token = strtok(NULL, " ");
			logic_id=atoi(token);

			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}
			if ((nwrite = write(cons1fd, &logic_id,INTLENGTH )) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}		
			sleep(4);

			if (read(cons2fd,&logic_id,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}
			printf("Sent suspend signal to JobID = %d\n", logic_id);
		}

		else if (strcmp("resume", token) == 0) {
			command='j';
			token = strtok(NULL, " ");
			logic_id=atoi(token);

			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}
			if ((nwrite = write(cons1fd, &logic_id,INTLENGTH )) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}		
			sleep(4);

			if (read(cons2fd,&logic_id,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}
			printf("Sent resume signal to JobID = %d\n", logic_id);
		}

		else if (strcmp("show-active", token) == 0) {
			command='d';
			char* results;
		
			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}
			sleep(6);
			if (read(cons2fd,&msgsize,INTLENGTH) == 4){

				// printf("message from coord received msgsize = %d\n", msgsize);
			}
			// printf("message from coord received msgsize = %d\n", msgsize);

			text = malloc(msgsize + 1);
					
			
		    bytesleft = msgsize;

			while ((cons2bytes = read(cons2fd, text, msgsize)) > bytesleft) {
					if (cons2bytes > 0) {
							text += cons2bytes;
							bytesleft -= cons2bytes;
					}
			}
			text += cons2bytes;
			*text = '\0';
			text -= msgsize;

		    printf("Active Jobs with JobIds = %s\n", text);

		}


		else if (strcmp("status", token) == 0) {
			command='b';
			token = strtok(NULL, " ");
			logic_id=atoi(token);

			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}
			if ((nwrite = write(cons1fd, &logic_id,INTLENGTH )) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}		

			sleep(5);

			if (read(cons2fd,&command,1) == 1){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}

			if (read(cons2fd,&logic_id,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}

			if(command=='r')
			    printf("JobId %d status Active\n", logic_id);
			else if(command=='s')
				 printf("JobId %d status Suspended\n", logic_id);
		    else if(command=='f')
		    	 printf("JobId %d status Finished\n", logic_id);
		}

		else if (strcmp("status-all", token) == 0) {
			command='c';
			
			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}

			sleep(5);

			if (read(cons2fd,&mikos_1,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}
			if (read(cons2fd,&mikos_2,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}

			if (read(cons2fd,&mikos_3,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}

			text1 = malloc(mikos_1 + 1);
					
			
		    bytesleft = mikos_1;

			while ((cons2bytes = read(cons2fd, text1, mikos_1)) > bytesleft) {
					if (cons2bytes > 0) {
							text1 += cons2bytes;
							bytesleft -= cons2bytes;
					}
			}
			text1 += cons2bytes;
			*text1 = '\0';
			text1 -= mikos_1;
			printf("Active jobs are:%s\n",text1);



			text2 = malloc(mikos_2 + 1);
					
			
		    bytesleft = mikos_2;

			while ((cons2bytes = read(cons2fd, text2, mikos_2)) > bytesleft) {
					if (cons2bytes > 0) {
							text2 += cons2bytes;
							bytesleft -= cons2bytes;
					}
			}
			text2 += cons2bytes;
			*text2 = '\0';
			text2 -= mikos_2;
			printf("Suspended jobs are:%s\n",text2);

			text3 = malloc(mikos_3 + 1);
					
			
		    bytesleft = mikos_3;

			while ((cons2bytes = read(cons2fd, text3, mikos_3)) > bytesleft) {
					if (cons2bytes > 0) {
							text3 += cons2bytes;
							bytesleft -= cons2bytes;
					}
			}
			text3 += cons2bytes;
			*text3 = '\0';
			text3 -= mikos_3;
			printf("Finished jobs are:%s\n",text3);
		
		}
		else if (strcmp("show-finished", token) == 0) {
			command='f';
			
			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}

			sleep(5);

			if (read(cons2fd,&mikos_1,INTLENGTH) == 4){

				 //printf("Sent suspend signal to JobID = %d\n", logic_id);
			}
			text1 = malloc(mikos_1 + 1);
					
			
		    bytesleft = mikos_1;

			while ((cons2bytes = read(cons2fd, text1, mikos_1)) > bytesleft) {
					if (cons2bytes > 0) {
							text1 += cons2bytes;
							bytesleft -= cons2bytes;
					}
			}
			text1 += cons2bytes;
			*text1 = '\0';
			text1 -= mikos_1;
			printf("Finished jobs are:%s\n",text1);
	
		}
		else if (strcmp("shutdown", token) == 0) {
			command = 'i';
			if ((nwrite = write(cons1fd, &command, 1)) == -1) {
					perror("Error in Writing (console)");
					exit(2);
			}
			break;
		}
		else {
			printf("Wrong command\n");
		}
	}
  
	exit(0);
}
