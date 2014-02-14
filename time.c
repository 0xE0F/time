#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <spawn.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

extern char **environ;
enum { ARGS_COUNT_MAX = 32, NANO_IN_SEC = 1000000000 };

/* Calc difference between two time
 * @param start - start time point
 * @param end 	- end time point
 * @return - diff between two time point
 * */
static struct timespec diff(struct timespec *start, struct timespec *end)
{
	struct timespec temp;

	assert(start != NULL);
	assert(end != NULL);

	if ((end->tv_nsec - start->tv_nsec) < 0) {
		temp.tv_sec = end->tv_sec - start->tv_sec - 1;
		temp.tv_nsec = NANO_IN_SEC + end->tv_nsec - start->tv_nsec;
	} else {
		temp.tv_sec = end->tv_sec - start->tv_sec;
		temp.tv_nsec = end->tv_nsec - start->tv_nsec;
	}
	return temp;
}


static void show_usage(void)
{
	printf( "Run as:\t time /bin/sh/ script.sh argScript0 argScript1 ...\n");
}


/* As the first argument should be path to shell program (eg /bin/sh).
 * Second argumnet - path to shell script.
 * */
int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Not enough arguments\n");
		show_usage();
		exit(EXIT_FAILURE);
	}

	if (argc >= ARGS_COUNT_MAX) {
		fprintf(stderr, "Too many argumnens. Max count is: %d", ARGS_COUNT_MAX);
		show_usage();
		exit(EXIT_FAILURE);
	}

	struct timespec start;
	if (clock_gettime(CLOCK_MONOTONIC, &start)) {
		perror("Unable to get clock_gettime before proces start - ");
		exit(EXIT_FAILURE);
	}

	char *args[ARGS_COUNT_MAX+1];
	int idx = 0;
	for (; idx < argc; ++idx) {
		args[idx] = argv[idx+1];
	}
	args[idx] = NULL;

	pid_t pid;
	int ret = posix_spawn(&pid, args[0], NULL, NULL, args, environ);
	if (ret) {
		perror("Unable to spawn process - ");
		exit(EXIT_FAILURE);
	}

	int status;
	ret = waitpid(pid, &status, 0);
	if (ret < 0) {
		perror("Waitpid for child process fail - ");
		exit(EXIT_FAILURE);
	} else {
		struct timespec stop;
		if (clock_gettime(CLOCK_MONOTONIC, &stop)) {
			perror("Unable to get clock_gettime after proces exit - ");
			exit(EXIT_FAILURE);
		}

		struct timespec delta = diff(&start, &stop);
		printf("Runnig time: %lld s and %.9ld nsec\n", (long long )delta.tv_sec, delta.tv_nsec);
		exit(EXIT_SUCCESS);
	}

	return 0;
}
