#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <errno.h>

#define MAX 1000

struct process{
    int pid;
    bool submitted;
    bool queued;
    bool completed;
    char* cmd;};

struct procTable{
    struct process processArray[MAX];
    int count;
    sem_t mutex;

};
struct Queue{
    int head,tail,max,size;
    struct process **array;
};

bool isEmpty(struct Queue *q){
    return (*q).head==(*q).tail;
}
bool isFull(struct Queue *q){
    if ((*q).tail==(*q).max-1){
        return (*q).head==0;
    }
    return (*q).head==(*q).tail+1;
}
void enqueue(struct Queue *q, struct process *proc){
    if (isFull(q)){
        printf("queue overflow\n");
        return;
    }
    (*q).size++;
    (*q).array[q->tail] = proc;
    if ((*q).tail==(*q).max-1){
        (*q).tail=0;}
    else{(*q).tail++;}
}
void dequeue(struct Queue *q){
    if (isEmpty(q)){
        printf("queue underflow\n");
        return;
    }
   (*q).size--;
    if ((*q).tail==(*q).max-1){
        (*q).tail=0;}
    else{
        (*q).tail++;}  
}
char* NCPU;
char* TSLICE;
struct procTable* processTable;
int sharedMemory;
struct Queue *running;
struct Queue *ready;
int main(int argc, char const *argv[]){
    NCPU = atoi(argv[1]);
    TSLICE = atoi(argv[2])*1000;
    sharedMemory=shm_open("/shm26",O_RDWR, 0666);
    if (sharedMemory==-1){
        perror("shm_open error");
        exit(1);
    }
    //printf("Shared Memory FD: %d\n", sharedMemory);
    if (ftruncate(sharedMemory,sizeof(struct procTable))==-1){
        perror("ftruncate error");
        exit(1);
    }
    processTable=mmap(NULL,sizeof(struct procTable),PROT_READ|PROT_WRITE, MAP_SHARED,sharedMemory,0);
    if (processTable==MAP_FAILED){
        perror("mmap");
        exit(1);
    }
}
