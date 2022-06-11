#define COUNT_CHILDS 8
#define SIGNAL 101
#define FILE_NAME "PID"
#include <sys/types.h>
#include <signal.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/time.h> 
#include <string.h>
#include <sys/wait.h> 
#include <unistd.h> 

typedef struct _Child_t {
	int parent;
	int* childs;
	int count_of_childs;
	int count_of_signals;
	struct sigaction action;
	struct sigaction action_Term;
	int* receive_Signal;
	sigset_t ignore_Signals;
} Child_t;

int countUSR1 = 0, countUSR2 = 0;
Child_t Childs[COUNT_CHILDS + 1];


void Initialization(void) {

	FILE* fp = fopen(FILE_NAME, "w+b");

	if (fp == NULL)
	{
		perror("Can not open temp file");
		exit(-1);
	}

	int initial = 0;
	for (int i = 0; i <= COUNT_CHILDS; i++)
	{
		fwrite(&initial, sizeof(int), 1, fp);
	}

	if (fclose(fp) == EOF)
	{
		perror("Can not close temp file");
		exit(-1);
	}

	return;
}

int* readAllPID(void)
{
	FILE* fp = fopen(FILE_NAME, "rb");

	if (fp == NULL)
	{
		perror("Can not open temp file\n");
		exit(-1);
	}

	int* pids = (int*)malloc((COUNT_CHILDS + 1) * sizeof(int));
	if (pids == NULL)
	{
		perror("Can not allocate memory");
		exit(-1);
	}

	fread(pids, sizeof(int), COUNT_CHILDS + 1, fp);

	if (fclose(fp) == EOF)
	{
		perror("Can not close temp file");
		exit(-1);
	}

	return pids;
}


void writePID(int ordChild, pid_t newPID)
{
	int* pids = readAllPID();
	pids[ordChild] = newPID;

	FILE* fp = fopen(FILE_NAME, "w+b");
	if (fp == NULL)
	{
		perror("Can not open temp file");
		exit(-1);
	}

	fwrite(pids, sizeof(int), COUNT_CHILDS + 1, fp);

	if (fclose(fp) == EOF)
	{
		perror("Can not close temp file");
		exit(-1);
	}

	free(pids);

	return;
}


pid_t readPID(int ordChild)
{
	int* pids = readAllPID();
	pid_t pid = pids[ordChild];
	free(pids);

	return pid;
}

long long getCurrTime()
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		perror("Can not get current time");
		return -1;
	}

	return tv.tv_sec * 1000000 + tv.tv_usec;
}

void Output_inf(int Num, char* act, int sigNum)
{
	printf("¹ %d PID: %d PPID: %d %8s SIGUSR%d Time: %lld \n", Num, getpid(), getppid(), act, sigNum, getCurrTime());
}

void Terminate(int Num)
{
	printf("¹ %d PID: %d PPID: %d completed work after %ds SIGUSR1 and %ds SIGUSR2\n", Num, getpid(), getppid(), countUSR1, countUSR2);
}

void Child1_handler(int sig, siginfo_t* siginfo, void* code)
{
	Output_inf(1, "receive", 2);

	if (countUSR1 + countUSR2 == SIGNAL) {

		sleep(1);
		if (kill(readPID(6), SIGTERM) == -1)
		{
			perror("kill");
			exit(-1);
		}

		for (int j = 0; j < Childs[1].count_of_childs; j++)
		{
			if (waitpid(readPID(Childs[1].childs[j]), NULL, 0) == -1)
			{
				perror("waitpid");
				exit(-1);
			}

		}

		Terminate(1);
		exit(0);
	}
	else
	{
		if (kill(readPID(6), SIGUSR1) == -1)
		{
			perror("kill");
			exit(-1);
		}
		else
			Output_inf(1, "sent", 1);

		countUSR1++;
	}
}

void Child2_handler(int sig, siginfo_t* siginfo, void* code)
{

	if (kill(readPID(1), SIGUSR2) == -1)
	{
		perror("kill");
		exit(-1);
	}
	else
		Output_inf(2, "sent", 2);

	countUSR2++;
}

void Child4_handler(int sig, siginfo_t* siginfo, void* code)
{
	Output_inf(4, "receive", 2);

	if (kill(readPID(8), SIGUSR1) == -1)
	{
		perror("kill");
		exit(-1);
	}
	else
		Output_inf(4, "sent", 1);

	countUSR1++;
}

