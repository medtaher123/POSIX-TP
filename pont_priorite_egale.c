#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // Pour sleep()
#include <time.h>   // Pour srand()

// --- Constantes du problème ---
#define CHARGE_MAX_PONT 15
#define POIDS_VOITURE    5
#define POIDS_CAMION    15

#define NB_VOITURES 5
#define NB_CAMIONS  3
#define NB_TRAVERSEES 1 // Une seule traversée

// --- Définition du Moniteur ---

// 1. Les données partagées (simplifiées)
int charge_actuelle_pont = 0;
// La variable 'camions_en_attente' a été SUPPRIMÉE.

// 2. Le verrou d'exclusion mutuelle
pthread_mutex_t mutex_pont = PTHREAD_MUTEX_INITIALIZER;

// 3. La variable de condition
pthread_cond_t cv_pont_disponible = PTHREAD_COND_INITIALIZER;

// --- Fonctions du Moniteur (Simplifiées) ---

/**
 * Fonction d'acquisition (Priorité égale)
 */
void acq_pont(int poids, int id, const char* type_vehicule) {
    // 1. Verrouiller le moniteur
    pthread_mutex_lock(&mutex_pont);

    
    while ( (charge_actuelle_pont + poids > CHARGE_MAX_PONT) ) 
    {
        printf("    [%s %d] ATTEND. Charge: %d/%d. (Veut %d t)\n", 
               type_vehicule, id, charge_actuelle_pont, CHARGE_MAX_PONT, poids);
        
        pthread_cond_wait(&cv_pont_disponible, &mutex_pont);
    }

    // 3. Acquisition de la ressource
    charge_actuelle_pont += poids;
    printf("==> [%s %d] ENTRE. Charge actuelle: %d/%d\n", 
           type_vehicule, id, charge_actuelle_pont, CHARGE_MAX_PONT);

    // 4. Libérer le verrou du moniteur
    pthread_mutex_unlock(&mutex_pont);
}

/**
 * Fonction de libération (INCHANGÉE)
 */
void lib_pont(int poids, int id, const char* type_vehicule) {
    pthread_mutex_lock(&mutex_pont);

    charge_actuelle_pont -= poids;
    printf("<== [%s %d] SORT. Charge actuelle: %d/%d\n", 
           type_vehicule, id, charge_actuelle_pont, CHARGE_MAX_PONT);

    // Réveille tout le monde.
    pthread_cond_broadcast(&cv_pont_disponible);

    pthread_mutex_unlock(&mutex_pont);
}


// --- Fonctions des Threads (Simulation) ---
// (INCHANGÉES)

void* thread_voiture(void* arg) {
    int id = *(int*)arg;
    const char* type = "Voiture";
    
    for (int i = 0; i < NB_TRAVERSEES; i++) {
        sleep(rand() % 5 + 1); 
        acq_pont(POIDS_VOITURE, id, type);
        
        printf("    [%s %d] ...traverse...\n", type, id);
        sleep(rand() % 2 + 1);
        
        lib_pont(POIDS_VOITURE, id, type);
    }
    printf("--- [%s %d] a fini sa traversée ---\n", type, id);
    return NULL;
}

void* thread_camion(void* arg) {
    int id = *(int*)arg;
    const char* type = "CAMION";
    
    for (int i = 0; i < NB_TRAVERSEES; i++) {
        sleep(rand() % 6 + 2);
        acq_pont(POIDS_CAMION, id, type);
        
        printf("    [%s %d] ...traverse...\n", type, id);
        sleep(rand() % 3 + 1);
        
        lib_pont(POIDS_CAMION, id, type);
    }
    printf("--- [%s %d] a fini sa traversée ---\n", type, id);
    return NULL;
}


// --- Programme Principal ---
// (INCHANGÉ)

int main() {
    pthread_t threads_voitures[NB_VOITURES];
    pthread_t threads_camions[NB_CAMIONS];
    
    int id_voitures[NB_VOITURES];
    int id_camions[NB_CAMIONS];

    srand(time(NULL));

    printf("Simulation du pont (PRIORITÉ ÉGALE / Risque de famine). Charge max: %d t.\n", CHARGE_MAX_PONT);
    printf("Lancement de %d voitures (%dt) et %d camions (%dt).\n\n", 
           NB_VOITURES, POIDS_VOITURE, NB_CAMIONS, POIDS_CAMION);

    // Lancement des threads voitures
    for (int i = 0; i < NB_VOITURES; i++) {
        id_voitures[i] = i + 1;
        pthread_create(&threads_voitures[i], NULL, thread_voiture, &id_voitures[i]);
    }

    // Lancement des threads camions
    for (int i = 0; i < NB_CAMIONS; i++) {
        id_camions[i] = i + 1;
        pthread_create(&threads_camions[i], NULL, thread_camion, &id_camions[i]);
    }

    // Attendre la fin
    for (int i = 0; i < NB_VOITURES; i++) {
        pthread_join(threads_voitures[i], NULL);
    }
    for (int i = 0; i < NB_CAMIONS; i++) {
        pthread_join(threads_camions[i], NULL);
    }

    pthread_mutex_destroy(&mutex_pont);
    pthread_cond_destroy(&cv_pont_disponible);

    printf("\nSimulation terminée. Tous les véhicules ont fait leur unique traversée.\n");
    return 0;
}