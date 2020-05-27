#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>


//sleep macro
#define sleepEM(time){ if (time != 0) usleep((rand() % time) * 1000);}


//variable declaration
int *PI = NULL;
int *IG = NULL;
int *JG = NULL;
int *IT = NULL;
int *JT = NULL;

int *NE = NULL;
int *NC = NULL;
int *NB = NULL;

int *action_counter = NULL;
int *IMM_counter = NULL;
int *proc_done = NULL;
int *solved_counter = NULL;
int *collected_counter = NULL;
int *to_collect = NULL;
bool *forkfailed = NULL;
FILE *filep = NULL;

sem_t *imm_enters = NULL;
sem_t *imm_checks = NULL;
sem_t *judge_in = NULL;
sem_t *decided = NULL;
sem_t *alldone = NULL;
sem_t *outsync = NULL;


//shared memory and variable initialization
int variable_map(){

    PI = mmap(NULL, sizeof(*(PI)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    IG = mmap(NULL, sizeof(*(IG)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    JG = mmap(NULL, sizeof(*(JG)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    IT = mmap(NULL, sizeof(*(IT)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    JT = mmap(NULL, sizeof(*(JT)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    NE = mmap(NULL, sizeof(*(NE)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *NE = 0;
    NC = mmap(NULL, sizeof(*(NC)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *NC = 0;
    NB = mmap(NULL, sizeof(*(NB)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *NB = 0;

     action_counter= mmap(NULL, sizeof(*(action_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *action_counter = 0;
    IMM_counter = mmap(NULL, sizeof(*(IMM_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *IMM_counter = 0;
    proc_done = mmap(NULL, sizeof(*(proc_done)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *proc_done = -2;
    solved_counter = mmap(NULL, sizeof(*(solved_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *solved_counter = 0;
    collected_counter = mmap(NULL, sizeof(*(collected_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *collected_counter = 0;
    to_collect = mmap(NULL, sizeof(*(to_collect)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *to_collect = 0;
    forkfailed = mmap(NULL, sizeof(*(forkfailed)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *forkfailed = false;
    filep = mmap(NULL, sizeof(*(filep)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    filep = fopen("proj2.out", "w");
    setbuf(filep, NULL);

    if ((imm_enters = sem_open("/xjacko05.2020.imm_enters", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    if ((imm_checks = sem_open("/xjacko05.2020.imm_checks", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    if ((judge_in = sem_open("/xjacko05.2020.judge_in", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    if ((decided = sem_open("/xjacko05.2020.decided", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1;
    if ((alldone = sem_open("/xjacko05.2020.alldone", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1;
    if ((outsync = sem_open("/xjacko05.2020.outsync", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    
    return 0;
}


//shared memory cleanup
void cleanup(){

    munmap((PI), sizeof((PI)));
    munmap((IG), sizeof((IG)));
    munmap((JG), sizeof((JG)));
    munmap((IT), sizeof((IT)));
    munmap((JT), sizeof((JT)));

    munmap((NE), sizeof((NE)));
    munmap((NC), sizeof((NC)));
    munmap((NB), sizeof((NB)));

    munmap((action_counter), sizeof((action_counter)));
    munmap((IMM_counter), sizeof((IMM_counter)));
    munmap((proc_done), sizeof((proc_done)));
    munmap((solved_counter), sizeof((solved_counter)));
    munmap((collected_counter), sizeof((collected_counter)));
    munmap((to_collect), sizeof((to_collect)));
    munmap((forkfailed), sizeof((forkfailed)));
    munmap((filep), sizeof((filep)));

    sem_close(imm_enters);
    sem_close(imm_checks);
    sem_close(judge_in);
    sem_close(alldone);
    sem_close(decided);
    sem_close(outsync);

    sem_unlink("xjacko05.2020.imm_enters");
    sem_unlink("xjacko05.2020.imm_checks");
    sem_unlink("xjacko05.2020.judge_in");
    sem_unlink("xjacko05.2020.alldone");
    sem_unlink("xjacko05.2020.decided");
    sem_unlink("xjacko05.2020.outsync");

}


//IMM prints
void print_imm_starts(int imm_id)   { sem_wait(outsync); fprintf(filep, "%i\t: IMM %i\t\t: starts\n",                                 ++*action_counter, imm_id); sem_post(outsync); }
void print_imm_enters(int imm_id)   { sem_wait(outsync); fprintf(filep, "%i\t: IMM %i\t\t: enters\t\t: %i\t: %i\t: %i\n",             ++*action_counter, imm_id, ++*NE, *NC, ++*NB); sem_post(outsync); }
void print_imm_checks(int imm_id)   { sem_wait(outsync); fprintf(filep, "%i\t: IMM %i\t\t: checks\t\t: %i\t: %i\t: %i\n",             ++*action_counter, imm_id, *NE, ++*NC, *NB); sem_post(outsync); }
void print_imm_wants(int imm_id)    { sem_wait(outsync); fprintf(filep, "%i\t: IMM %i\t\t: wants certificate\t: %i\t: %i\t: %i\n",    ++*action_counter, imm_id, *NE, *NC, *NB); sem_post(outsync); }
void print_imm_got(int imm_id)      { sem_wait(outsync); fprintf(filep, "%i\t: IMM %i\t\t: got certificate\t: %i\t: %i\t: %i\n",      ++*action_counter, imm_id, *NE, *NC, *NB); sem_post(outsync); }
void print_imm_leaves(int imm_id)   { sem_wait(outsync); fprintf(filep, "%i\t: IMM %i\t\t: leaves\t\t: %i\t: %i\t: %i\n",             ++*action_counter, imm_id, *NE, *NC, --*NB); sem_post(outsync); }
//JUDGE prints
void print_judge_wants()    { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: wants to enter\n",                        ++*action_counter); sem_post(outsync); }
void print_judge_enters()   { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: enters\t\t: %i\t: %i\t: %i\n",            ++*action_counter, *NE, *NC, *NB); sem_post(outsync); }
void print_judge_waits()    { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: waits for imm\t\t: %i\t: %i\t: %i\n",     ++*action_counter, *NE, *NC, *NB); sem_post(outsync); }
void print_judge_starts()   { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: starts confirmation\t: %i\t: %i\t: %i\n", ++*action_counter, *NE, *NC, *NB); sem_post(outsync); }
void print_judge_ends()     { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: ends confirmation\t: %i\t: %i\t: %i\n",   ++*action_counter, *NE, *NC, *NB); sem_post(outsync); }
void print_judge_leaves()   { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: leaves\t\t: %i\t: %i\t: %i\n",            ++*action_counter, *NE, *NC, *NB); sem_post(outsync); }
void print_judge_finishes() { sem_wait(outsync); fprintf(filep, "%i\t: JUDGE\t\t: finishes\n",                              ++*action_counter); sem_post(outsync); }

//err prints
void print_inv_args() {fprintf(stderr, "Invalind arguments\n"); exit(1);}

//immigrant generatoe process
void IMM_generator(){

    for (int i = 0; i < *PI; i++){

        pid_t IMM = fork();
        if (IMM < 0) {fprintf(stderr, "Immigrant process fork error\n"); *forkfailed = true; cleanup(); exit(1);}
        if (IMM == 0){
            //immmigrant
            int id = ++*IMM_counter;

            //starts
            print_imm_starts(id);

            //enters
            sem_wait(imm_enters);
            print_imm_enters(id);
            sem_post(imm_enters);

            //checks
            sem_wait(imm_checks);
            print_imm_checks(id);
            sem_post(imm_checks);

            //wants & got
            sem_wait(decided);
            print_imm_wants(id);
            ++*collected_counter;
            if (*collected_counter != *to_collect){
                sem_post(decided);
            }
            sleepEM(*IT);
            print_imm_got(id);

            //leaves
            sem_wait(judge_in);
            print_imm_leaves(id);
            sem_post(judge_in);

            //immingrant processs termination
            if (++*proc_done == *PI) sem_post(alldone);
            exit(0);
        }
        sleepEM(*IG);
    }
}


int main(int argc, char *argv[]){

    //case of interruped previous run
    cleanup();

	//argument count check
    if (argc != 6)print_inv_args();

    //semaphore & variable init check
    if (variable_map() == 1) cleanup();

	//PI check
    *PI = atoi(argv[1]);
    if (*PI < 1)print_inv_args();

	//IG check
    *IG = atoi(argv[2]);
    if (*IG == 0 && strcmp(argv[2], "0") != 0)print_inv_args();
    if (*IG < 0 || *IG > 2000)print_inv_args();

	//JG check
    *JG = atoi(argv[3]);
    if (*JG == 0 && strcmp(argv[3], "0") != 0)print_inv_args();
    if (*JG < 0 || *JG > 2000)print_inv_args();

	//IT check
    *IT = atoi(argv[4]);
    if (*IT == 0 && strcmp(argv[4], "0") != 0)print_inv_args();
    if (*IT < 0 || *IT > 2000)print_inv_args();

	//JT chcek
    *JT = atoi(argv[5]);
    if (*JT == 0 && strcmp(argv[5], "0") != 0)print_inv_args();
    if (*JT < 0 || *JT > 2000) print_inv_args();

    //file opening
    FILE *filep = fopen("proj2.out", "w");

    ///immigrant generator process created
    pid_t proc_IMM_gen = fork();
    if(proc_IMM_gen < 0) {fprintf(stderr, "Immigrant generator process fork error\n"); *forkfailed = true; cleanup(); exit(1);}
    if(proc_IMM_gen == 0){
        IMM_generator();
        if (++*proc_done == *PI) sem_post(alldone);
        exit(0);
    }

    //judge process created
    pid_t proc_JUDGE = fork();
    if(proc_JUDGE < 0) {fprintf(stderr, "JUDGE process fork error\n"); cleanup(); exit(1);}
    if(proc_JUDGE == 0){

        //loops until all immigrants are dealt with
        while(*solved_counter != *PI && *forkfailed == false){

            //wants & enters
            sleepEM(*JG);
            print_judge_wants();
            sem_wait(imm_checks);//closes entrance
            sem_wait(imm_enters);
            sem_wait(judge_in);//closes exit
            print_judge_enters();

            //waits
            if (*NE != *NC) print_judge_waits();
            sem_post(imm_checks);
            while(*NE != *NC){}
            sem_wait(imm_checks);

            //confirms
            print_judge_starts();
            sleepEM(*JT);
            *solved_counter += *NC;
            *to_collect += *NC;
            *NE = 0;
            *NC = 0;
            print_judge_ends();
            if (*to_collect != 0) sem_post(decided);

            //leaves
            sleepEM(*JT);
            print_judge_leaves();
            sem_post(imm_checks);
            sem_post(imm_enters);//opens entrance
            sem_post(judge_in);//opens exit
        }

        //judge process termination
        print_judge_finishes();
        if (++*proc_done == *PI) sem_post(alldone);
        exit(0);
    }

    //main parent process waits for all other processes
    sem_wait(alldone);
    fclose(filep);
    cleanup();

    return 0;
}