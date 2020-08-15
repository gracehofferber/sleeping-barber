//Created by Grace Hofferber

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	char cmdline[100];
	char *whitespace;


	while(1){

		printf("\n Welcome to Grace's shell>>");
		fgets(cmdline,sizeof(cmdline),stdin);
	//getting user input and storing into cmdlinei

		if(strcmp(cmdline,"exit\n")==0){
			printf("\n...Exiting...\n");
			exit(0);
		}
	//the command line is empty.
		if(cmdline == NULL){
			return 0;
		}
	//strchr: searches for the first occurance of a character. Then replacing it w \0.
		char* end = strchr(cmdline, '\n');
		*end = '\0';		
		
	//strtok splits a string at a specific point.Returns null when there are no more that match
		whitespace = strtok(cmdline," ");
		int i = 0;
		while(whitespace!= NULL && i <100){
			argv[i]=whitespace;
			whitespace = strtok(NULL," ");
			i++;	
			argc++;
		}
		argv[i]= NULL;//setting last index to null.
				
	//forking
	
		int pid=fork();
	
		if(pid == 0){ 
	//child process
			execvp(argv[0],argv);
			perror("\n...Exec failure...");
			exit(0);
		}
		else if(pid < 0){
			printf("\n...Forking didn't work...");
		}
		else{ 
	//parent process
			if(strcmp(argv,"&")==0){
				printf("\n...I'm not waiting...");
				return 0;
			}
			else{
				printf("\n...I'm waiting...");
				wait(NULL);
			
			}	
		}
		
		fflush(stdout);
	
	}
return 0;
}
