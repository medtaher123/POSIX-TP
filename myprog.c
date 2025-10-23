#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* print_numbers(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= 5; i++) {
        printf("Thread %d: %d\n", id, i);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2, id0 = 0;

    pthread_create(&t1, NULL, print_numbers, &id1);
    sleep(0.5);
    pthread_create(&t2, NULL, print_numbers, &id2);

    

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Both threads finished!\n");
    return 0;
}
