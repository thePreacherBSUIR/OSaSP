#define SLEEP 100 * 1000
#define COUNT_CHILDS 2
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int Mssg_count = 0;
pid_t Childs[COUNT_CHILDS];

long getCurrTime() {
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		perror("Can not get current time\n");
		return -1;
	}

	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void Parent_handler(int sig, siginfo_t* siginfo, void* code)
{
	printf("%d PID: %d PPID: %d Current time: %ld Parent get SIGUSR2 from %d\n", Mssg_count++, getpid(), getppid(), getCurrTime(), siginfo->si_pid);

	usleep(SLEEP);

	if (kill(0, SIGUSR1) == -1)
	{
		perror("kill");
		exit(-1);
	}

	printf("%d PID: %d PPID: %d Current time: %ld Parent put SIGUSR1\n", Mssg_count++, getpid(), getppid(), getCurrTime());
}

void Child_handler(int sig, siginfo_t* siginfo, void* code)
{
	pid_t pid = getpid();
	pid_t ppid = getppid();

	printf("%d PID: %d PPID: %d Current time: %ld Child%d get SIGUSR1\n", Mssg_count++, pid, ppid, getCurrTime(), pid == Childs[0] ? 1 : 2);

	if (kill(ppid, SIGUSR2) == -1)
	{
		perror("kill");
		exit(-1);
	}

	printf("%d PID: %d PPID: %d Current time: %ld Child%d put SIGUSR2\n", Mssg_count++, pid, ppid, getCurrTime(), pid == Childs[0] ? 1 : 2);
}

int main()
{
	sigset_t USR1, USR2;

	if (sigemptyset(&USR1) == -1)
	{
		perror("sigemptyset");
		exit(-1);
	}
	if (sigaddset(&USR1, SIGUSR1) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}

	if (sigemptyset(&USR2) == -1)
	{
		perror("sigemptyset");
		exit(-1);
	}
	if (sigaddset(&USR2, SIGUSR2) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}

	struct sigaction Parent_action;
	Parent_action.sa_sigaction = Parent_handler;
	Parent_action.sa_mask = USR2;
	Parent_action.sa_flags = SA_SIGINFO;

	if (sigaction(SIGUSR2, &Parent_action, 0) == -1)
	{
		perror("sigaction");
		exit(-1);
	}

	struct sigaction Child_action;
	Child_action.sa_sigaction = Child_handler;
	Child_action.sa_mask = USR1;
	Child_action.sa_flags = SA_SIGINFO;

	printf("Parent PID: %d PPID: %d Current time: %ld\n", getpid(), getppid(), getCurrTime());


	for (int i = 0; i < COUNT_CHILDS; i++)
	{
		Childs[i] = fork();

		switch (Childs[i])
		{
		case 0:
		{
			Childs[i] = getpid();
			if (sigprocmask(SIG_SETMASK, &USR2, 0) == -1)
			{
				perror("sigprocmask");
				exit(-1);
			}

			if (sigaction(SIGUSR1, &Child_action, 0) == -1)
			{
				perror("sigaction");
				exit(-1);
			}

			printf("Child%d PID: %d PPID: %d Current time: %ld\n", i + 1, getpid(), getppid(), getCurrTime());
			while (1) {}
			break;
		}

		case -1:
		{
			perror("A child cannot be created.\n");
			exit(-1);
		}

		}
	}

	if (sigprocmask(SIG_SETMASK, &USR1, 0) == -1)
	{
		perror("sigprocmask");
		exit(-1);
	}

	sleep(1);

	if (kill(0, SIGUSR1) == -1)
	{
		perror("kill");
		exit(-1);
	}

	while (1) {}

	return 0;
}