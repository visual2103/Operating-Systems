
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h> 
#define simThreads 6 

typedef struct THREAD_struct{
    int id ; 
}THREAD_struct ; 

sem_t sem94 , sem95   ; 
sem_t *sem39 =NULL;
sem_t *sem93=NULL; 

void* function3(void *args){
    THREAD_struct * t2 = (THREAD_struct*) args ;
    if(t2->id ==1){
        sem_wait(sem93);
    }
    info(BEGIN , 3 ,t2->id);
    info(END , 3 ,t2->id);
    if(t2->id ==4){
        sem_post(sem39);
    }
    return NULL ;
}


void* function9(void *arg){
    THREAD_struct * t = (THREAD_struct*) arg ;
    if (t->id ==4){
        sem_wait(&sem94) ; 
    }
    if (t->id == 2){
        sem_wait(sem39);
    }
    info(BEGIN , 9 ,t->id);
    if(t->id == 5){
        sem_post(&sem94);
        sem_wait(&sem95);
    }
    info(END , 9 ,t->id);
    if(t->id == 4){
        sem_post(&sem95);
    }
    if(t->id==2){
        sem_post(sem93);
    }
    return NULL ;
}


sem_t* sem , sem6 ,sem12 , protect; 
int i = 0 ;
void* function6(void *args){
    THREAD_struct * t1 = (THREAD_struct*) args ;
    sem_wait(&sem6);
    int var ;
    info(BEGIN , 6 ,t1->id);
    sem_wait(&protect);
    if(i < 5 && t1->id !=12){
        i++;
        if (i==5){
            sem_post(&sem12);
        }   
        var= i ; 
        sem_post(&protect);
        
        sem_wait(&sem[var]); 
    }
    else {
        sem_post(&protect) ;
        }   
    if (t1->id == 12){
        sem_wait(&sem12);
    }
    info(END , 6 ,t1->id);
     if (t1->id == 12){
        for(int i = 1 ; i <= 5 ; i++){
            sem_post(&sem[i]);
        }
    }
    sem_post(&sem6) ; 
    return NULL;
}


