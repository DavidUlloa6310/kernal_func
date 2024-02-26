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
	char* curr_arg = args;
	while (curr_arg != NULL) {
		printf("%s ", curr_arg);
		curr_arg = curr_arg + 1;
	}
	printf("\n");
}

char** parse_args(char* arg_line) {
	char** args = (char**) malloc(sizeof(char*) * MAX_ARGS);
	int arg_count = 0;
	char* token;
	while ((token = strtok(arg_line, ' ') != NULL)) {
		args[arg_count] = token;
		++arg_count;
	}
	args[arg_count] = NULL;
	return args;
}

ProgInfo parse_prog(char* line) {
	line[strlen(line) - 1] = '\0'; // Remove '\n'
	char* ptr = strchr(line, ' ');

	// Split string by space
	long int pos = ptr - line;
	line[pos] = '\0'; 
	char* args = ptr + 1;

	ProgInfo *prog_info = malloc(sizeof(ProgInfo));
	prog_info -> file = strdup(line);
	prog_info -> args = parse_args(args);
	return *prog_info;
}

int main(int argc, char** argv) {
	ProgInfo *progs = (ProgInfo*)malloc(sizeof(ProgInfo) * MAX_PROGS);

	int exec_count = 0;
	char* line = NULL;
	size_t len = 0;
	while (getline(&line, &len, stdin) != -1) {
		ProgInfo prog_info = parse_prog(line);
		progs[exec_count] = prog_info;
		++exec_count;
	}

	int curr_prog = 0;
	while (progs[curr_prog] != NULL) {
		log_prog(progs[curr_prog]);
		++curr_prog;
	}
	return 0;
}
