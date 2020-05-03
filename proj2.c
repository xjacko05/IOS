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


#define sleepEM(time){ if (time != 0) usleep((rand() % time) * 1000);}
//#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
//#define UNMAP(ptr) {munmap((ptr), sizeof((ptr)));}


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
int* proc_done = NULL;
bool *judge_inside = NULL;
int *solved_counter = NULL;
int *collected_counter = NULL;
int *to_collect = NULL;
int *left_counter = NULL;
int *to_leave = NULL;
FILE *filep = NULL;

sem_t *imm_enters = NULL;
sem_t *imm_checks = NULL;
sem_t *judge_in = NULL;
sem_t *judge_waits = NULL;
sem_t *decided = NULL;
sem_t *alldone = NULL;

int variable_map(){

    PI = mmap(NULL, sizeof(*(PI)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(PI)
    IG = mmap(NULL, sizeof(*(IG)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(IG)
    JG = mmap(NULL, sizeof(*(JG)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(JG)
    IT = mmap(NULL, sizeof(*(IT)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(IT)
    JT = mmap(NULL, sizeof(*(JT)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(JT)

    NE = mmap(NULL, sizeof(*(NE)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(NE)
    *NE = 0;
    NC = mmap(NULL, sizeof(*(NC)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(NC)
    *NC = 0;
    NB = mmap(NULL, sizeof(*(NB)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(NB)
    *NB = 0;

     action_counter= mmap(NULL, sizeof(*(action_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(action_counter)
    *action_counter = 0;
    IMM_counter = mmap(NULL, sizeof(*(IMM_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(IMM_counter)
    *IMM_counter = 0;
    proc_done = mmap(NULL, sizeof(*(proc_done)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(proc_done)
    *proc_done = -2;
    judge_inside = mmap(NULL, sizeof(*(judge_inside)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(judge_inside)
    *judge_inside = false;
    solved_counter = mmap(NULL, sizeof(*(solved_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(solved_counter)
    *solved_counter = 0;
    collected_counter = mmap(NULL, sizeof(*(collected_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(collected_counter)
    *collected_counter = 0;
    to_collect = mmap(NULL, sizeof(*(to_collect)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(to_collect)
    *to_collect = 0;
    left_counter = mmap(NULL, sizeof(*(left_counter)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(left_counter)
    *left_counter = 0;
    to_leave = mmap(NULL, sizeof(*(to_leave)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(to_leave)
    *to_leave = 0;
    filep = mmap(NULL, sizeof(*(filep)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);//MMAP(filep)
    filep = fopen("proj2.out", "w");//open("proj2.out", O_WRONLY | O_APPEND | O_CREAT, 0644);
    setbuf(filep, NULL);

    if ((imm_enters = sem_open("/xjacko05.2020.imm_enters", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    if ((imm_checks = sem_open("/xjacko05.2020.imm_checks", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    if ((judge_in = sem_open("/xjacko05.2020.judge_in", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED) return 1;
    if ((judge_waits = sem_open("/xjacko05.2020.judge_waits", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1;
    if ((decided = sem_open("/xjacko05.2020.decided", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1;
    if ((alldone = sem_open("/xjacko05.2020.alldone", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED) return 1;
    
    return 0;
}


void cleanup(){

    munmap((PI), sizeof((PI)));//UNMAP(PI)
    munmap((IG), sizeof((IG)));//UNMAP(IG)
    munmap((JG), sizeof((JG)));//UNMAP(JG)
    munmap((IT), sizeof((IT)));//UNMAP(IT)
    munmap((JT), sizeof((JT)));//UNMAP(JT)

    munmap((NE), sizeof((NE)));//UNMAP(NE)
    munmap((NC), sizeof((NC)));//UNMAP(NC)
    munmap((NB), sizeof((NB)));//UNMAP(NB)

    munmap((action_counter), sizeof((action_counter)));//UNMAP(action_counter)
    munmap((IMM_counter), sizeof((IMM_counter)));//UNMAP(IMM_counter)
    munmap((proc_done), sizeof((proc_done)));//UNMAP(proc_done)
    munmap((judge_inside), sizeof((judge_inside)));//UNMAP(judge_inside)
    munmap((solved_counter), sizeof((solved_counter)));//UNMAP(solved_counter)
    munmap((collected_counter), sizeof((collected_counter)));//UNMAP(collected_counter)
    munmap((to_collect), sizeof((to_collect)));//UNMAP(to_collect)
    munmap((left_counter), sizeof((left_counter)));//UNMAP(left_counter)
    munmap((to_leave), sizeof((to_leave)));//UNMAP(to_leave)
    munmap((filep), sizeof((filep)));

    sem_close(imm_enters);
    sem_close(imm_checks);
    sem_close(judge_in);
    sem_close(judge_waits);
    sem_close(alldone);
    sem_close(decided);

    sem_unlink("xjacko05.2020.imm_enters");
    sem_unlink("xjacko05.2020.imm_checks");
    sem_unlink("xjacko05.2020.judge_in");
    sem_unlink("xjacko05.2020.judge_waits");
    sem_unlink("xjacko05.2020.alldone");
    sem_unlink("xjacko05.2020.decided");

}


//IMM prints
void print_imm_starts(int imm_id)   { fprintf(filep, "%i\t: IMM %i\t\t: starts\n",                                 ++*action_counter, imm_id); }
void print_imm_enters(int imm_id)   { fprintf(filep, "%i\t: IMM %i\t\t: enters\t\t: %i\t: %i\t: %i\n",             ++*action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_checks(int imm_id)   { fprintf(filep, "%i\t: IMM %i\t\t: checks\t\t: %i\t: %i\t: %i\n",             ++*action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_wants(int imm_id)    { fprintf(filep, "%i\t: IMM %i\t\t: wants certificate\t: %i\t: %i\t: %i\n",    ++*action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_got(int imm_id)      { fprintf(filep, "%i\t: IMM %i\t\t: got certificate\t: %i\t: %i\t: %i\n",      ++*action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_leaves(int imm_id)   { fprintf(filep, "%i\t: IMM %i\t\t: leaves\t\t: %i\t: %i\t: %i\n",             ++*action_counter, imm_id, *NE, *NC, *NB); }
//JUDGE prints
void print_judge_wants()    { fprintf(filep, "%i\t: JUDGE\t\t: wants to enter\t: %i\t: %i\t: %i\n",      ++*action_counter, *NE, *NC, *NB); }
void print_judge_enters()   { fprintf(filep, "%i\t: JUDGE\t\t: enters\t\t: %i\t: %i\t: %i\n",            ++*action_counter, *NE, *NC, *NB); }
void print_judge_waits()    { fprintf(filep, "%i\t: JUDGE\t\t: waits fo imm\t: %i\t: %i\t: %i\n",        ++*action_counter, *NE, *NC, *NB); }
void print_judge_starts()   { fprintf(filep, "%i\t: JUDGE\t\t: starts confirmation\t: %i\t: %i\t: %i\n", ++*action_counter, *NE, *NC, *NB); }
void print_judge_ends()     { fprintf(filep, "%i\t: JUDGE\t\t: ends confirmation\t: %i\t: %i\t: %i\n",   ++*action_counter, *NE, *NC, *NB); }
void print_judge_leaves()   { fprintf(filep, "%i\t: JUDGE\t\t: leaves\t\t: %i\t: %i\t: %i\n",            ++*action_counter, *NE, *NC, *NB); }
void print_judge_finishes() { fprintf(filep, "%i\t: JUDGE\t\t: finishes\n",                                ++*action_counter); }


void IMM_generator(){

    for (int i = 0; i < *PI; i++){

        pid_t IMM = fork();
        if (IMM < 0) {fprintf(stderr, "Immigrant process fork error\n"); sem_post(alldone); exit(1);}
        if (IMM == 0){
            //immmigrant
            int id = ++*IMM_counter;

            //starts
            print_imm_starts(id);

            //enters
            sem_wait(imm_enters);
            ++*NE;
            ++*NB;
            print_imm_enters(id);
            sem_post(imm_enters);

            //checks
            sem_wait(imm_checks);
            ++*NC;
            print_imm_checks(id);
            if (*judge_inside == true && *NE == *NC) sem_post(judge_waits);
            sem_post(imm_checks);

            //wants & got
            sem_wait(decided);
            print_imm_wants(id);
            ++*collected_counter;
            if (*collected_counter != *to_collect){
                sem_post(decided);
            }
 /*           else{
                *collected_counter = 0;
                *to_collect = 0;
            }
 */         sleepEM(*IT);
            print_imm_got(id);

            //leaves
            sem_wait(judge_in);
            --*NB;
            print_imm_leaves(id);
            ++*left_counter;
/*            if (*left_counter != *to_leave){
                sem_post(judge_in);
            }
            else{
                *left_counter = 0;
                *to_leave = 0;
            }
*/          sem_post(judge_in);

            //IMM termination
            if (++*proc_done == *PI) sem_post(alldone);
            exit(0);
        }
        sleepEM(*IG);
    }
}


int main(int argc, char *argv[]){

    //KOLOTOCARINA AF opravit TODO
    cleanup();

	//argument count check
    if (argc != 6){fprintf(stderr, "Invalind arguments\n"); return 1;}

    //semaphore init check
    if (variable_map() == 1) cleanup();

	//PI check
    *PI = atoi(argv[1]);//printf("PI=%i\n", *PI);
    if (*PI < 1){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//IG check
    *IG = atoi(argv[2]);//printf("IG=%i\n", *IG);
    if (*IG == 0 && strcmp(argv[2], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*IG < 0 || *IG > 2000){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//JG check
    *JG = atoi(argv[3]);//printf("JG=%i\n", *JG);
    if (*JG == 0 && strcmp(argv[3], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*JG < 0 || *JG > 2000){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//IT check
    *IT = atoi(argv[4]);//printf("IT=%i\n", *IT);
    if (*IT == 0 && strcmp(argv[4], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*IT < 0 || *IT > 2000){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//JT chcek
    *JT = atoi(argv[5]);//printf("JT=%i\n", *JT);
    if (*JT == 0 && strcmp(argv[5], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*JT < 0 || *JT > 2000) return 1;

    ///ACTUAL CODE
    FILE *filep = fopen("proj2.out", "w");

    pid_t proc_JUDGE = fork();
    if(proc_JUDGE < 0) {fprintf(stderr, "JUDGE process fork error\n"); sem_post(alldone); return 1;}
    if(proc_JUDGE == 0){

        while(*solved_counter != *PI){

            //wants & enters
            sleepEM(*JG);
            print_judge_wants();
            sem_wait(imm_enters);//closes entrance
            sem_wait(judge_in);//closes exit
            *judge_inside = true;
            print_judge_enters();

            //waits
            sem_wait(imm_checks);
            if (*NE != *NC){
                print_judge_waits();
                sem_post(imm_checks);
                sem_wait(judge_waits);
            }
            sem_post(imm_checks);

            //confirms
            print_judge_starts();
            sleepEM(*JT);
            *solved_counter += *NC;
            *to_collect += *NC;
            //*to_leave = *NC;
            *NE = 0;
            *NC = 0;
            print_judge_ends();
            if (*to_collect != 0) sem_post(decided);

            //leaves
            sleepEM(*JT);
            print_judge_leaves();
            *judge_inside = false;
            //if (*to_leave != 0) sem_post(judge_in);//opens exit
            sem_post(imm_enters);//opens entrance
            sem_post(judge_in);
        }

        print_judge_finishes();
        if (++*proc_done == *PI) sem_post(alldone);
        exit(0);
    }
    pid_t proc_IMM_gen = fork();
    if(proc_IMM_gen < 0) {fprintf(stderr, "Immigrant generator process fork error\n"); sem_post(alldone); return 1;}
    if(proc_IMM_gen == 0){
        IMM_generator();
        if (++*proc_done == *PI) sem_post(alldone);
        exit(0);
    }
    
    sem_wait(alldone);
    fclose(filep);
    cleanup();

    return 0;
}