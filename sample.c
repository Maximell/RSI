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

void changeDirectory(char* directory) {
	if(chdir(directory) == -1) {
		printf("Not a valid path.\n");
	}
}

char* createPrompt(const char* start, char* middle, const char* end, char* prompt){ //takes the 3 parts of the prompt, and puts them together.

	free(middle);
	middle = getcwd(NULL, -1);

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

int getLengthOfDoubleArray(char** argv) {

	int i = 0;

	while (1) {
		if (argv[i] == NULL) {
			return i;
		}
		i++;
	}
}

int main() {

	/* variables used for displaying the prompt */
	const char* prePrompt = "RSI: ";
	const char* postPrompt = " > ";
	char prompt[PATH_MAX+6+4];
	char* currentDir; //allocate the maximum possible length of path, from linux/limits.h
	char* argv[MAX_ARGS]; 	/* variable that holds arguments for the execute command */

	currentDir = getcwd(NULL, -1);

	if (currentDir != NULL){ //attempts to get the current working directory
		createPrompt(prePrompt, currentDir, postPrompt, prompt);
	} else {
		perror("getcwd() error");
		return -1;
	}

	int bailout = 0;
	while (!bailout) {
		char* reply = readline(prompt);

		parseInput(reply, argv);

		if (!strcmp(reply, "bye")) {
			bailout = 1;
		} else if (!strcmp(reply, "cd")) {

			if (getLengthOfDoubleArray(argv) > 2) {
				printf("Error: cd takes one argument.\n");
			} else if (getLengthOfDoubleArray(argv) == 1) {
				printf("changing directory to /home/user/\n");
				chdir("/home/user");
				free(currentDir);
				currentDir = getcwd(NULL, -1);
			} else {

				if (strstr(argv[1], "~") != NULL) {
					printf("contains ~\n");
				} else if (strstr(argv[1], "..") != NULL) {
					// need to see if it starts with ..
					if (argv[1][0] == '.' && argv[1][1] == '.') {
						free(currentDir);
						currentDir = realpath(argv[1], NULL);
						changeDirectory(currentDir);
						createPrompt(prePrompt, currentDir, postPrompt, prompt);
					} else { /* if it doesn't start with it, then it is somewhere in the middle */
						currentDir = strcat(currentDir, "/");
						currentDir = strcat(currentDir, argv[1]);
						currentDir = realpath(currentDir, NULL);
						
						changeDirectory(currentDir);
						createPrompt(prePrompt, currentDir, postPrompt, prompt);
					}

				} else {
					currentDir = strcat(currentDir, "/");
					currentDir = strcat(currentDir, argv[1]);
					currentDir = realpath(currentDir, NULL);
					
					changeDirectory(currentDir);
					createPrompt(prePrompt, currentDir, postPrompt, prompt);
				}

			}

		} else {

			pid_t pid;

			int error = 0;

			pid = fork();
			if(pid < 0) { /* error occurred */
				perror("fork() failed");
			} else if (pid == 0) { /* child process */
				error = execvp(argv[0], argv);
				if(error == -1){
					printf("Error in execvp occurred\n");
				}
			} else { /* parent process */
				wait(NULL);
			}
		}
	
		free(reply);
	}
	printf("Bye Bye\n");
}
