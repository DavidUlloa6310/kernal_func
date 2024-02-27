#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LEN 1000
#define MAX_PROGS 100
#define MAX_ARGS 10

typedef struct {
	char* file;
	char** args;
} ProgInfo;

void log_prog(ProgInfo prog_info) {
	printf("name: %s, args: ", prog_info.file);
	char** curr_arg = prog_info.args;
	while (*curr_arg != NULL) {
		printf("%s ", *curr_arg);
		++curr_arg;
	}
	printf("\n");
}

char** parse_args(char* arg_line) {
	if (arg_line == NULL) {
		return NULL; // program has no args
	}

	char** args = (char**) malloc(sizeof(char*) * MAX_ARGS);
	char* token = strtok(arg_line, " ");
	if (token == NULL) {
		args[0] = arg_line;
		args[1] = NULL;
		return args; // program has one arg
	}

	int arg_count = 0;
	while (token != NULL) {
		args[arg_count] = token;
		token = strtok(NULL, " ");
		++arg_count;
	}
	args[arg_count] = NULL;
	return args;
}

ProgInfo parse_prog(char* line) {
	line[strlen(line) - 1] = '\0'; // Remove '\n'
	char* ptr = strchr(line, ' ');
	if (ptr == NULL) {
		ProgInfo *prog_info = malloc(sizeof(ProgInfo));
		prog_info -> file = strdup(line);
		prog_info -> args = NULL;
		return *prog_info;
	}

	// Split name from line
	long int pos = ptr - line;
	char* name = strdup(line);
	name[pos] = '\0';

	ProgInfo *prog_info = malloc(sizeof(ProgInfo));
	prog_info -> file = name;
	prog_info -> args = parse_args(line);
	return *prog_info;
}

int main(int argc, char** argv) {
	ProgInfo progs[MAX_PROGS];

	int exec_count = 0;
	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stdin) != -1) {
		ProgInfo prog_info = parse_prog(line);
		progs[exec_count] = prog_info;
		++exec_count;
	}

	pid_t pids[exec_count];
	for (int i = 0; i < exec_count; ++i) {
		pid_t pid = fork();
		if (pid == 0) {
			ProgInfo prog = progs[i];
			execvp(prog.file, prog.args);
			exit(1);
		} else if (pid > 0) {
			pids[i] = pid;
			printf("Starting process: %ld\n", pid);
		} else {
			perror("Fork failed");
			exit(1);
		}
	}

	return 0;

	// Code to report summary of processes

	char* summary[exec_count];
	for (int i = 0; i < exec_count; ++i) {
		int status;
		waitpid(pids[i], &status, 0);
		char buffer[512];
		snprintf(buffer, sizeof(buffer), "Child process %d exited with status: %d\n", pids[i], WEXITSTATUS(status)); 
		summary[i] = buffer;
	}

	printf("============== Process Summary ================\n");
	for (int i = 0; i < exec_count; ++i) {
		printf(summary[i]);
	}
	printf("===============================================\n");

	return 0;
}
