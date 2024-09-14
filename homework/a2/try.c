#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <stdlib.h>

int main(int argc , char*argv[]){
    init(); 
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
                    // 5 pe nivel 2
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
                    info(END, 3, 0) ;
                    exit(0) ;
                }
                else{
                    waitpid(pid3,NULL,0) ; 
                    // 9 pe nivel 1
                    pid9 = fork() ; 
                    if (pid9 == 0){
                        info(BEGIN, 9, 0) ;
                        info(END, 9, 0) ; 
                        exit(0) ;
                    }
                    else{
                        waitpid(pid9 ,NULL , 0);
                        info(END , 1 , 0) ;
                        exit(0);
                    }
        
            }
        }     
    return 0 ;
}