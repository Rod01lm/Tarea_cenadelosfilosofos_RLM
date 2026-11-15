// Rodrigo Lara Meza
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_FILOSOFOS 5
#define NUM_COMIDAS 3

pthread_mutex_t tenedores[NUM_FILOSOFOS];

void *filosofo(void *arg) {
    int id = *(int *)arg;
    int izq = id;
    int der = (id + 1) % NUM_FILOSOFOS;

    for (int comida = 1; comida <= NUM_COMIDAS; comida++) {
        // Estado: Pensando
        printf("[Filosofo %d] Esta pensando...\n", id + 1);
        usleep((rand() % 150 + 50) * 1000);

        // Estado: Hambriento (evita deadlock invirtiendo orden en el ultimo filosofo)
        printf("[Filosofo %d] Tiene hambre (busca tenedores %d y %d)\n", id + 1, izq + 1, der + 1);

        if (id == NUM_FILOSOFOS - 1) {
            pthread_mutex_lock(&tenedores[der]);
            printf("  -> [Filosofo %d] Tomo tenedor derecho (%d)\n", id + 1, der + 1);
            pthread_mutex_lock(&tenedores[izq]);
            printf("  -> [Filosofo %d] Tomo tenedor izquierdo (%d)\n", id + 1, izq + 1);
        } else {
            pthread_mutex_lock(&tenedores[izq]);
            printf("  -> [Filosofo %d] Tomo tenedor izquierdo (%d)\n", id + 1, izq + 1);
            pthread_mutex_lock(&tenedores[der]);
            printf("  -> [Filosofo %d] Tomo tenedor derecho (%d)\n", id + 1, der + 1);
        }

        // Estado: Comiendo (Seccion critica sobre ambos tenedores)
        printf("  *** [Filosofo %d] ESTA COMIENDO (comida %d/%d) ***\n", id + 1, comida, NUM_COMIDAS);
        usleep((rand() % 150 + 50) * 1000);

        // Libera ambos tenedores
        pthread_mutex_unlock(&tenedores[izq]);
        pthread_mutex_unlock(&tenedores[der]);
        printf("  <- [Filosofo %d] Termino comida %d/%d y solto tenedores (%d y %d)\n",
               id + 1, comida, NUM_COMIDAS, izq + 1, der + 1);
    }

    printf(">>> [Filosofo %d] Finalizo sus %d comidas y se retira de la mesa.\n", id + 1, NUM_COMIDAS);
    return NULL;
}

int main(void) {
    pthread_t hilos[NUM_FILOSOFOS];
    int ids[NUM_FILOSOFOS];

    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_mutex_init(&tenedores[i], NULL);
    }

    printf("=== INICIO CENA DE LOS FILOSOFOS (%d Filosofos | %d comidas c/u) ===\n\n",
           NUM_FILOSOFOS, NUM_COMIDAS);

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        ids[i] = i;
        if (pthread_create(&hilos[i], NULL, filosofo, &ids[i]) != 0) {
            perror("Error al crear hilo filosofo");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("\n=== CENA FINALIZADA: Todos los filosofos comieron sin interbloqueo ===\n");

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_mutex_destroy(&tenedores[i]);
    }

    return 0;
}
