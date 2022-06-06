#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>

void CurrentTime();

int main()
{
	pid_t child_pid;
	
	printf("\nparent: PID %d, parent PID %d", getpid(), getppid());
	
	CurrentTime();
	
	for(int i = 1; i <= 2; i++) {
	
		child_pid = fork();
		
		switch(child_pid) {
		
			case 0:
				printf("child %d: PID %d, parent PID %d", i, getpid(), getppid());
				CurrentTime();
				exit(EXIT_SUCCESS);
				
			case -1: 
				fprintf(stderr, "error! cannot create the child process\n");
				exit(EXIT_FAILURE);
			
			default:
				break;		
		}
	}
			 
	if(system("ps -x") == -1)
		perror("system");
		
	while(wait(NULL) != -1);		
	
	printf ("\n");
	
	return 0;
}

void CurrentTime()
{
	struct timeval time;
	
	if(gettimeofday(&time, NULL) == -1) {
		fprintf(stderr, "error! cannot calculate the current time\n");
	}
	else {		
		int hour = (time.tv_sec / 3600) % 24;		
		int min = (time.tv_sec / 60) % 60;		
		int sec = time.tv_sec % 60;
		int msec = time.tv_usec / 1000;
		
		printf("\ntime: %02d:%02d:%02d:%03d (GMT)\n\n", hour, min, sec, msec);
	}
}
