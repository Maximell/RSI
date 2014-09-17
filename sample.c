#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

int main() {

	char* currentDir; //need to change to dynamically allocated

	if (getwd(currentDir) != NULL){ //attempts to get the current working directory
		printf("%s\n\n", currentDir); // if successful, 
		printf("blah");
	} else {
		perror("getcwd() error");
		return -1;
	}
	printf("blah");

	int bailout = 0;
	while (!bailout) {
		printf("blah");
		char* reply = readline(currentDir);
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
