#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <string.h>
#include "ezipc.h"

//global variables
void quit_handler(int signum);
char* served;

void main(){
	SETUP();
	int barberpid,pid,pid2;
	int size = 20;
	int haircut_time = 100;//random generator.
	int time = (rand() % 10)+1;
	int max_customers = 0;
	int max_chairs = 0;
	char* waiting = SHARED_MEMORY(size);//how many people are in the waiting room(availability)
	served = SHARED_MEMORY(size);//how many people got hair cuts.
	*served = 0;
//Getting usier input:
	printf("\n----Welcome to the Sleeping Barber!----\n");
	printf("\n How many customers are there? ");
	scanf("%d",&max_customers);
	printf("\n How many waiting chairs are there? ");
	scanf("%d",&max_chairs);
	*waiting = max_chairs;
	printf("\n The barber shop is now open... \n");
//Semaphore set-up:
	int wait_customers = SEMAPHORE(SEM_CNT,0);
	int barber = SEMAPHORE(SEM_BIN,0);
	int mutex = SEMAPHORE(SEM_BIN,1);
	pid = fork();
	if(pid ==0){//customers
		for(int i=0; i<max_customers;i++){
			sleep(time);
			pid2 =fork();
			if(pid2==0){//creating individual customers
				printf("\nCustomer %d has entered the shop\n", i);	
				P(mutex);
				if(*waiting >0){ //YAY! Seats available.
					*waiting = *waiting -1; //somebody is waiting.
					printf("\n--Customer %d is in the waiting room. The number of seats available are: %d \n",i,*waiting);
					V(mutex);
					V(wait_customers);//notify barber.
					P(barber);
					printf("\n HAIR CUT DONE: customer %d \n",i);
					printf("\n--Customer %d paid and is leaving.\n",i);
					exit(0);
				}
				else{// No seats.
					printf("\n ----UH OH! There are no seats available, Customer %d is leaving.----\n",i);
					V(mutex);
					exit(0);
				}
			}
		}
		printf("\n$$ MAX CUSTOMERS REACHED $$\n");
		exit(0);	
	}
	else if(pid <0){
		perror("\n Forking failed!");
	}
	else{//barber
		barberpid =getpid();
		printf("\n $$ To terminate: kill -USR1 %d $$ \n",barberpid);
		signal(SIGUSR1,quit_handler);
		while(1){
			P(wait_customers);
			printf("\n -The barber is waking up or there are still customers.\n");
			*waiting= *waiting +1;
			printf("\n -Barber is taking a customer for a hair cut. Seats available: %d \n", *waiting);
			printf("\n BARBER IS GIVING HAIRCUT!\n");
			sleep(time);
			V(barber);
			*served = *served +1;				
		}
	} 
}

void quit_handler(int signum){
		printf("! ! !USR1 smoke signal recieved, barber shop is closing! ! !\n");
		printf("! ! !Customers Served: %d ! ! !\n",*served);
		printf("! ! ! Bye bye ! ! !\n");
		exit(0);

}
	