void Child5_handler(int sig, siginfo_t* siginfo, void* code)
{
	Output_inf(5, "receive", 2);

	if (kill(readPID(2), SIGUSR1) == -1)
	{
		perror("kill");
		exit(-1);
	}
	else
		Output_inf(5, "sent", 1);

	countUSR1++;

	Output_inf(2, "receive", 1);
}

void Child6_handler(int sig, siginfo_t* siginfo, void* code)
{
	Output_inf(6, "receive", 1);

	if (kill(readPID(7), SIGUSR1) == -1)
	{
		perror("kill");
		exit(-1);
	}
	else
		Output_inf(6, "sent", 1);

	countUSR1++;
}

void Child7_handler(int sig, siginfo_t* siginfo, void* code)
{
	Output_inf(7, "receive", 1);

	if (kill(-readPID(4), SIGUSR2) == -1)
	{
		perror("kill");
		exit(-1);
	}
	else
		Output_inf(7, "sent", 2);

	countUSR2++;
}

void Child8_handler(int sig, siginfo_t* siginfo, void* code)
{
	Output_inf(8, "receive", 1);

	if (kill(readPID(2), SIGUSR2) == -1)
	{
		perror("kill");
		exit(-1);
	}
	else
		Output_inf(8, "sent", 2);

	countUSR2++;

	Output_inf(2, "receive", 2);
}

void Child2_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	Terminate(2);
	exit(0);
}

void Child3_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	if (waitpid(readPID(4), NULL, 0) == -1)
	{
		perror("waitpid");
		exit(-1);
	}

	Terminate(3);
	exit(0);
}

void Child4_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	if (kill(readPID(8), SIGTERM) == -1)
	{
		perror("kill");
		exit(-1);
	}

	for (int j = 0; j < Childs[4].count_of_childs; j++)
	{
		if (waitpid(readPID(Childs[4].childs[j]), NULL, 0) == -1)
		{
			perror("waitpid");
			exit(-1);
		}

	}

	Terminate(4);
	exit(0);
}

void Child5_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	if (kill(readPID(2), SIGTERM) == -1)
	{
		perror("kill");
		exit(-1);
	}

	Terminate(5);
	exit(0);
}

void Child6_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	if (kill(readPID(7), SIGTERM) == -1)
	{
		perror("kill");
		exit(-1);
	}

	if (waitpid(readPID(7), NULL, 0) == -1)
	{
		perror("waitpid");
		exit(-1);
	}

	Terminate(6);
	exit(0);
}

void Child7_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	if (kill(-readPID(4), SIGTERM) == -1)
	{
		perror("kill");
		exit(-1);
	}

	if (waitpid(readPID(8), NULL, 0) == -1)
	{
		perror("waitpid");
		exit(-1);
	}

	Terminate(7);
	exit(0);
}

void Child8_handlerTerm(int sig, siginfo_t* siginfo, void* code)
{
	if (kill(readPID(3), SIGTERM) == -1)
	{
		perror("kill");
		exit(-1);
	}

	Terminate(8);
	exit(0);
}

