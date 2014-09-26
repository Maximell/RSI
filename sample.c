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

typedef struct processNode {
	int processId;
	char* program;
	struct processNode* prev;
	struct processNode* next;
} processNode;

/* Global Variables */
processNode* headProcess;

void addProcessNode(processNode* newNode) {

	newNode->next = headProcess;
	if(newNode->next != NULL){
		newNode->next->prev = newNode;
	}
	headProcess = newNode;

}

processNode* newNode(int processId, char* program) {

	processNode* newNode = (processNode*) malloc(sizeof(processNode));

	newNode->processId = processId;
	newNode->program = program;
	newNode->prev = NULL;
	newNode->next = NULL;

	return newNode;
}

/* this is not complete */
void deleteNode(processNode* node) {
	 /*free(&node->program); implement this after malloc for the program name is working */
	 free(node);
}

/* returns 1 for success, -1 for failure */
int removeProcessNode(int processId) {

	processNode* currentNode = headProcess;

	while (currentNode != NULL) {

		if (currentNode->processId == processId) {
			if(currentNode->prev != NULL) {
				currentNode->prev->next = currentNode->next;
			} else { /* if the 'prev' attribute is null, then it must be the first node in the list*/
				headProcess = currentNode->next;
			}
			if (currentNode->next != NULL) {
				currentNode->next->prev = currentNode->prev;
			}
			deleteNode(currentNode);
			return 1;
		}
		currentNode = currentNode->next;
	}
	return -1;
}

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

void childSignalHandler() {
	pid_t pid = waitpid(-1, NULL, WNOHANG);
	waitpid(pid, NULL, WNOHANG);

	if (removeProcessNode(pid) == 1) {
		printf("Process with id %d terminated.\n", pid);
	}
}

int main() {

	/* variables used for displaying the prompt */
	const char* prePrompt = "RSI: ";
	const char* postPrompt = " > ";
	char prompt[PATH_MAX+6+4];
	char* currentDir; //allocate the maximum possible length of path, from linux/limits.h
	char* argv[MAX_ARGS]; 	/* variable that holds arguments for the execute command */

	signal(SIGCHLD, &childSignalHandler);

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
		} else if (!strcmp(reply, "bg")) {

			pid_t pid;

			int error = 0;

			pid = fork();
			addProcessNode(newNode(pid, "sample string"));
			if(pid < 0) { /* error occurred */
				perror("fork() failed");
			} else if (pid == 0) { /* child process */
				error = execvp(argv[1], &argv[1]);
				if(error == -1){
					printf("Error in execvp occurred\n");
				}
			}

		} else if (!strcmp(reply, "bglist")){

			processNode* currentNode = headProcess;
			int bgCount = 0;

			while(currentNode != NULL) {
				printf("%d: %s\n", currentNode->processId, currentNode->program);
				currentNode = currentNode->next;
				bgCount++;
			}
			printf("Total Background Jobs:  %d\n", bgCount);
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
					exit(0);
				}
			} else { /* parent process */
				wait(NULL);
			}
		}
	
		free(reply);
	}
	printf("Bye Bye\n");
}