int main(int argc , char*argv[]){
    init(); 
    const char* name1 = "/smph1" ; 
    const char* name2 = "/smph2" ; 
    sem39=sem_open(name1,O_CREAT,0644,0);
    sem93=sem_open(name2,O_CREAT ,0644,0);
    if( sem39  == NULL){
        printf("Error in initializing semaphores in fct 3\n");
        return - 1;
    }
    if(sem93 == NULL){
        printf("Error in initializing semaphores in fct 9\n");
        return -1;
    }
    info(BEGIN,1,0);
    //copii lui 1 
    pid_t pid2=-1 , pid3=-1 ,pid9=-1 ;
    //primu copil
    pid2 = fork();
    if (pid2 ==-1){
        perror("Child process can't be created !\n") ;
        return -1; 
    }
    else 
        if(pid2 == 0 ){
            //copii lui 2 
            info(BEGIN, 2, 0) ;
            pid_t pid4 =-1 , pid5 =-1 , pid6 =-1 ;
            pid4 =fork();
            if(pid4 == -1){
                perror("Child process can't be created !\n") ;
                return -1; 
            }
            else 
                if(pid4 == 0){
                    //copii lui 4 ->8
                    info(BEGIN,4,0);
                    pid_t pid8 =-1 ;
                    pid8 = fork();
                    if (pid8 == -1){
                        perror("Child process can't be created !\n") ;
                        return -1; 
                    }
                    else    
                        if(pid8 == 0){
                            //8 nu are copii 
                            info(BEGIN, 8, 0);			
							info(END, 8, 0);
                            exit(0) ; 
                        }
                        else{
                            waitpid(pid8 , NULL,0);
                            info(END,4,0);
                            exit(0);
                        }
                }else{
                    waitpid(pid4 , NULL , 0);
                    // 5 pe nivel 2yy
                    pid5=fork();
                    if(pid5 == -1){
                        perror("Child process can't be created !\n") ;
                        return -1; 
                    }
                    else
                        if (pid5 == 0){
                            //5 nu are copii 
                            info(BEGIN, 5, 0);			
							info(END, 5, 0);
                            exit(0) ; 
                        }
                        else{
                            waitpid(pid5,NULL,0);
                            //6 pe nivel 2 
                            pid6 = fork();
                            if (pid6 == -1){
                                perror("Child process can't be created !\n") ;
                                return -1; 
                            }
                            else 
                                if(pid6 == 0 ){
                                    //copii lui 6 -> 7 
                                    info(BEGIN, 6, 0);	
                                    //creez 43 de threaduri 
                                    sem_init(&sem6,0,6);
                                    sem = (sem_t*)malloc(simThreads*sizeof(sem_t));
                                     for(int j = 0 ; j <  simThreads ; j++){
                                            sem_init(&sem[j],0,0);
                                    }
                                    sem_init(&sem12,0,0);
                                    sem_init(&protect,0,1);
                                    pthread_t tID6[42];
                                    THREAD_struct parameters[42] ; 
                                    
                                    for(int i= 0 ; i<43 ; i++){

                                        parameters[i].id = i+1 ; 
                                        pthread_create(&tID6[i], NULL , function6 , &parameters[i]);
                                    }
                                    for(int i= 0 ; i<42; i++){
                                        
                                        
                                        pthread_join(tID6[i+1] , NULL);
                                    }
                                    pid_t pid7 = -1 ;
                                    pid7=fork();
                                    if (pid7 == -1){
                                        perror("Child process can't be created !\n") ;
                                        return -1; 
                                    }
                                    else    
                                        if(pid7 == 0){
                                            //7 nu are copii 
                                            info(BEGIN, 7, 0);			
                                            info(END, 7, 0);
                                            exit(0) ; 
                                        }
                                        else{
                                            waitpid(pid7, NULL,0);
                                            info(END,6,0);
                                            sem_destroy(&sem6);
                                            free(sem);
                                            sem_destroy(sem);
                                            sem_destroy(&protect);
                                            sem_destroy(&sem12);
                                            exit(0);
                                        }
                                }
                                else {
                                    waitpid(pid6,NULL,0);
                                    info(END,2,0);
                                    exit(0) ; 
                                }
                        }
                }
        }
        else {
            waitpid(pid2,NULL,0);
            // 3 pe nivel 1
            pid3 = fork() ; 
            if(pid3 == -1){
                perror("Child process can't be created !\n") ;
                return -1; 
            }
            else
                if (pid3==0){
                    info(BEGIN, 3, 0) ;
                    pthread_t tID3[3];
                    THREAD_struct param3[3] ; 
                    for(int i= 0 ; i< 4 ; i++){
                        param3[i].id = i+1 ; 
                        pthread_create(&tID3[i], NULL , function3 , &param3[i]);
                    }
                    for(int i= 0 ; i<=3; i++){
                            pthread_join(tID3[i] , NULL);
                         }
                    info(END, 3, 0) ;
                    exit(0) ;
                }
                else{
                    // 9 pe nivel 1
                    pid9 = fork() ; 
                    if (pid9 == 0){
                        sem_init(&sem94,0,0);
                        sem_init(&sem95,0,0);
                        info(BEGIN, 9, 0) ;
                        pthread_t tID9[4];
                        THREAD_struct parameters[4] ; 
                        for(int i= 0 ; i<5 ; i++){
                            parameters[i].id = i+1 ; 
                            pthread_create(&tID9[i], NULL , function9 , &parameters[i]);
                        }
                         for(int i= 0 ; i<=4; i++){
                            pthread_join(tID9[i] , NULL);
                         }
                        info(END, 9, 0) ; 
                        sem_destroy(&sem94);
                        sem_destroy(&sem95);
                      
                        exit(0) ;
                    }
                    else{
                        waitpid(pid9 ,NULL , 0);
                        sem_unlink(name1);
                        waitpid(pid3,NULL,0) ; 
                        sem_unlink(name2);
                        info(END , 1 , 0) ;
                       
                        exit(0);
                         
                   
                    }
        
            }
        }     
    return 0 ;
}