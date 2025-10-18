#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


void* fonc(void* arg){
    int i;
    for(i=0;i<7;i++){
        printf("Tache %d : %d\n", *(int*) arg, i);
        usleep(1000000); //attendre 1 seconde
    } 
}

int main(void)
{
    pthread_t tache1;
    pthread_attr_t attr;
    int id1 = 1;
    pthread_attr_init(&attr);

    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&tache1, &attr, fonc, &id1);
    
    pthread_attr_destroy(&attr);

    return 0; 
}