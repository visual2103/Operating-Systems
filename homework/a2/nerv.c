#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>   
#include <unistd.h>   

int main() {
    sem_t *sem; 
    const char *sem_name = "/my_semaphore";


    sem = sem_open(sem_name, O_CREAT, 0644, 1);
    if(sem == SEM_FAILED) {
        perror("Nu s-a putut crea/deschide semaforul");
        exit(EXIT_FAILURE);
    }

    
    printf("Asteptam sa obtinem blocarea semaforului...\n");
    sem_wait(sem);
    printf("Am obtinut blocarea semaforului!\n");

  
    printf("Realizam o operatie critica...\n");
    sleep(3);

    
    sem_post(sem);
    printf("Am eliberat blocarea semaforului!\n");


    sem_close(sem);
 
    sem_unlink(sem_name);

    return 0;
}
