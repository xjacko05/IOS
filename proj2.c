#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>


#define sleepEM(time){ if (time != 0) usleep(rand() % time);}
#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1,0);}
#define UNMAP(ptr) {nunmap((ptr), sizeof((pointer)));}


int *PI = NULL;
int *IG = NULL;
int *JG = NULL;
int *IT = NULL;
int *JT = NULL;

int *NE = NULL;
int *NC = NULL;
int *NB = NULL;

int *action_counter = NULL;
//int *IMM_counter = NULL;

//IMM prints
void print_imm_starts(int imm_id)   { fprintf(stdout, "%i\t: IMM %i\t\t: starts\n",                                 *action_counter, imm_id); }
void print_imm_enters(int imm_id)   { fprintf(stdout, "%i\t: IMM %i\t\t: enters\t\t: %i\t: %i\t: %i\n",             *action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_checks(int imm_id)   { fprintf(stdout, "%i\t: IMM %i\t\t: checks\t\t: %i\t: %i\t: %i\n",             *action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_wants(int imm_id)    { fprintf(stdout, "%i\t: IMM %i\t\t: wants certificate\t: %i\t: %i\t: %i\n",    *action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_got(int imm_id)      { fprintf(stdout, "%i\t: IMM %i\t\t: got certificate\t: %i\t: %i\t: %i\n",      *action_counter, imm_id, *NE, *NC, *NB); }
void print_imm_leaves(int imm_id)   { fprintf(stdout, "%i\t: IMM %i\t\t: leaves\t\t: %i\t: %i\t: %i\n",             *action_counter, imm_id, *NE, *NC, *NB); }
//JUDGE prints
void print_judge_wants()  { fprintf(stdout, "%i\t: JUDGE\t\t\t: wants to enter\t: %i\t: %i\t: %i\n",        *action_counter, *NE, *NC, *NB); }
void print_judge_enters() { fprintf(stdout, "%i\t: JUDGE\t\t\t: enters\t\t: %i\t: %i\t: %i\n",              *action_counter, *NE, *NC, *NB); }
void print_judge_starts() { fprintf(stdout, "%i\t: JUDGE\t\t\t: starts confirmation\t: %i\t: %i\t: %i\n",   *action_counter, *NE, *NC, *NB); }
void print_judge_ends()   { fprintf(stdout, "%i\t: JUDGE\t\t\t: ends confirmation\t: %i\t: %i\t: %i\n",     *action_counter, *NE, *NC, *NB); }
void print_judge_leaves() { fprintf(stdout, "%i\t: JUDGE\t\t\t: leaves\t\t: %i\t: %i\t: %i\n",              *action_counter, *NE, *NC, *NB); }

/*void IMM_generator(int PI, int IG){

    for (int i =0; i < PI; i++){

        pid_t IMM = fork();
        if (IMM == 0){
            //immmigrant
            sleepEM(PI);
        }
    }
}*/


int main(int argc, char *argv[]){
    
	//argument count check
    if (argc != 6){fprintf(stderr, "Invalind arguments\n"); return 1;}

    MMAP(PI);
    MMAP(IG);
    MMAP(JG);
    MMAP(IT);
    MMAP(JT);

    MMAP(NE);
    *NE = 0;
    MMAP(NC);
    *NC = 0;
    MMAP(NB);
    *NB = 0;

    MMAP(action_counter);
    *action_counter = 0;

	//PI check
    *PI = atoi(argv[1]);
    if (*PI < 1){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//IG check
    *IG = atoi(argv[2]);
    if (*IG == 0 && strcmp(argv[2], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*IG < 0 || *IG > 2000){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//JG check
    *JG = atoi(argv[3]);
    if (*JG == 0 && strcmp(argv[3], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*JG < 0 || *JG > 2000){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//IT check
    *IT = atoi(argv[4]);
    if (*IT == 0 && strcmp(argv[4], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*IT < 0 || *IT > 2000){fprintf(stderr, "Invalind arguments\n"); return 1;}

	//JT chcek
    *JT = atoi(argv[5]);
    if (*JT == 0 && strcmp(argv[5], "0") != 0){fprintf(stderr, "Invalind arguments\n"); return 1;}
    if (*JT < 0 || *JT > 2000) return 1;
/*
    ///ACTUAL CODE
    pid_t proc_JUDGE = fork();
    if(proc_JUDGE == 0){
        
    }
    pid_t proc_IMM_gen = fork();
    if(proc_IMM_gen == 0){
        IMM_generator(PI, IG);
    }
*/

	
    return 0;
}