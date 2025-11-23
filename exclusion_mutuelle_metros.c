#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <unistd.h> 
#include <time.h> 

#define TRAVEL_TIME_US 2000000 
#define NB_TRAINS      3        


#define COLOR_RESET  "\033[0m"
#define COLOR_MR1    "\033[1;36m" 
#define COLOR_MR2    "\033[1;33m" 
#define COLOR_MR3    "\033[1;35m" 

time_t start_time;
pthread_mutex_t mut_AB, mut_BC, mut_CD, mut_FA, mut_EB;

void log_action(const char* color, char* train_name, int id, const char* message) {
    long elapsed = time(NULL) - start_time;
    printf("%s[T+%lds] [%s #%d] %s%s\n", 
           color, elapsed, train_name, id, message, COLOR_RESET);
}

void use_segment(pthread_mutex_t *mutex, char *segment_name, char *train_name, int id, const char* color) {
    char buffer[100];

    sprintf(buffer, "... Attend le segment %s", segment_name);
    log_action(color, train_name, id, buffer);

    pthread_mutex_lock(mutex); 
    sprintf(buffer, ">>> OCCUPE le segment %s", segment_name);
    log_action(color, train_name, id, buffer);

    usleep(TRAVEL_TIME_US); 

    sprintf(buffer, "<<< LIBERE le segment %s", segment_name);
    log_action(color, train_name, id, buffer);
    pthread_mutex_unlock(mutex); 
}

// --- 3. Threads ---

void* thread_MR1(void *arg){
    int id = *(int*)arg;

    log_action(COLOR_MR1, "MR1", id, "Départ station A");
    
    use_segment(&mut_AB, "AB", "MR1", id, COLOR_MR1); 
    use_segment(&mut_BC, "BC", "MR1", id, COLOR_MR1); 
    use_segment(&mut_CD, "CD", "MR1", id, COLOR_MR1); 
    
    log_action(COLOR_MR1, "MR1", id, "Terminus station D");
}

void* thread_MR2(void *arg){
    int id = *(int*)arg;

    log_action(COLOR_MR2, "MR2", id, "Départ station F");
    
    use_segment(&mut_FA, "FA", "MR2", id, COLOR_MR2);
    
    log_action(COLOR_MR2, "MR2", id, "Terminus station A");
}

void* thread_MR3(void *arg){
    int id = *(int*)arg;

    log_action(COLOR_MR3, "MR3", id, "Départ station E");
    
    use_segment(&mut_EB, "EB", "MR3", id, COLOR_MR3);
    
    log_action(COLOR_MR3, "MR3", id, "Terminus station B");
    return NULL;
}

int main() {
    start_time = time(NULL);

    pthread_mutex_init(&mut_FA, NULL);
    pthread_mutex_init(&mut_AB, NULL);
    pthread_mutex_init(&mut_BC, NULL);
    pthread_mutex_init(&mut_CD, NULL);
    pthread_mutex_init(&mut_EB, NULL);

    pthread_t threads[NB_TRAINS * 3];
    int count = 0;

    printf("=== Simulation Started (Cyan=MR1, Jaune=MR2, Magenta=MR3) ===\n");

    for (int i = 1; i <= NB_TRAINS; i++) {
        int* arg; 

        arg = malloc(sizeof(int)); *arg = i;
        pthread_create(&threads[count++], NULL, thread_MR1, arg);

        arg = malloc(sizeof(int)); *arg = i;
        pthread_create(&threads[count++], NULL, thread_MR2, arg);

        arg = malloc(sizeof(int)); *arg = i;
        pthread_create(&threads[count++], NULL, thread_MR3, arg);
    }

    for (int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("=== Simulation Terminée ===\n");

    return 0;
}