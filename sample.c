#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include <linux/limits.h> for lab machines - defines MAX_PATH
#include <sys/syslimits.h> // for osx - TODO remove before handing in
#include <sys/types.h> /* included for getpid and getppid */

#define MAX_ARGS 64 /* defines the maximum number of command line arguments. This should be dynamically allocated in future. */

char* createPrompt(const char* start, char* middle, const char* end, char* prompt){ //takes the 3 parts of the prompt, and puts them together.

	strcpy(prompt, start); //copy the start into the prompt
	strcat(prompt, middle); //add on the middle
	strcat(prompt, end); //add on the end

	return prompt; //return the prompt
}

/* take an input string tokenizes it using spaces into argv */
void parseInput(char* input, char** argv){

	int i = 0;

	argv[0] = strtok(input, " ");
	i++;

	while (1) {

		argv[i] = strtok(NULL, " ");

		if(argv[i] == NULL) {
			return;
		}
		i++;
	}
}

int main() {

	/* variables used for displaying the prompt */
	const char* prePrompt = "RSI: ";
	const char* postPrompt = " > ";
	char prompt[PATH_MAX+6+4];
	char currentDir[PATH_MAX]; //allocate the maximum possible length of path, from linux/limits.h

	/* variable that holds arguments for the execute command */
	char* argv[MAX_ARGS];

	if (getcwd(currentDir, sizeof(currentDir)) != NULL){ //attempts to get the current working directory
		printf("%s\n\n", currentDir); // if successful, 
	} else {
		perror("getcwd() error");
		return -1;
	}

	createPrompt(prePrompt, currentDir, postPrompt, prompt);

	int bailout = 0;
	while (!bailout) {
		char* reply = readline(prompt);

		parseInput(reply, argv);

		if (!strcmp(reply, "bye")) {
			bailout = 1;
		} else if (!strcmp(reply, "cd")) {
			printf("Need to implement cd\n");
			/* will need to recreate the prompt again createPrompt(prePrompt, currentDir, postPrompt, prompt); */
		} else {

			pid_t pid;

			int error = 0;

			pid = fork();
			if(pid < 0) { /* error occurred */
				perror("fork() failed");
			} else if (pid == 0) { /* child process */
				printf("I am in the child process\n");
				error = execvp(argv[0], argv);
				if(error == -1){
					printf("Error in execvp occurred\n");
				}
			} else { /* parent process */
				printf("I am in the parent process\n");
				wait(NULL);
				printf("Child Complete\n");
			}
		}
	
		free(reply);
	}
	printf("Bye Bye\n");
}
