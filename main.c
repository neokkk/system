#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <input_process.h>
#include <system_process.h>
#include <web_process.h>

#define PROCESS_NUM 3

static pid_t pids[PROCESS_NUM];
static pid_t (*p_funcs[])() = {create_system_process, create_web_process, create_input_process};

void sigchld_handler(int sig)
{
	int status, saved_errno;
	pid_t child_pid;

	saved_errno = errno;

	while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0) {
		printf("SIGCHLD(%d) caught\n", child_pid);
	}

	if (child_pid < 0 && errno != ECHILD) {
		perror("fail to wait child process");
		exit(1);
	}

	errno = saved_errno;
}

int main()
{
	signal(SIGCHLD, sigchld_handler);

	for (int i = 0; i < PROCESS_NUM; i++) {
		pids[i] = p_funcs[i]();
	}

	for (int i = 0; i < PROCESS_NUM; i++) {
		if (waitpid(pids[i], NULL, 0) < 0) {
			perror("fail to wait child process");
			exit(1);
		}
	}

	return 0;
}