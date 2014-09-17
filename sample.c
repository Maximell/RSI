#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include <linux/limits.h> for lab machines - defines MAX_PATH
#include <sys/syslimits.h> // for osx - TODO remove before handing in



char* createPrompt(const char* start, char* middle, const char* end, char* prompt){ //takes the 3 parts of the prompt, and puts them together.

	strcpy(prompt, start); //copy the start into the prompt
	strcat(prompt, middle); //add on the middle
	strcat(prompt, end); //add on the end

	return prompt; //return the prompt
}

int main() {
	const char* prePrompt = "RSI: ";
	const char* postPrompt = " > ";
	char prompt[PATH_MAX+6+4];
	char currentDir[PATH_MAX]; //allocate the maximum possible length of path, from linux/limits.h

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

		if (!strcmp(reply, "bye")) {
			bailout = 1;
		} else {
			printf("\nYou said: %s\n\n", reply);
		}
	
		free(reply);
	}
	printf("Bye Bye\n");
}