void Childs_Items(Child_t* Tree_node)
{
	for (int i = 0; i <= COUNT_CHILDS; i++)
	{
		memset(&Tree_node[i], 0, sizeof(Tree_node[i]));
		Tree_node[i].parent = i;
		Tree_node[i].action.sa_flags = SA_SIGINFO;
		Tree_node[i].action_Term.sa_flags = SA_SIGINFO;
	}

	sigset_t USR1, USR2, AllSig, USR1_2;

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

	if (sigemptyset(&AllSig) == -1)
	{
		perror("sigemptyset");
		exit(-1);
	}
	if (sigaddset(&AllSig, SIGUSR1) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}
	if (sigaddset(&AllSig, SIGUSR2) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}
	if (sigaddset(&AllSig, SIGTERM) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}

	if (sigemptyset(&USR1_2) == -1)
	{
		perror("sigemptyset");
		exit(-1);
	}
	if (sigaddset(&USR1_2, SIGUSR1) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}
	if (sigaddset(&USR1_2, SIGUSR2) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}

	sigset_t TERM;

	if (sigemptyset(&TERM) == -1)
	{
		perror("sigemptyset");
		exit(-1);
	}
	if (sigaddset(&TERM, SIGTERM) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}


	Tree_node[0].count_of_childs = 1;
	Tree_node[0].childs = malloc(sizeof(int) * Tree_node[0].count_of_childs);
	if (Tree_node[0].childs == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[0].childs[0] = 1;
	Tree_node[0].count_of_signals = 0;
	Tree_node[0].ignore_Signals = AllSig;

	Tree_node[1].count_of_childs = 2;
	Tree_node[1].childs = malloc(sizeof(int) * Tree_node[1].count_of_childs);
	if (Tree_node[1].childs == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[1].childs[0] = 2;
	Tree_node[1].childs[1] = 3;
	Tree_node[1].count_of_signals = 1;
	Tree_node[1].receive_Signal = malloc(sizeof(int) * Tree_node[1].count_of_signals);
	if (Tree_node[1].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[1].receive_Signal[0] = SIGUSR2;
	Tree_node[1].action.sa_mask = USR2;
	Tree_node[1].ignore_Signals = USR1;
	if (sigaddset(&Tree_node[1].ignore_Signals, SIGTERM) == -1)
	{
		perror("sigaddset");
		exit(-1);
	}
	Tree_node[1].action.sa_sigaction = Child1_handler;

	Tree_node[2].count_of_childs = 0;
	Tree_node[2].count_of_signals = 2;
	Tree_node[2].receive_Signal = malloc(sizeof(int) * Tree_node[2].count_of_signals);
	if (Tree_node[2].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[2].receive_Signal[0] = SIGUSR2;
	Tree_node[2].receive_Signal[1] = SIGUSR1;
	Tree_node[2].action.sa_mask = USR1_2;
	Tree_node[2].action_Term.sa_mask = TERM;
	Tree_node[2].action.sa_sigaction = Child2_handler;
	Tree_node[2].action_Term.sa_sigaction = Child2_handlerTerm;

	Tree_node[3].count_of_childs = 1;
	Tree_node[3].childs = malloc(sizeof(int) * Tree_node[3].count_of_childs);
	if (Tree_node[3].childs == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[3].childs[0] = 4;
	Tree_node[3].count_of_signals = 0;
	Tree_node[3].ignore_Signals = USR1_2;
	Tree_node[3].action_Term.sa_mask = TERM;
	Tree_node[3].action_Term.sa_sigaction = Child3_handlerTerm;

	Tree_node[4].count_of_childs = 2;
	Tree_node[4].childs = malloc(sizeof(int) * Tree_node[4].count_of_childs);
	if (Tree_node[4].childs == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[4].childs[0] = 5;
	Tree_node[4].childs[1] = 6;
	Tree_node[4].count_of_signals = 1;
	Tree_node[4].receive_Signal = malloc(sizeof(int) * Tree_node[4].count_of_signals);
	if (Tree_node[4].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[4].receive_Signal[0] = SIGUSR2;
	Tree_node[4].action.sa_mask = USR2;
	Tree_node[4].action_Term.sa_mask = TERM;
	Tree_node[4].ignore_Signals = USR1;
	Tree_node[4].action.sa_sigaction = Child4_handler;
	Tree_node[4].action_Term.sa_sigaction = Child4_handlerTerm;

	Tree_node[5].count_of_childs = 0;
	Tree_node[5].count_of_signals = 1;
	Tree_node[5].receive_Signal = malloc(sizeof(int) * Tree_node[5].count_of_signals);
	if (Tree_node[5].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[5].receive_Signal[0] = SIGUSR2;
	Tree_node[5].action.sa_mask = USR2;
	Tree_node[5].action_Term.sa_mask = TERM;
	Tree_node[5].ignore_Signals = USR1;
	Tree_node[5].action.sa_sigaction = Child5_handler;
	Tree_node[5].action_Term.sa_sigaction = Child5_handlerTerm;

	Tree_node[6].count_of_childs = 1;
	Tree_node[6].childs = malloc(sizeof(int) * Tree_node[6].count_of_childs);
	if (Tree_node[6].childs == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[6].childs[0] = 7;
	Tree_node[6].count_of_signals = 1;
	Tree_node[6].receive_Signal = malloc(sizeof(int) * Tree_node[6].count_of_signals);
	if (Tree_node[6].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[6].receive_Signal[0] = SIGUSR1;
	Tree_node[6].action.sa_mask = USR1;
	Tree_node[6].action_Term.sa_mask = TERM;
	Tree_node[6].ignore_Signals = USR2;
	Tree_node[6].action.sa_sigaction = Child6_handler;
	Tree_node[6].action_Term.sa_sigaction = Child6_handlerTerm;

	Tree_node[7].count_of_childs = 1;
	Tree_node[7].childs = malloc(sizeof(int) * Tree_node[7].count_of_childs);
	if (Tree_node[7].childs == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[7].childs[0] = 8;
	Tree_node[7].count_of_signals = 1;
	Tree_node[7].receive_Signal = malloc(sizeof(int) * Tree_node[7].count_of_signals);
	if (Tree_node[7].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[7].receive_Signal[0] = SIGUSR1;
	Tree_node[7].action.sa_mask = USR1;
	Tree_node[7].action_Term.sa_mask = TERM;
	Tree_node[7].ignore_Signals = USR2;
	Tree_node[7].action.sa_sigaction = Child7_handler;
	Tree_node[7].action_Term.sa_sigaction = Child7_handlerTerm;

	Tree_node[8].count_of_childs = 0;
	Tree_node[8].count_of_signals = 1;
	Tree_node[8].receive_Signal = malloc(sizeof(int) * Tree_node[8].count_of_signals);
	if (Tree_node[8].receive_Signal == NULL)
	{
		perror("malloc");
		exit(-1);
	}
	Tree_node[8].receive_Signal[0] = SIGUSR1;
	Tree_node[8].action.sa_mask = USR1;
	Tree_node[8].action_Term.sa_mask = TERM;
	Tree_node[8].ignore_Signals = USR2;
	Tree_node[8].action.sa_sigaction = Child8_handler;
	Tree_node[8].action_Term.sa_sigaction = Child8_handlerTerm;
}

void Process_Tree(Child_t Tree_node)
{
	if (Tree_node.parent != 4 && Tree_node.parent != 5)
	{
		if (setpgid(readPID(Tree_node.parent), readPID(Tree_node.parent)) == -1)
		{
			perror("setpgid");
			exit(-1);
		}
	}

	if (Tree_node.parent == 4 || Tree_node.parent == 5)
	{
		if (setpgid(readPID(Tree_node.parent), readPID(4)) == -1)
		{
			perror("setpgid");
			exit(-1);
		}
	}

	printf("PID: %d PPID: %d Time: %lld CHILD%d\n", getpid(), getppid(), getCurrTime(), Tree_node.parent);

	if (sigprocmask(SIG_SETMASK, &Tree_node.ignore_Signals, 0) == -1)
	{
		perror("sigprocmask");
		exit(-1);
	}

	if (Tree_node.parent != 0 || Tree_node.parent != 3)
	{
		for (int k = 0; k < Tree_node.count_of_signals; k++)
		{
			if (sigaction(Tree_node.receive_Signal[k], &Tree_node.action, 0) == -1)
			{
				perror("sigation");
				exit(-1);
			}
		}

	}

	if (Tree_node.parent > 1)
	{
		if (sigaction(SIGTERM, &Tree_node.action_Term, 0) == -1)
		{
			perror("sigation1");
			exit(-1);
		}
	}


	writePID(Tree_node.parent, getpid());


	for (int i = 0; i < Tree_node.count_of_childs; i++)
	{
		while (readPID(Tree_node.childs[i] - 1) == 0) {}


		pid_t child = fork();

		switch (child)
		{
		case 0:
		{
			Process_Tree(Childs[Tree_node.childs[i]]);
			while (1) {}
			break;
		}

		case -1:
		{
			perror("A child cannot be created\n");
			exit(-1);
		}

		default:
		{
			if (Tree_node.parent == 1 && i + 1 == Tree_node.count_of_childs)
			{
				for (int i = 0; i <= COUNT_CHILDS; i++)
				{
					while (readPID(i) == 0) {}
				}

				if (kill(readPID(6), SIGUSR1) == -1)
				{
					perror("kill");
					exit(-1);
				}

				Output_inf(1, "sent", 1);
				countUSR1++;

				while (1) {}
			}
			break;
		}

		}
	}

	return;
}

int main()
{
	Initialization();

	Childs_Items(Childs);

	Process_Tree(Childs[0]);

	while (readPID(1) == 0) {}
	if (waitpid(readPID(1), NULL, 0) == -1)
	{
		perror("waitpid");
		exit(-1);
	}

	Terminate(0);

	return 0;
}