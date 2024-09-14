#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <fcntl.h>

#define PROCESS_NR 10
#define ALINA1 5
#define ALINA2 43
#define ALINA3 4

pid_t p[PROCESS_NR];
pthread_t t1[ALINA1];
pthread_t t2[ALINA2];
pthread_t t3[ALINA3];

sem_t s1[2];
sem_t s2;
sem_t *s3[2];
sem_t s4, s5;
sem_t *sem;

int i = 0;

pthread_mutex_t locked = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int entry = 0;
int threads = 0;

typedef struct{
    int id;
}th;

void* thr1(void* arg){
    th *a = (th*)arg;
    if(a->id == 2){
        sem_wait(s3[0]);
    }
    if(a->id == 4){
        sem_wait(&s1[0]);
    }
    info(BEGIN, 9, a->id);
    if(a->id == 5){
        sem_post(&s1[0]);
        sem_wait(&s1[1]);
    }
    info(END, 9, a->id);
    if(a->id == 4){
        sem_post(&s1[1]);
    }
    if(a->id == 2){
        sem_post(s3[1]);
    }
    return NULL;
}

void* thr2(void* arg){
    th *a = (th*)arg;
    sem_wait(&s2);
    int var;
    info(BEGIN, 6, a->id);
    sem_wait(&s4);
    if(i < 5 && a->id != 12){
        i++;
        if(i == 5){
            sem_post(&s5);
        }
        var = i;
        sem_post(&s4);
        sem_wait(&sem[var]);
    }else{
        sem_post(&s4);
    }
    if(a->id == 12){
        sem_wait(&s5);
    }
    info(END, 6, a->id);
    if(a->id == 12){
        for(int i = 1 ; i <= 5 ; i++){
            sem_post(&sem[i]);
        }
    }
    sem_post(&s2);
    return NULL;
}

void* thr3(void* arg){
    th *a = (th*)arg;
    if(a->id == 1){
        sem_wait(s3[1]);
    }
    info(BEGIN, 3, a->id);
    info(END, 3, a->id);
    if(a->id == 4){
        sem_post(s3[0]);
    }
    return NULL;
}

int main(){
    s3[0] = sem_open("aa1",O_CREAT,0644,0);
    s3[1] = sem_open("aa2",O_CREAT,0644,0);
    init();
    info(BEGIN, 1, 0);
    p[3] = fork();
    if(p[3] == 0){
        info(BEGIN, 3, 0);
        th th1[ALINA3];
        for(int a = 0; a < ALINA3; a++){
            th1[a].id = a + 1;
            pthread_create(&t3[a], NULL, thr3, &th1[a]);
        }
        for(int a = 0; a < ALINA3; a++){
            pthread_join(t3[a], NULL);
        }
        info(END, 3, 0);
    }else{
        p[9] = fork();
        if(p[9] == 0){
            info(BEGIN, 9, 0);
            th th1[ALINA1];
            sem_init(&s1[0], 0, 0);
            sem_init(&s1[1], 0, 0);
            for(int a = 0; a < ALINA1; a++){
                th1[a].id = a + 1;
                pthread_create(&t1[a], NULL, thr1, &th1[a]);
            }
            for(int a = 0; a < ALINA1; a++){
                pthread_join(t1[a], NULL);
            }
            info(END, 9, 0);
        }else{
            p[2] = fork();
            if(p[2] == 0){
                info(BEGIN, 2, 0);
                p[5] = fork();
                if(p[5] == 0){
                    info(BEGIN, 5, 0);
                    info(END, 5, 0);
                }else{
                    p[4] = fork();
                    if(p[4] == 0){
                        info(BEGIN, 4, 0);
                        p[8] = fork();
                        if(p[8] == 0){
                            info(BEGIN, 8, 0);
                            info(END, 8, 0);
                        }else{
                            waitpid(p[8], NULL, 0);
                            info(END, 4, 0);
                        }
                    }else{
                        p[6] = fork();
                        if(p[6] == 0){
                            info(BEGIN, 6, 0);
                            p[7] = fork();
                            if(p[7] == 0){
                                info(BEGIN, 7, 0);
                                info(END, 7, 0);
                            }else{
                                waitpid(p[7], NULL, 0);
                                th th1[ALINA2];
                                sem_init(&s2, 0, 6);
                                sem_init(&s4, 0, 1);
                                sem_init(&s5, 0, 0);
                                sem = (sem_t*)malloc(6 * sizeof(sem_t));
                                for(int j = 0; j < 6; j++){
                                        sem_init(&sem[j], 0, 0);
                                }
                                for(int a = 0; a < ALINA2; a++){
                                    th1[a].id = a + 1;
                                    pthread_create(&t2[a], NULL, thr2, &th1[a]);
                                }
                                for(int a = 0; a < ALINA2; a++){
                                    pthread_join(t2[a], NULL);
                                }
                                info(END, 6, 0);
                            }
                        }else{
                            waitpid(p[5], NULL, 0);
                            waitpid(p[6], NULL, 0);
                            waitpid(p[4], NULL, 0);
                            info(END, 2, 0);
                        }
                    }
                }
            }else{
                waitpid(p[3], NULL, 0);
                waitpid(p[9], NULL, 0);
                waitpid(p[2], NULL, 0);
                info(END, 1, 0);
            }
        }
    }
    return 0;
}
