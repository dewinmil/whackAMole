#include <string.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <time.h>
#include "arguments.h"

#define SIZE 16

void display(int width, int height, int* board, char (upmole[11][22]),
        char (downmole[11][22]));

void newThread(int threadNum, int* board, int* moles, int total, int shmId,
        int semId, int *numMoles, struct sembuf*, int max, int up, int down,
	int width, int height, char (upmole[11][22]), char (downmole[11][22]));

int main (int argc, char* argv[]) {
    struct arguments values;
    int status;
    int *moles;
    int shmId, semId;
    int width, height, max, total, up, down;
    struct sembuf sbuf[4];
    pid_t pid;
    int *numMoles = calloc(1, sizeof(int));
    numMoles = 0;
    values = setup(argc, argv);
    width = values.width;
    height = values.height;
    max = values.max;
    total = values.total;
    up = values.up;
    down = values.down;

    int *board = calloc(width * height, sizeof(int));
    semId = semget(IPC_PRIVATE, 1, 00600);
    sbuf[0].sem_num = 0;
    sbuf[0].sem_op = -1;
    sbuf[0].sem_flg = 0;
    sbuf[1].sem_num = 0;
    sbuf[1].sem_op = 1;
    sbuf[1].sem_flg = 0;
    srand(time(NULL));
    char upmole[11][22];
    strcpy(upmole[0], "* * * * * * * * * * *");
    strcpy(upmole[1], "*                   *");
    strcpy(upmole[2], "*     _.-=-._       *");
    strcpy(upmole[3], "*  o~`  '  > `.     *");
    strcpy(upmole[4], "*  `.  ,       :    *");
    strcpy(upmole[5], "*   ``-.__/    `.   *");
    strcpy(upmole[6], "*      /       ::   *");
    strcpy(upmole[7], "*     / .:    .:|   *");
    strcpy(upmole[8], "*    :       .::!.  *");
    strcpy(upmole[9], "*   /'| ::  :::'!!  *");
    strcpy(upmole[10], "* * * * * * * * * * *");
    char downmole[11][22];
    
    strcpy(downmole[0], "* * * * * * * * * * *");
    strcpy(downmole[1], "*       :::::       *");
    strcpy(downmole[2], "*     :::   :::     *");
    strcpy(downmole[3], "*   :::       :::   *");
    strcpy(downmole[4], "*  :::         :::  *");
    strcpy(downmole[5], "* :::           ::: *");
    strcpy(downmole[6], "*  :::         :::  *");
    strcpy(downmole[7], "*   :::       :::   *");
    strcpy(downmole[8], "*     :::   :::     *");
    strcpy(downmole[9], "*       :::::       *");
    strcpy(downmole[10], "* * * * * * * * * * *");

    if ((shmId = shmget (IPC_PRIVATE, SIZE,
                         IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
        perror ("i can't get no..\n");
        exit (1);
    }
    if ((moles = shmat (shmId, 0, 0))  == (void *) -1) {
        perror ("can't attach\n");
        exit (1);
    }
    //putting variables into shared memory block
    board = moles + 3;
    numMoles = board + width*height;

    //number of moles that have appeared
    moles[0] = 0;

    //number of moles that have been whacked
    moles[1] = 0;

    //number of moles waiting to pop out
    moles[2] = 0;

    //create all my threads / moles
    if (!(pid = fork ())) {	
        for(int k=1; k < width*height; k++){
            if (!(pid = fork ())) {
	        newThread(k, board, moles, total, shmId, semId, numMoles, sbuf, max, up,
                	down, width, height, upmole, downmole);
            }
        }
	newThread(0, board, moles, total, shmId, semId, numMoles, sbuf, max, up, down,
		width, height, upmole, downmole);
    }else {
	char* str = malloc(sizeof(char) * 80);
   	while(moles[0] < total){
		fgets(str, 80, stdin);
		char* token;
		//user hit enter or space
		if(strcmp(str, "\n") == 0){
			continue;
		}
		token = strtok(str, " \n");
	
		int tokenIsNumber = 1;
		for(int i = 0; i <strlen(token); i++){
			if(!isdigit((int)token[i])){
				tokenIsNumber = 0;
			}
		}
		//invalid input / just keep going
		if(!tokenIsNumber){
			continue;
		}else{
			//turn user input into a number
			int tokenNumber = atoi(token);
			
			if(board[tokenNumber] == 1){
				board[tokenNumber] = 0;
				moles[1]++;
				display(width, height, board, upmole, downmole);
        			fprintf(stderr, "Score: %i / %i\n", moles[1], moles[0]);
			}
						
		}
	} 
    }

    wait (&status);
    fprintf(stderr, "Score: %i\n", moles[1]);
    if (shmdt (moles) < 0) {
        perror ("Error: Can't Detatch Shared Memory Segment\n");
        exit (1);
    }
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("Error: Can't Deallocate Shared Memory\n");
        exit (1);
    }
    return 0;
}

void newThread(int threadNum, int* board, int* moles, int total, int shmId,
        int semId, int* numMoles,  struct sembuf* sbuf, int max, int up, int down,
	int width, int height, char (upmole[11][22]), char (downmole[11][22]) ){
    int status;
    int r, range1, range2;
 
    semctl(semId, threadNum, SETVAL, 1);
    while(moles[0] < total) {
	//without this the random numbers will be identical between threads
	for(int count=0; count <= threadNum; count++){ 
            range1 = .8 * down;
            range2 = down * .4;
            r = rand() % (range2);
            r = r + range1;
	}
        usleep(r * 1000);
        if(moles[0] < total){
	    while(numMoles[0] >= max){
	        usleep(r * 1000);
	    }
	    semop(semId, &sbuf[0], 1);
        }else{
	    semctl(semId, 0, IPC_RMID);
	    wait (&status);
	    exit(1);
        }
        if(moles[0] >= total){
	    semctl(semId, 0, IPC_RMID);
	    wait (&status);
	    exit(1);
        }
        numMoles[0]++;
        moles[0]++; 
        board[threadNum] = 1;
	display(width, height, board, upmole, downmole);
        fprintf(stderr, "Score: %i / %i\n", moles[1], moles[0]);
	
        semop(semId, &sbuf[1], 1);
        range1 = .8 * up;
        range2 = up * .4;
        r = rand() % (range2);
        r = r + range1;
	usleep(r * 1000);
	board[threadNum] = 0;
	display(width, height, board, upmole, downmole);
        fprintf(stderr, "Score: %i / %i\n", moles[1], moles[0]);
        numMoles[0]--;
    }
    semctl(semId, 0, IPC_RMID);
    if (shmdt (moles) < 0) {
        perror ("\nError: Can't Detatch Shared Memory Segment\n");
        wait (&status);
        exit (1);
    }
    wait (&status);
    if(status == 1){
        exit(1);
    }
    exit (0);
}

void display(int width, int height, int* board, char (upmole[11][22]),
        char (downmole[11][22])){
   int i, ind, index;
   fprintf(stderr, "\n\n");
   for(i=0; i<height; i++){
       for(ind=0; ind<11; ind++){
           for(index=0; index<width; index++){
              if(board[(i * width) + index] == 0){
                  fprintf(stderr, "%s ", downmole[ind]);
              }else{
                  fprintf(stderr, "%s ", upmole[ind]);
              }
           }
           fprintf(stderr, "\n");
       }
   }
}
