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

// 1. Les données partagées
int charge_actuelle_pont = 0;
// Variable pour gérer la priorité
int camions_en_attente = 0; 

// 2. Le verrou d'exclusion mutuelle
pthread_mutex_t mutex_pont = PTHREAD_MUTEX_INITIALIZER;

// 3. La variable de condition
pthread_cond_t cv_pont_disponible = PTHREAD_COND_INITIALIZER;

// --- Fonctions du Moniteur (Avec priorité) ---

/**
 * Fonction d'acquisition avec priorité aux camions.
 */
void acq_pont(int poids, int id, const char* type_vehicule) {
    // 1. Verrouiller le moniteur
    pthread_mutex_lock(&mutex_pont);

    // 2. Si je suis un camion, je signale mon attente
    if (poids == POIDS_CAMION) {
        camions_en_attente++;
    }

    // 3. Condition d'attente (avec priorité)
    // "Je m'endors TANT QUE..."
    while ( 
        // Condition 1: Il n'y a pas assez de place
        (charge_actuelle_pont + poids > CHARGE_MAX_PONT) 
        || 
        // Condition 2 (PRIORITÉ): Je suis une voiture ET un camion attend
        ( (poids == POIDS_VOITURE) && (camions_en_attente > 0) )
    ) 
    {
        printf("    [%s %d] ATTEND. Charge: %d/%d. (Veut %d t). [Camions en attente: %d]\n", 
               type_vehicule, id, charge_actuelle_pont, CHARGE_MAX_PONT, poids, camions_en_attente);
        
        pthread_cond_wait(&cv_pont_disponible, &mutex_pont);
    }

    // 4. Si j'étais un camion, j'ai eu ma place, je ne suis plus en attente
    if (poids == POIDS_CAMION) {
        camions_en_attente--;
    }

    // 5. Acquisition de la ressource
    charge_actuelle_pont += poids;
    printf("==> [%s %d] ENTRE. Charge actuelle: %d/%d. [Camions en attente: %d]\n", 
           type_vehicule, id, charge_actuelle_pont, CHARGE_MAX_PONT, camions_en_attente);

    // 6. Libérer le verrou du moniteur
    pthread_mutex_unlock(&mutex_pont);
}

/**
 * Fonction de libération (INCHANGÉE).
 */
void lib_pont(int poids, int id, const char* type_vehicule) {
    pthread_mutex_lock(&mutex_pont);

    charge_actuelle_pont -= poids;
    printf("<== [%s %d] SORT. Charge actuelle: %d/%d\n", 
           type_vehicule, id, charge_actuelle_pont, CHARGE_MAX_PONT);

    // Réveille tout le monde. La logique dans acq_pont fera le tri.
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

    printf("Simulation du pont (PRIORITÉ CAMIONS). Charge maximale: %d tonnes.\n", CHARGE_MAX_PONT);
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