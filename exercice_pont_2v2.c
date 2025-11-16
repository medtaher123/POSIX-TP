#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <time.h>

sem_t pont; 
pthread_mutex_t verrou; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
int nb_camions_en_attente = 0;
int nb_voiture

void* voiture(void *arg){
    

    sem_wait(&pont); 
    while (nb_camions_en_attente > 0){
        pthread_cond_wait(&cond, &pont); 
    }


    
    
    printf("voiture %s entre .....\n",(char*)arg);
    int wait_sec = 2 + rand() % 7; 
    usleep(wait_sec  *1000000);

    
    sem_post(&pont);  
    printf("voiture %s sort \n",(char*)arg);
}


void* camion(void *arg){

    nb_camions_en_attente ++;

    
    sem_wait(&pont);
    sem_wait(&pont);
    sem_wait(&pont); 
    
    
    
    printf("camion %s entre .....\n",(char*)arg);
    int wait_sec = 2 + rand() % 5; 
    usleep(wait_sec  *1000000);


    nb_camions_en_attente --;
    sem_post(&pont);
    sem_post(&pont);
    sem_post(&pont);  

    printf("camion %s sort \n",(char*)arg);
}

int main(){
    srand(time(NULL));
    pthread_t v1,v2,v3,v4,v5,v6,c1,c2,c3;
    sem_init( &pont, 0, 3);
    
    
    pthread_create (&v1, NULL, voiture, "1");
    pthread_create (&v2, NULL, voiture, "2");
    pthread_create (&c1, NULL, camion, "1");
    pthread_create (&v3, NULL, voiture, "3");
    pthread_create (&v4, NULL, voiture, "4");
    pthread_create (&c2, NULL, camion, "2");
    pthread_create (&v5, NULL, voiture, "5");
    pthread_create (&c3, NULL, camion, "3");
    pthread_create (&v6, NULL, voiture, "6");
    

    pthread_join(v1, NULL); 
    pthread_join(v2, NULL); 
    pthread_join(v3, NULL); 
    pthread_join(v4, NULL); 
    pthread_join(v5, NULL); 
    pthread_join(v6, NULL); 

    pthread_join(c1, NULL); 
    pthread_join(c2, NULL); 
    pthread_join(c3, NULL); 
    return 0;
}