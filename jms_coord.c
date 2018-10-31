#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>


#define COMMAND 1
#define INTLENGTH 4


int main(int argc, char *argv[]){
	int pidfd, dirfd;
	pid_t pid;
	int fd,st;
	int channelid;
	int sendchann;
	int byteswriten;
	int msgsize, filesize;
	int bytesleft, bytesread;
	int templength, templength2;
	int cons1fd, cons2fd, pool1fd, pool2fd;
	int cons1bytes, cons2bytes, pool1bytes, pool2bytes,mikos_1,mikos_2,mikos_3;
	char querybuf,command;
	int i,nwrite;
	int status,logic_id,linux_pid;
	char *userpath;
	char *text, *text2,*consolefifo1,*consolefifo2,*text1,*text3;
	DIR *dir_ptr;
	char* jobs;
	//struct stat st = {0};
    userpath=malloc(30*sizeof(char));
    consolefifo1=malloc(30*sizeof(char));
    consolefifo2=malloc(30*sizeof(char));
    jobs=malloc(10*sizeof(char));
   
	//	Check if the arguments number is valid
	if (argc < 5) {
		printf("Usage: Provide all arguments\n");
		exit(1);
	}

	// if (signal(SIGINT, SIG_IGN) != SIG_IGN)
  //   signal(SIGINT, ignore);
	//signal(SIGINT, SIG_IGN);


    for (i = 1; i < argc; i = i+2) {

		if (!strcmp(argv[i], "-l")) {
			strcpy(userpath,argv[i+1]);

		} else if (!strcmp(argv[i], "-r")) {
          
          strcpy(consolefifo1,argv[i+1]);
			
			
		} else if (!strcmp(argv[i],"-n")){
          
         strcpy(jobs,argv[i+1]);

	    } else if(!strcmp(argv[i],"-w"))

	      strcpy(consolefifo2,argv[i+1]);	  
		
	}
    
    st=mkdir(userpath,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if ((dir_ptr = opendir(userpath)) == NULL) {
		fprintf(stderr, "cannot open or create %s\n", userpath);
		exit(1);
	}


    if ( mkfifo(consolefifo1, 0666) == -1 ){
				if ( errno != EEXIST ) {
					perror("receiver: consolefifo1");
					exit(6); };
	}
	if ( mkfifo(consolefifo2, 0666) == -1 ){
				if ( errno != EEXIST ) {
					perror("receiver: consolefifo2");
					exit(6); };
	}
	if ( mkfifo("/tmp/sdi1300017/_poolfifo1", 0666) == -1 ){
				if ( errno != EEXIST ) {
					perror("receiver: poolfifo1");
					exit(6); };
	}
	if ( mkfifo("/tmp/sdi1300017/_poolfifo2", 0666) == -1 ){
				if ( errno != EEXIST ) {
					perror("receiver: poolfifo2");
					exit(6); };
	}


	if ( (cons1fd = open(consolefifo1, O_RDONLY | O_NONBLOCK)) < 0){
				perror("consolefifo1 open problem");
				exit(3);
				}
	if ( (pool1fd = open("/tmp/sdi1300017/_poolfifo1", O_RDONLY | O_NONBLOCK)) < 0){
				perror("poolfifo1 open problem");
				exit(3);
				}

			
	strcat(userpath,"pid");
	if ((dirfd = open(userpath, O_RDONLY)) == -1) {

		dirfd = creat(userpath, 0777);
    }
		printf("Entering\n");

		if((pid = fork())==-1){
			printf("failed to fork\n");
			exit(1);
		}
		if(pid==0){ //child process aka pool Process

		  if(execl("./pools","pools",jobs,(char*)NULL) < 0){
						
				perror("exec");
				exit(1);

		   }else{	
			  printf("ok  exec pools\n");
		   }
		}

		if(pid){
		
			pidfd = open("/tmp/sdi1300017/pid", O_WRONLY);
			char temp[4];
			sprintf(temp, "%d",pid);
			write(pidfd, temp, 4);
			close(pidfd);
    
			
			for (;;) {


				if ((cons1bytes = read(cons1fd, &querybuf, COMMAND)) == 1) {
				   /*	if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){
						perror("consolefifo2 open problem");
						exit(3);
					}*/
					
					if ( (pool2fd = open("/tmp/sdi1300017/_poolfifo2", O_WRONLY)) < 0){
						perror("poolfifo2 open problem");
						exit(3);
					}

				//	printf("coord entolicood:%c \n",querybuf);

					if (querybuf == 'a') {
						
					
						if ( (read(cons1fd, &msgsize, INTLENGTH)) == 4) {

							 //printf("message from console received length = %d\n", msgsize);
						}

                      
						text = malloc(msgsize + 1);
					
			
						bytesleft = msgsize;

						while ((cons1bytes = read(cons1fd, text, msgsize)) > bytesleft) {
							if (cons1bytes > 0) {
								text += cons1bytes;
								bytesleft -= cons1bytes;
							}
						}
						text += cons1bytes;
						*text = '\0';
						text -= msgsize;
						

						//printf("Textcoord: %s \n",text);

						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }

			            if (write(pool2fd,&msgsize, INTLENGTH) == -1) {
								perror("Error in Writing (server to pool)");
								exit(2);
						}
                        

						if (write(pool2fd,text, msgsize) == -1) {
								perror("Error in Writing (server to pool)");
								exit(2);
						}

						sleep(3);

						if ((pool1bytes = read(pool1fd, &logic_id,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}
					    
						      		      

						      if ( (read(pool1fd, &linux_pid, INTLENGTH)) == 4) {

							    // printf("message from pools received linux_pid = %d\n", linux_pid);
						      }
						      
						     
						     if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){

						          perror("consolefifo2 open problem");
						          exit(3);
					          }						      			      

			                  if (write(cons2fd,&logic_id, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						       }
                        

						       if (write(cons2fd,&linux_pid, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						       }
						       										 	 
					 }
					 else if(querybuf=='g'){
	
					 	if ( (read(cons1fd, &logic_id, INTLENGTH)) == 4) {

							 //printf("message from console received logic_id(suspend) = %d\n", logic_id);
						}

						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }

						 if (write(pool2fd,&logic_id, INTLENGTH) == -1) {
								perror("Error in Writing (coord to pool)");
								exit(2);
						}

						sleep(2);
       
						if ((pool1bytes = read(pool1fd, &logic_id,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}



						 if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){

						          perror("consolefifo2 open problem");
						          exit(3);
					     }	

					     if (write(cons2fd,&logic_id, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }				      	
					 }	
					 else if(querybuf=='j'){
	
					 	if ( (read(cons1fd, &logic_id, INTLENGTH)) == 4) {

							 //printf("message from console received logic_id(suspend) = %d\n", logic_id);
						}

						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }

						 if (write(pool2fd,&logic_id, INTLENGTH) == -1) {
								perror("Error in Writing (coord to pool)");
								exit(2);
						}

						sleep(2);
       
						if ((pool1bytes = read(pool1fd, &logic_id,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}



						 if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){

						          perror("consolefifo2 open problem");
						          exit(3);
					     }	

					     if (write(cons2fd,&logic_id, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }				      	
					 }	
					  else if(querybuf=='b'){
	
					 	if ( (read(cons1fd, &logic_id, INTLENGTH)) == 4) {

							 //printf("message from console received logic_id(suspend) = %d\n", logic_id);
						}

						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }

						 if (write(pool2fd,&logic_id, INTLENGTH) == -1) {
								perror("Error in Writing (coord to pool)");
								exit(2);
						}

						sleep(3);

						if ((pool1bytes = read(pool1fd, &command,1)) ==1){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}
						

						if ((pool1bytes = read(pool1fd, &logic_id,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}


						 if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){

						          perror("consolefifo2 open problem");
						          exit(3);
					     }	

					     if (write(cons2fd,&command, 1) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }				      	

					     if (write(cons2fd,&logic_id, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }				      	
					 }	
					 else if(querybuf=='c'){
	
						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }

						sleep(3);

						if ((pool1bytes = read(pool1fd,&mikos_1,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}
						

						if ((pool1bytes = read(pool1fd, &mikos_2,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}

						if ((pool1bytes = read(pool1fd, &mikos_3,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}


						text1 = malloc(mikos_1 + 1);
					
			
						bytesleft = mikos_1;

						while ((pool1bytes = read(pool1fd, text1, mikos_1)) > bytesleft) {
							if (pool1bytes > 0) {
								text1 += pool1bytes;
								bytesleft -= pool1bytes;
							}
						}
						text1 += pool1bytes;
						*text1 = '\0';
						text1 -= mikos_1;


						text2 = malloc(mikos_2 + 1);
					
			
						bytesleft = mikos_2;

						while ((pool1bytes = read(pool1fd, text2, mikos_1)) > bytesleft) {
							if (pool1bytes > 0) {
								text2 += pool1bytes;
								bytesleft -= pool1bytes;
							}
						}
						text2 += pool1bytes;
						*text2 = '\0';
						text2 -= mikos_2;


						text3 = malloc(mikos_3 + 1);
					
			
						bytesleft = mikos_3;

						while ((pool1bytes = read(pool1fd, text3, mikos_3)) > bytesleft) {
							if (pool1bytes > 0) {
								text3 += pool1bytes;
								bytesleft -= pool1bytes;
							}
						}
						text3 += pool1bytes;
						*text3 = '\0';
						text3 -= mikos_3;


						 if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){

						          perror("consolefifo2 open problem");
						          exit(3);
					     }	


					     if (write(cons2fd,&mikos_1, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }		

						  if (write(cons2fd,&mikos_2, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }

						   if (write(cons2fd,&mikos_3, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }

						  if (write(cons2fd,text1, mikos_1) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }	

						  if (write(cons2fd,text2, mikos_2) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }

						   if (write(cons2fd,text3, mikos_3) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }							 							    		 			    		      	
					 }
					  else if(querybuf=='f'){
	
						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }

						sleep(3);

						if ((pool1bytes = read(pool1fd,&mikos_1,INTLENGTH)) ==4){

							//printf("message from pools received logic_id = %d\n", logic_id);
						}
						

					


						text1 = malloc(mikos_1 + 1);
					
			
						bytesleft = mikos_1;

						while ((pool1bytes = read(pool1fd, text1, mikos_1)) > bytesleft) {
							if (pool1bytes > 0) {
								text1 += pool1bytes;
								bytesleft -= pool1bytes;
							}
						}
						text1 += pool1bytes;
						*text1 = '\0';
						text1 -= mikos_1;
			

						 if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){

						          perror("consolefifo2 open problem");
						          exit(3);
					     }	


					     if (write(cons2fd,&mikos_1, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }		

						 
						  if (write(cons2fd,text1, mikos_1) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }	
				    		 			    		      	
					 }	
                     else if(querybuf=='d'){

				 	
						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }			

			            sleep(3);		
                         
						if ((pool1bytes = read(pool1fd,&msgsize,INTLENGTH)) ==4){

						}	

						//printf("coord_msgsize:%d\n",msgsize);

						text = malloc(msgsize + 1);
					
			
						bytesleft = msgsize;

						while ((pool1bytes = read(pool1fd, text, msgsize)) > bytesleft) {
							if (pool1bytes > 0) {
								text += pool1bytes;
								bytesleft -= pool1bytes;
							}
						}
						text += pool1bytes;
						*text = '\0';
						text -= msgsize;

                      // printf("results_coord:%s\n",text);

										
						if ( (cons2fd = open(consolefifo2, O_WRONLY)) < 0){
						         perror("consolefifo2 open problem");
						         exit(3);
					      }	

					     if (write(cons2fd,&msgsize, INTLENGTH) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }	

						 if (write(cons2fd,text, msgsize) == -1) {

								  perror("Error in Writing (coord to console)");
								  exit(2);
						  }			

					 }	
					else if (querybuf == 'i') {

						if ( write(pool2fd, &querybuf, 1) == -1) {
					           perror("Error in Writing (console)");
					           exit(2);
			            }
			            sleep(2);	            
						close(cons1fd);
						close(cons2fd);
						close(pool1fd);
						close(pool2fd);
						unlink(consolefifo1);
						unlink(consolefifo2);
						unlink("/tmp/sdi1300017/_poolfifo1");
						unlink("/tmp/sdi1300017/_poolfifo2");
						break;
					}

				}
	        }   
        }

	 exit(0);	 
}
