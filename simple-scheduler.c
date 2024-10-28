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

void scheduler(int ncpu, int tslice){
    while(true){
        unsigned int rem_sleep=sleep(tslice/1000);
        if(rem_sleep>0){
            printf("Sleep interrupted after "+rem_sleep+" seconds\n");
            exit(1);
        }
        if(sem_wait(&(*processTable).mutex)==-1){
            perror("sem_wait");
            exit(1);
        }
        if(isEmpty(running) && isEmpty(ready) && (*processTable).count==0){
            printf("Terminating processor as no processes remaining.")
            terminate();
        }
        //moving process to ready queue
        for(int i=0; i<(*processTable)count; i++){
            if((*processTable).processArray[i].submitted && !(*processTable).processArray[i].completed) && !(*processTable).processArray[i].queued){
                if((*ready).size < (*ready).max-1){
                    (*processTable).processArray[i].queued=true;
                    enqueue(ready, &(*processTable).processArray[i]);
                }else{break;}
            }
        }
        //pausing process in running queue
        if(!isEmpty(running)){
            for(int i=0; i<ncpu, i++){
                if(!isEmpty(running)){
                    struct process *proc=(*running).array[(*running).head];
                    if(!(*proc).completed){
                        enqueue(ready,proc);
                        (*proc).execution_time+=end_time(&(*proc).start);
                        start_time=&(*proc).start;
                        if(kill((*proc).pid,SIGSTOP==-1){
                            perror("kill");
                            exit(1);
                        }
                        dequeue(running);
                    }else{dequeue(running);}    
        }}}
        //resuming process in ready queue
        if(!isEmpty(ready)){
            for(int i=0; i<ncpu;i++){
                if(!isEmpty(ready)){
                    struct process *proc=(*ready).array[(*ready).head];
                    dequeue(ready);
                    (*proc).wait_time+=end_time(&(*proc).start);
                    start_time(&(*proc).start);
                    if(kill(proc.pid,SIGCONT)==-1){
                    perror("kill");
                    exit(1);
                }
                enqueue(running,proc);
        }}}
        if(sem_post(&(*processTable).mutex)==-1){
            perror("sem_post");
            exit(1);
        }
}}
int main(int argc, char const *argv[]){
    //NCPU = atoi(argv[1]);
    //TSLICE = atoi(argv[2])*1000;
    NCPU=(char*)argv[1];
    TSLICE=(char*)argv[2];
    int ncpu=atoi(NCPU);
    int tslice=atoi(TSLICE)*1000;
    
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
