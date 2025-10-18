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
    pthread_t tache1, tache2;
    int id1 = 1, id2 = 2;
    pthread_create(&tache1, NULL, fonc, &id1);
    //usleep(500000);
    pthread_create(&tache2, NULL, fonc, &id2);

    pthread_join(tache1, NULL);
    pthread_join(tache2, NULL);
    return 0; 
}