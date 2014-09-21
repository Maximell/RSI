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

int main() {

	/* variables used for displaying the prompt */
	const char* prePrompt = "RSI: ";
	const char* postPrompt = " > ";
	char prompt[PATH_MAX+6+4];
	char currentDir[PATH_MAX]; //allocate the maximum possible length of path, from linux/limits.h

	/* variable that holds arguments for the execute command */
	char* argv[MAX_ARGS];

	argv[0] = "/bin/ls";
	argv[1] = "-l";
	argv[2] = NULL;

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
		/* Note that readline strips away the final \n */
		/* For Perl junkies, readline automatically chomps the line read */

		/*
			need to isolate inital command

			and then tokenize the rest
		*/


		if (!strcmp(reply, "bye")) {
			bailout = 1;
		} else if(strcmp(reply, "ls") == 0) {

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
		} else {
			printf("\nYou said: %s\n\n", reply);
		}
	
		free(reply);
	}
	printf("Bye Bye\n");
}
